#include<stdint.h>
#include <printf.h>
#include <ide.h>
#define ROOT_DIRECTORY_SIZE (1 * 4096)  // Assuming 1 cluster for the root directory (4 KB)
#define SECTOR_SIZE 512
#define FAT_LOAD_ADDR 0x500000
typedef struct {
    uint8_t  jmp[3];                  // Jump instruction (for booting)
    uint8_t  oem[8];                  // OEM string
    uint16_t bytes_per_sector;        // Bytes per sector (typically 512)
    uint8_t  sectors_per_cluster;     // Sectors per cluster
    uint16_t reserved_sector_count;   // Reserved sectors before FAT table
    uint8_t  num_fats;                // Number of FAT tables (usually 2)
    uint16_t root_entry_count;        // Number of root directory entries
    uint16_t total_sectors_16;        // Total sectors if < 32 bits
    uint8_t  media_descriptor;        // Media descriptor byte
    uint16_t fat_size_16;             // Size of one FAT table in sectors
    uint16_t sectors_per_track;       // Sectors per track
    uint16_t num_heads;               // Number of heads (tracks per cylinder)
    uint32_t hidden_sectors;          // Hidden sectors (for partitions)
    uint32_t total_sectors_32;        // Total sectors (if > 32-bit)
    uint32_t fat_size_32;             // Size of one FAT in 32-bit (sectors)
    uint16_t ext_flags;               // Extended flags
    uint16_t fs_version;              // File system version
    uint32_t root_cluster;            // Starting cluster of root directory
    uint16_t fs_info;                 // Sector number of FS info structure
    uint16_t backup_boot_sector;      // Sector number of backup boot sector
    uint8_t reserved[12];             // Reserved space
    uint8_t drive_number;             // Drive number (for DOS)
    uint8_t reserved2;                // Reserved
    uint8_t boot_signature;           // Boot signature (0x29)
    uint32_t volume_id;               // Volume ID
    uint8_t volume_label[11];         // Volume label (e.g., "MYDISK")
    uint8_t fs_type[8];               // File system type (e.g., "FAT32")
} __attribute__((packed)) FAT32_BootSector;



typedef struct {
    uint8_t name[8];          // Name of the file (8.3 format)
    uint8_t ext[3];           // Extension (3 chars)
    uint8_t attributes;       // File attributes (0x10 for directories, 0x20 for files)
    uint8_t reserved;         // Reserved
    uint8_t creation_time_t;  // Creation time (tenths of second)
    uint16_t creation_time;   // Creation time
    uint16_t creation_date;   // Creation date
    uint16_t last_access_date; // Last access date
    uint16_t first_cluster_hi; // High 16 bits of first cluster
    uint16_t last_write_time; // Last write time
    uint16_t last_write_date; // Last write date
    uint16_t first_cluster_lo; // Low 16 bits of first cluster
    uint32_t file_size;       // File size in bytes
} __attribute__((packed)) FAT32_DirEntry;



typedef struct {
    uint8_t status;
    uint8_t chs_first[3];
    uint8_t partition_type;
    uint8_t chs_last[3];
    uint32_t lba_first_sector; // <-- you need this
    uint32_t total_sectors;
} __attribute__((packed)) MBRPartitionEntry;



// --- Boot Sector ---
typedef struct __attribute__((packed)) FAT16_BootSector {
    uint8_t  jump_boot[3];
    uint8_t  oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t  num_fats;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t  media;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    uint8_t  volume_label[11];
    uint8_t  file_system_type[8];
} FAT16_BootSector;

// --- Directory Entry ---
typedef struct __attribute__((packed)) FAT16_DirEntry {
    uint8_t  name[8];
    uint8_t  ext[3];
    uint8_t  attributes;
    uint8_t  reserved;
    uint8_t  creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_hi;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_lo;
    uint32_t file_size;
} FAT16_DirEntry;


uint16_t fat_get_next_cluster(uint16_t current_cluster);
void fat_read_file_content(uint16_t start_cluster, uint32_t file_size, uint8_t* buffer);
void fat_list_files();
void fs_init();
FAT16_DirEntry* fat_find_file(const char* target_filename);