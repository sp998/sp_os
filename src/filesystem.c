#include <filesystem.h>
#include <stdint.h>
#include <ide.h>
#include <io.h>

uint32_t sector_index=2;




void list_all(){
  
     uint32_t last_index=1;
     ide_read_int(1,&last_index);

     if(last_index==0){
     
        print("\n");
        return;
     }
     print("\n");
    sector_index=last_index;
    for(uint32_t i=2;i<sector_index;i++){
       
        char buffer[512];
        ide_read_disk(i,buffer,1);
        print(buffer);
        print("\n");

    }

}
void make_dir(char* input){
     
      ide_write_disk(sector_index,input,1);
      sector_index++;
    
      uint32_t last_index =sector_index;
      if(ide_write_int(1,last_index)){
      }
}