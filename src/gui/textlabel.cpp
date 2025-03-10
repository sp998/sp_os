#include <gui/textlabel.h>
#include <string.h>



TextLabel::TextLabel(char *text, uint32_t x, uint32_t y, uint32_t w, uint32_t h):SPWidget(x,y,w,h)
{
    this->text=text;
}

void TextLabel::SetTextColor(uint8_t colorIndex)
{
    this->textColor = colorIndex;
}
void TextLabel::SetText(char *text)
{
    this->text = text;
}
void TextLabel::Render(SPCanvas *canvas)
{
    
    SPWidget::Render(canvas);
    uint32_t width = this->x;
    uint32_t height = this->y+(this->h/2);
    canvas->SetTextColor(this->textColor);
    canvas->SetFontType(FONT_5x8);
    canvas->DrawText(width,height,this->text);
    
}