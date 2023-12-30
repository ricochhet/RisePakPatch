#[derive(Clone)]
pub struct FileEntry {
    pub file_name: String,
    pub file_name_lower: u32,
    pub file_name_upper: u32,
    pub offset: u64,
    pub uncomp_size: u64,
}

impl Default for FileEntry {
    fn default() -> Self {
        FileEntry {
            file_name: (&"").to_string(),
            file_name_lower: 0,
            file_name_upper: 0,
            offset: 0,
            uncomp_size: 0,
        }
    }
}