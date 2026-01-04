use crate::println;
use alloc::string::String;
use alloc::vec::Vec;
use core::slice; // Import println macro

extern "C" {
    fn ide_read_disk(sector: u32, data: *mut u8, sector_count: u32) -> bool;
}

// --------------------------------------------------------
// Data Structures (Packed to match on-disk format)
// --------------------------------------------------------

#[repr(C, packed)]
struct PartitionEntry {
    status: u8,
    chs_first: [u8; 3],
    type_code: u8,
    chs_last: [u8; 3],
    lba_first: u32,
    sector_count: u32,
}

#[repr(C, packed)]
struct BPB {
    jmp: [u8; 3],
    oem: [u8; 8],
    bytes_per_sector: u16,
    sectors_per_cluster: u8,
    reserved_sectors: u16,
    num_fats: u8,
    root_entries: u16,
    total_sectors_16: u16,
    media: u8,
    fat_size_16: u16,
    sectors_per_track: u16,
    heads: u16,
    hidden_sectors: u32,
    total_sectors_32: u32,
    drive_num: u8,
    reserved: u8,
    boot_sig: u8,
    vol_id: u32,
    vol_label: [u8; 11],
    fs_type: [u8; 8],
}

#[repr(C, packed)]
#[derive(Clone, Copy)]
pub struct DirEntry {
    pub name: [u8; 8],
    pub ext: [u8; 3],
    pub attr: u8,
    pub reserved: u8,
    pub create_time_tenth: u8,
    pub create_time: u16,
    pub create_date: u16,
    pub access_date: u16,
    pub first_cluster_hi: u16,
    pub write_time: u16,
    pub write_date: u16,
    pub first_cluster_lo: u16,
    pub size: u32,
}

// --------------------------------------------------------
// FileSystem State
// --------------------------------------------------------

static mut PARTITION_LBA: u32 = 0;
static mut BYTES_PER_SECTOR: u16 = 512;
static mut SECTORS_PER_CLUSTER: u8 = 1;
static mut RESERVED_SECTORS: u16 = 1;
static mut NUM_FATS: u8 = 2;
static mut FAT_SIZE: u32 = 1; // Sectors per FAT
static mut ROOT_ENTRIES: u16 = 512;
static mut ROOT_DIR_SECTOR: u32 = 0;
static mut DATA_AREA_SECTOR: u32 = 0;

pub fn init() -> bool {
    unsafe {
        // 1. Read MBR
        let mut mbr = [0u8; 512];
        if !ide_read_disk(0, mbr.as_mut_ptr(), 1) {
            println!("FS: Failed to read MBR");
            return false;
        }

        // Check Partition 1 (Offset 446)
        let part1_ptr = mbr.as_ptr().add(446) as *const PartitionEntry;
        let part1 = &*part1_ptr;

        PARTITION_LBA = part1.lba_first;
        // println!("FS: Partition LBA: {}", PARTITION_LBA);

        // 2. Read Boot Sector
        let mut bpb_buf = [0u8; 512];
        if !ide_read_disk(PARTITION_LBA, bpb_buf.as_mut_ptr(), 1) {
            println!("FS: Failed to read BPB");
            return false;
        }

        let bpb = &*(bpb_buf.as_ptr() as *const BPB);
        BYTES_PER_SECTOR = bpb.bytes_per_sector;
        SECTORS_PER_CLUSTER = bpb.sectors_per_cluster;
        RESERVED_SECTORS = bpb.reserved_sectors;
        NUM_FATS = bpb.num_fats;

        FAT_SIZE = if bpb.fat_size_16 != 0 {
            bpb.fat_size_16 as u32
        } else {
            // TODO: FAT32 support if needed, but FAT16 usually
            0
        };

        ROOT_ENTRIES = bpb.root_entries;

        // Calc locations
        let fat_start = PARTITION_LBA + RESERVED_SECTORS as u32;
        let root_dir_size_sectors =
            ((ROOT_ENTRIES as u32 * 32) + (BYTES_PER_SECTOR as u32 - 1)) / BYTES_PER_SECTOR as u32;

        ROOT_DIR_SECTOR = fat_start + (NUM_FATS as u32 * FAT_SIZE);
        DATA_AREA_SECTOR = ROOT_DIR_SECTOR + root_dir_size_sectors;

        // println!("FS: Root Dir Sector: {}", ROOT_DIR_SECTOR);
        // println!("FS: Data Area Sector: {}", DATA_AREA_SECTOR);

        true
    }
}

// Read a cluster chain
// For simplicity in this step, we'll assume contiguous files or small directory scans first
// Since full FAT traversal is complex code, let's implement basic directory reading first.

