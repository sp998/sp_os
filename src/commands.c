#include <commands.h>
#include <io.h>
#include <file.h>
#include <sysvars.h>
#include <utils.h>
#include <memdisk.h>



void clear_command(){
    clear_screen();
  
}

void touch(){

}

void ls_command(int argc,char* argv[]){
    list_files(get_root());
}

void write(){
wirte_to_mem_disk(0,"hello",strlen("hello"));
wirte_to_mem_disk(1,"hi",strlen("hi"));
}

void read(){
    char buffer[10];
    read_from_mem_disk(0,buffer,10);
    print(strcombine(buffer,"\n"));

    read_from_mem_disk(1,buffer,10);
    print(strcombine(buffer,"\n"));
}



Command* init_commands(){
   Command* commandHead=createCommand("clear",clear_command);
   register_command(commandHead,createCommand("ls",ls_command));
   register_command(commandHead,createCommand("write",write));
   register_command(commandHead,createCommand("read",read));
   return commandHead;
}