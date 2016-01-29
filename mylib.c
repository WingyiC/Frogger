#include "mylib.h"
#include "font.h"

u16* videoBuffer = (u16*)VRAM;

void setPixel4(int x, int y, u8 index) {
	int whichPixel = OFFSET(x, y, 240);
	int whichShort = whichPixel/2;
	if (x & 1) {
		// odd column
		videoBuffer[whichShort] = (videoBuffer[whichShort] & 0x00FF) | (index << 8);
	} else {
		// even column
		videoBuffer[whichShort] = (videoBuffer[whichShort] & 0xFF00) | (index);
	}
}

void drawImage4(int x, int y, int width, int height, const u16* image) {
	for(int r = 0; r < height; r++) {
		DMA[3].src = &image[OFFSET(0, r, width/2)];
		DMA[3].dst = &videoBuffer[OFFSET(x, y + r, 240)/2];
		DMA[3].cnt = width/2 | DMA_ON;
	}
}

void flipPage() {
  	if(REG_DISPCNT & BUFFER1FLAG) {
   		REG_DISPCNT &= ~BUFFER1FLAG;
    	videoBuffer = BUFFER1;
  	} else {
    	REG_DISPCNT |= BUFFER1FLAG;
    	videoBuffer = BUFFER0;
  	}
}

void fillScreen4(u8 index) {
	volatile u16 color = index | (index << 8);
	DMA[3].src = &color;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = 19200 | DMA_ON | DMA_SOURCE_FIXED;
}

void fillPalette(const u16 *palette, int start, int pal_size) {
	for (int i = start; i < pal_size; i++) {
		PALETTE[i] = palette[i];
	}
}

void drawChar4(int x, int y, char ch, u8 index) {
	int c, r;
	for(c = 0; c < 6; c++) {
		for(r = 0; r < 8; r++) {
			if(fontdata_6x8[OFFSET(c, r, 6) + ch * 48]) {
				setPixel4(x + c, y + r, index);
			}
		}
	}
}

void drawString4(int x, int y, char *str, u8 index) {
	while(*str) {
		drawChar4(x, y, *str++, index);
		x += 6;
	}
}

void delay(int n) {
	volatile int x = 0;
	for (int i = 0; i < n*10000; i++) {
		x++;
	} 	
}

void waitForVblank() {
	while (SCANLINECOUNTER >= 160);
	while (SCANLINECOUNTER < 160);
}
