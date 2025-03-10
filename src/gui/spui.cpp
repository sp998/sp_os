#include <gui/spui.h>
#include <kernel/drivers/mouse.h>



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



SPWidget::SPWidget(uint32_t x, uint32_t y,uint32_t w, uint32_t h)
{
    this->x =x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->background=0xf;
    this->onClick = [](SPWidget* widget){};
}
void SPWidget::SetBackgroundColor(uint8_t colorIndex)
{
    this->background=colorIndex;
}
void SPWidget::SetOnClick(void (*onClick)(SPWidget* widget))
{
  this->onClick = onClick;
}
void SPWidget::Render(SPCanvas *canvas)
{       
    int mouse_x ,mouse_y;
        mouse_x= getMouseX();
        mouse_y = getMouseY();
        
        if (getLeftButtonPress()) {
            
             if (!grabbing) {
                 if (mouse_x >= x && mouse_x <= x + w &&
                     mouse_y >= y && mouse_y <= y + h) {
                     grabbing = true;
                     x_offset = mouse_x - x;  // Capture the offset within the window
                     y_offset = mouse_y - y;
                 }
             }
     
             if (grabbing) {
                 this->onClick(this);
                 x =mouse_x - x_offset;  // Move window while maintaining offset
                 y = abs(mouse_y - y_offset);
                
                if(x<0){
                    x=0;
                } 
                
             }
         } else {
             grabbing = false;
         }
     
         
    canvas->SetColor(this->background);
    canvas->DrawRect(this->x,this->y,this->w,this->h);

}