#include <gui/spui.h>
#include <kernel/drivers/mouse.h>




SPWidget* grabbedWidget = nullptr;

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
    if (this->fillShape) {
        // Efficiently fill the rectangle by drawing horizontal lines
        for (uint32_t j = y; j < y + h; j++) {
            for (uint32_t i = x; i < x + w; i++) {
                DrawPixel(i, j);
            }
        }
    } else {
        // Draw four edges of the rectangle
        for (uint32_t i = x; i < x + w; i++) {
            DrawPixel(i, y);          // Top edge
            DrawPixel(i, y + h - 1);  // Bottom edge
        }
        for (uint32_t j = y; j < y + h; j++) {
            DrawPixel(x, j);          // Left edge
            DrawPixel(x + w - 1, j);  // Right edge
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

    if (getLeftButtonPress()) {  // If mouse is clicked
       
        if (grabbedWidget == nullptr || grabbedWidget == this) {  // Only grab if no other widget is grabbed
            if (mouse_x >= this->x && mouse_x <= this->x + this->w &&
                mouse_y >= this->y && mouse_y <= this->y + 16) {  // Clicked inside title bar
                grabbedWidget = this;
                this->onClick(this);
                if(dragabble){
                this->x = mouse_x - (this->w / 2); // Update X
                this->y = mouse_y - 8; // Update Y
                }
            }

        }
    } else {  // If mouse is released
        if (grabbedWidget == this) {
            grabbedWidget = nullptr;  // Release grab
        }
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

void SPWindow::SetActive(bool active) {
    this->isActive = active;
    this->dirty = true;
}

SPWindow::SPWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h, char *title)
        : SPWidget(x, y, w, h) {
        this->title = title;
        this->isActive = false;
    }
    



bool isMouseInsideRect(int mouseX, int mouseY, int bx, int by, int bw, int bh) {
    return (mouseX >= bx && mouseX <= bx + bw && mouseY >= by && mouseY <= by + bh);
}



void SPWindow::Render(SPCanvas *canvas) {
    int mouse_x = getMouseX();
    int mouse_y = getMouseY();

    // Button dimensions
    int buttonWidth = 16;
    int buttonHeight = 16;
    int buttonSpacing = 4;

    // Button positions (Minimize and Destroy buttons)
    int minimizeBtnX = this->x + this->w - (buttonWidth * 2) - buttonSpacing;
    int destroyBtnX = this->x + this->w - buttonWidth - buttonSpacing;

    // Handle dragging logic
    if (getLeftButtonPress()) {
        // Check if mouse is over the title bar (for dragging)
        if (isMouseInsideRect(mouse_x, mouse_y, this->x, this->y, this->w, 16)) {
            if (!isDragging) {
                isDragging = true; // Start dragging
                // Calculate the offset relative to the window position
                dragOffsetX = mouse_x - this->x;
                dragOffsetY = mouse_y - this->y;
            }
        }

        // Handle Minimize and Destroy buttons
        if (isMouseInsideRect(mouse_x, mouse_y, minimizeBtnX, this->y + 1, buttonWidth, buttonHeight)) {
            this->Minimize(); // Toggle minimize state
            this->dirty = true;
        }

        if (isMouseInsideRect(mouse_x, mouse_y, destroyBtnX, this->y + 1, buttonWidth, buttonHeight)) {
            this->Destroy(); // Destroy the window
        }
    }

    // Stop dragging when mouse button is released
    if (!getLeftButtonPress()) {
        isDragging = false; // Stop dragging
    }

    // Update window position if dragging is true
    if (isDragging) {
        this->x = mouse_x - dragOffsetX;
        this->y = mouse_y - dragOffsetY;
    }

    // Draw window background and title bar
    canvas->SetColor(this->background);
    canvas->DrawRect(this->x, this->y, this->w, this->h);

    // Draw title bar
    canvas->SetColor(0x07); // Light gray for title bar
    canvas->DrawRect(this->x, this->y, this->w, 16);

    // Draw title text
    canvas->SetTextColor(0x00); // Black text for title
    canvas->DrawText(this->x + 4, this->y + 4, this->title);

    // Draw Minimize Button
    canvas->SetColor(0x0F); // White for minimize button
    canvas->DrawRect(minimizeBtnX, this->y + 1, buttonWidth, buttonHeight); // Minimize button shape
    canvas->SetTextColor(0x00); // Black text for minimize button
    canvas->DrawText(minimizeBtnX + 4, this->y + 4, "-");

    // Draw Destroy Button
    canvas->SetColor(0x0F); // Light red for destroy button
    canvas->DrawRect(destroyBtnX, this->y + 1, buttonWidth, buttonHeight); // Destroy button shape
    canvas->SetTextColor(0x00); // Black text for destroy button
    canvas->DrawText(destroyBtnX + 4, this->y + 4, "X");

    // If the window is minimized, don't render its content
    if (!this->isMinimized) {
        // Render window content here
        canvas->SetColor(0x1F); // Example content color
        canvas->DrawRect(this->x + 2, this->y + 18, this->w - 4, this->h - 20);  // Content area
    }

    this->dirty = false;
}