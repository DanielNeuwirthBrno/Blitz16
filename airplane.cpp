/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <curses.h>

#include <airplane.h>

// initialize static members
const std::string Airplane::_fuselage = ">==I>";

Bombs::Bombs(const Location inLocation, const uint16 inBombSpeed):
    _bombCoords(inLocation), _speedInterval(inBombSpeed) {
    draw();
}

void Bombs::setBombCoords(const Location inCoords) {

    _bombCoords = inCoords;

    return;
}

void Bombs::setSpeedInt(const uint16 inSpeed) {

    _speedInterval = inSpeed;

    return;
}

bool Bombs::draw(const colors inColor) const {

    mvaddch(_bombCoords.getX(), _bombCoords.getY(), 'o' | COLOR_PAIR(inColor));

    refresh();
    return (true);
}

Airplane::Airplane(const uint16 inBricks, const Settings * settings):
                   _dropInterval(settings->dropSpeed),
                   _noOfBombs((static_cast<uint16>
          (static_cast<double>(inBricks) * settings->bombsToBricksRatio))) {

    // plane coords
    Location initCoords(_initCoordX, _initCoordY);
    _planeCoords = initCoords;

    // bombs
    _bombs = new std::vector<Bombs>;
}

Airplane::~Airplane() {

    delete _bombs;
}

void Airplane::setPlaneCoords(const Location inCoords) {

    _planeCoords = inCoords;

    return;
}

// Airplane basic draw() function
bool Airplane::draw(const colors inColor) const {

    mvaddch(_planeCoords.getX()-1, _planeCoords.getY()-1,
            'I' | COLOR_PAIR(inColor));
    for (int i=0; i<static_cast<int>(_fuselage.length()); ++i)
        mvaddch(_planeCoords.getX(), _planeCoords.getY()-_fuselage.length()+i+1,
                _fuselage[i] | COLOR_PAIR(inColor));
    mvaddch(_planeCoords.getX()+1, _planeCoords.getY()-1,
            'I'| COLOR_PAIR(inColor));

    refresh();
    return (true);
}

// Airplane draw() function with setting _planeCoords
bool Airplane::draw(const Location inCoords, const colors inColor) {

    setPlaneCoords(inCoords);
    return (draw(inColor));
}

void Airplane::move() {

    draw(DELETE);
    ++(_planeCoords);
    draw();
    printCoords();

    refresh();
    return;
}

bool Airplane::bomb(const uint16 inBricks, const Settings * inSettings) {

    if (_noOfBombs == 0)
        return (false);

    Location bombCoords(_planeCoords.getX()+1, _planeCoords.getY()-2);
    Bombs newBomb(bombCoords, inSettings->bombSpeed);
    _bombs->push_back(newBomb);
    --(_noOfBombs);

    colors color = GAMEBOARD;
    if (_noOfBombs <
        static_cast<double>(inBricks)*((inSettings->bombsToBricksRatio-1)/2+1))
        color = CRITICAL;
    printBombs(color);

    refresh();
    return (true);
}

void Airplane::printCoords() const {

    mvprintw(1,37,"%2i",_planeCoords.getX());
    mvprintw(1,45,"%2i",_planeCoords.getY());

    return;
}

void Airplane::printBombs(const colors inColor) const {

    uint16 bombs = _noOfBombs;
    uint16 position = 60;
    mvprintw(1,position-3,"    ");

    do {
        mvaddch(1,position--, ((bombs - (bombs/10) * 10) +'0') |
                COLOR_PAIR(inColor));
        bombs /= 10;

    } while (bombs != 0);

    return;
}
