#include <cstdint>
#include <tonc.h>
#include <cstdio>


/*DISCLAIMER !!!!!!!!!!

NEVER EVER USE THIS AS A TEMPLATE, USE BETTER CODE!
Featuring butchered copies from:
https://www.gbadev.org/demos.php?showinfo=323
https://www.coranac.com/tonc/text/bitmaps.htm

*/
typedef uint16_t COLOR; 


#define REG_DISPCNT *(volatile uint32_t*)(REG_BASE+0x0000)
#define MODE_0 0x0	//set mode 0 - tiled, bkgs 0123, no rot/scale
#define MODE_1 0x1	//set mode 1 - tiled, bkgs 012, rot/scale 2
#define MODE_2 0x2	//set mode 2 - tiled, bkgs 23,  rot/scale 23
#define MODE_3 0x3	//set mode 3 - 16bit buffer (enable bkg 2 to use)
#define MODE_4 0x4	//set mode 4 - 8bit buffer, double bufferable
#define MODE_5 0x5	//set mode 5 - 16bit buffer, double bufferable at 160x128
#define BKG0_ENABLE 0x100	//enable bkg 0
#define BKG1_ENABLE 0x200	//enable bkg 1	 
#define BKG2_ENABLE 0x400	//enable bkg 2
#define BKG3_ENABLE 0x800	//enable bkg 3
#define OBJ_ENABLE 0x1000 	//enable objects
#define PALETTE 0x5000000 //palette start





uint16_t newpalette[14] = {0x329E, 0x2529, 0x0000, 0x7680, 0x001F, 0x18D5, 0x03FF, 0x08A9, 0x2218, 0x4B3F, 0x7FFF, 0x4A52, 0x109B, 0x329E};
                        // dirt,   bg    , black , blue  , red   , brick , yellow, dbrown,clothes, skin  , white ,rocgray, skinred, dirt again

namespace GFX{

    
    unsigned short *bkgpal = (unsigned short*)PALETTE;

    void init() {

        irq_init(NULL);
        irq_enable(II_VBLANK);
        
        REG_DISPCNT=(MODE_4|BKG2_ENABLE);
        for (int i = 0; i < 14; i++)
        {
            bkgpal[i] = newpalette[i];
        }

        //bkgpal[13] = 0x329E;
        
    }

    

    //["F4A560","4F4F4F","000000","00A2E8","FF0000","AD3636","F9F801","4C2D17","C68642","FCCA90"]
    //{0x329E   ,0x2529  , 0x0000   , 0x7680   , 0x001F   , 0x18D5   , 0x03FF   , 0x08A9   , 0x2218   , 0x4B3F};
    
    

    INLINE void m4_plot(int x, int y, u8 clrid)
    {
        u16 *dst= &vid_page[(y*240+x)/2];  // Division by 2 due to u8/u16 pointer mismatch!
        if(x&1)
            *dst= (*dst& 0xFF) | (clrid<<8);    // odd pixel
        else
            *dst= (*dst&~0xFF) |  clrid;        // even pixel
    }

    void clear (u8 color){
        
        for (int x = 0; x < 240; x++)
        {
            for(int y = 0; y < 160; y++)
            {
                m4_plot(x, y, color);
            }
        }
        
    }

    void m4_doubleTextWriting(int x, int y, char* txt)
    {
        tte_set_pos(-80 + x, 171 + y);
        tte_write(txt);
        tte_set_pos(x, y);
        tte_write(txt);
    }

    void m4_doubleTextWriting(int x, int y, char* txt, int intTxt)/*64 char limit total*/
    {
        char finString[64];
        sprintf(finString, "%s%d",txt, intTxt);
        tte_set_pos(-80 + x, 171 + y);
        tte_write(finString);
        tte_set_pos(x, y);
        tte_write(finString);
    }

    void drawRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, u8 color){
        if(x > 240) {
            x -= 240;
        }
        if(y > 160) {
            y -= 160;
        }
        if(x + w > 240){
            w = 240 - x;

        }
        if(y + h > 160){
            h = 160 - y;
        }

        int off = x + (y * 240);
        for(unsigned int y1 = 0; y1 < h; y1++) {
            for(unsigned int x1 = 0; x1 < w; x1++) {
                //disp_buffer[x1 + off +y1 * 240] = color;
                m4_plot(x + x1, y + y1, color);
            }
        }
    }

    void drawImage(unsigned int x, unsigned int y, unsigned int w, unsigned int h, u8 image[]){
        if(x > 240) {
            x -= 240;
        }
        if(y > 160) {
            y -= 160;
        }
        if(x + w > 240){
            w = 240 - x;

        }
        if(y + h > 160){
            h = 160 - y;
        }

        int off = x + (y * 240);
        int pixelProgress = 0;
        for(unsigned int y1 = 0; y1 < h; y1++) {
            for(unsigned int x1 = 0; x1 < w; x1++) {
                //disp_buffer[x1 + off +y1 * 240] = image[pixelProgress];
                m4_plot(x + x1, y + y1, image[pixelProgress]);
                pixelProgress += 1;
            }
        }
        
    }
    

    
}