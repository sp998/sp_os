#include <file.h>
#include <mem.h>
#include <utils.h>
#include <string.h>
#include <io.h>
#include <sysvars.h>

File* current_directory = NULL;
File* root=NULL;

uint32_t last_offset = 0;


File* create_directory(char* name){
    File* file = create_file(name);
    file->is_directory = true;
    return file;
}

File* create_file(char* name) {
    File* file = (File*)malloc(sizeof(File));
    file->level =get_current_directory()->level+1;
    file->parent = get_current_directory();
    if (!file) {
        // Handle memory allocation failure
        return NULL;
    }

    // Temporary buffers to hold the split parts
    char tokens[10][20];

    // Split the name and extension
    int token_count=split_string(name,'.',tokens,2);


    // Allocate memory for name
    file->name = (char*)malloc(strlen(tokens[0]) + 1);
    if (!file->name) {
        free(file);
        return NULL;
    }
    strcpy(tokens[0],file->name);
    // Allocate memory for extension only if it's not empty
    if (token_count > 1) {
    
        file->ext = (char*)malloc(strlen(tokens[1]) + 1);
        strcpy(tokens[1],file->ext);
    } else {
        file->ext = NULL; // Set ext to NULL if no extension exists
    }

    return file;
}



File* get_root(){
   return root;
    
}

void init_root(){
    root=create_directory("/");
    root->is_root = true;
    root->level = 0;
    current_directory=root;
}
void add_directory(File* rootFile,char* name){
    File* currentFile = create_directory(name);
   File* current = rootFile;
   while (current->next!=NULL)
   {
    current=current->next;
   }
   current->next = currentFile;
   
}
void add_file(File* rootFile,char* name){
   File* currentFile = create_file(name);
   currentFile->offset = last_offset;
   currentFile->size = 512;
   last_offset+=512;

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
       
        if(current->is_root || current->parent !=rootFile || (current->level != (rootFile->level+1))){
            current=current->next;
            continue;
        }
        
        uint8_t currentColor = WHITE;
        if(current->is_directory){
            currentColor=MAGENTA;
        }
        if(current->ext!=NULL){
        printc(strcombine(strcombine(current->name,"."),current->ext),currentColor);
        }else{
            printc(current->name,currentColor);
        }
        print("\n");
        current= current->next;
    }

}


void set_current_directory(File* new_root){
        current_directory->is_root=false;
        new_root->is_root=true;
        current_directory =new_root;
        char* prompt="";
        File* current = new_root;
        while(current->parent!=NULL){
          
            char* temp = strcombine("/",current->name);
            prompt = strcombine(temp,prompt);
            
            current= current->parent;
        }
        set_shell_prompt(strcombine(prompt," $p>"));
        
        
}

File* get_current_directory(){
    return current_directory;
}
File* find_file(File* rootFile,char* name){
    if(strcmp(name,"/")==0){
        return get_root();
    }
File* current = rootFile;
    while (current!=NULL)
    {
        if(current->is_root || current->parent !=rootFile || (current->level != (rootFile->level+1))){
            current=current->next;
            continue;
        }
        if(current->ext!=NULL){
        if(strcmp(strcombine(strcombine(current->name,"."),current->ext),name)==0){
            return current;
        }}else{
            if(strcmp(current->name,name)==0){
            return current;
        }
        }
        current= current->next;
    }
  return NULL;
}