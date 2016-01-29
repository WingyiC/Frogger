#include "mylib.h"
#include "startScreen.h"
#include "gameOverScreen.h"
#include "GPalette.h"

volatile u32* KEYS = (volatile u32*)0x04000130;
extern int state; // 0 STARTSCREEN 1 PLAY 2 END OF GAME

#define numOfTrucks 1
#define numOfCars 4
#define numOfWoods 3
#define numOfCrocodiles 2

typedef struct {
	int x;
	int y;
	int dx;
	int dy;
	const u16* imgU1;
	const u16* imgU2;
	const u16* imgU3;
	const u16* imgD1;
	const u16* imgD2;
	const u16* imgD3;
	const u16* imgL1;
	const u16* imgL2;
	const u16* imgL3;
	const u16* imgR1;
	const u16* imgR2;
	const u16* imgR3;
} FROG;

typedef struct {
	int x;
	int y;
	int dx;
	const u16* img;
} TRUCK;

typedef struct {
	int x;
	int y;
	int dx;
	const u16* img;
} CAR;

typedef struct {
	int x;
	int y;
	int dx;
	const u16* img;
} WOOD;

typedef struct {
	int x;
	int y;
	int dx;
	const u16* img;
} CROCODILE;

int state = 0;
int numOfLives = 3;
int frogFrame = 0;

FROG frog;
TRUCK trucks[numOfTrucks];
CAR cars[numOfCars];
WOOD woods[numOfWoods];
CROCODILE crocodiles[numOfCrocodiles];

const u16* prevImg = frogU1;
int homeFlags[5];

// function prototypes
void startGame();
void lose();
static void loadAvatar();
static void drawBG();
static void drawFrog();
static void drawTrucks();
static void drawCars();
static void drawWoods();
static void drawCrocodiles();
static void collision();
static void loseLife();
static void reset();
static int gotHome(int isSafe);


int main() {
	setMode(MODE_4 | BG2_ENABLE);
	startGame();
	while(1) {
		if (KEY_DOWN_NOW(KEY_START) && state == 0) {			
			state = 1;
			numOfLives = 3;
			loadAvatar();
			fillPalette(GPalette_palette, 0, GPALETTE_PALETTE_SIZE);
			PALETTE[200] = BLACK;
			PALETTE[201] = WHITE;
			PALETTE[202] = RED;
			PALETTE[203] = GREEN;
			PALETTE[204] = BLUE;
			
			for (int i = 0; i < 5; i++) {
				homeFlags[i] = 0;
			}
		}
		if (state == 1) {
			drawBG();
			char liveStr[7];
		if (numOfLives >= 0) {
			sprintf(liveStr, "%d", numOfLives);
			drawString4(10, 147, "LIVES LEFT:  ", PALETTE[1]);
			drawString4(80, 147, liveStr, PALETTE[1]);
		}
			drawTrucks();
			drawCars();
			drawCrocodiles();
			drawWoods();
			drawFrog();
			collision();
			reset();
			delay(2);
			waitForVblank();
			flipPage();
		}
		if (state == 2) {
			while (1) {
				if (KEY_DOWN_NOW(KEY_START)) {
					state = 0;
					break;
				}
			}
		}
	}
	return 0;
}

void startGame() {
	fillPalette(startScreen_palette, 0, STARTSCREEN_PALETTE_SIZE);
	drawImage4(0, 0, 240, 160, start);
	drawString4(30, 140, "PRESS <START> TO PLAY", PALETTE[3]);
}

void lose() {
	fillPalette(gameOverScreen_palette, 0, GAMEOVERSCREEN_PALETTE_SIZE);
	drawImage4(0, 0, 240, 160, gameOver);
	drawString4(90, 130, "TRY AGAIN?", PALETTE[2]);
	drawString4(60, 145, "PRESS <START> TO RESTART", PALETTE[2]);
	state = 2;
}

void win() {
	fillPalette(startScreen_palette, 0, STARTSCREEN_PALETTE_SIZE);
	drawImage4(0, 0, 240, 160, start);
	drawString4(90, 130, "YOU WIN!", PALETTE[2]);
	drawString4(60, 145, "PRESS <START> TO RESTART", PALETTE[2]);
	state = 2;
}

static void reset() {
	if (KEY_DOWN_NOW(KEY_SELECT)) {
		startGame();
		state = 0;
	}
}