fn read_sector(lba: u32, buf: &mut [u8]) -> bool {
    if buf.len() < 512 {
        return false;
    }
    unsafe { ide_read_disk(lba, buf.as_mut_ptr(), 1) }
}

// Convert filename (8.3) to "NAME.EXT" or "NAME"
fn entry_name(entry: &DirEntry) -> String {
    let mut name = String::new();
    // Name
    for &b in entry.name.iter() {
        if b != 0x20 {
            name.push(b as char);
        }
    }
    // Ext
    let mut has_ext = false;
    for &b in entry.ext.iter() {
        if b != 0x20 {
            has_ext = true;
            break;
        }
    }
    if has_ext {
        name.push('.');
        for &b in entry.ext.iter() {
            if b != 0x20 {
                name.push(b as char);
            }
        }
    }
    name
}

// List all filenames in a directory
pub fn enumerate_dir(path: &str) -> Vec<String> {
    let mut files = Vec::new();

    unsafe {
        if path == "/" {
            let sectors = ((ROOT_ENTRIES as u32 * 32) + 511) / 512;
            let mut buf = [0u8; 512];
            for i in 0..sectors {
                if !read_sector(ROOT_DIR_SECTOR + i, &mut buf) {
                    break;
                }
                let entries_per_sector = 512 / 32;
                let entries =
                    slice::from_raw_parts(buf.as_ptr() as *const DirEntry, entries_per_sector);
                for e in entries {
                    if e.name[0] == 0 {
                        return files;
                    }
                    if e.name[0] == 0xE5 {
                        continue;
                    }
                    if e.attr == 0x0F {
                        continue;
                    }

                    let name = entry_name(e);
                    // Filter out unwanted entries
                    if name == "." || name == ".." || name.starts_with('_') {
                        continue;
                    }

                    files.push(name);
                }
            }
        } else if let Some(e) = find_by_path(path) {
            if (e.attr & 0x10) != 0 {
                let cluster = e.first_cluster_lo;
                if cluster >= 2 {
                    let lba = cluster_lba(cluster);
                    let mut buf = [0u8; 512];
                    for s in 0..SECTORS_PER_CLUSTER {
                        if !read_sector(lba + s as u32, &mut buf) {
                            break;
                        }
                        let entries_per_sector = 512 / 32;
                        let entries = slice::from_raw_parts(
                            buf.as_ptr() as *const DirEntry,
                            entries_per_sector,
                        );
                        for e in entries {
                            if e.name[0] == 0 {
                                return files;
                            }
                            if e.name[0] == 0xE5 {
                                continue;
                            }
                            if e.attr == 0x0F {
                                continue;
                            }

                            let name = entry_name(e);
                            // Filter out unwanted entries
                            if name == "."
                                || name == ".."
                                || name.starts_with('_')
                                || name.contains("_PROG")
                            {
                                continue;
                            }

                            files.push(name);
                        }
                    }
                }
            }
        }
    }
    files
}

// Locate a file/dir entry in the Root Directory
pub fn find_in_root(target: &str) -> Option<DirEntry> {
    unsafe {
        let sectors = ((ROOT_ENTRIES as u32 * 32) + 511) / 512;
        let mut buf = [0u8; 512];

        for i in 0..sectors {
            if !read_sector(ROOT_DIR_SECTOR + i, &mut buf) {
                return None;
            }

            let entries_per_sector = 512 / 32;
            let entries =
                slice::from_raw_parts(buf.as_ptr() as *const DirEntry, entries_per_sector);

            for e in entries {
                if e.name[0] == 0 {
                    return None;
                } // End of dir
                if e.name[0] == 0xE5 {
                    continue;
                } // Deleted
                if e.attr == 0x0F {
                    continue;
                } // LFN

                let name = entry_name(e);
                if name.eq_ignore_ascii_case(target) {
                    return Some(*e);
                }
            }
        }
    }
    None
}

// Helper to convert Cluster -> LBA
fn cluster_lba(cluster: u16) -> u32 {
    unsafe { DATA_AREA_SECTOR + ((cluster as u32 - 2) * SECTORS_PER_CLUSTER as u32) }
}

// Find a file inside a subdirectory (given by first_cluster)
pub fn find_in_cluster(target: &str, cluster: u16) -> Option<DirEntry> {
    if cluster < 2 {
        return None;
    }

    // We only scan the FIRST cluster of the directory for now (simplicity)
    // A proper driver strictly follows the FAT chain.

    unsafe {
        let lba = cluster_lba(cluster);
        // We'll read SECTORS_PER_CLUSTER sectors
        let mut buf = [0u8; 512];

        for s in 0..SECTORS_PER_CLUSTER {
            if !read_sector(lba + s as u32, &mut buf) {
                return None;
            }

            let entries_per_sector = 512 / 32;
            let entries =
                slice::from_raw_parts(buf.as_ptr() as *const DirEntry, entries_per_sector);

            for e in entries {
                if e.name[0] == 0 {
                    return None;
                }
                if e.name[0] == 0xE5 {
                    continue;
                }
                if e.attr == 0x0F {
                    continue;
                }

                let name = entry_name(e);
                if name.eq_ignore_ascii_case(target) {
                    return Some(*e);
                }
            }
        }
    }
    None
}

