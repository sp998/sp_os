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
    this->dragabble=true;
    this->dirty = true;
}
bool SPWidget::IsDirty()
{
    return this->dirty;
}
void SPWidget::SetBackgroundColor(uint8_t colorIndex)
{
    this->background=colorIndex;
    this->dirty = true;
}
void SPWidget::SetOnClick(void (*onClick)(SPWidget* widget))
{
  this->onClick = onClick;
}
void SPWidget::Render(SPCanvas *canvas)
{   
    
    int mouse_x = getMouseX();
    int mouse_y = getMouseY();

    if (getLeftButtonPress()) {
        if (!grabbing) {
            if (mouse_x >= x && mouse_x <= x + w &&
                mouse_y >= y && mouse_y <= y + h) {
                grabbing = true;
                x_offset = mouse_x - x;
                y_offset = mouse_y - y;
            }
        }

        if (grabbing) {
            this->onClick(this);

            if (dragabble) {
                int new_x = mouse_x - x_offset;
                int new_y = abs(mouse_y - y_offset);

                if (new_x < 0) new_x = 0;

                if (new_x != x || new_y != y) {  // Check if position actually changes
                    x = new_x;
                    y = new_y;
                    dirty = true;  // Mark widget as needing a redraw
                }
            }
        }
    } else {
        grabbing = false;
    }

    if (dirty) {  // Only redraw if marked as dirty
     
        canvas->SetColor(this->background);
        canvas->DrawRect(this->x, this->y, this->w, this->h);
        //dirty = false;  // Reset dirty flag after rendering
    }
}
void SPWidget::SetDraggable(bool value)
{
    this->dragabble =value;
}