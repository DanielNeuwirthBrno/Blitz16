/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

// initColors() function
// drawPlanePicture() function
// drawMenu() function
// errorMessage() function
// endOfHelpPage() function
// help() function
// Location class

#ifndef BASE_H
#define BASE_H

typedef unsigned short int uint16;

enum errors { UNSPECIFIED = 0, NO_COLORS = 10, GAME_SELECT = 20,
              NO_BRICKS = 30, DRAW_BUILDINGS = 40, DRAW_PLANE = 50,
              CRASH_EVAL = 60, NO_FILE = 70, EMPTY_FILE = 75,
              BAD_DIMENSIONS = 80, BAD_FILE_FORMAT = 90, EXCEPTION = 99 };

enum colors { DEFAULT, DELETE, GAMEBOARD, BUILDINGS,
              AIRPLANE, BOMBS, CRITICAL, END_OF_COLORS };

enum menuSelection { EXIT = 48, GAME1, GAME2, SETTINGS, HELP };

const uint16 pictureLines = 25;
const uint16 pictureColsInBytes = 10;

// menu screen picture representation
const unsigned char picture[] = {
  0, 25, 96,  0,  4, 30, 32, 79,159,  0,  1,112,  0,  4, 17, 32, 66,  1,  0,  1,
 60,  0,  4, 17, 32, 66,  2,  0,  1, 30,  0,  4, 30, 32, 66,  4,  0,  1, 31,  0,
  4, 17, 32, 66, 8,  16, 30,  0,  4, 17, 32, 66, 16,  8, 15,128,  0,  3, 30,124,
 66, 31, 12, 15,  0,  8, 14, 15,128,  0,  7, 31,255,254,  0,  7, 31,255,254,  0,
  7, 14, 15,128,  0,  4,  1, 28,  0,  1, 12, 15,  0,  5,  3, 32,  0,  1,  8, 15,
128,  0,  4,  5, 32,  0,  1, 16, 30,  0,  5,  9, 60,  0,  2, 31,  0,  5,  1, 34,
  0,  2, 30,  0,  5,  1, 34,  0,  2, 60,  0,  5,  1, 28,  0,  2,112,  0,  9, 96,
  0, 34};

bool initColors();

bool drawPlanePicture();

void drawMenu();

bool errorMessage(errors = UNSPECIFIED);

inline void endOfHelpPage() {

    mvprintw(LINES-1,COLS-16,"<press any key>");
    refresh();
    while (getch() == ERR);

    return;
}

void help();

class Location {

    public:
        Location() {}
        Location(const uint16 inX, const uint16 inY);

        inline uint16 getX() const { return _xcoord; }
        inline uint16 getY() const { return _ycoord; }
        void setCoords(const uint16, const uint16);

        // airplane movement
        Location operator++ ();
        Location operator++ (int) { return (operator++()); }
        // collision detection
        bool operator== (const Location &) const;
        bool operator!= (const Location &) const;

    private:
        uint16 _xcoord;
        uint16 _ycoord;
};

#endif // BASE_H
