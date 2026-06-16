#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

// define max and min
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define MAX_CHARACTERS 4
#define CORNERS 4
#define MAX_HEALTH 5
#define MAX_HEALTH_HM 3
#define MAX_GANON_HEALTH 3
#define MAX_GANON_HEALTH_HM 5

#define ZELDA_R 128
#define ZELDA_C 208
#define ZELDA_FIRE 8

#define CORNERS 4

#define ZELDA_FIRE 8

enum gba_state {
  START,
  PLAY,
  WIN,
  LOSE,
};

enum c_direction {
    DOWN,
	UP,
	LEFT,
	RIGHT,
};

struct obj {
    int row;
    int col;
    int spawn;
    int cd;
    int rd;
    int height;
    int width;
    enum c_direction direction;
    int corners[CORNERS];
    const u16 *image;
};

struct character {
	int row;
	int col;
	int size;
	int health;
	int cd;
	int rd;
	enum c_direction direction;
	int corners[CORNERS];
	const u16 *image;
} ;


void adjustCharacterPosition(struct character *character, int objectCol, int objectRow, int objectWidth, int objectHeight);

#endif
