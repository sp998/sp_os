#ifndef COMMAND_H
#define COMMAND_H

typedef void (*CommandHandler)(int,char**);

typedef struct Command{
    char* name;
   CommandHandler commandHandler;
   struct Command* next;   
}Command;

Command* createCommand(char* name,CommandHandler handler);
void register_command(Command* commandHead,Command* command);
Command* get_command(char* name,Command* commandHead);
#endif