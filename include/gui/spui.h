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
        bool fillShape;

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
        void FillShape(bool value);
        void DrawRect(uint32_t x,uint32_t y, uint32_t w,uint32_t h);
        void RenderDisplay();
};

class SPWidget{
    private:
        uint32_t x;
        uint32_t y;
        uint32_t w;
        uint32_t h;
        bool grabbing;
        int x_offset = 0, y_offset = 0;
        uint8_t background;
    public:
        SPWidget(uint32_t x, uint32_t y,uint32_t w,uint32_t h);
        void SetBackgroundColor(uint8_t colorIndex);
        virtual void Render(SPCanvas *canvas);

};
#endif