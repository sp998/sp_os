#ifndef TYPES_H
#define TYPES_H

#define NULL (void*)0x0
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int int16_t;
typedef unsigned short int uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int int_64_t;
typedef unsigned long long int uint64_t;

typedef void (*call_module_t)(void);
typedef unsigned int size_t;
#endif