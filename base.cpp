/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <curses.h>
#include <fstream>
#include <sstream>
#include <string>

#include <base.h>

// display help-file
void help() {

    clear();

    std::ifstream inputFile("manual.txt", std::ios::in | std::ios::ate);
    std::stringstream fileContents;

    if (inputFile.is_open()) {

        if (inputFile.tellg() != 0) {

            inputFile.seekg(0,inputFile.beg);
            fileContents << inputFile.rdbuf();
        }
        else
            fileContents << "Help file is empty.";
    }
    else
        fileContents << "Help file not found.";

    inputFile.close();

    std::string rowOfHelpFile, partOfRowOfHelpFile;
    uint16 currentRow = 1;

    while (std::getline(fileContents, rowOfHelpFile)) {

        // discard empty lines at the beginning of new screen
        if (currentRow == 1 &&
            rowOfHelpFile.find_first_not_of(' ') == std::string::npos)
            continue;

        uint16 position = 0;
        while (position < rowOfHelpFile.length()) {

            // break long lines (exceeding number of columns) to fit on screen
            partOfRowOfHelpFile = rowOfHelpFile.substr(position, COLS-1);

            // break is performed after last white-space character
            if (position+partOfRowOfHelpFile.length() < rowOfHelpFile.length())
                partOfRowOfHelpFile = partOfRowOfHelpFile.substr(0, partOfRowOfHelpFile.find_last_of(' '));

            mvprintw(currentRow,1,"%s",partOfRowOfHelpFile.c_str());
            position += partOfRowOfHelpFile.length();

            if (++currentRow == LINES-1) {

                endOfHelpPage();
                currentRow = 1;
                clear();
                break;
            }
        }
    }

    if (currentRow != 1)
        endOfHelpPage();
    return;
}

// error message
bool errorMessage(errors inErrorType) {

    bool crash = true;
    std::string message;
    switch (inErrorType) {
        case NO_COLORS:
            message = "   INIT COLORS FAILED   ";
            crash = false;
            break;
        case GAME_SELECT:
            message = "  GAME SELECTION ERROR  ";
            break;
        case NO_BRICKS:
            message = "   NO BRICKS AVAILABLE  ";
            break;
        case DRAW_BUILDINGS:
            message = "  DRAW BUILDINGS FAILED ";
            break;
        case DRAW_PLANE:
            message = "    DRAW PLANE FAILED   ";
            break;
        case CRASH_EVAL:
            message = " CRASH EVALUATION FAILED";
            break;
        case NO_FILE:
            message = "   FILE DOESN'T EXIST   ";
            break;
        case EMPTY_FILE:
            message = " FILE IS EMPTY, NO DATA ";
            break;
        case BAD_DIMENSIONS:
            message = "  SIZE IS OUT OF RANGE  ";
            break;
        case BAD_FILE_FORMAT:
            message = "  INCORRECT FILE FORMAT ";
            break;
        case EXCEPTION:
            message = "   CRITICAL EXCEPTION   ";
            crash = false;
            break;
        case UNSPECIFIED:
        default:
            message = "    UNSPECIFIED ERROR   ";
            crash = false;
    }

    mvprintw(LINES/2-2,COLS/2-14,"****************************");
    mvprintw(LINES/2-1,COLS/2-14,"*                          *");
    mvprintw(LINES/2,COLS/2-14,  "*                          *");
    mvprintw(LINES/2,COLS/2-12,message.c_str());
    mvprintw(LINES/2+1,COLS/2-14,"*                          *");
    mvprintw(LINES/2+2,COLS/2-14,"****************************");
    if (crash == false)
        mvprintw(LINES/2+3,COLS/2-14,"(program will be terminated)");
    refresh();

    while (getch() == ERR);
    return (crash);
}

// initialization of colour-pairs
bool initColors() {

    // has_colors() function indicates if the terminal supports
    // and can manipulate color
    if (!has_colors())
        return (false);

    uint16 useColors[] = { COLOR_BLACK, COLOR_YELLOW, COLOR_WHITE,
                           COLOR_CYAN, COLOR_WHITE, COLOR_RED };

    for (int i=1; i<END_OF_COLORS; ++i)
        if (init_pair(i, useColors[i-1], COLOR_BLACK) == ERR)
            return (false);

    return (true);
}

Location::Location(const uint16 inX, const uint16 inY):
                   _xcoord(inX), _ycoord(inY) {}

Location Location::operator++() {

    if (_ycoord < COLS-2) ++_ycoord;
    else {
        ++(this->_xcoord);
        this->_ycoord = 5;
    }
    return (*this);
}

bool Location::operator== (const Location & rhs) const {

    if (this->_xcoord == rhs._xcoord && this->_ycoord == rhs._ycoord)
        return (true);
    else
        return(false);
}

bool Location::operator!= (const Location & rhs) const {
    return (!(*this == rhs));
}

void Location::setCoords(const uint16 inX, const uint16 inY) {

    _xcoord = inX;
    _ycoord = inY;

    return;
}
