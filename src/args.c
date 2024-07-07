#include <args.h>

static char* args[MAX_ARGS];       // Static array to hold the pointers to arguments
static char string_buffers[MAX_ARGS][STRING_BUFFER_SIZE];  // Static buffers for each argument

char** get_args(char* input, size_t length) {
    size_t argsIndex = 0;
    size_t buffIndex = 0;

    for(size_t i = 0; i < length; i++) {
        char c = input[i];
        if (c == ' ' || c == '\0') {
            if (buffIndex > 0) {  // Only if there's something to copy
                string_buffers[argsIndex][buffIndex] = '\0';  // Null-terminate the current argument
                // Point args to the current string buffer
                args[argsIndex] = string_buffers[argsIndex];
                argsIndex++;
                buffIndex = 0;
                if (c == '\0' || argsIndex >= MAX_ARGS) {
                    break;  // If we reached the end of the input or max args, break the loop
                }
            }
        } else {
            if (buffIndex < STRING_BUFFER_SIZE - 1) {  // Check to avoid buffer overflow
                string_buffers[argsIndex][buffIndex++] = c;  // Add the character to the buffer
            }
        }
    }

    if (buffIndex > 0 && argsIndex < MAX_ARGS) {  // Handle the last argument if the string didn't end with a space
        string_buffers[argsIndex][buffIndex] = '\0';
        args[argsIndex] = string_buffers[argsIndex];
        argsIndex++;
    }

    // Null-terminate the args array
    args[argsIndex] = NULL;

    return args;
}

void clear_args() {
    for (size_t i = 0; i < MAX_ARGS; i++) {
        args[i] = NULL;  // Reset pointers to NULL
        // Optionally clear the buffers if necessary
        // memset(string_buffers[i], 0, BUFFER_SIZE);  // Uncomment this line to clear the buffers
    }
}
