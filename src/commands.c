#include <commands.h>
#include <io.h>



void clear_command(){
    clear_screen();
  
}

void ls_command(int argc,char* argv[]){
    print("work in progress\n");
}



Command* init_commands(){
   Command* commandHead=createCommand("clear",clear_command);
   register_command(commandHead,createCommand("ls",ls_command));
   return commandHead;
}