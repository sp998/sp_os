#include <system_events.h>


Event* events[MAX_SYSTEM_EVENTS] = {NULL}; 

void init_system_events() {
    Event* reset_screen_event = createEvent();

    events[0] = reset_screen_event; 


}

Event* get_event(uint32_t event_index) {
    if (event_index < MAX_SYSTEM_EVENTS) {
        return events[event_index]; 
    }
    return NULL; 
}
