/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

// Game class

#ifndef GAME_H
#define GAME_H

#include <string>

#include <airplane.h>
#include <board.h>
#include <settings.h>

class Game {

    public:
        enum typeOfGame { UNKNOWN, RANDOM_SHUFFLE, PLAYER_SELECTION };
        enum gameResult { ERROR = -1, LOST, WON };

        // Game() {}
        Game(const typeOfGame, const Settings &, const DimensionLimits &);
        ~Game();

        inline errors getErrorType() const { return _errorType; }
        inline void setErrorType(const errors inErrorType)
                        { _errorType = inErrorType; }

        bool drawBoard() const;
        bool drawBuildings() const;
        inline void endOfBombsInterval(const colors inColor = DELETE) const {
            mvaddch(LINES-2,1,'X' | COLOR_PAIR(inColor));
            mvaddch(LINES-2,COLS-2,'X' | COLOR_PAIR(inColor));
            refresh();
        }

        void pause() const;
        void gameOver() const;
        void victory() const;

        gameResult newGame();
        gameResult gameLoop();

    private:
        const unsigned long _gameSpeedStep;
        Settings * _currentSettings;
        Board * _gameBoard;
        Airplane * _plane;
        errors _errorType;
        uint16 _bricks;
        std::string _name;
};

#endif // GAME_H
