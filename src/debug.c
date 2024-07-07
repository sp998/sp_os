#include <io.h>

void disk_successful(){
    print("Disk initialized successful");
    show_buffer();
}

void disk_not_init(){
     print("Disk not init\n");
    show_buffer();
}