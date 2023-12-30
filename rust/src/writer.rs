use std::fs::File;
use std::io::{self, Seek, SeekFrom, Write};

pub struct Writer {
    file: File,
    position: i64,
}

impl Writer {
    pub fn new(file_path: &str) -> io::Result<Writer> {
        let file = File::create(file_path)?;
        let position = 0;
        Ok(Writer { file, position })
    }

    pub fn seek(&mut self, pos: i64) -> io::Result<()> {
        self.file.seek(SeekFrom::Current(pos))?;
        self.position = pos;
        Ok(())
    }

    pub fn seek_from_beginning(&mut self, pos: i64) -> io::Result<()> {
        self.file.seek(SeekFrom::Start(pos as u64))?;
        self.position = pos as i64;
        Ok(())
    }

    pub fn seek_from_end(&mut self, pos: i64) -> io::Result<()> {
        self.file.seek(SeekFrom::End(pos))?;
        self.position = pos;
        Ok(())
    }

    pub fn write_uint8(&mut self, value: u8) -> io::Result<()> {
        let bytes = value.to_le_bytes();
        self.file.write_all(&bytes)?;
        self.position += 4;
        Ok(())
    }

    pub fn write_uint32(&mut self, value: u32) -> io::Result<()> {
        let bytes = value.to_le_bytes();
        self.file.write_all(&bytes)?;
        self.position += 4;
        Ok(())
    }

    pub fn write_uint64(&mut self, value: u64) -> io::Result<()> {
        let bytes = value.to_le_bytes();
        self.file.write_all(&bytes)?;
        self.position += 8;
        Ok(())
    }

    pub fn write(&mut self, data: &[u8]) -> io::Result<()> {
        println!("Position: {}", self.position()?);
        self.file.write_all(data)?;
        self.position += data.len() as i64;
        Ok(())
    }

    pub fn position(&mut self) -> io::Result<i64> 
    {
        Ok(self.position as i64)
    }
    
    pub fn size(&self) -> io::Result<i64> 
    {
        Ok(self.file.metadata()?.len() as i64)
    }

    pub fn close(&mut self) -> io::Result<()> {
        self.file.sync_all()?;
        Ok(())
    }
}

impl Drop for Writer {
    fn drop(&mut self) {
        let _ = self.close();
    }
}