/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

// Bombs class
// Airplane class

#ifndef AIRPLANE_H
#define AIRPLANE_H

#include <curses.h>
#include <string>
#include <vector>

#include <base.h>
#include <settings.h>

class Bombs {

    public:
        // Bombs() {}
        Bombs(const Location, const uint16);

        inline Location getBombCoords() const { return _bombCoords; }
        inline uint16 getSpeedInt() const { return _speedInterval; }
        void setBombCoords(const Location);
        void setSpeedInt(const uint16);

        bool draw(const colors = BOMBS) const;

    private:
        Location _bombCoords;
        uint16 _speedInterval;
};

class Airplane {

    public:
        // Airplane() {}
        Airplane(const uint16, const Settings *);
        ~Airplane();

        std::vector<Bombs> & getBombs() const { return (*_bombs); }
        inline uint16 getDropInt() const { return _dropInterval; }
        inline static std::string getFuselageShape() { return _fuselage; }
        inline Location getPlaneCoords() const { return _planeCoords; }
        void setPlaneCoords(const Location);

        inline static unsigned long int
            adjustSpeed(uint16 coordX = _initCoordX+1)
                { return ((LINES*2 - coordX) * 20); }

        bool draw(const colors = AIRPLANE) const;
        bool draw(const Location, const colors = AIRPLANE);
        void move();
        bool bomb(const uint16, const Settings *);
        void printCoords() const;
        void printBombs(const colors = GAMEBOARD) const;

    private:
        const static uint16 _initCoordX = 4;
        const static uint16 _initCoordY = 5;
        const static std::string _fuselage;
        const uint16 _dropInterval;
        uint16 _noOfBombs;
        std::vector<Bombs> * _bombs;
        Location _planeCoords;
};

#endif // AIRPLANE_H
