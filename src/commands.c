#include <commands.h>
#include <io.h>
#include <file.h>
#include <sysvars.h>
#include <utils.h>
#include <memdisk.h>
#include <fat32.h>
#include <syslib.h>
#include <gui/init.h>
#include <gdt.h>



void clear_command(){
    clear_screen();
  
}


void gui_command(){
    start_process(main, (uint32_t)get_user_stack() + USER_STACK_SIZE);
    
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

void file_exists(int argc,char* argv[]){
    if(argc<2){
        print("file name is required.");
        print("\n");
        return;
    }
    File* file = find_file(get_root(),argv[1]);

    if(file==NULL){
        print("file not found\n");
    }else{
        print("file found\n");
    }

}

void ls_command(int argc,char* argv[]){
    fat_list_files();
}

void write(int argc, char* argv[]) {
    if (argc < 2) {
        print("file name is required.\n");
        return;
    }
    
    if (argc < 3) {
        print("content to write is required.\n");
        return;
    }

    // argv[1] is the filename (not used in RAM disk directly)
    // Combine all arguments after filename as content
     File* file = find_file(get_current_directory(),argv[1]);
    if(file==NULL){
        print("file not found\n");
        return;
    }
    int current_offset = file->offset;

    int total_length = 0;
    for (int i = 2; i < argc; i++) {
        total_length += strlen(argv[i]);
        if (i < argc - 1) total_length++; // Add space between arguments
    }

    if (current_offset + total_length > RAM_DISK_SIZE) {
        print("Error: Not enough space in RAM disk\n");
        return;
    }

    // Write each argument to RAM disk
    for (int i = 2; i < argc; i++) {
        int arg_length = strlen(argv[i]);
        write_to_mem_disk(current_offset, argv[i], arg_length);
        current_offset += arg_length;
        
        // Add space between arguments (except after last one)
        if (i < argc - 1) {
            write_to_mem_disk(current_offset, " ", 1);
            current_offset++;
        }
    }

    print("write successful\n");
}

void read(int argc, char* argv[]) {
    if (argc < 2) {
        print("file name is required.\n");
        return;
    }

    File* file = find_file(get_current_directory(),argv[1]);
    if(file==NULL){
        print("file not found\n");
        return;
    }
    int current_offset = file->offset;
    int length = file->size;
    char* buffer = (char*)malloc(length + 1);
    read_from_mem_disk(current_offset, buffer, length);
    buffer[length] = '\0';
    print(buffer);
    print("\n");
    free(buffer);
}



Command* init_commands(){
   Command* commandHead=createCommand("clear",clear_command);
   register_command(commandHead,createCommand("ls",ls_command));
   register_command(commandHead,createCommand("createfile",touch));
   register_command(commandHead,createCommand("cd",cd_command));
   register_command(commandHead,createCommand("mkdir",mkdir_command));
   register_command(commandHead,createCommand("chfile",file_exists));
   register_command(commandHead,createCommand("write",write));
   register_command(commandHead,createCommand("read",read));
   register_command(commandHead,createCommand("spui",gui_command));

   return commandHead;
}