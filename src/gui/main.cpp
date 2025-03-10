#include <kernel/drivers/vga.h>
#include <kernel/drivers/mouse.h>
#include <gui/spui.h>
#include <gui/textlabel.h>


extern  "C" char read_key();

void delay(int ms) {
    volatile int count = ms * 10000;  // Adjust based on CPU speed
    while (count--) {
        asm volatile("nop");  // No-operation to prevent aggressive optimization
    }
}
int count=0;
TextLabel* label1;

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
     TextLabel* rect1 = new TextLabel("Welcome To SP OS",10,20,100,100);
     rect1->SetBackgroundColor(0x21);
     rect1->SetTextColor(0xf);

     
     TextLabel* rect2 = new TextLabel("Increment",120,150,65,25);
     rect2->SetDraggable(false);
     rect2->SetBackgroundColor(0x34);
     rect2->SetTextColor(0xf);
     label1 = new TextLabel("Count:0",120,20,100,100);

     label1->SetTextColor(0xf);
     label1->SetBackgroundColor(0x28);

    rect2->SetOnClick([](SPWidget* widget){
        label1->SetBackgroundColor(0x5);
        char buff[4];
        count++;
        delay(500);
        itoa(count,buff);
        
        label1->SetText(strcombine("count:",buff));
    });

     
     while (true)
     {   
        restoreBackground();
        saveBackground();
         rect1->Render(canvas);
         rect2->Render(canvas);
         label1->Render(canvas);
         draw_cursor(getMouseX(),getMouseY(), 0xE);
         canvas->RenderDisplay();
     }

     
    


   



}