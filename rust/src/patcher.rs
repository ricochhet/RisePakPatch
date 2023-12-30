use std::collections::HashMap;
use std::fs;
use std::io;
use std::path::{Path, PathBuf};
use std::vec;

use crate::entry::FileEntry;
use crate::mmh3_32::murmurhash3_x86_32;
use crate::reader::Reader;
use crate::utils;
use crate::utils::to_utf16;
use crate::utils::utf16_to_utf8;
use crate::writer::Writer;

const MAX_SINGLE_FILE_SIZE: u64 = 1_073_741_824;

pub struct RisePakPatch {
    reverse_lookup_table: HashMap<u32, String>,
}

impl RisePakPatch {
    pub fn new() -> RisePakPatch {
        RisePakPatch {
            reverse_lookup_table: HashMap::new(),
        }
    }

    pub fn reverse_lookup(&self, hash: u32) -> String {
        match self.reverse_lookup_table.get(&hash) {
            Some(value) => value.clone(),
            None => String::new(),
        }
    }

    pub fn process_directory(
        &mut self,
        path: &str,
        output_file: &str,
        embed_lookup_table: bool,
    ) -> io::Result<()> {
        let directory = fs::canonicalize(path)?;
        if !directory.is_dir() {
            return Ok(());
        }

        if fs::metadata(output_file).is_ok() {
            println!("Deleting existing output file...");
            fs::remove_file(output_file)?;
        }

        let mut sorted_files: Vec<String> = Vec::new();
        visit_dirs(&directory.join("natives"), &mut sorted_files)?;

        sorted_files.sort_by(|a, b| {
            let path_a = Path::new(a);
            let path_b = Path::new(b);
            if path_a.parent() == path_b.parent() {
                path_a.file_name()
                    .unwrap()
                    .to_str()
                    .unwrap()
                    .cmp(path_b.file_name().unwrap().to_str().unwrap())
            } else {
                path_a.parent().unwrap().cmp(path_b.parent().unwrap())
            }
        });

        println!("Processing {} files", sorted_files.len());

        let mut list: Vec<FileEntry> = Vec::new();
        let mut writer = Writer::new(output_file)?;

        writer.write_uint32(1095454795)?;
        writer.write_uint32(4)?;
        writer.write_uint32(sorted_files.len() as u32)?;
        writer.write_uint32(0)?;

        let pos = writer.position();
        let _ = writer.seek_from_beginning(48 * sorted_files.len() as i64 + pos?);

        for obj in sorted_files {
            let mut file_entry = FileEntry::default();
            let mut text = obj.replace(directory.to_string_lossy().as_ref(), "");
            text = text.replace("\\", "/");

            if text.starts_with('/') {
                text.remove(0);
            }
            
            let hash = murmurhash3_x86_32(utf16_to_utf8(&to_utf16(&text.to_lowercase())), std::u32::MAX);
            let hash2 = murmurhash3_x86_32(utf16_to_utf8(&to_utf16(&text.to_uppercase())), std::u32::MAX);
            let array2: Vec<u8> = utils::read_all_bytes(&obj)?;

            file_entry.file_name_lower = hash;
            file_entry.file_name_upper = hash2;
            file_entry.offset = writer.position()? as u64;

            file_entry.uncomp_size = array2.len() as u64;

            list.push(file_entry.clone());
            writer.write(&array2)?;

            self.reverse_lookup_table.insert(hash, text.clone());
            self.reverse_lookup_table.insert(hash2, text);
        }

        writer.seek_from_beginning(16)?;
        for item in list {
            writer.write_uint32(item.file_name_lower)?;
            writer.write_uint32(item.file_name_upper)?;
            writer.write_uint64(item.offset)?;
            writer.write_uint64(item.uncomp_size)?;
            writer.write_uint64(item.uncomp_size)?;
            writer.write_uint64(0)?;
            writer.write_uint32(0)?;
            writer.write_uint32(0)?;
        }

        writer.seek_from_end(0)?;
        let mut lookup_table_size = 0;
        if embed_lookup_table {
            for (hash, entry) in &self.reverse_lookup_table {
                let first_size = std::mem::size_of::<u32>() as u32;
                let second_size = entry.len() as u32 + 1;
                lookup_table_size += first_size + second_size;

                writer.write_uint32(*hash)?;
                writer.write(entry.as_bytes())?;
                writer.write_uint8(0)?;
            }

            writer.write_uint32(lookup_table_size)?;
        }

        Ok(())
    }

    pub fn extract_directory(
        &mut self,
        input_file: &str,
        output_directory: &str,
        embed_lookup_table: bool,
    ) -> io::Result<()> {
        let mut reader = Reader::new(input_file)?;
        self.reverse_lookup_table.clear();

        if embed_lookup_table {
            reader.seek_from_end(std::mem::size_of::<i64>() as i64)?;
            let lookup_table_size = reader.read_uint32()?;
            let _ = reader.seek_from_end(lookup_table_size as i64 - std::mem::size_of::<i64>() as i64);

            println!("{}", reader.position()?);
            while reader.position()? < reader.size()? - (std::mem::size_of::<u32>() as i64) {
                let hash = reader.read_uint32()?;

                let mut file_name = String::new();
                while let Some(c) = reader.read_char()? {
                    if c == '\0' {
                        break;
                    }
                    file_name.push(c);
                }

                self.reverse_lookup_table.insert(hash, file_name);
            }
        }

        reader.seek(0)?;

        if !reader.is_valid() {
            println!("Error opening input file: {}", input_file);
            return Ok(());
        }

        let unk0 = reader.read_uint32()?;
        let unk1 = reader.read_uint32()?;
        let unk2 = reader.read_uint32()?;
        reader.read_uint32()?;

        if unk0 != 1095454795 || unk1 != 4 {
            println!("Invalid file format");
            return Ok(());
        }

        let mut file_list: Vec<FileEntry> = Vec::new();
        for _ in 0..unk2 {
            let mut file_entry = FileEntry::default();
            file_entry.file_name_lower = reader.read_uint32()?;
            file_entry.file_name_upper = reader.read_uint32()?;
            file_entry.offset = reader.read_uint64()?;
            file_entry.uncomp_size = reader.read_uint64()?;
            reader.skip(8)?;
            reader.skip(8)?;
            reader.skip(4)?;
            reader.skip(4)?;

            file_list.push(file_entry);
        }

        for file_entry in file_list {
            let file_path = PathBuf::from(output_directory).join(self.reverse_lookup(file_entry.file_name_lower));
            fs::create_dir_all(file_path.parent().unwrap())?;
            println!("{}", file_path.to_string_lossy());

            let mut file_data = vec![0; file_entry.uncomp_size as usize];
            reader.seek(file_entry.offset as i64)?;
            reader.read(&mut file_data)?;

            if file_data.len() as u64 > MAX_SINGLE_FILE_SIZE {
                println!("File data exceeded 1GB.");
                break;
            }

            utils::write_all_bytes(&file_path, &file_data)?;
        }

        Ok(())
    }
}

fn visit_dirs(dir: &std::path::Path, sorted_files: &mut Vec<String>) -> std::io::Result<()> {
    for entry in fs::read_dir(dir)? {
        let entry = entry?;
        let path = entry.path();

        if path.is_file() {
            sorted_files.push(path.to_string_lossy().to_string());
        } else if path.is_dir() {
            visit_dirs(&path, sorted_files)?;
        }
    }
    Ok(())
}