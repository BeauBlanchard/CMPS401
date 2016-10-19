// By: Blake Weimer
// By: Beau Blanchard
// Program 5.1 - GALAXIPAC
// CMPS 455
// 5-9-2016

#define USE_CONSOLE
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include "allegro.h"
#include <fstream>
#include <time.h>

#define SCREEN_H 750
#define SCREEN_W 700
#define TILESIZE 30
#define numEnemies 1
#define numBullets 1
#define rowMax 25
#define columnMax 23
#define explosionTimer 25
#define WALL '#'
#define CHARACTER 'C'
#define VOID ' '
#define LILFOOD '*'
#define BIGFOOD '@'
#define EMPTY 'E'
#define GHOST 'G'
#define UP 2
#define DOWN 3
#define LEFT 1
#define RIGHT 0

using namespace std;

BITMAP *buffer, *explosionSprite, *hero, *heroUDBullet, *heroLRBullet, *heroDeath, *ghost, *lilfood, *bigfood;
SAMPLE *bulletBoom, *gameMusic;

int blue, black, cyan, orange, green, red, white, yellow;

void loadColors(void);
void switchMap(void);
bool dead(void);
bool shootBullet(void);

char level[30][30];
auto frame = 0;
auto timer = 0;
auto indTimer = 0;
auto canShoot = 1;
auto dframe = 0;
auto died = false;
auto bulletLive = false;
auto currentMap = "level1.txt";
auto points = 0;
auto gameStart = true;
auto gameEnd = false;
auto moved = false;
bool once = true;

class MAP{
	public: int xHero, yHero, xGhost, yGhost, xPos, yPos, heroXPos, heroYPos;
			char map;
	
	void init(void){
		xHero = 0;
		yHero = 0;
		xGhost = 0;
		yGhost = 0;
		xPos = 0;
		yPos = 0;
	}

	static bool loadMap(char* map){
		ifstream in;
		in.open(map);
		for(auto i = 0; i < rowMax; i++){
			for(auto j = 0; j < columnMax; j++){
				in >> level[j][i];
			}
		}
		in.close();
		return true;
	}

	static void processMap(int score){
		for(auto i = 0; i < rowMax; i++){
			for(auto j = 0; j < columnMax; j++){
				if(level[j][i] == LILFOOD){
					circlefill(buffer, (j * TILESIZE) + 20, (i * TILESIZE) + 15, 3, white);
				}
				else if(level[j][i] == BIGFOOD){
					circlefill(buffer, (j * TILESIZE) + 20, (i * TILESIZE) + 15, 10, yellow);
				}
				else if(level[j][i] == EMPTY){
				//	circle(buffer, (j * TILESIZE) + 20, (i * TILESIZE) + 15, 15, red);
				}
				else if(level[j][i] == CHARACTER){
				//	circle(buffer, (j * TILESIZE) + 20, (i * TILESIZE) + 15, 15, yellow);
				}
				else if(level[j][i] == GHOST){		
				//	circle(buffer, (j * TILESIZE) + 20, (i * TILESIZE) + 15, 15, green);
				}
				else if(level[j][i] == WALL){
					//rect(buffer, (j * TILESIZE) + 7, (i * TILESIZE) + 1, ((j * TILESIZE) + 25) + 7, (i * TILESIZE) + 26, blue);
					if(currentMap == "level1.txt"){
						rect(buffer, (j * TILESIZE) + 7, (i * TILESIZE) + 1, ((j * TILESIZE) + 25) + 7, (i * TILESIZE) + 26, blue);
					}
					if(currentMap == "level2.txt"){
						rect(buffer, (j * TILESIZE) + 7, (i * TILESIZE) + 1, ((j * TILESIZE) + 25) + 7, (i * TILESIZE) + 26, green);
					}
					if(currentMap == "level3.txt"){
						rect(buffer, (j * TILESIZE) + 7, (i * TILESIZE) + 1, ((j * TILESIZE) + 25) + 7, (i * TILESIZE) + 26, red);
					}
				}

			}
			textprintf_ex(buffer, font, 142, 13, white, -1, "SCORE: %d", score);
		}
	}

};

