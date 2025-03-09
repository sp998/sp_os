#include <gui/spui.h>



SPCanvas::SPCanvas()
{
    setVgaMode(320,200,8);
    this->fillShape=false;
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

void SPCanvas::FillShape(bool value)
{
    this->fillShape = value;
}

void SPCanvas::DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    if(this->fillShape){
        for(uint32_t i = x;i<x+w;i++){
            for(uint32_t j =y;j<y+h;j++){
                DrawPixel(i,j);
            }
        }
    }else{
        uint32_t x0,y0;
        x0=x;
        y0=y;

        while(x0<w){
            DrawPixel(x0,y0);
            x0++;
        }
        x0=x;

        while (y0<h)
        {
           DrawPixel(x0,y0);
           y0++;
        }

        while(x0<w){
            DrawPixel(x0,y0);
            x0++;
        }

        while(y0>y){
            DrawPixel(x0,y0);
            y0--;
        }
    
    }
}

void SPCanvas::RenderDisplay()
{
    updateDisplay();
}
