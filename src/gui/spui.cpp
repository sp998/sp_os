#include <gui/spui.h>

SPCanvas::SPCanvas()
{
    setVgaMode(320,200,8);
}
void SPCanvas::SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b)
{
}
void SPCanvas::SetBackgroundColor(uint8_t colorIndex)
{
    this->backgroundColor = colorIndex;
    setBackground(this->backgroundColor);
}

void SPCanvas::SetFontType(uint8_t fontType)
{
    this->fontType = fontType;
}

void SPCanvas::SetTextColor(uint8_t r, uint8_t g, uint8_t b)
{
}

void SPCanvas::SetTextColor(uint8_t colorIndex)
{
    this->textColor = colorIndex;
}

void SPCanvas::DrawPixel(uint32_t x, uint32_t y)
{
    putPixelWithIndex(x,y,this->color);
}

void SPCanvas::SetColor(uint8_t colorIndex)
{
    this->color=colorIndex;
}

void SPCanvas::SetColor(uint8_t r, uint8_t g, uint8_t b)
{
}

void SPCanvas::DrawText(uint32_t x, uint32_t y, char *text)
{
    draw_string(x,y,text,this->textColor,this->fontType);
}