MAP myMap;

class HERO{
	public:	int xPos, yPos, direction, lastDirection;
			float allegroHeading, degreeHeading, offset, xTurrent, yTurrent;
			bool freeSpot;
			
	void init(int x, int y){
		allegroHeading = 0;
		degreeHeading = 0;
		direction = RIGHT;
		xPos = x;
		yPos = y;
		xTurrent = 0;
		yTurrent = 0;
		freeSpot = false;
	}
	
	void moveHero(void){
		auto spriteWidth = 31;
		auto spriteHeight = 29;
		auto xScaled = ((xPos * TILESIZE) + 1);
		auto yScaled = ((yPos * TILESIZE) - 1);
		
		if(moved == false){
			masked_blit(hero, buffer, 1 * spriteWidth, direction * spriteHeight, xScaled, yScaled, spriteWidth, spriteHeight);
			draw_sprite(screen, buffer, 0, 0);
		}
		if(!key[KEY_W] && !key[KEY_A] && !key[KEY_S] && !key[KEY_D] || key[KEY_W] && key[KEY_D] || key[KEY_S] && key[KEY_D] || key[KEY_A] && key[KEY_S] || key[KEY_W] && key[KEY_A] || key[KEY_W] && key[KEY_S] || key[KEY_A] && key[KEY_D]){
			moved = false;
			masked_blit(hero, buffer, 1 * spriteWidth, direction * spriteHeight, xScaled, yScaled, spriteWidth, spriteHeight);
			draw_sprite(screen, buffer, 0, 0);
		}
		if(bulletLive == false){
			if(key[KEY_W] && !key[KEY_A] && !key[KEY_S] && !key[KEY_D]){
				moved = true;
				direction = UP;
				if(canMove(direction)){
					masked_blit(hero, buffer, frame * spriteWidth, direction * spriteHeight, xScaled, yScaled, spriteWidth, spriteHeight);
					draw_sprite(screen, buffer, 0, 0);
					//clear_bitmap(buffer);
					rest(65);
					frame++;
					if(frame == 3){
						frame = 0;
					}
				}
			}
			if(key[KEY_S] && !key[KEY_A] && !key[KEY_W] && !key[KEY_D]){
				moved = true;
				direction = DOWN;
				if(canMove(direction)){
					masked_blit(hero, buffer, frame * spriteWidth, direction * spriteHeight, xScaled, yScaled, spriteWidth, spriteHeight);
					draw_sprite(screen, buffer, 0, 0);
					//clear_bitmap(buffer);
					rest(65);
					frame++;
					if(frame == 3){
						frame = 0;
					}
				}
			}
			if(key[KEY_A] && !key[KEY_W] && !key[KEY_S] && !key[KEY_D]){
				moved = true;
				direction = LEFT;
				if(canMove(direction)){
					masked_blit(hero, buffer, frame * 33, direction * spriteHeight, xScaled, yScaled, spriteWidth, spriteHeight);
					draw_sprite(screen, buffer, 0, 0);
					//clear_bitmap(buffer);
					rest(65);
					frame++;
					if(frame == 3){
						frame = 0;
					}
				}
			}
			if(key[KEY_D] && !key[KEY_A] && !key[KEY_S] && !key[KEY_W]){
				moved = true;
				direction = RIGHT;
				if(canMove(direction)){
					masked_blit(hero, buffer, frame * spriteWidth, direction * spriteHeight, xScaled, yScaled, spriteWidth, spriteHeight);
					draw_sprite(screen, buffer, 0, 0);
					//clear_bitmap(buffer);
					rest(65);
					frame++;
					if(frame == 3){
						frame = 0;
					}
				}
			}
			clear_bitmap(buffer);
		}
		xTurrent = xPos;
		yTurrent = yPos;
	}