static void loadAvatar() {
	frogFrame = 0;
	prevImg = frogU1;
	// frog 
	frog.x = 110;frog.y = 145;frog.dx = 3;frog.dy = 6;
	frog.imgU1 = frogU1;frog.imgU2 = frogU2;frog.imgU3 = frogU3;
	frog.imgD1 = frogD1;frog.imgD2 = frogD2;frog.imgD3 = frogD3;
	frog.imgL1 = frogL1;frog.imgL2 = frogL2;frog.imgL3 = frogL3;
	frog.imgR1 = frogR1;frog.imgR2 = frogR2;frog.imgR3 = frogR3;
	// truck 
	int xOffset = 0;
	for (int i = 0; i < numOfTrucks; i++) {
		trucks[i].x = 200 - xOffset; trucks[i].y = 115; trucks[i].dx = 1;
		trucks[i].img = truck1Right;
		xOffset += 100; 
	}
	// cars 
	xOffset = 0;
	for (int i = 0; i < numOfCars; i++) {
		cars[i].x = 200 - xOffset; cars[i].y = 95; cars[i].dx = 4;
		cars[i].img = car1Right;
		xOffset += 60; // space between two cars
	}
	// crocodiles 
	xOffset = 0;
	for (int i = 0; i < numOfCrocodiles; i++) {
		crocodiles[i].x = 200 - xOffset; crocodiles[i].y = 55; crocodiles[i].dx = 1;
		crocodiles[i].img = crocodile;
		xOffset += 120; 
	}
	// woods 
	xOffset = 0;
	for (int i = 0; i < numOfWoods; i++) {
		woods[i].x = 10 + xOffset; woods[i].y = 35; woods[i].dx = 3;
		woods[i].img = wood;
		xOffset += 80; 
	}
}

static void drawBG() {
	fillScreen4(200);
	drawImage4(10, 10, 220, 140, frogBG);
}


static void drawTrucks() {
	for (int i = 0; i < numOfTrucks; i++) {
		if (trucks[i].x >= 230) {
			trucks[i].x = 0;
		}
		trucks[i].x += trucks[i].dx;
		drawImage4(trucks[i].x, trucks[i].y, 128, 16, trucks[i].img);
	}
}

static void drawCars() {
	for (int i = 0; i < numOfCars; i++) {
		if (cars[i].x + 30 <= 10) {
			cars[i].x = 220;
		}
		cars[i].x -= cars[i].dx;
		drawImage4(cars[i].x, cars[i].y, 16, 10, cars[i].img);
	}
}

static void drawWoods() {
	for (int i = 0; i < numOfWoods; i++) {
		if (woods[i].x + 30 <= 10) {
			woods[i].x = 220;
		}
		woods[i].x -= woods[i].dx;
		drawImage4(woods[i].x, woods[i].y, 42, 12, woods[i].img);
	}
}

static void drawCrocodiles() {
	for (int i = 0; i < numOfCrocodiles; i++) {
		if (crocodiles[i].x >= 230) {
			crocodiles[i].x = 0;
		}
		crocodiles[i].x += crocodiles[i].dx;
		drawImage4(crocodiles[i].x, crocodiles[i].y, 46, 16, crocodiles[i].img);
	}
}

static void drawFrog() {
	drawImage4(frog.x, frog.y, 14, 14, prevImg);
	if (KEY_DOWN_NOW(KEY_LEFT)) {
		frog.x -= frog.dx;
		drawImage4(frog.x, frog.y, 14, 14, frog.imgL1);
		drawImage4(frog.x, frog.y, 14, 14, frog.imgL2);
		drawImage4(frog.x, frog.y, 14, 14, frog.imgL3);
		prevImg = frog.imgL1;
	} else if (KEY_DOWN_NOW(KEY_RIGHT)) {
		frog.x += frog.dx;
		drawImage4(frog.x, frog.y, 14, 14, frog.imgR1);
		drawImage4(frog.x, frog.y, 14, 14, frog.imgR2);
		drawImage4(frog.x, frog.y, 14, 14, frog.imgR3);
		prevImg = frog.imgR1;
	} else if (KEY_DOWN_NOW(KEY_UP)) {
		frog.y -= frog.dy;
		drawImage4(frog.x, frog.y, 14, 14, frog.imgU1);
		drawImage4(frog.x, frog.y, 14, 14, frog.imgU2);
		drawImage4(frog.x, frog.y, 14, 14, frog.imgU3);
		prevImg = frog.imgU1;
	} else if (KEY_DOWN_NOW(KEY_DOWN)) {
		frog.y += frog.dy;
		drawImage4(frog.x, frog.y, 14, 14, frog.imgD1);
		drawImage4(frog.x, frog.y, 14, 14, frog.imgD2);
		drawImage4(frog.x, frog.y, 14, 14, frog.imgD3);
		prevImg = frog.imgD1;
	}
}	

