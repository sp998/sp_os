#include<fat32.h>
#include <utils.h>
#include <printf.h>
#include <string.h>


#define ROOT_DIRECTORY_SIZE (1 * 4096)  // 4 KB, 1 cluster

#define PARTITION_LBA_START 1
#define FAT_EOC 0x0FFFFFF8
uint32_t partition_start_lba;


uint32_t get_partition_start() {
    uint8_t* mbr = (uint8_t*)FAT_LOAD_ADDR;

    if (!ide_read_disk(0, mbr, 1)) {
        printf("Failed to read MBR.\n");
        return 0;
    }

    // Partition entry starts at 446 (0x1BE)
    uint32_t partition_start_lba = *(uint32_t*)(mbr + 446 + 8);

    printf("Partition Start LBA: %u\n", partition_start_lba);
    return partition_start_lba;
}

static uint32_t fat_start_sector = 0;
static uint16_t bytes_per_sector, sectors_per_cluster;
static uint32_t reserved_sectors, num_fats, fat_size, root_entry_count;

// call once after reading boot sector
void fat_init_from_bpb(FAT16_BootSector* bpb) {
    bytes_per_sector    = bpb->bytes_per_sector;
    sectors_per_cluster = bpb->sectors_per_cluster;
    reserved_sectors    = bpb->reserved_sector_count;
    num_fats            = bpb->num_fats;
    fat_size            = bpb->fat_size_16;
    root_entry_count    = bpb->root_entry_count;

    fat_start_sector = reserved_sectors;
}


void fs_init() {
    partition_start_lba = get_partition_start();
    if (partition_start_lba == 0) {
       // error…
    }
}



uint32_t get_first_cluster_lba(uint16_t cluster) {
    uint32_t root_dir_sectors = ((root_entry_count * 32) + (bytes_per_sector - 1)) / bytes_per_sector;
    uint32_t data_start = reserved_sectors + (num_fats * fat_size) + root_dir_sectors;
    return data_start + (cluster - 2) * sectors_per_cluster;
}


uint32_t get_fat_table_start() {
    uint8_t* sector_buffer = (uint8_t*)FAT_LOAD_ADDR;

    // Step 1: Read the boot sector to get required values
    if (!ide_read_disk(partition_start_lba, sector_buffer, 1)) {
        printf("Failed to read boot sector.\n");
        return 0;
    }

    FAT16_BootSector* bpb = (FAT16_BootSector*)sector_buffer;

    // Get the values from the boot sector
    uint32_t reserved = bpb->reserved_sector_count;
    uint32_t fats = bpb->num_fats;
    uint32_t fat_sz = bpb->fat_size_16;

    // Step 2: Calculate the FAT table start
    uint32_t fat_table_start = reserved + fats * fat_sz;
    return fat_table_start;
}
uint16_t fat_get_next_cluster(uint16_t current_cluster) {
    if (current_cluster < 2) {
        printf("Invalid current cluster %u\n", current_cluster);
        return 0xFFFF;
    }

    uint8_t* buf = (uint8_t*)FAT_LOAD_ADDR;
    uint32_t fat_offset = current_cluster * 2;              // 2 bytes per entry
    uint32_t sector_ofs = fat_offset / bytes_per_sector;
    uint32_t entry_ofs = fat_offset % bytes_per_sector;
    uint32_t on_disk_sector = partition_start_lba + fat_start_sector + sector_ofs;

    if (!ide_read_disk(on_disk_sector, buf, 1)) {
        printf("Failed to read FAT sector %u\n", on_disk_sector);
        return 0xFFFF;
    }

    uint16_t next = *(uint16_t*)(buf + entry_ofs);
    if (next >= 0xFFF8) return 0xFFFF;   // end-of-chain
    if (next == 0x0000 || next == 0xFFF7) {
        printf("Corrupt FAT entry: cluster %u → 0x%04X\n", current_cluster, next);
        return 0xFFFF;
    }
    return next;
}


void fat_read_file_content(uint16_t start_cluster,
    uint32_t file_size,
    uint8_t* out) {
uint32_t bytes_left = file_size;
uint16_t cl = start_cluster;
int clusters_read = 0;

while (bytes_left && cl != 0xFFFF) {
clusters_read++;
//printf("Reading cluster %d (clusters so far: %d)\n", cl, clusters_read);

uint32_t lba = partition_start_lba + get_first_cluster_lba(cl);
for (uint32_t s = 0; s < sectors_per_cluster && bytes_left; s++) {
if (!ide_read_disk(lba + s, (uint8_t*)FAT_LOAD_ADDR, 1)) {
printf("Sector read error at %u\n", lba + s);
return;
}
uint32_t chunk = bytes_left > bytes_per_sector ? bytes_per_sector : bytes_left;
memcpy(out, (uint8_t*)FAT_LOAD_ADDR, chunk);
out += chunk;
bytes_left -= chunk;
}

cl = fat_get_next_cluster(cl);
}

//printf("Done. clusters_read=%d, bytes_left=%u\n", clusters_read, bytes_left);
}



