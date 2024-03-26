/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <cstdlib>
#include <curses.h>
#include <fstream>
#include <stdexcept>

#include <base.h>
#include <board.h>

Cell::Cell(const Location inCoords, const cellType inContent):
           _cellCoords(inCoords), _cellContent(inContent) {}

void Cell::setCell(const Location inCoords, const cellType inContent) {

    _cellCoords = inCoords;
    _cellContent = inContent;

    return;
}

void Cell::setContent(const cellType inContent) {

    _cellContent = inContent;

    return;
}

Board::Board(const uint16 columns, const uint16 rows, const Settings* settings):
             _remainingBricks(0) {

    _boardCell = new std::vector<Cell>;
    _boardCell->reserve
        ((rows-settings->heightOfTown)*(columns-settings->widthOfTown));
}

Board::~Board() {

    delete _boardCell;
}

// Board: function for displaying exception context
void Board::printExceptionCatch(const Location inCoords,
                                const std::string & functionName,
                                const std::exception & e) const {

    mvprintw(3,2,"critical failure in ");
    printw(functionName.c_str());
    printw(" at %2i",inCoords.getX());
    printw(",");
    printw("%2i",inCoords.getY());
    printw(": ");
    printw(e.what());
    refresh();

    return;
}

// Board: function for detecting collisions (optimized)
Board::collision Board::collisionDetectedOpt
    (const Location inCrashCoords, const Settings * inSettings) const {

    // try-catch block for debugging purposes only;
    // unless inappropriate changes are made to the code,
    // exception shouldn't be thrown during execution
    try {
        const uint16 index =
            (inCrashCoords.getX() - inSettings->heightOfTown + 1) +
            (inCrashCoords.getY() - (inSettings->widthOfTown / 2)) *
            (LINES - inSettings->heightOfTown);

        if (_boardCell->at(index).getCellCoords() == inCrashCoords &&
            _boardCell->at(index).getCellContent() != Cell::EMPTY) {

            _boardCell->at(index).setContent(Cell::EMPTY);
            return (COLLISION);
        }

        return (NO_COLLISION);
    }
    catch (const std::out_of_range & e) {

        printExceptionCatch(inCrashCoords, __FUNCTION__, e);
        return (ERROR_CATCH);
    }
}

// Board: function for detecting collisions (standard)
Board::collision Board::collisionDetectedStd
    (const Location inCrashCoords) const {

    // try-catch block for debugging purposes only;
    // unless inappropriate changes are made to the code,
    // exception shouldn't be thrown during execution
    try {
        for (std::vector<Cell>::iterator i = _boardCell->begin();
             i < _boardCell->end(); ++i) {

            if (i->getCellCoords() == inCrashCoords &&
                                      i->getCellContent() != Cell::EMPTY) {

                i->setContent(Cell::EMPTY);
                return (COLLISION);
            }
        }
        return (NO_COLLISION);
    }
    catch (const std::out_of_range & e) {

        printExceptionCatch(inCrashCoords, __FUNCTION__, e);
        return (ERROR_CATCH);
    }
}

// test only in this coords' subarea
bool Board::testCoords
    (const uint16 inX, const uint16 inY, const Settings * inSettings) const {

    if (inX >= (inSettings->heightOfTown-1) &&
        inY >= (inSettings->widthOfTown/2) &&
        inY <= COLS-(inSettings->widthOfTown/2)-1)
        return (true);

    return (false);
}

