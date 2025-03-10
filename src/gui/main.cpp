#include <kernel/drivers/vga.h>
#include <kernel/drivers/mouse.h>
#include <gui/spui.h>


extern  "C" char read_key();

void delay(int ms) {
    volatile int count = ms * 10000;  // Adjust based on CPU speed
    while (count--) {
        asm volatile("nop");  // No-operation to prevent aggressive optimization
    }
}

extern "C" void main(){
    char key;
    key = read_key();

    SPCanvas* canvas = new SPCanvas();
    canvas->SetBackgroundColor(0x22);
    canvas->SetColor(0x11);
    canvas->SetTextColor(0xE);



    canvas->SetColor(0x0f);
    canvas->FillShape(true);
    canvas->DrawRect(0,0,WIDTH,13);

    canvas->SetTextColor(0x60);
    canvas->SetFontType(FONT_5x8);
    canvas->DrawText(0,2," Home Settings Files   Tools     Help          About");
    
    canvas->SetColor(0x21);

     int x,y;
     x=10;
     y=10;
 
     saveBackground();
     SPWidget* rect1 = new SPWidget(10,10,100,100);
     SPWidget* rect2 = new SPWidget(20,120,50,25);

   

     
     while (true)
     {   
        restoreBackground();
        saveBackground();
         rect1->Render(canvas);
         rect2->Render(canvas);
         draw_cursor(getMouseX(),getMouseY(), 0xE);
         canvas->RenderDisplay();
     }

     
    


   



}