	bool canMove(int direction){
		if(bulletLive == false){
			if(direction == UP && (level[xPos][yPos - 1] == VOID || level[xPos][yPos - 1] == CHARACTER || level[xPos][yPos - 1] == EMPTY || level[xPos][yPos - 1] == LILFOOD || level[xPos][yPos - 1] == BIGFOOD || level[xPos][yPos - 1] == GHOST)){
				freeSpot = true;
				--yPos;
			}
			if(direction == DOWN && (level[xPos][yPos + 1] == VOID || level[xPos][yPos + 1] == CHARACTER || level[xPos][yPos + 1] == EMPTY || level[xPos][yPos + 1] == LILFOOD || level[xPos][yPos + 1] == BIGFOOD || level[xPos][yPos + 1] == GHOST)){
				freeSpot = true;
				++yPos;
			}
			if(direction == LEFT && (level[xPos - 1][yPos] == VOID || level[xPos - 1][yPos] == CHARACTER || level[xPos - 1][yPos] == EMPTY || level[xPos - 1][yPos] == LILFOOD || level[xPos - 1][yPos] == BIGFOOD || level[xPos - 1][yPos] == GHOST)){
				freeSpot = true;
				--xPos;
				if(xPos - 1 == -1){
					xPos = 22;
				}
			}
			if(direction == RIGHT && (level[xPos + 1][yPos] == VOID || level[xPos + 1][yPos] == CHARACTER || level[xPos + 1][yPos] == EMPTY || level[xPos + 1][yPos] == LILFOOD || level[xPos + 1][yPos] == BIGFOOD || level[xPos + 1][yPos] == GHOST)){
				freeSpot = true;
				++xPos;
				if(xPos + 1 == 23){
					xPos = 0;
				}
			}
			if(level[xPos][yPos] == WALL){
				freeSpot = false;
			}
		}
		calculateHeading();
		return freeSpot;
	}

	void calculateHeading(void){
		if(direction == UP){
			degreeHeading = 90;
			allegroHeading = 192;
		}
		if(direction == DOWN){
			degreeHeading = 270;
			allegroHeading = 64;
		}
		if(direction == LEFT){
			degreeHeading = 180;
			allegroHeading = 128;
		}
		if(direction == RIGHT){
			degreeHeading = 0;
			allegroHeading = 0;
		}
	}

	static void findHero(int& x, int& y){
		for(auto i = 0; i < rowMax; i++){
			for(auto j = 0; j < columnMax; j++){
				if(level[j][i] == CHARACTER){
					x = j;
					y = i;
				}
			}
		}
	}

	void heroDeathAnimation(void){
		direction = UP;
		auto spriteWidth = 20;
		auto spriteHeight = 24;
		auto xScaled = ((xPos * TILESIZE) - 0);
		auto yScaled = ((yPos * TILESIZE) + 0);

		circlefill(buffer, xScaled + 20, yScaled + 13, 16, black);
		masked_blit(heroDeath, buffer, dframe * spriteWidth, 0 * spriteHeight, xScaled + 9, yScaled + 1, spriteWidth, spriteHeight);
		draw_sprite(screen, buffer, 0, 0);
		clear_bitmap(buffer);
		dframe++;
		rest(175);
		if(dframe == 11){
			rest(1000);
			level[xPos][yPos] = GHOST;
		}
	}

	int score(void)const{
		auto score = 0;
		int decUp = yPos - 1;
		int decLeft = xPos - 1;

		if(level[xPos][yPos] == LILFOOD){
			score += 50;
		}
		if(level[xPos][yPos] == BIGFOOD){
			score += 100;
		}
		level[xPos][yPos] = VOID;
		return score;
	}
};

HERO myHero;

class ENEMY{
	public: int xPos, yPos, eframe, direction, living;
			bool alive, enemyExplosion, freeSpot;