uint16 Board::generateContent(const Settings * inSettings, errors & inError) {

    Cell::cellType cellContent = Cell::EMPTY;
    const uint16 border = inSettings->widthOfTown/2;

    for (int i=border, randomNumber; i<(COLS-border);
         ++i, cellContent = Cell::EMPTY)
        for (int j=(inSettings->heightOfTown-1); j<(LINES-1); ++j) {

            Location currentLocation(j,i);

            randomNumber = rand()* inSettings->densityCoeff / (RAND_MAX+1/100);
            // build current floor
            if (randomNumber > 0) {
                cellContent = Cell::BUILT;
                _remainingBricks++;
            }

            Cell currentCell(currentLocation, cellContent);
            _boardCell->push_back(currentCell);

            // build remaining floors all the way down
            if (cellContent) {
                for (++j; j<(LINES-1); ++j) {

                    currentLocation.setCoords(j,i);
                    currentCell.setCell(currentLocation, cellContent);
                    _boardCell->push_back(currentCell);
                    ++(_remainingBricks);
                }
            }
        }

    if (_remainingBricks == 0)
        inError = NO_BRICKS;
    return (_remainingBricks);
}

uint16 Board::loadFromFile(Settings * inSettings,
                           const DimensionLimits & limits, errors & inError) {

    // input file name
    echo();
    char fileName[12];
    mvprintw(LINES-2,2,"Enter filename: ");
    getnstr(fileName,12);
    noecho();

    // open file
    std::ifstream inputFile(fileName, std::ios::in);
    if (!inputFile.is_open()) {
        inError = NO_FILE;
        return 0;
    }

    // initialize storage (vector)
    std::vector<std::string> * bufferedLines = new std::vector<std::string>;
    bufferedLines->reserve(limits.maxTownHeight-limits.minTownHeight+1);

    // load first line of input file into vector
    std::string textBuffer;
    uint16 height = 1, width = 0;
    if (std::getline(inputFile, textBuffer)) {
        width = textBuffer.length();
        // out of bounds check - is number of columns within range ?
        if (width < COLS-limits.maxTownWidth ||
            width > COLS-limits.minTownWidth) {
            inError = BAD_DIMENSIONS;
            return 0;
        }
        bufferedLines->push_back(textBuffer);
    }
    else {
        // file is empty
        inError = EMPTY_FILE;
        return 0;
    }

    // load remaining lines of input file into vector
    while (std::getline(inputFile, textBuffer)) {

        // out of bounds check - number of lines hit upper bound
        if (height >= (LINES-limits.minTownHeight))
            /* non-critical error - ignore remaining lines */ break;
        ++height;
        // out of bounds check - number of columns not matching preceding value
        if (width > textBuffer.length()) {
            inError = BAD_FILE_FORMAT;
            return 0;
        }
        if (width < textBuffer.length())
            // non-critical error - longer lines trimmed
            textBuffer.substr(0,width);
        bufferedLines->push_back(textBuffer);
    }

    // out of bounds check - insufficient height
    if (height < (LINES-limits.maxTownHeight)) {
        inError = BAD_DIMENSIONS;
        return 0;
    }

    inSettings->heightOfTown = LINES-height;
    inSettings->widthOfTown = COLS-width;

    for (int i=0; i < width; ++i) {

        Cell::cellType cellContent = Cell::EMPTY;
        uint16 currentLine = inSettings->heightOfTown-1;
        for (int j=0; j < height; ++j, ++currentLine, cellContent = Cell::EMPTY) {

                Location currentLocation(currentLine, i+(inSettings->widthOfTown)/2);

                // try-catch block for debugging purposes only
                // unless inappropriate changes are made to the code,
                // exception shouldn't be thrown during execution
                try {
                    if (bufferedLines->at(j).at(i) != '0' &&
                        bufferedLines->at(j).at(i) != ' ') {
                        cellContent = Cell::BUILT;
                        ++(_remainingBricks);
                    }
                }
                catch (const std::out_of_range &) {

                    inError = BAD_FILE_FORMAT;
                    return 0;
                }

                Cell currentCell(currentLocation, cellContent);
                _boardCell->push_back(currentCell);
        }
    }
    inputFile.close();
    delete bufferedLines;

    if (_remainingBricks == 0)
        inError = NO_BRICKS;
    return (_remainingBricks);
}

void Board::subtractBrick() {

    _remainingBricks--;
    printBricks();

    refresh();

    return;
}

void Board::printBricks() const {

    mvprintw(1,73,"%4i",_remainingBricks);

    return;
}
