#ifndef TEXT_LABEL_H
#define TEXT_LABEL_H
#include<gui/spui.h>

class TextLabel:public SPWidget{
    private:
        char* text;
        uint8_t textColor;
    public:
        TextLabel(char* text,uint32_t x, uint32_t y, uint32_t w,uint32_t h);
        void SetTextColor(uint8_t colorIndex);
        void SetText(char* text);
        void Render(SPCanvas *canvas) override;
};

#endif