static void collision() {
	int isSafe = 0;
	isSafe = gotHome(isSafe);

	int forgive = 5;
	if (frog.x <= 10 || frog.x >= 230 || frog.y <= 10 || frog.y >= 150) {
		numOfLives--;
		loseLife();
	}
	
	//crocodiles
	for (int i = 0; i < numOfCrocodiles; i++) {
		if (frog.x + 14 >= crocodiles[i].x &&
			frog.x <= crocodiles[i].x + 20 &&
			frog.y + 14 >= crocodiles[i].y &&
			frog.y <= crocodiles[i].y + 16 - forgive) {
			frog.x += crocodiles[i].dx;
			isSafe = 1;
		}
	}
	//woods
	for (int i = 0; i < numOfWoods; i++) {
		if (frog.x + 14 >= woods[i].x &&
			frog.x <= woods[i].x + 42 - forgive &&
			frog.y + 14 >= woods[i].y &&
			frog.y <= woods[i].y + 12 - forgive) {
			frog.x -= woods[i].dx;
			isSafe = 1;
		}
	}
	//cars
	for (int i = 0; i < numOfCars; i++) {
		if (frog.x + 14 >= cars[i].x &&
			frog.x <= cars[i].x + 16 - forgive &&
			frog.y + 14 >= cars[i].y &&
			frog.y <= cars[i].y + 10 - forgive) {
			loseLife();
		}
	}
	//trucks
		for (int i = 0; i < numOfTrucks; i++) {
			if (frog.x + 14 >= trucks[i].x &&
			frog.x <= trucks[i].x + 20 &&
			frog.y + 14 >= trucks[i].y &&
			frog.y <= trucks[i].y + 16 - forgive) {
			frog.x += trucks[i].dx;
			isSafe = 1;
		}
	}
	//river
	if (isSafe == 0 && frog.y <= 60 && frog.y >= 10) {
		loseLife();
	}
}

static void loseLife() {
	numOfLives--;
	if (numOfLives >= 0) {
		drawString4(95, 75, "LIVES   -1", PALETTE[1]);
		flipPage();
		delay(20);
		loadAvatar();
	} else {
		lose();
	}
}

static int gotHome(int isSafe) {
	if (homeFlags[0] == 0 && frog.y >= 10 && frog.y <= 30 && frog.x >= 10 && frog.x <= 25) {
		drawString4(95, 75, "YOU ARE SAVE NOW!", PALETTE[1]);
		flipPage();
		homeFlags[0] = 1;
		loadAvatar();
		isSafe = 1;
		delay(20);
	}
	if (homeFlags[0]) {
		drawImage4(18, 15, 14, 14, frog.imgU1);
	}
	if (homeFlags[1] == 0 && frog.y >= 10 && frog.y <= 30 && frog.x >= 60 && frog.x <= 70) {
		drawString4(95, 75, "YOU ARE SAVE NOW!", PALETTE[1]);
		flipPage();
		homeFlags[1] = 1;
		loadAvatar();
		isSafe = 1;
		delay(20);
	}
	if (homeFlags[1]) {
		drawImage4(66, 15, 14, 14, frog.imgU1);
	}
	if (homeFlags[2] == 0 && frog.y >= 10 && frog.y <= 30 && frog.x >= 108 && frog.x <= 118) {
		drawString4(95, 75, "YOU ARE SAVE NOW!", PALETTE[1]);
		flipPage();
		homeFlags[2] = 1;
		loadAvatar();
		isSafe = 1;
		delay(20);
	}
	if (homeFlags[2]) {
		drawImage4(114, 15, 14, 14, frog.imgU1);
	}
	if (homeFlags[3] == 0 && frog.y >= 10 && frog.y <= 30 && frog.x >= 155 && frog.x <= 165) {
		drawString4(95, 75, "YOU ARE SAVE NOW!", PALETTE[1]);
		flipPage();
		homeFlags[3] = 1;
		loadAvatar();
		isSafe = 1;
		delay(20);
	}
	if (homeFlags[3]) {
		drawImage4(160, 15, 14, 14, frog.imgU1);
	}
	if (homeFlags[4] == 0 && frog.y >= 10 && frog.y <= 30 && frog.x >= 195 && frog.x <= 210) {
		drawString4(95, 75, "YOU ARE SAVE NOW!", PALETTE[1]);
		flipPage();
		homeFlags[4] = 1;
		loadAvatar();
		isSafe = 1;
		delay(20);
	}
	if (homeFlags[4]) {
		drawImage4(208, 15, 14, 14, frog.imgU1);
	}
	if (homeFlags[0] && homeFlags[1] && homeFlags[2] && homeFlags[3] && homeFlags[4]) {
		win();
	}
	return isSafe;
}



