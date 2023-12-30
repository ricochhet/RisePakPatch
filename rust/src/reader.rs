use std::fs::File;
use std::io::{self, Read, Seek, SeekFrom};

pub struct Reader {
    file: File,
    position: i64,
}

impl Reader {
    pub fn new(file_path: &str) -> io::Result<Reader> {
        let file = File::open(file_path)?;
        let position = 0;
        Ok(Reader { file, position })
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

    pub fn read_char(&mut self) -> io::Result<Option<char>> {
        let mut buf = [0; 1];
        match self.file.read_exact(&mut buf) {
            Ok(_) => {
                self.position += 1;
                Ok(Some(buf[0] as char))
            }
            Err(ref e) if e.kind() == io::ErrorKind::UnexpectedEof => Ok(None),
            Err(e) => Err(e),
        }
    }

    pub fn read_uint32(&mut self) -> io::Result<u32> {
        let mut buf = [0; 4];
        self.file.read_exact(&mut buf)?;
        self.position += 4;
        Ok(u32::from_le_bytes(buf))
    }

    pub fn read_uint64(&mut self) -> io::Result<u64> {
        let mut buf = [0; 8];
        self.file.read_exact(&mut buf)?;
        self.position += 8;
        Ok(u64::from_le_bytes(buf))
    }

    pub fn read(&mut self, buffer: &mut [u8]) -> io::Result<usize> {
        let bytes_read = self.file.read(buffer)?;
        self.position += bytes_read as i64;
        Ok(bytes_read)
    }

    pub fn position(&mut self) -> io::Result<i64> 
    {
        Ok(self.position as i64)
    }
    
    pub fn size(&self) -> io::Result<i64> 
    {
        Ok(self.file.metadata()?.len() as i64)
    }

    pub fn skip(&mut self, count: i64) -> io::Result<()> {
        self.seek(self.position + count)?;
        Ok(())
    }

    pub fn is_valid(&self) -> bool {
        self.position <= self.file.metadata().map_or(0, |m| m.len() as i64)
    }
}