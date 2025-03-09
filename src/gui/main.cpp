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

   
    


    // for (uint16_t x = 0; x < 320; x++) {
        
    //     uint16_t y = (x * 199) / 319;   
    //     canvas->DrawPixel(x,y);
    // }

    // for (uint16_t x = 0; x < 320; x++) {
       
    //     uint16_t y = 199-(x * 199) / 319;  
    //     canvas->DrawPixel(x,y); 
    // }


    // for (uint16_t x = 0; x <= WIDTH ; x++) {
       
    //     uint16_t y1 = (x * HEIGHT) / WIDTH;

       
    //     uint16_t y2 = HEIGHT - (x * HEIGHT) / WIDTH;

       
    //     uint16_t y = min(y1, y2);
    //     uint16_t y3 = max(y1,y2);


    //        for(uint8_t i=0;i<100;i++){
    //         canvas->DrawPixel(x,y-i);
    //         canvas->DrawPixel(x,y3+i);

    //        }
        
    // }


    // for (uint32_t y = 0; y <= HEIGHT; y++) {
       
    //     uint32_t x1 = (y * WIDTH) / HEIGHT;
    //     uint16_t x2 = WIDTH - (y * WIDTH) / HEIGHT;

    //     uint32_t x = min(x1, x2);
    //     uint32_t x3 = max(x1,x2);

    
    //     for (uint32_t i = 0; i <=30; i++) {
    //         if(x-i<WIDTH/2){
    //         canvas->DrawPixel(x-i,y);
    //         }
        
    //         if(x3+i>WIDTH/2 && x3+i<WIDTH){
    //         canvas->DrawPixel(x3+i,y);
    //         }
        
    //     }
    // }

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
     uint32_t x0,y0;
     int x_dir,y_dir;
     uint32_t last_x,last_y,new_x,new_y;
     saveBackground();

    bool grabbing = false;
    while (true)
    {   
        
        restoreBackground();

        saveBackground();
        
        if(getLeftButtonPress()){
            uint32_t x_delta=getMouseX();
            uint32_t  y_delta= getMouseY();
        
            if(!grabbing&& x_delta>=x && x_delta<=x+200){
                if(y_delta>=y && y_delta<=y+100){
                    grabbing = true;
                    x0= x_delta;
                    y0= y_delta;

                }
            }
            if(grabbing){
                
                x+= getMouseXDelta();
                if(x<0){
                    x=0;
                }
                if(x+200>WIDTH){
                    x=WIDTH-1;
                }

                

                
                
                y+= getMouseYDelta();

                if(y<0){
                    y=0;
                }
                if(y+100>HEIGHT){
                    y=HEIGHT-1;
                }
            }
            
        }else{
            grabbing =false;
        }
        delay(16);
        canvas->SetColor(0xf);
        canvas->DrawRect(x,y,200,100);
        draw_cursor(getMouseX(),getMouseY(),0xE);
        canvas->RenderDisplay();


       
    }
    


   



}