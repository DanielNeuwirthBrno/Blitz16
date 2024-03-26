/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <cstdlib>
#include <ctime>
#include <curses.h>
#include <string>

#include <base.h>
#include <game.h>

Game::Game(const typeOfGame inTypeOfGame, const Settings & settings,
           const DimensionLimits & limits):
            _gameSpeedStep(limits.gameSpeedStep), _errorType(UNSPECIFIED) {

    srand((unsigned)time(0));

    // no pointer types in Settings => default copy constructor sufficient
    _currentSettings = new Settings(settings);

    _gameBoard = new Board(COLS, LINES, _currentSettings);

    switch (inTypeOfGame) {

        case RANDOM_SHUFFLE:
            _name = "random shuffle";
            _bricks = _gameBoard->generateContent(_currentSettings, _errorType);
            break;
        case PLAYER_SELECTION:
            _name = "file selection";
            _bricks = _gameBoard->loadFromFile(_currentSettings, limits, _errorType);
            break;
        default:
            _errorType = GAME_SELECT;
    }
    _plane = new Airplane(_bricks, _currentSettings);
}

Game::~Game() {

    delete _plane;
    delete _gameBoard;
    delete _currentSettings;
}

Game::gameResult Game::newGame() {

    // draw gameboard
    drawBoard();
    if (!drawBuildings()) {
        _errorType = DRAW_BUILDINGS;
        return (ERROR);
    }

    // draw basic game info
    mvprintw(1,8,(_name+" *").c_str());
    _plane->printCoords();
    _plane->printBombs();
    mvprintw(1,73,"%4i",_gameBoard->getRemainingBricks());

    // draw plane
    if (!_plane->draw()) {
        _errorType = DRAW_PLANE;
        return (ERROR);
    }

    while (getch() == ERR);

    // go to game loop
    gameResult result = gameLoop();

    return (result);
}

void Game::pause() const {

    std::string pause = " Game paused. Press F10 to continue. ";

    mvprintw(LINES-1,(COLS-pause.length())/2,pause.c_str());
    refresh();

    while (getch() != KEY_F(10));

    for (int i=0; i < static_cast<int>(pause.length()); ++i)
        mvaddch(LINES-1, i+(COLS-pause.length())/2, '*'| COLOR_PAIR(GAMEBOARD));
    refresh();

    return;
}

void Game::gameOver() const {

    mvprintw(LINES/2-2,COLS/2-10,"*********************");
    mvprintw(LINES/2-1,COLS/2-10,"*                   *");
    mvprintw(LINES/2,COLS/2-10,  "* CRASH! GAME OVER! *");
    mvprintw(LINES/2+1,COLS/2-10,"*                   *");
    mvprintw(LINES/2+2,COLS/2-10,"*********************");
    refresh();

    while (getch() != KEY_F(10));
    return;
}

void Game::victory() const {

    mvprintw(LINES/2-3,COLS/2-11,"*********************");
    mvprintw(LINES/2-2,COLS/2-11,"*                   *");
    mvprintw(LINES/2-1,COLS/2-11,"* !CONGRATULATIONS! *");
    mvprintw(LINES/2,COLS/2-11,"* You win the game. *");
    mvprintw(LINES/2+1,COLS/2-11,"*                   *");
    mvprintw(LINES/2+2,COLS/2-11,"*********************");
    refresh();

    while (getch() != KEY_F(10));
    return;
}