// Read file content
pub fn read_file(entry: &DirEntry) -> Vec<u8> {
    // Reads only contiguous files or up to first few clusters if not implementing full FAT logic yet
    // For ISO based simple FAT, files are often contiguous.
    // FULL FAT SUPPORT: Need to read FAT table.
    // Let's implement basic cluster chaining

    let mut data = Vec::new();
    let mut current_cluster = entry.first_cluster_lo;
    let mut bytes_left = entry.size as usize;
    // println!("FS: Reading file size: {} bytes, Start Cluster: {}", bytes_left, current_cluster);

    unsafe {
        // Find FAT table start
        let fat_start_sector = PARTITION_LBA + RESERVED_SECTORS as u32;

        // Allocate buffer on heap to avoid stack overflow in ISR
        let mut buf_vec = Vec::with_capacity(512);
        buf_vec.resize(512, 0);

        while bytes_left > 0 && current_cluster >= 2 && current_cluster < 0xFFF8 {
            // Read Cluster Data
            let lba = cluster_lba(current_cluster);

            // println!("FS: Reading cluster {} LBA {}", current_cluster, lba);

            for s in 0..SECTORS_PER_CLUSTER {
                if bytes_left == 0 {
                    break;
                }

                if !read_sector(lba + s as u32, &mut buf_vec) {
                    println!("FS: Failed to read sector");
                    break;
                }

                let chunk = if bytes_left > 512 { 512 } else { bytes_left };
                data.extend_from_slice(&buf_vec[0..chunk]);
                bytes_left -= chunk;
            }

            // Look up next cluster in FAT
            // FAT16: 2 bytes per entry
            let fat_offset = (current_cluster as u32) * 2;
            let fat_sector_offset = fat_offset / 512;
            let fat_ent_offset = (fat_offset % 512) as usize;

            if !read_sector(fat_start_sector + fat_sector_offset, &mut buf_vec) {
                println!("FS: Failed to read FAT sector");
                break;
            }

            let next_cluster_ptr = buf_vec.as_ptr().add(fat_ent_offset) as *const u16;
            current_cluster = *next_cluster_ptr;
            // println!("FS: Next cluster: {}", current_cluster);
        }
    }
    data
}

// Find entry by Full Path e.g., "/sp/programs/shell"
pub fn find_by_path(path: &str) -> Option<DirEntry> {
    // 1. Split path
    let parts: Vec<&str> = path.split('/').filter(|s| !s.is_empty()).collect();
    if parts.is_empty() {
        return None;
    }

    // 2. Start at Root
    let mut current_entry = find_in_root(parts[0])?;

    // 3. Traverse
    for i in 1..parts.len() {
        if (current_entry.attr & 0x10) == 0 {
            // Not a directory
            return None;
        }

        let cluster = current_entry.first_cluster_lo;
        current_entry = find_in_cluster(parts[i], cluster)?;
    }

    Some(current_entry)
}

// --------------------------------------------------------
// C Interface (FFI)
// --------------------------------------------------------

#[no_mangle]
pub extern "C" fn rust_fs_find_path(path: *const u8) -> bool {
    // Convert C string to Rust &str
    let mut len = 0;
    unsafe {
        while *path.add(len) != 0 {
            len += 1;
        }
    }
    let path_slice = unsafe { slice::from_raw_parts(path, len) };
    let path_str = match core::str::from_utf8(path_slice) {
        Ok(s) => s,
        Err(_) => return false,
    };

    find_by_path(path_str).is_some()
}

#[no_mangle]
pub extern "C" fn rust_fs_load_file(path: *const u8, buffer: *mut u8) -> u32 {
    let mut len = 0;
    unsafe {
        while *path.add(len) != 0 {
            len += 1;
        }
    }
    let path_slice = unsafe { slice::from_raw_parts(path, len) };
    let path_str = match core::str::from_utf8(path_slice) {
        Ok(s) => s,
        Err(_) => return 0,
    };

    if let Some(entry) = find_by_path(path_str) {
        let data = read_file(&entry);
        // Copy to buffer
        unsafe {
            for (i, b) in data.iter().enumerate() {
                *buffer.add(i) = *b;
            }
        }
        return data.len() as u32;
    }
    0
}
