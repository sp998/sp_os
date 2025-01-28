#include <commands.h>
#include <io.h>
#include <file.h>
#include <sysvars.h>
#include <utils.h>
#include <memdisk.h>



void clear_command(){
    clear_screen();
  
}

void mkdir_command(int argc, char* argv[]){
    if(argc<2){
        print("directory name is required.");
        print("\n");
        return;
    }
    add_directory(get_root(),argv[1]);
}

void cd_command(int argc,char* argv[]){
    if(argc<2){
        print("directory name is required.");
        print("\n");
        return;
    }
       
  
    File* new_root = find_file(get_current_directory(),argv[1]);
    if(!new_root->is_directory){
        print(argv[1]);
        print(": is not a directory\n");
        return;
    }
    if(new_root==NULL){
        print(argv[1]);
        print(":No such directory exist\n");
        return;
    }
    set_current_directory(new_root);

}

void touch(int argc,char* argv[]){
  if(argc<2){
    print("file name is required.");
    print("\n");
    return;
  }
   add_file(get_current_directory(),argv[1]);
}

void ls_command(int argc,char* argv[]){
    list_files(get_current_directory());
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
   register_command(commandHead,createCommand("touch",touch));
   register_command(commandHead,createCommand("cd",cd_command));
   register_command(commandHead,createCommand("mkdir",mkdir_command));

   return commandHead;
}