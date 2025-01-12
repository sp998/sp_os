#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H

#define MAX_SYSTEM_EVENTS 10
#define SCREEN_RESET_EVENT 0

#include <event.h>
#include <stdint.h>
#include <stdint.h>
#include <stddef.h> 


void init_system_events();

Event* get_event(uint32_t event_index);
#endif