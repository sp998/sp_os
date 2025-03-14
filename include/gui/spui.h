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
        uint32_t w;
        uint32_t h;

    public:
        SPCanvas(uint32_t w, uint32_t h);
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
        uint32_t GetWidth();
        uint32_t GetHeight();

};

class SPWidget{
    protected:
        int32_t x;
        int32_t y;
        uint32_t w;
        uint32_t h;
        bool grabbing;
        bool dragabble;
        bool dirty;
        int x_offset = 0, y_offset = 0;
        uint8_t background;
        void (*onClick)(SPWidget* widget);
        
    public:
        SPWidget(uint32_t x, uint32_t y,uint32_t w,uint32_t h);
        bool IsDirty();
        void SetBackgroundColor(uint8_t colorIndex);
        void SetOnClick(void(*onClick)(SPWidget* widget));
        virtual void Render(SPCanvas *canvas);
        void SetDraggable(bool value);

};


class SPWindow : public SPWidget {
    private:
        char* title;
        bool isActive;
        bool isMinimized; // Track the minimized state
        bool isDestroyed; // Track the destroyed state
        bool isDragging;
        int dragOffsetX, dragOffsetY;
    
    public:
        SPWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h, char* title);
        void Render(SPCanvas* canvas);
    
        // Add setters/getters for minimizing/destroying, if needed
        bool IsMinimized() const { return isMinimized; }
        bool IsDestroyed() const { return isDestroyed; }
        void SetActive(bool value);
    
        void Minimize() { isMinimized = !isMinimized; }
        void Destroy() { isDestroyed = true; }
    };
    
            
#endif