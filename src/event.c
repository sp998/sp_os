#include<event.h>
#include <mem.h>
#include <stddef.h>

Event* createEvent(){
    Event* event = (Event*) malloc(sizeof(Event));
    return event;
}
void subscribe_to_event(Event* event,EventHandler* handler){
    Event* current = event;
    while (current->next !=NULL)
    {
        current=current->next;
    }
    current->handler=handler;
}


void dispatch_event(Event* event){
    Event* current = event;
    while(current!=NULL){
        if(current->handler!=NULL){
            current->handler();
        }
        current=current->next;
    }
}


