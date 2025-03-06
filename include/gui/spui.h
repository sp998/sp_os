#ifndef SP_UI_H
#define SP_UI_H
#include <kernel/drivers/vga.h>
#include <gui/font.h>

class SPCanvas{
    private:
        uint8_t backgroundColor;
        uint8_t fontType;
        uint8_t textColor;
        uint8_t color; 

    public:
        SPCanvas();
        void SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
        void SetBackgroundColor(uint8_t colorIndex);
        void SetFontType(uint8_t fontType);
        void SetTextColor(uint8_t r,uint8_t g, uint8_t b);
        void SetTextColor(uint8_t colorIndex);
        void DrawPixel(uint32_t x, uint32_t y);
        void SetColor(uint8_t colorIndex);
        void SetColor(uint8_t r, uint8_t g, uint8_t b);
        void DrawText(uint32_t x, uint32_t y, char* text);
    
};







#endif