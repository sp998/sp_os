#include <kernel/drivers/vga.h>



void putPixelWithIndex(uint32_t x, uint32_t y, uint8_t colorIndex){

    uint8_t* pixelAddress = getFrameBufferSegment() + WIDTH*y + x;
    if(pixelAddress==NULL){
        return;
    }
     *pixelAddress = colorIndex;

}
uint8_t getColorIndex(uint8_t r,uint8_t g,uint8_t b){

     if(r==0x00&&g==0x00&&b==0xA8){
        return 0x01;
     }
}


void setBackground(uint8_t colorIndex){
    uint8_t *frameBuffer = getFrameBufferSegment();  // Get framebuffer segment
    if(frameBuffer==NULL){
        return;
    }

    // Fill the framebuffer with the color index
    uint32_t totalPixels = 200 * 320;  // For a 320x200 screen
    for(uint32_t i = 0; i < totalPixels; i++) {
        frameBuffer[i] = colorIndex;  // Set each pixel to the color index
    }
}
void draw_char(int x, int y, char c, uint8_t color, uint8_t font_type) {
    uint8_t width, height;
    unsigned char *font = getFont(font_type, &width, &height);
    if (font == NULL) return;  // Handle invalid font type

    uint32_t offset = (int)c;  // Calculate offset based on ASCII value (assuming printable characters start from 32)

    // Loop through each row of the character and draw the corresponding pixels
    for (int row = 0; row < height; row++) {
        uint8_t line = font[offset * height + row];  // Access the font data based on the offset

        // Loop through each column and draw the corresponding pixel
        for (int col = 0; col < width; col++) {
            if (line & (1 << (7 - col))) {  // Check if the bit is set
                putPixelWithIndex(x + col, y + row, color);  // Draw pixel at (x+col, y+row)
            }
        }
    }
}
void draw_string(int x, int y, const char *str, uint8_t color, uint8_t font_type) {
    if (!str) return; // Check if the string is valid

    uint8_t char_width, char_height;
    unsigned char *font = getFont(font_type, &char_width, &char_height);  // Fetch font and dimensions
    if (!font) return; // Return if font is unavailable

    while (*str) {
        draw_char(x, y, *str, color, font_type);  // Draw the current character
        x += char_width + 1;  // Move x-coordinate by the width of the character + 1 pixel space
        str++;  // Move to the next character
    }
}


uint8_t getPixelColor(uint32_t x, uint32_t y)
{
    uint8_t* pixelAddress = getFrameBufferSegment() + WIDTH*y + x;
     return *pixelAddress;

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
