#include <memdisk.h>

char ram_disk[RAM_DISK_SIZE];
void init_mem_disk(){
    memset(ram_disk,0,RAM_DISK_SIZE);
}

void wirte_to_mem_disk(int offset,const char* data,int length){
    if(offset+length>RAM_DISK_SIZE){
        return;
    }
    memcpy(ram_disk+offset,data,length);
}

void read_from_mem_disk(int offset,char* buffer, int length){
    if(offset+length>RAM_DISK_SIZE){
        return;
    }
    memcpy(buffer,ram_disk+offset,length);
}