	void init(int x, int y){
		eframe = 0;
		alive = true;
		xPos = x;
		yPos = y;
		direction = UP;
		enemyExplosion = false;
		freeSpot = false;
		living = numEnemies;
	}
	
	void drawEnemy(int direction, int& prevTile){
		auto ghostWidth = 22;
		auto ghostHeigth = 21;
		auto xScaled = ((xPos * TILESIZE) - 0);
		auto yScaled = ((yPos * TILESIZE) + 0);

		if(canMove(direction) && dframe < 5){
			level[xPos][yPos] = prevTile;
			if(direction == UP){
				eframe = 0;
				masked_blit(ghost, buffer, eframe * ghostWidth, 0 * ghostHeigth, xScaled + 7, yScaled + 5, ghostWidth, ghostHeigth);
				if(moved == false){
					rest(65);
				}
				eframe++;
				if(eframe == 1){
					eframe = 0;
				}
			}
			if(direction == DOWN){
				eframe = 2;
				masked_blit(ghost, buffer, eframe * ghostWidth, 0 * ghostHeigth, xScaled + 9, yScaled + 5, ghostWidth, ghostHeigth);
				if(moved == false){
					rest(65);
				}
				eframe++;
				if(eframe == 3){
					eframe = 2;
				}
			}
			if(direction == LEFT){
				eframe = 4;
				masked_blit(ghost, buffer, eframe * ghostWidth, 0 * ghostHeigth, xScaled + 11, yScaled + 5, ghostWidth, ghostHeigth);
				if(moved == false){
					rest(65);
				}
				eframe++;
				if(eframe == 5){
					eframe = 4;
				}
			}
			if(direction == RIGHT){
				eframe = 6;
				masked_blit(ghost, buffer, eframe * ghostWidth, 0 * ghostHeigth, xScaled + 13, yScaled + 5, ghostWidth, ghostHeigth);
				if(moved == false){
					rest(65);
				}
				eframe++;
				if(eframe == 7){
					eframe = 6;
				}
			}
		}
		if(level[xPos][yPos] != GHOST && level[xPos][yPos] != CHARACTER){
			level[xPos][yPos] = level[xPos][yPos];
		}
		if(level[xPos][yPos] == CHARACTER){
			prevTile = VOID;
		}
		level[xPos][yPos] = GHOST;
	}

	bool canMove(int direction){
		if(direction == UP && (level[xPos][yPos - 1] == VOID || level[xPos][yPos - 1] == CHARACTER || level[xPos][yPos - 1] == EMPTY || level[xPos][yPos - 1] == LILFOOD || level[xPos][yPos - 1] == BIGFOOD || level[xPos][yPos - 1] == GHOST)){
			freeSpot = true;
			yPos--;
		}
		if(direction == DOWN && (level[xPos][yPos + 1] == VOID || level[xPos][yPos + 1] == CHARACTER || level[xPos][yPos + 1] == EMPTY || level[xPos][yPos + 1] == LILFOOD || level[xPos][yPos + 1] == BIGFOOD || level[xPos][yPos + 1] == GHOST)){
			freeSpot = true;
			yPos++;
		}
		if(direction == LEFT && (level[xPos - 1][yPos] == VOID || level[xPos - 1][yPos] == CHARACTER || level[xPos - 1][yPos] == EMPTY || level[xPos - 1][yPos] == LILFOOD || level[xPos - 1][yPos] == BIGFOOD || level[xPos - 1][yPos] == GHOST)){
			freeSpot = true;
			xPos--;
		}
		if(direction == RIGHT && (level[xPos + 1][yPos] == VOID || level[xPos + 1][yPos] == CHARACTER || level[xPos + 1][yPos] == EMPTY || level[xPos + 1][yPos] == LILFOOD || level[xPos + 1][yPos] == BIGFOOD || level[xPos + 1][yPos] == GHOST)){
			freeSpot = true;
			xPos++;
		}
		if(level[xPos][yPos] == WALL){
			freeSpot = false;
		}
		return freeSpot;
	}

