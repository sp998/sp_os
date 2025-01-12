#include<command.h>
#include<mem.h>
#include<string.h>

Command* createCommand(char* name,CommandHandler handler){
         Command* command =(Command*)malloc(sizeof(Command));
         command->name=name;
         command->commandHandler=handler;
        return command;
}

void register_command(Command* commandHead,Command* command){
       Command* current = commandHead;
       while (current->next!=NULL)
       {
        current=current->next;
       }
       current->next = command;   
}

Command* get_command(char* name,Command* commandHead){
    Command* current = commandHead;
    while (current!=NULL)
    {
       
        if(strcmp(current->name,name)==0)
            return current;
        
        current=current->next;
    }
    return NULL;
}