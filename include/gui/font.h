#ifndef FONT_H
#define FONT_H
#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stddef.h>

#define FONT_8x8 0x1
#define FONT_4x7 0x2
#define FONT_5x8 0x3

unsigned char *getFont(uint8_t type, uint8_t *width, uint8_t *height);

#ifdef __cplusplus
}
#endif
#endif