	void drawExplosion(void){
		auto xScaled = ((xPos * TILESIZE) - 0);
		auto yScaled = ((yPos * TILESIZE) + 0);
		auto xOffset = 20;
		auto yOffset = 20;

		if(timer < explosionTimer){
			level[xPos][yPos] = EMPTY;
			circle(buffer, xScaled + xOffset, yScaled + yOffset, timer + 30, blue);
			circle(buffer, xScaled + xOffset, yScaled + yOffset, timer + 25, blue);
			circle(buffer, xScaled + xOffset, yScaled + yOffset, timer + 20, blue);
			circle(buffer, xScaled + xOffset, yScaled + yOffset, timer + 15, blue);
			circle(buffer, xScaled + xOffset, yScaled + yOffset, timer + 10, blue);
			circle(buffer, xScaled + xOffset, yScaled + yOffset, timer + 5, blue);
			circle(buffer, xScaled + xOffset, yScaled + yOffset, timer + 0, blue);
			timer = timer + 1;
		}
		else{
			timer = 0;
			enemyExplosion = false;
		}
	}
	static void findEnemy(int& x, int& y){
		for(auto i = 0; i < rowMax; i++){
			for(auto j = 0; j < columnMax; j++){
				if(level[j][i] == GHOST){
					x = j;
					y = i;
				}
			}
		}
	}

	void moveEnemy(int& prevTile)const{
			level[xPos][yPos] = prevTile;
	}

	static int randDirection(void){
		auto direction = rand() % 4;
		return direction;
	}
};

ENEMY myEnemy[numEnemies];

class BULLET{
	public: int xPos, yPos, xExplosion, yExplosion, heading;
			float direction;
			bool bulletExplosion;

	void init(void){
		xPos = 0;
		yPos = 0;
		bulletExplosion = false;
		heading = 0;
	}

	void drawBullet(void)const{
		auto UDspriteWidth = 25;
		auto UDspriteHeigth = 40;
		auto LRSpriteWidth = 40;
		auto LRSpriteHeigth = 25;
		auto xScaled = ((xPos * TILESIZE) - 0);
		auto yScaled = ((yPos * TILESIZE) + 0);

		if(myHero.direction == UP){
			masked_blit(heroUDBullet, buffer, 0 * UDspriteWidth, 0 * UDspriteHeigth, xScaled + 8.0, yScaled + 23, UDspriteWidth, UDspriteHeigth);
			draw_sprite(screen, buffer, 0, 0);
		}
		if(myHero.direction == DOWN){
			masked_blit(heroUDBullet, buffer, 0 * UDspriteWidth, 0 * UDspriteHeigth, xScaled + 8.0, yScaled - 33, UDspriteWidth, UDspriteHeigth);
			draw_sprite(screen, buffer, 0, 0);
			rest(25);
		}
		if(myHero.direction == LEFT){
			masked_blit(heroLRBullet, buffer, 0 * LRSpriteWidth, 0 * LRSpriteHeigth, xScaled + 27, yScaled + 3.5, LRSpriteWidth, LRSpriteHeigth);
			draw_sprite(screen, buffer, 0, 0);
			rest(25);
		}
		if(myHero.direction == RIGHT){
			masked_blit(heroLRBullet, buffer, 0 * LRSpriteWidth, 0 * LRSpriteHeigth, xScaled - 27, yScaled + 3.5, LRSpriteWidth, LRSpriteHeigth);
			draw_sprite(screen, buffer, 0, 0);
			rest(25);
		}
		clear_bitmap(buffer);
	}

