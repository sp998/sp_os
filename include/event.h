#ifndef EVENT_H
#define EVENT_H

typedef void (*EventHandler)(void);
typedef struct Event
{
  EventHandler handler;
  struct Event *next;
}Event; 

Event* createEvent();
void subscribe_to_event(Event* event,EventHandler* handler);
void dispatch_event(Event* event);
#endif