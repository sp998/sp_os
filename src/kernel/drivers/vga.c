#include <kernel/drivers/vga.h>

void putPixelWithIndex(uint32_t x, uint32_t y, uint8_t colorIndex){

    uint8_t* pixelAddress = getFrameBufferSegment() + WIDTH*y + x;
     *pixelAddress = colorIndex;

}
uint8_t getColorIndex(uint8_t r,uint8_t g,uint8_t b){

     if(r==0x00&&g==0x00&&b==0xA8){
        return 0x01;
     }
}

void writeRegisters(uint8_t *registers)
{

    // misc
    outPortB(MISC_PORT,*(registers++));
    
    // sequencer
    for(uint8_t i =0 ;i<5;i++){
      outPortB(SEQUENCER_INDEX_PORT,i);
      outPortB(SEQUENCER_DATA_PORT,*(registers++));
    }

    // cathode ray tube controller
    outPortB(CRTC_INDEX_PORT,0x03);
    outPortB(CRTC_DATA_PORT,inPortB(CRTC_DATA_PORT) | 0x80);

    outPortB(CRTC_INDEX_PORT,0x11);
    outPortB(CRTC_DATA_PORT,inPortB(CRTC_DATA_PORT) & ~0x80);
    

    registers[0x03] = registers[0x03] |0x80;
    registers[0x11] = registers[0x11] &-0x80;
    for(uint8_t i =0 ;i<25;i++){
        outPortB(CRTC_INDEX_PORT,i);
        outPortB(CRTC_DATA_PORT,*(registers++));
      }

      //graphics controlelr
      for(uint8_t i =0 ;i<9;i++){
        outPortB(GRAPHICS_CONTROLLER_INDEX_PORT,i);
        outPortB(GRAPHICS_CONTROLLER_DATA_PORT,*(registers++));
      }
     
      //Attribute controller 
      for(uint8_t i =0 ;i<21;i++){
        uint8_t resetValue = inPortB(ATTRIBUTE_CONTROLLER_RESET_PORT);
        outPortB(ATTRIBUTE_CONTROLLER_INDEX_PORT,i);
        outPortB(ATTRIBUTE_CONTROLLER_WRITE_PORT,*(registers++));
      }

      uint8_t resetValue = inPortB(ATTRIBUTE_CONTROLLER_RESET_PORT);
      outPortB(ATTRIBUTE_CONTROLLER_INDEX_PORT,0x20);

}
uint8_t *getFrameBufferSegment()
{
   outPortB(GRAPHICS_CONTROLLER_INDEX_PORT,0x06);
   uint8_t segmentNumber = (inPortB(GRAPHICS_CONTROLLER_DATA_PORT) >>2) &0x03;

   switch(segmentNumber){
    case 0: return (uint8_t*)0x00000;
    case 1: return (uint8_t*)0xA0000;
    case 2: return (uint8_t*)0xB0000;
    case 3: return (uint8_t*)0xB8000;
    default: NULL;
   }
}
bool supportsMode(uint32_t width, uint32_t height, uint32_t colorDepth)
{
    return width ==320 && height == 200 && colorDepth ==8;
}
bool setVgaMode(uint32_t width, uint32_t height, uint32_t colorDepth)
{
    if(!supportsMode(width,height,colorDepth)){
        return false;
    }

    unsigned char g_320x200x256[] =
    {
        /* MISC */
            0x63,
        /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
            0xFF,
        /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
        /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00, 0x00
    };

    writeRegisters(g_320x200x256);
    return true;
   
}

void putPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    putPixelWithIndex(x,y,getColorIndex(r,g,b));
}
