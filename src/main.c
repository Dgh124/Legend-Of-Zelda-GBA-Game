#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

// start screen
#include "start_screen.h"

// back animation
#include "link_back.h"
#include "back_walk.h"
// right animation
#include "link_right.h"
#include "right_walk.h"
// front animation
#include "front_walk.h"
#include "link_front.h"
// left animation
#include "left_walk.h"
#include "left_link.h"
// sword
#include "sword_down.h"
#include "sword_up.h"
#include "sword_right.h"
#include "sword_left.h"
//fire
#include "fire.h"
// end screen
#include "game_over.h"
// heart
#include "heart.h"

// backdrop
#include "back_drop.h"

// ganons
#include "ganon1.h"
#include "ganon2.h"
#include "ganon3.h"
#include "ganon4.h"
#include "ganon5.h"

// hard mode ganons
#include "h_ganon1.h"
#include "h_ganon2.h"
#include "h_ganon3.h"
#include "h_ganon4.h"
#include "h_ganon5.h"

// win screen
#include "win_screen.h"

// purple heart
#include "purple_heart.h"

// include zeldas
#include "zelda1.h"
#include "zelda2.h"

// more fire
#include "z_fire1.h"
#include "z_fire2.h"


// Function to adjust character position upon collision with an object
void adjustCharacterPosition(struct character *character, int objectCol, int objectRow, int objectWidth, int objectHeight) {
    int charWidth  = character->size;
    int charHeight = character->size;

    // Calculate the intersection rectangle
    int colOverlap = max(0, min(character->col + charWidth, objectCol + objectWidth) - max(character->col, objectCol));
    int rowOverlap = max(0, min(character->row + charHeight, objectRow + objectHeight) - max(character->row, objectRow));

    // Determine which axis has the least overlap
    if (colOverlap < rowOverlap) {
        // Adjust character position horizontally
        if (character->col < objectCol) {
            character->col -= colOverlap;
        } else {
            character->col += colOverlap;
        }
    } else {
        // Adjust character position vertically
        if (character->row < objectRow) {
            character->row -= rowOverlap;
        } else {
            character->row += rowOverlap;
        }
    }
}


struct character player, ganon;

struct obj sword;
struct obj obj_fire;
struct obj *z_fire[ZELDA_FIRE];


int colors[] = {WHITE, RED, GREEN, BLUE, CYAN, YELLOW };
int ncolors = sizeof(colors) / sizeof(colors[0]);
int color_ind = 0;

char *text = "Press back to try again.";

char *hint = "Don't let Ganon (or his fire) hit you.";
char *hint1 = "Hint: Use your sword!";

struct character *cc, *pc;
struct character *cg, *pg;
struct obj *bp, *obp;
struct obj *fp, *ofp;

int high_score = 0;
char score[50];
    

