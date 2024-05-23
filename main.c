/*	Coder:		Decay
	E-mail:		thefountainofdecay@hotmail.com
	Home Page:	http://members.tripod.co.uk/GBCE/index.html (note case)
	File:		Main.c
	Date:		27/02/2002
	Purpose:	very simple demo to show double buffering using mode 4 on the gba
*/

#include "mario.h"	//contains map and palettes for the image

//list defines 
#define REG_DISPCNT *(unsigned short*)0x4000000	//this controls vid mode, bkgs, objs etc
#define MODE_0 0x0	//set mode 0 - tiled, bkgs 0123, no rot/scale
#define MODE_1 0x1	//set mode 1 - tiled, bkgs 012, rot/scale 2
#define MODE_2 0x2	//set mode 2 - tiled, bkgs 23,  rot/scale 23
#define MODE_3 0x3	//set mode 3 - 16bit buffer (enable bkg 2 to use)
#define MODE_4 0x4	//set mode 4 - 8bit buffer, double bufferable
#define MODE_5 0x5	//set mode 5 - 16bit buffer, double bufferable at 160x128
#define BACKBUFFER	0x10	//controls which buffer is active in mode 4/5
#define BKG0_ENABLE 0x100	//enable bkg 0
#define BKG1_ENABLE 0x200	//enable bkg 1	 
#define BKG2_ENABLE 0x400	//enable bkg 2
#define BKG3_ENABLE 0x800	//enable bkg 3
#define OBJ_ENABLE 0x1000 	//enable objects
#define VBLANK_CNT	*(volatile unsigned short*)0x4000006	//the vertical line being draw, volatile as changed outside of code
#define BKG_PALETTE	0x5000000	//start of bkg palettes
#define OBJ_PALETTE	0x5000200	//start of objs palettes
#define VRAM 0x6000000	//start of vram
#define VRAM_BUFFER 0x600A000	//start of vram buffer for mode 45

unsigned short *VideoBuffer;	//global pointer we are going to use to point to VRAM

void WaitVBlank(void)
{
	while (VBLANK_CNT<160) {};	//do nothing untill vblank has completed
}

void SwapBuffer(void)	//this swaps the locations of the current display and current writing location
{
	if (REG_DISPCNT&BACKBUFFER)		//backbuffer is being displayed so swap
	{
		REG_DISPCNT &= ~BACKBUFFER;	//make display show the frontbuffer (remove backbuffer using it's bitwise compliement)
		VideoBuffer=(unsigned short*)VRAM_BUFFER;	//make backbuffer the one we draw to
	}
	else		//frontbuffer is being displayed so swap
	{
		REG_DISPCNT|=BACKBUFFER;	//make display show the backbuffer (add backbuffer)
		VideoBuffer=(unsigned short*)VRAM;	//make frontbuffer the one we draw to
	}
}

int init()
{
	//Init mode 5------------------------------------------------------------------
	*(u16*)0x4000000 = 0x405;                                           //mode 5 background 2
	*(u16*)0x400010A = 0x82;                                            //enable timer for fps
	*(u16*)0x400010E = 0x84;                                            //cnt timer overflow

	//scale small mode 5 screen to full screen-------------------------------------
	REG_BG2PA=256/2;                                                    //256=normal 128=scale 
	REG_BG2PD=256/2; 
}

int main(void)
{
	unsigned short counter, *bkgpal;

	VideoBuffer=(unsigned short*)VRAM_BUFFER;	//point to BACK VRAM
	REG_DISPCNT=(MODE_4|BKG2_ENABLE);	//mode 4, need bkg2 enabled

	//load the image map into BACK_VRAM, remember, in mode 4 these are not actual colours but palette array references
	counter=0;
	while (counter<38400)	//we have 38400 8bit values
	{
		//we can only write in 16bits but the map in held in 8bits (char) so ...
		//add two together using bitshifting by 8 (a char). it writes the lower bits first
		*VideoBuffer++=(Mario_Map[counter+1]<<8) + Mario_Map[counter];
		counter+=2;	//skip by 2 since we added 2
	}

	counter=0;
	bkgpal=(unsigned short*)BKG_PALETTE;	//set up a pointer to the location of the bkg palette
	while (counter<256)		//we have 256 16bit palettes
		*bkgpal++=(unsigned short)Mario_Palette[counter++];	//fill in the palette

	WaitVBlank();	//wait for vblank
	SwapBuffer();	//swap buffers

	counter=38400;	//we have 38400 8bit values
	do
	{
		//this loads the picture reversed upside down
		*VideoBuffer++=(Mario_Map[counter-2]<<8)+Mario_Map[counter-1];
		counter-=2;	//skip by 2 since we added 2 above
	} while (counter>0);

	//would do this if you were loading a different picture
	counter=0;
	bkgpal=(unsigned short*)BKG_PALETTE;	//set up a pointer to the location of the bkg palette
	while (counter<256)		//we have 256 16bit palettes
		*bkgpal++=(unsigned short)Mario_Palette[counter++];	//fill in the palette

	WaitVBlank();	//wait for vblank
	SwapBuffer();	//swap buffers
	//loops
}
