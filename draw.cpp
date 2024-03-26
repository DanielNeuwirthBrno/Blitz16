/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <curses.h>

#include <base.h>
#include <game.h>

bool drawPlanePicture() {

    erase();
    // check if picture fits onto the screen
    if (LINES < pictureLines || COLS < pictureColsInBytes*8)
        return (false);

    // check if last element of pictureArray is nonzero otherwise it doesn't
    // conform to the specification used during compression stage
    // and cannot be successfully decompressed
    const std::size_t sizeOfPictureArray =
        sizeof(::picture) / sizeof(::picture[0]);
    if (::picture[sizeOfPictureArray-1] == 0)
        return (false);

    std::vector<unsigned char> * picture =
        new std::vector<unsigned char>(pictureLines*pictureColsInBytes, 0);
    std::vector<unsigned char>::iterator decompressed = picture->begin();

    // picture - decompression phase
    for (std::size_t compressed=0; decompressed<picture->end(); ++compressed) {

        // check for out-of-bounds access attempt
        if (compressed >= sizeOfPictureArray)
            return (false);

        if (::picture[compressed] == 0) {
            decompressed += ::picture[compressed+1];
            ++compressed;
        }
        else {
            *decompressed = ::picture[compressed];
            ++decompressed;
        }
    }
\
    // picture - bit expansion phase
    for (uint16 row = 0; row < pictureLines; ++row)
        for (uint16 bitPattern=0; bitPattern<pictureColsInBytes; ++bitPattern) {

            unsigned char currentChar =
                picture->at(row*pictureColsInBytes+bitPattern);
            if (currentChar == 0) continue;
            unsigned char charToCompare = 128;  // 10000000 in binary

            for (uint16 bitNumber = 0; bitNumber < 8; ++bitNumber) {
                if (currentChar & charToCompare)
                    mvaddch(row,bitPattern*8+bitNumber,'*');
                charToCompare >>= 1;
            }
        }
    return (true);
}

void drawMenu() {

    uint16 column = drawPlanePicture() ? (COLS-21) : 2;

    mvprintw(4,column,"1. New game");
    mvprintw(5,column,"   RANDOM SHUFFLE");
    mvprintw(7,column,"2. New game");
    mvprintw(8,column,"   LOAD FROM FILE");
    mvprintw(10,column,"3. Settings");
    mvprintw(12,column,"4. Help");
    mvprintw(14,column,"0. Exit");
    mvprintw(23,COLS-31,"Copyright 2016 Daniel Neuwirth");

    refresh();
    return;
}

bool Game::drawBoard() const {

    erase();

    // draw screen border
    move(0,0);
    for (int i=0; i<COLS; ++i) addch('*');
    addch('*');
    mvaddch(1,COLS-26,'*');
    mvaddch(1,COLS-1,'*');
    for (int i=0; i<COLS; ++i) addch('*');
    for (int i=3; i<LINES-1; ++i) {
        addch('*');
        move(i,COLS-1);
        addch('*');
    }
    for (int i=0; i<COLS; ++i) addch('*');

    // draw information panel
    mvprintw(1,2,"Game: ");
    mvprintw(1,25,"coords: <X>     <Y>");
    mvprintw(1,49,"bombs: [    ]");
    mvprintw(1,64,"bricks: [    ]");

    refresh();
    return (true);
}

bool Game::drawBuildings() const {

    char charToDraw;
    bool drawRoof = false;
    uint16 coordY = 0;

    for (std::vector<Cell>::const_iterator
         i = _gameBoard->getBoardCell().begin();
         i < _gameBoard->getBoardCell().end(); ++i, charToDraw = 'H') {

        if (i->getCellCoords().getY() != coordY)
            drawRoof = true;

        if (i->getCellContent() == Cell::EMPTY)
            charToDraw = ' ';
        else if (drawRoof) {
            charToDraw = 'A';
            drawRoof = false;
        }

        mvaddch(i->getCellCoords().getX(), coordY = i->getCellCoords().getY(),
                charToDraw | COLOR_PAIR(BUILDINGS));
    }

    refresh();
    return (true);
}
