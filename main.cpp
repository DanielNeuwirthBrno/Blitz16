/* Application name:    Blitz16 (blitz16.exe)
 * Current version:     0.30
 *
 * Author:              Daniel Neuwirth (d.neuwirth@tiscali.cz)
 * Created:             2016-01-14
 * Lastly modified:     2016-09-28
 *
 * Licensed under:      GNU General Public License 3.0 (http://www.gnu.org/)
 *
 * Compiler:            MinGW 5.3.0 32-bit
 * Language standard:   C++98/C++03
 *
 * External libraries:  pdcurses 3.4
 *
 * Objective:           clone of '84 C16 game Blitz
 *                      /project for BUoT FoME software museum/
 *
 * Game description:    A plane moves across the screen at a steady speed.
 *                      When the plane reaches the end of the screen it moves
 *                      to the other side and drops down one line, with the
 *                      speed increasing each time the plane drops a line.
 *                      Below is a cityscape composed of blocks. The player
 *                      has to drop bombs from the plane, and each bomb which
 *                      hits a building removes one block. As the plane descends
 *                      it risks hitting any remaining blocks so priority has to
 *                      be given to bombing the tallest "buildings". The game
 *                      is completed when all blocks are removed. */

#include <curses.h>

#include <base.h>
#include <game.h>

int main()
{
    /* The first curses routine called should be initscr(). This will
     * determine the terminal type and initialize all curses data structures.
     * The initscr() function also arranges that the first call to refresh()
     * will clear the screen. */
    initscr();

    /* cbreak() puts the terminal into cbreak mode. In cbreak mode, characters
     * typed by the user are immediately available to the program and erase/kill
     * character processing is not performed. Interrupt and flow control
     * characters are unaffected by this mode. */
    cbreak();

    /* echo() and noecho() control whether typed characters are echoed by the
     * input routine. Initially, input characters are echoed. */
    noecho();

    /* The keypad() function changes the keypad option of the user's terminal.
     * If enabled, the user can press a function key (such as the left arrow
     * key) and getch() will return a single value that represents the KEY_LEFT
     * function key. If disabled, nothing will be returned. */
    keypad(stdscr, true);

    /* The nodelay() function controls whether getch() is a non-blocking call.
     * If the option is enabled, and no input is ready, getch() will return ERR.
     *  If disabled, getch() will hang until input is ready. */
    nodelay(stdscr, false);

    /* curs_set() alters the appearance of the text cursor. A value of 0 for
     * visibility makes the cursor disappear; a value of 1 makes
        the cursor appear "normal" and 2 makes the cursor "highly visible" */
    curs_set(0);

    /* To use color manipulating routines, start_color() must be called,
     * usually immediately after initscr(). start_color() initializes eight
     * basic colors (black, red, green, yellow, blue, magenta, cyan, and
     * white), and two global variables; COLORS and COLOR_PAIRS */
    start_color();

    // initialisation of colours
    if (!initColors()) {

        errorMessage(NO_COLORS);
        endwin();
        return 0;
    }
    color_set(GAMEBOARD, NULL);

    // menu loop
    DimensionLimits limits;
    bool end = false;
    Game::typeOfGame typeOfGame;
    do {
        drawMenu();
        typeOfGame = Game::PLAYER_SELECTION;
        int input = getch();

        switch (input) {
            case GAME1: typeOfGame = Game::RANDOM_SHUFFLE;
            case GAME2: {
                nodelay(stdscr, true);
                Game * game = new Game(typeOfGame, globalSettings, limits);
                if (game->getErrorType() != UNSPECIFIED ||
                    game->newGame() == Game::ERROR) {
                    if (!errorMessage(game->getErrorType()))
                        { delete game; endwin(); return 0; }
                }
                delete game;
                nodelay(stdscr, false);
                break;
            }
            case SETTINGS:
                globalSettings.changeSettings(limits);
                break;
            case HELP:
                help();
                break;
            case EXIT:
                end = true;
            default: ;
        }
    } while (!end);

    /* nocbreak() puts the terminal out of cbreak mode. When out of cbreak mode,
     * the terminal driver will buffer characters typed until a newline or
     * carriage return is typed. */
    nocbreak();

    /* A program should always call endwin() before exiting (or escaping from
     * curses mode temporarily). This routine will restore tty modes, move the
     * cursor to the lower left corner of the screen and reset the terminal
     * into the proper non-visual mode. */
    endwin();

    return 0;
}