	void moveBullet(){
		auto xScaled = ((xPos * TILESIZE) - 0);
		auto yScaled = ((yPos * TILESIZE) + 0);

		if(myHero.direction == UP){
			--yPos;
		}
		if(myHero.direction == DOWN){
			++yPos;
		}
		if(myHero.direction == LEFT){	
			--xPos;
		}
		if(myHero.direction == RIGHT){
			++xPos;
		}

		for(auto i = 0; i < numBullets; i++){
			if(level[xPos][yPos] == WALL || xPos <= 0 || xPos >= 23){
				bulletLive = false;
				bulletExplosion = true;
				xExplosion = xScaled + 21;
				yExplosion = yScaled + 16;
				play_sample(bulletBoom, 125, 127, 1000, false);
			}
			if(level[xPos][yPos] == GHOST){
				bulletLive = false;
				bulletExplosion = true;
				myEnemy[i].alive = false;
				xExplosion = xScaled + 21;
				yExplosion = yScaled + 16;
				myEnemy[i].enemyExplosion = true;
				myEnemy->living = myEnemy->living - 1;
				level[xPos][yPos] = EMPTY;
			}
		}
	}

	void drawExplosion(void){
		auto i = 0;
		if(i == 100){
		//if(timer < explosionTimer){
			play_sample(bulletBoom, 75, 127, 1000, false);
			circle(buffer, xExplosion, yExplosion, timer + 10, yellow);
			circle(buffer, xExplosion, yExplosion, timer + 8, yellow);
			circle(buffer, xExplosion, yExplosion, timer + 6, orange);
			circle(buffer, xExplosion, yExplosion, timer + 4, orange);
			circle(buffer, xExplosion, yExplosion, timer + 2, red);
			circle(buffer, xExplosion, yExplosion, timer, red);
			timer = timer + 1;
		}
		else{
			timer = 0;
			bulletExplosion = false;
		}
	}

	 int  intDistance (int x0, int y0, int x1, int y1)const{	 
		return sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
	}
};

BULLET myBullet[numBullets];

volatile int ctimer;
void increment_timer(void){
	ctimer++;
}
END_OF_FUNCTION(increment_timer);

void freeze_timer(void){
	ctimer = ctimer;
}
END_OF_FUNCTION(freeze_timer);

