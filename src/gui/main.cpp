#include <kernel/drivers/vga.h>
#include <gui/spui.h>


extern  "C" char read_key();

extern "C" void main(){
    char key;
    key = read_key();

    SPCanvas* canvas = new SPCanvas();
    canvas->SetBackgroundColor(0x10);
    canvas->SetColor(0x2);
    canvas->SetTextColor(0xE);
    canvas->SetFontType(FONT_8x8);
   
    


    for (uint16_t x = 0; x < 320; x++) {
        
        uint16_t y = (x * 199) / 319;   
        canvas->DrawPixel(x,y);
    }

    for (uint16_t x = 0; x < 320; x++) {
       
        uint16_t y = 199-(x * 199) / 319;  
        canvas->DrawPixel(x,y); 
    }


    for (uint16_t x = 0; x <= WIDTH ; x++) {
       
        uint16_t y1 = (x * HEIGHT) / WIDTH;

       
        uint16_t y2 = HEIGHT - (x * HEIGHT) / WIDTH;

       
        uint16_t y = min(y1, y2);
        uint16_t y3 = max(y1,y2);


           for(uint8_t i=0;i<100;i++){
            canvas->DrawPixel(x,y-i);
            canvas->DrawPixel(x,y3+i);

           }
        
    }


    for (uint32_t y = 0; y <= HEIGHT; y++) {
       
        uint32_t x1 = (y * WIDTH) / HEIGHT;
        uint16_t x2 = WIDTH - (y * WIDTH) / HEIGHT;

        uint32_t x = min(x1, x2);
        uint32_t x3 = max(x1,x2);

    
        for (uint32_t i = 0; i <=30; i++) {
            if(x-i<WIDTH/2){
            canvas->DrawPixel(x-i,y);
            }
        
            if(x3+i>WIDTH/2 && x3+i<WIDTH){
            canvas->DrawPixel(x3+i,y);
            }
        
        }
    }

    canvas->SetColor(0x0f);
    canvas->FillShape(true);
    canvas->DrawRect(0,0,WIDTH,13);

    canvas->SetTextColor(0x60);
    canvas->SetFontType(FONT_5x8);
    canvas->DrawText(0,2," Home Settings Files   Tools     Help          About");
    
    canvas->SetColor(0x21);

   



}