void fat_list_files() {
    uint8_t* sector_buffer = (uint8_t*)FAT_LOAD_ADDR;


    // Step 1: Read boot sector
    if (!ide_read_disk(partition_start_lba, sector_buffer, 1)) {
        printf("Failed to read boot sector.\n");
        return;
    }

    FAT16_BootSector* bpb = (FAT16_BootSector*)sector_buffer;
    fat_init_from_bpb(bpb);

    uint32_t bytes_per_sector = bpb->bytes_per_sector;
    uint32_t sectors_per_cluster = bpb->sectors_per_cluster;
    uint32_t reserved = bpb->reserved_sector_count;
    uint32_t fats = bpb->num_fats;
    uint32_t fat_sz = bpb->fat_size_16;   // FAT16 => must use fat_size_16
    uint32_t root_entry_count = bpb->root_entry_count;

    // printf("bytes per sector: %u\n", bytes_per_sector);
    // printf("sectors per cluster: %u\n", sectors_per_cluster);
    // printf("number of FATs: %u\n", fats);
    // printf("root entries: %u\n", root_entry_count);


    // Step 2: Calculate root directory start sector
    uint32_t root_dir_sectors = ((root_entry_count * 32) + (bytes_per_sector - 1)) / bytes_per_sector;
    uint32_t first_root_dir_sector = reserved + (fats * fat_sz);

    // printf("Root directory starts at sector: %u (LBA: %u)\n", first_root_dir_sector, partition_start_lba + first_root_dir_sector);

    // Step 3: Read and list valid entries
    uint32_t total_entries = root_entry_count;
    uint32_t sectors_to_read = root_dir_sectors;

    for (uint32_t sec = 0; sec < sectors_to_read; sec++) {
        if (!ide_read_disk(partition_start_lba + first_root_dir_sector + sec, sector_buffer, 1)) {
            printf("Failed to read root directory sector %u\n", first_root_dir_sector + sec);
            return;
        }

        FAT16_DirEntry* entries = (FAT16_DirEntry*)sector_buffer;
        uint32_t n_entries = bytes_per_sector / sizeof(FAT16_DirEntry);

        for (uint32_t i = 0; i < n_entries && total_entries > 0; i++) {
            FAT16_DirEntry* e = &entries[i];
            total_entries--;

            // Skip invalid entries
            if (e->name[0] == 0x00) return;   // No more files
            if (e->name[0] == 0xE5) continue; // Deleted file
            if (e->attributes == 0x0F) continue; // Long filename entry

            // Prepare the filename
            char name[13];
            memset(name, 0, sizeof(name));
            memcpy(name, e->name, 8);
            memcpy(name + 8, ".", 1);
            memcpy(name + 9, e->ext, 3);

            // Trim trailing spaces
            for (int j = 7; j >= 0 && name[j] == ' '; j--) name[j] = '\0';
            for (int j = 11; j >= 9 && name[j] == ' '; j--) name[j] = '\0';

            // Skip files starting with '_'
            if (name[0] == '_') continue;

            uint16_t first_cluster = e->first_cluster_lo; // FAT16 uses 16-bit clusters

            // Only print files with a non-zero size
            if (e->file_size > 0) {
                printf("%s\n", name);
            }
        }
    }
}


FAT16_DirEntry* fat_find_file(const char* target_filename) {
    uint8_t* sector_buffer = (uint8_t*)FAT_LOAD_ADDR;

    if (!ide_read_disk(partition_start_lba, sector_buffer, 1)) {
        printf("Failed to read boot sector.\n");
        return NULL;
    }

    FAT16_BootSector* bpb = (FAT16_BootSector*)sector_buffer;
    fat_init_from_bpb(bpb);

    uint32_t bytes_per_sector = bpb->bytes_per_sector;
    uint32_t sectors_per_cluster = bpb->sectors_per_cluster;
    uint32_t reserved = bpb->reserved_sector_count;
    uint32_t fats = bpb->num_fats;
    uint32_t fat_sz = bpb->fat_size_16;
    uint32_t root_entry_count = bpb->root_entry_count;

    uint32_t root_dir_sectors = ((root_entry_count * 32) + (bytes_per_sector - 1)) / bytes_per_sector;
    uint32_t first_root_dir_sector = reserved + (fats * fat_sz);

    uint32_t total_entries = root_entry_count;
    uint32_t sectors_to_read = root_dir_sectors;

    for (uint32_t sec = 0; sec < sectors_to_read; sec++) {
        if (!ide_read_disk(partition_start_lba + first_root_dir_sector + sec, sector_buffer, 1)) {
            printf("Failed to read root directory sector %u\n", first_root_dir_sector + sec);
            return NULL;
        }

        FAT16_DirEntry* entries = (FAT16_DirEntry*)sector_buffer;
        uint32_t n_entries = bytes_per_sector / sizeof(FAT16_DirEntry);

        for (uint32_t i = 0; i < n_entries && total_entries > 0; i++) {
            FAT16_DirEntry* e = &entries[i];
            total_entries--;

            if (e->name[0] == 0x00) return NULL;
            if (e->name[0] == 0xE5) continue;
            if (e->attributes == 0x0F) continue;

            char name[13];
            memset(name, 0, sizeof(name));
            memcpy(name, e->name, 8);
            memcpy(name + 8, ".", 1);
            memcpy(name + 9, e->ext, 3);

            for (int j = 7; j >= 0 && name[j] == ' '; j--) name[j] = '\0';
            for (int j = 11; j >= 9 && name[j] == ' '; j--) name[j] = '\0';

            if (name[0] == '_') continue;

            if (strcmpi(name, target_filename)==0) {
            
                return e;
            }
        }
    }

    return NULL;
}