int main(){
	allegro_init();
	install_timer();
	install_keyboard();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_W, SCREEN_H, 0, 0);
	set_window_title("GALAXIPAC");
	buffer = create_bitmap(700, 750);
	hero = load_bitmap("Pacman_Sheet.bmp", nullptr);
	ghost = load_bitmap("Ghost_Move.bmp", nullptr);
	explosionSprite = load_bitmap("Explosion_Sheet.bmp", nullptr);
	heroUDBullet = load_bitmap("Pacman_UDBullet.bmp", nullptr);
	heroLRBullet = load_bitmap("Pacman_LRBullet.bmp", nullptr);
	heroDeath = load_bitmap("Pacman_Death.bmp", nullptr);
	loadColors();

	ctimer = 0;
	LOCK_FUNCTION(increment_timer);
	LOCK_VARIABLE(ctimer);
	install_int_ex(increment_timer, SECS_TO_TIMER(1));

	switchMap();

	if(install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "") != 0){
		allegro_message("Error initializing sound system.");
		return 1;
	}
	gameMusic = load_sample("gameMusic.wav");
	if(!gameMusic){
		allegro_message("Error reading gameMusic wav file.");
		return 1;
	}
	bulletBoom = load_sample("DepthChargeShort.wav");
	if(!bulletBoom){
		allegro_message("Error reading missileBoom wav file.");
		return 1;
	}

	auto hero_direction = RIGHT;
	auto hero_x = 0, hero_y = 0;
	myHero.findHero(hero_x, hero_y);
	auto died = false;
	myHero.init(hero_x, hero_y);

	auto ghost_dir = UP;
	auto ghost_x = 0, ghost_y = 0;
	for(auto i = 0; i < numEnemies; i++){
		myEnemy[i].findEnemy(ghost_x, ghost_y);
	}

	for(auto i = 0; i < numEnemies; i++){
		myEnemy[i].init(ghost_x, ghost_y);
	}

	int prevTile = VOID;

	srand(time(nullptr));

	while(!key[KEY_ESC]){
		textout_ex(buffer, font, "GALAXIPAC", 325, 300, green, -1);
		textout_ex(buffer, font, "Press [ENTER] to begin", 275, 350, white, -1);

		acquire_screen();
		blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
		release_screen();

		if(gameStart){
			play_sample(gameMusic, 100, 127, 1000, false);
		}
		gameStart = false;

		while(key[KEY_ENTER] || key[KEY_ENTER_PAD]){
			clear_to_color(buffer, black);
			while(!key[KEY_ESC]){
				switchMap();
				myMap.processMap(points);

				textprintf_ex(buffer, font, 530, 13, white, -1, "TIME: %d", ctimer);

				if(died == true){
					install_int_ex(increment_timer, SECS_TO_TIMER(0));
				}
				if(died == true && dframe > 20){
					while(!key[KEY_ESC]){
						clear_to_color(buffer, black);
						textprintf_ex(buffer, font, 142, 13, white, -1, "SCORE: %d", points);
						textprintf_ex(buffer, font, 530, 13, white, -1, "TIME: %d", ctimer);
						rect(buffer, 300, 325, 400, 400, red);
						textout_ex(buffer, font, "GAME OVER", 315, 350, red, -1);
						textout_ex(buffer, font, "YOU LOSE", 319, 370, red, -1);
						textout_ex(buffer, font, "PRESS [ESC] TO EXIT, SCRUB", 245, 732, white, -1);
						blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
						while(key[KEY_SPACE]){
							while(!key[KEY_ESC]){
								if(key[KEY_ENTER] || key[KEY_ENTER_PAD] || key[KEY_ESC]){
									return 0;
								}
							}
						}
					}
				}

				for(auto i = 0; i < numBullets; i++){
					if(myBullet[i].bulletExplosion == true){
						myBullet[i].drawExplosion();
					}
				}

				for(auto i = 0; i < numEnemies; i++){
					if(myEnemy[i].alive == true){
						myEnemy[i].moveEnemy(prevTile);
					}
				}

				for(auto i = 0; i < numEnemies; i++){
					if(myEnemy[i].alive == true){
						myEnemy[i].drawEnemy(myEnemy->randDirection(), prevTile);
						//myEnemy[i].drawEnemy(myHero.direction);

					}
				}

				for(auto i = 0; i < numEnemies; i++){
					if(myEnemy[i].enemyExplosion == true){
						myEnemy[i].drawExplosion();
					}
				}

				acquire_screen();
					blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
						if(!died && timer == 0){
							myHero.moveHero();
							points += myHero.score();
							for(auto i = 0; i < numEnemies; i++){
								died = dead();
							}
						}
						if(died == true){
							myHero.heroDeathAnimation();
						}
				release_screen();

				for(auto i = 0; i < numBullets; i++){
					if(bulletLive == true){
						myBullet[i].moveBullet();
					}
				}

				for(auto i = 0; i < numBullets; i++){
					if(bulletLive == true){
						myBullet[i].drawBullet();
					}
				}

				if(key[KEY_SPACE] && (died == false) && (canShoot == 1) && (!key[KEY_W] && !key[KEY_A] && !key[KEY_S] && !key[KEY_D])){
					shootBullet();
					canShoot = 0;
				}
				if(!key[KEY_SPACE]){
					if(canShoot == 0){
						canShoot = 1;
					}
				}
				if(gameEnd == true){
					while(!key[KEY_ESC]){
						clear_to_color(buffer, black);
						textprintf_ex(buffer, font, 142, 13, white, -1, "SCORE: %d", points);
						textprintf_ex(buffer, font, 530, 13, white, -1, "TIME: %d", ctimer);
						rect(buffer, 300, 325, 405, 400, red);
						textout_ex(buffer, font, "GOOD JOB", 323, 350, green, -1);
						textout_ex(buffer, font, "YOU WON", 326, 370, green, -1);
						textout_ex(buffer, font, "PRESS [ESC] TO EXIT, SCRUB", 245, 732, white, -1);
						blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
						while(key[KEY_SPACE]){
							while(!key[KEY_ESC]){
								if(key[KEY_ENTER] || key[KEY_ENTER_PAD] || key[KEY_ESC]){
									return 0;
								}
							}
						}
					}
				}
			}
		}
		rest(50);
		clear_bitmap(buffer);
	}
	destroy_bitmap(buffer);
	destroy_bitmap(hero);
	destroy_bitmap(heroLRBullet);
	destroy_bitmap(heroUDBullet);
	destroy_bitmap(ghost);
	return 0;
};

