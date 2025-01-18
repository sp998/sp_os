#include <file.h>
#include <mem.h>
#include <utils.h>
#include <string.h>

File* root = NULL;

File* create_file(char* name) {
    File* file = (File*)malloc(sizeof(File));
    if (!file) {
        // Handle memory allocation failure
        return NULL;
    }

    // Temporary buffers to hold the split parts
    char name_part[10];
    char ext_part[3];

    // Split the name and extension
    split_name_ext(name, name_part, ext_part);

    // Allocate memory for name
    file->name = (char*)malloc(strlen(name_part) + 1);
    if (!file->name) {
        free(file);
        return NULL;
    }
    strcpy(name_part,file->name);
    // Allocate memory for extension only if it's not empty
    if (strlen(ext_part) > 0) {
        file->ext = (char*)malloc(strlen(ext_part) + 1);
        if (!file->ext) {
            free(file->name);
            free(file);
            return NULL;
        }
        strcpy(ext_part,file->ext);
    } else {
        file->ext = NULL; // Set ext to NULL if no extension exists
    }

    return file;
}



File* get_root(){
    return root;
}

void init_root(){
    root=create_file("/");
}
void add_file(File* rootFile,char* name){
   File* currentFile = create_file(name);

   File* current = rootFile;
   while (current->next!=NULL)
   {
    current=current->next;
   }
   current->next = currentFile;
   
}
void list_files(File* rootFile){
    File* current = rootFile;
    while (current!=NULL)
    {
        if(current->ext!=NULL){
        print(strcombine(strcombine(current->name,"."),current->ext));
        }else{
            print(current->name);
        }
        print("\n");
        current= current->next;
    }

}