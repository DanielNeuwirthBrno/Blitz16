/*******************************************************************************
 Copyright 2016 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

// DimensionLimits struct
// Settings struct

#ifndef SETTINGS_H
#define SETTINGS_H

#include <curses.h>
#include <limits>

struct DimensionLimits {

    DimensionLimits();
    const uint16 minTownHeight;
    const uint16 maxTownHeight;
    const uint16 minTownWidth;
    const uint16 maxTownWidth;
    const unsigned long int gameSpeedStep;
};

static struct Settings {

    enum settingsMenu { EXIT = 48, GAME_SPEED, BOMB_SPEED, DROP_SPEED,
                        NUMBER_OF_BOMBS, DENSITY, TOWN_HEIGHT, TOWN_WIDTH,
                        DEFAULTS = 57 };
    Settings();
    ~Settings() {}

    bool initDefaults();
    void drawSettings() const;
    void changeSettings(const DimensionLimits &);

    inline void tag(const settingsMenu inRow,
                    const colors inColor = CRITICAL) const
        { mvaddch(inRow-45,COLS/2-10,'*' | COLOR_PAIR(inColor)); return; }
    inline void untag(const settingsMenu inRow) const
        { mvaddch(inRow-45,COLS/2-10,' '); return; }

    template<typename T>
    void changeSetting(T &, const T, const T = std::numeric_limits<T>::min(),
                       const T = std::numeric_limits<T>::max());

    unsigned long int gameSpeed;
    uint16 bombSpeed;
    uint16 dropSpeed;
    double bombsToBricksRatio;
    double densityCoeff;

    // actual height = LINES - heightOfTown = 15
    // (lines 9-23) in std implementation
    uint16 heightOfTown;        // <7, 24>
    // actual width = COLS - widthOfTown = 74
    // (columns 3-76) in std implementation
    uint16 widthOfTown;         // <6, 78>

} globalSettings;

#endif // SETTINGS_H