int main(void) {
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = BG2_ENABLE | MODE3;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  enum gba_state state = START;

  //struct character player;
  int started = 0;

  // start time
  int time = 61;

  // frame counter
  int t_count = 0;

  // sword cool down
  int t_sword = 0;

  // collision cool down
  int t_collision = 0;

  // ganon animation
  int t_ganon_anim = 0;

  // fireball cool down
  int f_cooldown = 0;

  // ganon movement
  int t_ganon = 0;

  // zelda animation
  int z_count = 0;

  // option for selecting hard mode
  int hard_mode_aval = 0;
  int hard_mode_sel = 0;


  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons
	t_count += 1;
	t_sword += 1;
	t_collision += 1;
	t_ganon_anim += 1;
	t_ganon += 1;
	f_cooldown += 1;
	z_count += 1;


	if (KEY_JUST_PRESSED(BUTTON_SELECT, BUTTONS, previousButtons)) {
		state = START;
		started = 0;
	}

	if (time < 0) {
		state = LOSE;
	}

	// if enter was pressed, set state to PLAY

    switch (state) {
      case START:

	  waitForVBlank();
		// Start screen
		drawFullScreenImageDMA(start_screen);
		// Erase and draw the new score
		sprintf(score, "Best: %d", high_score);
	
		int score_col = 18;
		int score_cd = 1;

		if (hard_mode_aval) {
			char *hm_str = "HARD MODE UNLOCKED: PRESS A";
			drawString(HEIGHT - 20, 40, hm_str, RED);
		}

		while (!KEY_JUST_PRESSED(BUTTON_START, BUTTONS, previousButtons) && !(KEY_JUST_PRESSED(BUTTON_A, BUTTONS, previousButtons) && hard_mode_aval)) {
			score_col += score_cd;
			if (score_col <= 18) {
				score_col = 18;
				score_cd = -score_cd;
			}
			if (score_col >= 175) {
				score_col = 175;
				score_cd = -score_cd;
			}

			waitForVBlank();
			undrawImageDMA(8, score_col, 70, 10, start_screen);

			drawString(8, score_col, score, colors[color_ind]);
			if (score_col % 60 == 0) {
				color_ind = (color_ind + 1) % ncolors;	
			}
		}

		if (KEY_JUST_PRESSED(BUTTON_A, BUTTONS, previousButtons) && hard_mode_aval) {
			hard_mode_sel = 1;
		} else {
			hard_mode_sel = 0;
		}

	  	time = hard_mode_sel ? 31 : 61;

		// Initialize player
		pc = cc = &player;
		cc->health = hard_mode_sel ? MAX_HEALTH_HM : MAX_HEALTH;
		cc->row = HEIGHT/2;
		cc->col = WIDTH/4;
		cc->size = 16;
		cc->image = link_front;
		cc->direction = DOWN;

		// Initialize ganon
		pg = cg = &ganon;
		cg->health = hard_mode_sel ? MAX_GANON_HEALTH_HM : MAX_GANON_HEALTH;
		cg->row = HEIGHT/2;
		cg->col = 180;
		cg->size = 32;
		cg->image = hard_mode_sel ? h_ganon1 : ganon1;
		cg->rd = -1;
		cg->cd = 0;

		// Initialize sword
		obp = bp = &sword;
		bp->height = 16;
		bp->width = 16;
		bp->image = sword_right;
		bp->direction = RIGHT;
		bp->cd = 5;
		bp->rd = 5;

		//Initialize fire
		ofp = fp = &obj_fire;
		fp->height = 16;
		fp->width = 16;
		fp->image = fire;
		fp->spawn = 1;
		fp->direction = RIGHT;
		fp->row = cg->row;
		fp->col = cg->col;
		fp->cd = hard_mode_sel ? -2 : -1;
		fp->rd = 0;


		state = PLAY;

        // state = ?
        break;
      case PLAY:

		if (cc->health <= 0) {
			state = LOSE;
			time = time < high_score ? high_score : time;
			break;
		}

		// randomly choose z_fire images
		for (int i = 0; i < ZELDA_FIRE; i++) {
			int r = rand() % 2;
			z_fire[i]->image = r > 0 ? z_fire1 : z_fire2;
		}

		waitForVBlank();

		//drawString(5, 30, warning, YELLOW);

		if (started == 0) {
			fillScreenDMA(BLACK);
			//drawFullScreenImageDMA(back_drop);
			started = 1;
		}

		
		// Erase old ganon
		drawRectDMA(cg->row,cg->col, 32, 32, BLACK);


		// Undraw hearts
		for (int i = MAX_HEALTH; i > cc->health; i--) {
			drawRectDMA(5, i * 16, 16, 16, BLACK);
		}

		// Draw hearts
		if (t_count > 15) {
			for (int i = 1; i < cc->health + 1; i++) {
			drawImageDMA(5, i * 16, 16, 16, heart);
		}
		}

		// Flash hearts
		if (t_count > 59) {
			for (int i = 1; i < cc->health + 1; i++) {
				drawRectDMA(5, i * 16, 16, 16, BLACK);
			}
		} 

		// Draw zelda 
		if (z_count > 200)  {
			z_count = 0;
			drawImageDMA(ZELDA_R, ZELDA_C, 16, 16, zelda2);
		} else if (z_count > 100) {
			drawImageDMA(ZELDA_R, ZELDA_C, 16, 16, zelda1);
		} else  {
			drawImageDMA(ZELDA_R, ZELDA_C, 16, 16, zelda2);
		}
		
		// Undraw ganon hearts
		for (int i = hard_mode_sel ? MAX_GANON_HEALTH_HM : MAX_GANON_HEALTH ; i > cg->health; i--) {
			undrawImageDMA(5, (i * 16) + 130, 16, 16, BLACK);
		}

		// Draw ganon hearts
		for (int i = 1; i < cg->health + 1; i++) {
			drawImageDMA(5, (i * 16) + 130, 16, 16, purple_heart);
		}

		// Undraw fire
		drawRectDMA(fp->row, fp->col, 16, 16, BLACK);

		//Erase old link
		undrawImageDMA(cc->row, cc->col, 16, 16, BLACK);

		// Check for player movement
		if (KEY_DOWN(BUTTON_UP, BUTTONS) != 0) {
			cc->row -= 1;
			if (t_count > 45 || (t_count < 30 && t_count > 15)) {
				cc->image = back_walk;
			} else {
				cc->image = link_back;
			}
			cc->direction = UP;
		}
		if (KEY_DOWN(BUTTON_RIGHT, BUTTONS) != 0) {
			cc->col += 1;
			if (t_count > 45 || (t_count < 30 && t_count > 15)) {
				cc->image = right_walk;
			} else {
				cc->image = link_right;
			}
			cc->direction = RIGHT;
		}
		if (KEY_DOWN(BUTTON_LEFT, BUTTONS) != 0) {
			cc->col -= 1;
			if (t_count > 45 || (t_count < 30 && t_count > 15)) {
				cc->image = left_link;
			} else {
				cc->image = left_walk;
			}
			cc->direction = LEFT;
		}
		if (KEY_DOWN(BUTTON_DOWN, BUTTONS) != 0) {
			cc->row += 1;
			if (t_count > 45 || (t_count < 30 && t_count > 15)) {
				cc->image = front_walk;
			} else {
				cc->image = link_front;
			}
			cc->direction = DOWN;
		}

		// Player collision with wall
		if(cc->row < 0) {
			cc->row = 0;
		}
		if(cc->row > HEIGHT-cc->size) {
			cc->row = HEIGHT - cc->size;
		}
		if(cc->col < 0) {
			cc->col = 0;
		}
		if(cc->col > WIDTH-cc->size) {
			cc->col = WIDTH - cc->size;
		}

		// Player collision with zelda = stop 
		if ( (ZELDA_C < (cc->col + cc->size)) &&
			 ((ZELDA_C + 16) > cc->col) &&
			 (ZELDA_R < (cc->row + cc->size)) &&
			 ((ZELDA_R + 16) > (cc->row)) ) {
			// collision detected
			// bottom edge
			adjustCharacterPosition(cc, ZELDA_C, ZELDA_R, 16, 16);
		}

		// Player collision with ganon = LOSE
		if ( (cg->col <= (cc->col + cc->size)) &&
			 ((cg->col + cg->size) >= cc->col) &&
			 (cg->row <= (cc->row + cc->size)) &&
			 ((cg->row + cg->size) >= (cc->row)) ) {
			// collision detected
			state = LOSE;
			time = time < high_score ? high_score : time;
			break;
		}

		// keep fireball going where it was going
		if (ofp->spawn) {
				fp->row += fp->rd;
				fp->col += fp->cd;
		// fireball tracking player position
		} else if (f_cooldown > 120) {
			// initializing new fireball
			f_cooldown = 0;
			fp->row = cg->row;
			fp->col = cg->col;
			fp->spawn = 1;
			// will be calculated as fp(x,y) - cc(x, y)
			int dif_col = (fp->col)-cc->col;
			int dif_row = (fp->row)-cc->row;
			//setting col direction speed
			if (abs(dif_col) < 60) {
				fp->cd = 1;
			} else if (abs(dif_col) < 120 && abs(dif_col) >= 60) {
				fp->cd = 2;
			} else {
				fp->cd = hard_mode_sel ? 3 : 2;
			}
			
			//setting row direction speed
			if (abs(dif_row) < 60) {
				fp->rd = 1;
			} else if (abs(dif_row) >= 60 && abs(dif_row) <= 180) {
				fp->rd = 2;
			} else {
				fp->rd = hard_mode_sel ? 3 : 2;
			}
			if (fp->row > cc->row) {
				// if difference is greater than 0, (fireball to below of player),
				// we want it to travel up towards player
				fp->rd = -fp->rd;
			} 
			if (fp->col > cc->col) {
				// if difference is greater than 0, (fireball left to player),
				// we want it to travel up towards player
				fp->cd = -fp->cd;
			}
			//update fireball position 
			fp->row += fp->rd;
			fp->cd +=fp->cd;
		}

		// check fireball collision with wall
		// if collision occurs, stop ball, remove spawn 
		if (fp->row < 0) {
			fp->spawn = 0;
		}
		if (fp->row > HEIGHT-fp->height) {
			fp->spawn = 0;
		}
		if (fp->col < 0) {
			fp->spawn = 0;
		}
		if (fp->col > WIDTH-fp->height) {
			fp->spawn = 0;
		}

		// Player collision with fire
		if ( (fp->col <= (cc->col + cc->size)) &&
			 ((fp->col + fp->width) >= cc->col) &&
			 (fp->row <= (cc->row + cc->size)) &&
			 ((fp->row + fp->height) >= (cc->row)) ) {
			// collision detected
			t_collision = 0;
			cc->health -= 1;
			fp->spawn = 0;
			fp->row = -50;
			fp->cd = -50;
		}

		//Draw fire
		if (fp->spawn) {
			drawImageDMA(fp->row, fp->col, 16, 16, fp->image);
		}

		//Draw new link
		drawImageDMA(cc->row, cc->col, 16, 16, cc->image);

		//Erase old sword
		if (obp->spawn) {
			undrawImageDMA(obp->row, obp->col, 16, 16, BLACK);
		}

		// If there was already a sword, keep it going until it collides with smt
		if (obp->spawn) {
			if (obp->direction == LEFT) {
				bp->col -= bp->cd;
			} else if (obp->direction == RIGHT) {
				bp->col += bp->cd;
			} else if (obp->direction == UP) {
				bp->row -= bp->rd;
			} else {
				bp->row += bp->rd;
			}
		} else if (t_sword > 60) {
			// Check for sword key press
		    if (KEY_DOWN(BUTTON_A, BUTTONS) != 0) {
				bp->row = cc->row;
				bp->col = cc->col;
				if (cc->direction == LEFT) {
					bp->col = -15 + cc->col;
					bp->image = sword_left;
					bp->direction = LEFT;
				} else if (cc->direction == RIGHT) {
					bp->col = 15 + cc->col;
					bp->image = sword_right;
					bp->direction = RIGHT;
				} else if (cc->direction == DOWN) {
					bp->row = 15 + cc->row;
					bp->image = sword_down;
					bp->direction = DOWN;
				} else if (cc->direction == UP) {
					bp->row = -15 + cc->row;
					bp->image = sword_up;
					bp->direction = UP;
				}
				bp->spawn = 1;
				t_sword = 0;
			} else {
				bp->spawn = 0;
			}

	}

	// Sword collision with wall
	if(bp->row < 0) {
		bp->spawn = 0;
	}
	if(bp->row > HEIGHT-bp->height) {
		bp->spawn = 0;
	}
	if(bp->col < 0) {
		bp->spawn = 0;
	}
	if(bp->col > WIDTH-bp->width) {
		bp->spawn = 0;
	}

	// Sword collision with ganon
	if ( (cg->col <= (bp->col + bp->width)) &&
			((cg->col + cg->size) >= bp->col) &&
			(cg->row <= (bp->row + bp->height)) &&
			((cg->row + cg->size) >= (bp->row)) ) {
		// collision detected
			t_collision = 0;
			cg->health -= 1;
			bp->spawn = 0;
			bp->row = cg->row + 160;
			bp->col = cg->col + 240;
	}

	// if ganon's health == 0, win screen
	if (cg->health <= 0) {
		state = WIN;
		time = time < high_score ? high_score : time;
		break;
	}

	// check bounds of ganon
	if (cg->row < 25) {
		cg->row = 25;
		// move left
		cg->cd = hard_mode_sel ? -2 : -1;
		cg->rd = 0;
	} else if (cg->row > (HEIGHT-cg->size) - 45) {
		cg->row = (HEIGHT-cg->size) - 45;
		// move right
		cg->cd = hard_mode_sel ? 2 : 1;
		cg->rd = 0;
	} else if (cg->col < 25) {
		cg->col = 25;
		// move down
		cg->rd = hard_mode_sel ? 2 : 1;
		cg->cd = 0;
	} else if (cg->col > (WIDTH-cg->size) - 25) {
		cg->col = (WIDTH-cg->size) - 25;
		// move up
		cg->rd = hard_mode_sel ? -2 : -1;
		cg->cd = 0;
	}
	
	// continue moving him
	cg->row += cg->rd;
	cg->col += cg->cd;

	// gannon animation loop
	if (t_ganon_anim > 180) {
		t_ganon_anim = 0;
	} else {
		if (t_ganon_anim <= 36) {
			cg->image = hard_mode_sel ? h_ganon1 : ganon1;
		}
		if (t_ganon_anim <= 72 && t_ganon_anim > 36) {
			cg->image = hard_mode_sel ? h_ganon2 : ganon2;
		}
		if (t_ganon_anim <= 108 && t_ganon_anim > 72) {
			cg->image = hard_mode_sel ? h_ganon3 : ganon3;
		}
		if (t_ganon_anim <= 144 && t_ganon_anim > 108) {
			cg->image = hard_mode_sel ? h_ganon4 : ganon4;
		}
		if (t_ganon_anim <= 180 && t_ganon_anim > 144) {
			cg->image = hard_mode_sel ? h_ganon5 : ganon5;
		}
	}

	//Draw new ganon
	drawImageDMA(cg->row, cg->col, 32, 32, cg->image);

		// Erase and draw the new time
		if (t_count > 60) {
			time--;
			t_count = 0;
			char buffer[51];
			sprintf(buffer, "Time: %d", time);
			drawRectDMA(150, 5, 50, 10, BLACK);
			drawString(150, 5, buffer, YELLOW);
		}

		// Draw new sword 
		if (bp->spawn) {
			drawImageDMA(bp->row, bp->col, 16, 16, bp->image);
		}
		
        // state = ?
        break;
      case WIN:

	  	waitForVBlank();

	  	drawFullScreenImageDMA(win_screen);

		// Start screen
		while (!KEY_JUST_PRESSED(BUTTON_SELECT, BUTTONS, previousButtons));

		if (time > high_score) {
			high_score = time;
		}

		hard_mode_aval = 1;

		previousButtons = currentButtons; // Store the current state of the buttons

		state = START;


        // state = ?
        break;
      case LOSE:

	  	waitForVBlank();

	  	drawFullScreenImageDMA(game_over);

		color_ind = 0;
		drawString(105, 55, text, colors[color_ind]);

		drawString(125, 5, hint, YELLOW);
		drawString(140, 60, hint1, YELLOW);

		// Start screen
		while (!KEY_JUST_PRESSED(BUTTON_SELECT, BUTTONS, previousButtons)) {
			t_count++;
			if (t_count > 100000) {
				t_count = 0;
				if (color_ind > ncolors - 1) {
					color_ind = 0;
				}
				waitForVBlank();
				color_ind += 1;
				drawString(105, 55, text, colors[color_ind]);
			}
		}

		// reset t_count
		t_count = 0;

		// reset time **Important so that we can backspace to start after time runs out
		time = 61;

		previousButtons = currentButtons; // Store the current state of the buttons

		state = START;

        // state = ?
        break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
	pc = cc; // Store current state of character
	obp = bp; // Store current state of sword
	pg = cg; // Store current ganon
	ofp = fp; // store current fireball 
  }

  return 0;
}