void loadColors(void){
	white = makecol(255, 255, 255);
	black = makecol(0, 0, 0);
	yellow = makecol(255, 255, 0);
	orange = makecol(255, 165, 0);
	red = makecol(150, 0, 0);
	blue = makecol(0, 0, 255);
	green = makecol(0, 255, 0);
	cyan = makecol(0, 255, 255);
}

void switchMap(void){
	myMap.init();
	bool Map1 = false;
	bool Map2 = false;
	bool Map3 = false;

	if(points == 0){
		Map1 = true;
	}
	if(points >= 3500){
		Map1 = false;
		Map2 = true;
	}
	if(points >= 7000){
		Map1 = false;
		Map2 = false;
		Map3 = true;
	}

	if(points == 0){
		currentMap = "level1.txt";
		myMap.loadMap("level1.txt");
	}
	if(points == 3500){
		myMap.processMap(points);
		currentMap = "level2.txt";
		myMap.loadMap("level2.txt");

		auto hero_direction = RIGHT;
		auto hero_x = 0, hero_y = 0;
		myHero.findHero(hero_x, hero_y);
		auto died = false;
		myHero.init(hero_x, hero_y);

		auto ghost_dir = UP;
		auto ghost_x = 0, ghost_y = 0;
		for(auto i = 0; i < numEnemies; i++){
			myEnemy[i].findEnemy(ghost_x, ghost_y);
		}
		for(auto i = 0; i < numEnemies; i++){
			myEnemy[i].init(ghost_x, ghost_y);
		}
	}
	if(points == 7000 || points == 7050){
		myMap.processMap(points);
		currentMap = "level3.txt";
		myMap.loadMap("level3.txt");

		auto hero_direction = RIGHT;
		auto hero_x = 0, hero_y = 0;
		myHero.findHero(hero_x, hero_y);
		auto died = false;
		myHero.init(hero_x, hero_y);

		auto ghost_dir = UP;
		auto ghost_x = 0, ghost_y = 0;
		for(auto i = 0; i < numEnemies; i++){
			myEnemy[i].findEnemy(ghost_x, ghost_y);
		}
		for(auto i = 0; i < numEnemies; i++){
			myEnemy[i].init(ghost_x, ghost_y);
		}
	}
	if(points >= 10500 && gameEnd == false){
		gameEnd = true;
	}
}

bool dead(void){
	auto dead = false;
	for(auto i = 0; i < numEnemies; i++){
		if(myHero.xPos == myEnemy[i].xPos && myHero.yPos == myEnemy[i].yPos){
			level[myHero.xPos][myHero.yPos] = GHOST;
			dead = true;
		}
	}
	return dead;
}

bool shootBullet(void){
	auto fired = false;
	auto i = 0;
	while(!fired && i < numBullets){
		if(bulletLive == false){
			bulletLive = true;
			myBullet[i].direction = myHero.allegroHeading;
			myBullet[i].heading = myHero.degreeHeading;
			myBullet[i].xPos = myHero.xTurrent;
			myBullet[i].yPos = myHero.yTurrent;
			fired = true;
		}
		else{
			i++;
		}
	}
	return fired;
}
