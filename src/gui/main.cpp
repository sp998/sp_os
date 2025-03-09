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

     bool grabbing = false;
     int x_offset = 0, y_offset = 0;
     uint32_t mouse_x ;
     uint32_t mouse_y;
     int width_decrease=0;

     
     while (true)
     {   
        mouse_x= getMouseX();
        mouse_y = getMouseY();
         restoreBackground();
         saveBackground();
         
         if (getLeftButtonPress()) {
            
             if (!grabbing) {
                 if (mouse_x >= x && mouse_x <= x + 200 &&
                     mouse_y >= y && mouse_y <= y + 100) {
                     grabbing = true;
                     x_offset = mouse_x - x;  // Capture the offset within the window
                     y_offset = mouse_y - y;
                 }
             }
     
             if (grabbing) {
                 x =mouse_x - x_offset;  // Move window while maintaining offset
                 y = abs(mouse_y - y_offset);
                
                if(x<0){
                    x=0;
                }
               
                
             }
         } else {
             grabbing = false;
         }
     
         
         canvas->SetColor(0xf);
         
         canvas->DrawRect(x, y, 200+width_decrease, 100);
         draw_cursor(mouse_x,mouse_y, 0xE);
         canvas->RenderDisplay();
     }
    


   



}