Game::gameResult Game::gameLoop() {

    uint16 plCoordX, plCoordY, bricks;
    uint16 betweenBombsInterval = _plane->getDropInt();
    Board::collision crash;

    while (_plane->getPlaneCoords().getX() < LINES-2) {

        _plane->move();
        plCoordX = _plane->getPlaneCoords().getX()+1;
        plCoordY = _plane->getPlaneCoords().getY()-1;

        // test for collision (plane->building)
        if (_gameBoard->testCoords(plCoordX,plCoordY, this->_currentSettings)) {

            Location crashLocation(plCoordX, plCoordY);
            crash = _gameBoard->collisionDetectedOpt(crashLocation,
                    this->_currentSettings);

            switch (crash) {

                case Board::COLLISION:
                    gameOver();
                    return (LOST);
                case Board::NO_COLLISION:
                    break;
                case Board::ERROR_CATCH:
                    _errorType = EXCEPTION;
                    return(ERROR);
                default:
                    _errorType = CRASH_EVAL;
                    return(ERROR);
            }
        }

        // bombs
        for (std::vector<Bombs>::iterator i = _plane->getBombs().begin();
             i < _plane->getBombs().end(); ++i) {

            // decrease speed interval
            i->setSpeedInt(i->getSpeedInt()-1);
            if (i->getSpeedInt() > 0) continue;
            else i->setSpeedInt(this->_currentSettings->bombSpeed);

            // erase bomb from screen
            i->draw(DELETE);

            // set new coords
            Location changedCoords(i->getBombCoords().getX()+1,
                                   i->getBombCoords().getY());
            i->setBombCoords(changedCoords);

            // remove bomb falling off screen from vector
            if (i->getBombCoords().getX()+1 == LINES) {

                i = _plane->getBombs().erase(i);
                --i;
                continue;
            }

            i->draw();

            // test for collision (bomb->building)
            if (_gameBoard->testCoords(i->getBombCoords().getX(),
                                       i->getBombCoords().getY(),
                                       this->_currentSettings)) {

                crash = _gameBoard->collisionDetectedOpt
                        (changedCoords, this->_currentSettings);
                switch (crash) {

                    case Board::COLLISION:
                        // erase building/bomb from screen after strike
                        i->draw(DELETE);

                        // remove bomb from vector after strike
                        i = _plane->getBombs().erase(i);
                        --i;

                        // decrease number of remaining bricks
                        _gameBoard->subtractBrick();

                        break;

                    case Board::NO_COLLISION:
                        break;
                    case Board::ERROR_CATCH:
                        _errorType = EXCEPTION;
                        return (ERROR);
                    default:
                        _errorType = CRASH_EVAL;
                        return(ERROR);
                }
            }
        }

        bricks = _gameBoard->getRemainingBricks();
        // check for win
        if (bricks == 0) {
            victory();
            return (WON);
        }
        endOfBombsInterval();

        // process user input
        int input = getch();
        switch (input) {
            case KEY_DOWN: {
                if (betweenBombsInterval != 0) {

                    _plane->bomb(bricks, this->_currentSettings);
                    --betweenBombsInterval;
                }
                else {

                    betweenBombsInterval = _plane->getDropInt();
                    endOfBombsInterval(CRITICAL);
                }
                break;
            }
            case KEY_F(10):
                pause();
                break;
            case KEY_LEFT: {

                if (this->_currentSettings->gameSpeed+this->_gameSpeedStep <=
                    std::numeric_limits<unsigned long int>::max()
                    / this->_plane->adjustSpeed())
                    this->_currentSettings->gameSpeed += this->_gameSpeedStep;
                std::string speed = " Speed decreased. ";
                mvprintw(LINES-1,(COLS-speed.length())/2,speed.c_str());
                break;
            }
            case KEY_RIGHT: {

                if (this->_currentSettings->gameSpeed-this->_gameSpeedStep >= 1000)
                    this->_currentSettings->gameSpeed -= this->_gameSpeedStep;
                std::string speed = " Speed increased. ";
                mvprintw(LINES-1,(COLS-speed.length())/2,speed.c_str());
                break;
            }
            case 'q':
                return(LOST);
            default: ;
        }

        // cycle speed
        for (unsigned long long int i=0,
             basicSpeed = _plane->adjustSpeed(plCoordX);
             i<(this->_currentSettings->gameSpeed*basicSpeed/500); ++i)
            mvaddch(1,1,' ');

    }
    return (WON);
}
