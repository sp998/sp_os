#include <syslib.h>


char user_stack[USER_STACK_SIZE];

char get_user_stack()
{
    return user_stack;
}
