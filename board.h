/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

// Cell class
// Board class

#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>

#include <base.h>
#include <settings.h>

class Cell {

    public:
        enum cellType { EMPTY, BUILT /*, other */ };

        // Cell() {}
        Cell(const Location, const cellType);

        inline Location getCellCoords() const { return _cellCoords; }
        inline cellType getCellContent() const { return _cellContent; }
        void setCell(const Location, const cellType);
        void setContent(const cellType = EMPTY);

    private:
        Location _cellCoords;
        cellType _cellContent;
};

class Board {

    public:
        enum collision { ERROR_CATCH = -1, NO_COLLISION, COLLISION };

        // Board() {}
        Board(const uint16, const uint16, const Settings *);
        ~Board();

        const std::vector<Cell> & getBoardCell() const { return (*_boardCell); }
        inline uint16 getRemainingBricks() const { return _remainingBricks; }

        void printExceptionCatch(const Location, const std::string &,
                                 const std::exception &) const;

        // member function collisionDetectedOpt()
        // version optimized for performance /extremely quick/
        // localization performed via index, without need for iterating
        // relies on specific ordering of elements in vector<Cell>
        // use in conjuction with original generateContent() function only!
        collision collisionDetectedOpt(const Location, const Settings *) const;
        // member function collisionDetectedStd() - standard version
        // localization performed via iterating through vector<Cell>
        // works properly regardless of element ordering in vector<Cell>
        // use in case of providing your own implementation of generateContent()
        collision collisionDetectedStd(const Location) const;

        bool testCoords(const uint16, const uint16, const Settings *) const;
        uint16 generateContent(const Settings *, errors &);
        uint16 loadFromFile(Settings *, const DimensionLimits &, errors &);
        void subtractBrick();
        void printBricks() const;

    private:
        uint16 _remainingBricks;
        std::vector<Cell> * _boardCell;
};

#endif // BOARD_H
