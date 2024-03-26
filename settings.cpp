/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <curses.h>

#include <airplane.h>
#include <settings.h>

DimensionLimits::DimensionLimits(): minTownHeight(12), maxTownHeight(LINES-1),
                                    minTownWidth(6), maxTownWidth(COLS-2),
                                    gameSpeedStep(5000) {}

Settings::Settings() {
    initDefaults();
}

bool Settings::initDefaults() {

    gameSpeed = 5e05;
    bombSpeed = 3;
    dropSpeed = 4;
    bombsToBricksRatio = 1.2;
    densityCoeff = 1.2;
    heightOfTown = 14;
    widthOfTown = 6;

    return (true);
}

void Settings::drawSettings() const {

    erase();

    mvprintw(2,COLS/2-5,"SETTINGS:");
    mvprintw(3,COLS/2-5,"---------");
    mvprintw(GAME_SPEED-45,COLS/2-8,"1. game speed");
    mvprintw(BOMB_SPEED-45,COLS/2-8,"2. speed of dropped bombs");
    mvprintw(DROP_SPEED-45,COLS/2-8,"3. interval between bombs");
    mvprintw(NUMBER_OF_BOMBS-45,COLS/2-8,"4. number of bombs available");
    mvprintw(DENSITY-45,COLS/2-8,"5. density of buildings");
    mvprintw(TOWN_HEIGHT-45,COLS/2-8,"6. max. height of buildings");
    mvprintw(TOWN_WIDTH-45,COLS/2-8,"7. width of town");
    mvprintw(12,COLS/2-8,"9. back to default values");
    mvprintw(13,COLS/2-8,"0. return to main menu");

    refresh();
    return;
}

void Settings::changeSettings(const DimensionLimits & limits) {

    drawSettings();

    bool end = false;
    do {
        int input = getch();
        switch (input) {
            case GAME_SPEED: {
                const unsigned long int upperBound = std::numeric_limits
                    <unsigned long int>::max() / Airplane::adjustSpeed();
                tag(GAME_SPEED);
                changeSetting<unsigned long int>
                    (gameSpeed, limits.gameSpeedStep, 1000ul, upperBound);
                untag(GAME_SPEED);
                break;
            }
            case BOMB_SPEED: {
                const uint16 upperBound =
                    COLS/2-Airplane::getFuselageShape().length();
                tag(BOMB_SPEED);
                changeSetting<uint16>(bombSpeed, 1u, 1u, upperBound);
                untag(BOMB_SPEED);
                break;
            }
            case DROP_SPEED: {
                const uint16 upperBound = ((COLS-widthOfTown)/6)+1;
                tag(DROP_SPEED);
                changeSetting<uint16>(dropSpeed, 1u, 1u, upperBound);
                untag(DROP_SPEED);
                break;
            }
            case NUMBER_OF_BOMBS: {
                tag(NUMBER_OF_BOMBS);
                changeSetting<double>(bombsToBricksRatio, 0.02, 1.01, 2.0);
                untag(NUMBER_OF_BOMBS);
                break;
            }
            case DENSITY: {
                tag(DENSITY);
                changeSetting<double>(densityCoeff, 0.01, 1.005, 20.0);
                untag(DENSITY);
                break;
            }
            case TOWN_HEIGHT: {
                tag(TOWN_HEIGHT);
                changeSetting<uint16>
                    (heightOfTown, 1u, limits.minTownHeight, limits.maxTownHeight);
                untag(TOWN_HEIGHT);
                break;
            }
            case TOWN_WIDTH: {
                tag(TOWN_WIDTH);
                changeSetting<uint16>
                    (widthOfTown, 2u, limits.minTownWidth, limits.maxTownWidth);
                untag(TOWN_WIDTH);
                if (dropSpeed > ((COLS-widthOfTown)/6)+1)
                    dropSpeed = ((COLS-widthOfTown)/6)+1;
                break;
            }
            case DEFAULTS: {
                initDefaults();
                break;
            }
            case EXIT: end = true;
            default: ;
        }
    } while (!end);

    return;
}

template<typename T>
void Settings::changeSetting(T & inValue, const T inStep, const T inLowerBound,
                             const T inUpperBound) {

    char format[] = "%.1u";
    if (!std::numeric_limits<T>::is_integer)
        { format[2] = '2'; format[3] = 'f'; }
    else if (std::numeric_limits<T>::is_signed)
        format[3] = 'i';

    bool end = false;
    do {
        mvprintw(23,2,"%s","<                                ");
        mvprintw(23,4,format,inValue);
        printw("%s"," > [");
        printw(format,inLowerBound);
        printw("%s","-");
        printw(format,inUpperBound);
        printw("%s","]");

        int input = getch();
        switch (input) {

            case KEY_UP:
            case KEY_RIGHT:
                if (inValue+inStep <= inUpperBound) inValue += inStep;
                break;
            case KEY_DOWN:
            case KEY_LEFT:
                if (inValue-inStep >= inLowerBound) inValue -= inStep;
                break;
            default: end = true;
        }
    } while (!end);

    mvprintw(23,2,"%s","                                 ");
    return;
}
