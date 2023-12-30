use std::fs::File;
use std::io::{self, Read, Write};
use std::path::PathBuf;

pub fn read_all_bytes(file_path: &str) -> io::Result<Vec<u8>> {
    let mut file = File::open(file_path)?;
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer)?;
    Ok(buffer)
}

pub fn write_all_bytes(file_path: &PathBuf, data: &[u8]) -> io::Result<()> {
    let mut file = File::create(file_path)?;
    file.write_all(data)?;
    Ok(())
}

pub fn to_utf16(text: &str) -> Vec<u16> {
    text.encode_utf16().collect()
}

pub fn utf16_to_utf8(v: &Vec<u16>) -> &[u8] {
    let u8_slice: &[u8] = unsafe { 
        std::slice::from_raw_parts(v.as_ptr() as *const u8, v.len() * 2) 
    };
    u8_slice
}