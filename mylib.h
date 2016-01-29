#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned long vu32;

#define REG_DISPCNT *(u16*)0x04000000
#define VRAM 0x06000000
#define M5_VRAM 0x0600A000 
#define BACKBUFFER  0x010
#define PALETTE ((u16 *)0x5000000)
#define MODE_3 0x03	
#define MODE_4 0x04
#define MODE_5 0x05
#define BUFFER0 (u16 *)0x6000000
#define BUFFER1 (u16 *)0x600A000
#define BUFFER1FLAG (1<<4)
#define BG2_ENABLE 0x0400
#define setMode(Mode) REG_DISPCNT=(Mode)

#define RED RGB(31, 0, 0)
#define GREEN RGB(0, 31, 0)
#define BLUE RGB(0, 0, 31)
#define BLACK RGB(0, 0, 0)
#define WHITE RGB(31, 31, 31)
#define CYAN RGB(0, 31, 31)
#define MAG	RGB(31, 0, 31)
#define YELLOW	RGB(31, 31, 0)
#define GRAY RGB(15, 15, 15)

#define OFFSET(x, y, width) ((x)+(width)*(y))
#define RGB(R, G, B) ((R) | (G)<<5 | (B)<<10)
#define SCANLINECOUNTER *(volatile u16*) 0x4000006

extern u16* videoBuffer;
extern u16* pallete_mem;

extern volatile u32* KEYS;
#define KEY_A 1 << 0
#define KEY_B 1 << 1
#define KEY_SELECT 1 << 2
#define KEY_START 1 << 3
#define KEY_RIGHT 1 << 4
#define KEY_LEFT 1 << 5
#define KEY_UP 1 << 6
#define KEY_DOWN 1 << 7
#define KEY_R 1 << 8
#define KEY_L 1 << 9
#define KEY_DOWN_NOW(key) (!((*KEYS)&key))

extern const u8 fontdata_6x8[12288];

#define REG_DMA0SAD         *(const volatile u32*)0x40000B0 
#define REG_DMA0DAD         *(volatile u32*)0x40000B4       
#define REG_DMA0CNT         *(volatile u32*)0x40000B8       

#define REG_DMA1SAD         *(const volatile u32*)0x40000BC 
#define REG_DMA1DAD         *(volatile u32*)0x40000C0       
#define REG_DMA1CNT         *(volatile u32*)0x40000C4       

#define REG_DMA2SAD         *(const volatile u32*)0x40000C8 
#define REG_DMA2DAD         *(volatile u32*)0x40000CC       
#define REG_DMA2CNT         *(volatile u32*)0x40000D0       

#define REG_DMA3SAD         *(const volatile u32*)0x40000D4 
#define REG_DMA3DAD         *(volatile u32*)
#define REG_DMA3CNT         *(volatile u32*)

typedef struct
{
	const volatile void *src;
	const volatile void *dst;
	unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

// function prototypes
void setPixel4(int x, int y, u8 index);
void drawChar4(int x, int y, char ch, u8 color);
void drawString4(int x, int y, char *str, u8 color);
void delay(int n);
void waitForVblank();
void drawImage4(int x, int y, int width, int height, const u16* image);
void fillScreen4(u8 index);
void flipPage();
void fillPalette(const u16 *palette, int start, int pal_size);
