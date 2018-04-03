#include <time.h>
#include <stdio.h>

#include "tomarto_draw.h"
#include "config.h"

static time_t startTime;
static time_t frameStart;
static double timeToRun = TOMARTO_TIME_TO_RUN;
static double timeToBreak = TOMARTO_TIME_TO_BREAK;
static bool running = false;
static bool onBreak = false;

// TODO: Context menu might be nice.
//   Bleh, this is weird. There's some commented out code that's the start of this.

void TomartoDraw(TmWindow *tw) {
    time_t currentTime = time(NULL);
    double frameTime = difftime(currentTime, frameStart);
    double *currentTimeToRun = onBreak ? &timeToBreak : &timeToRun;
    frameStart = currentTime;

    if (!running) {
        startTime += frameTime;
    }

    double diff = *currentTimeToRun - difftime(currentTime, startTime);

    // TODO rect function
    TmRGB bg;
    if (running && (int)diff == 0) {
        bg = onBreak ? TmCreateRGB(TOMARTO_COLOUR_BREAK) : TmCreateRGB(TOMARTO_COLOUR_WORK);
    } else {
        bg = TmCreateRGB(TOMARTO_COLOUR_BG);
    }
    for (int x = 0; x < tw->w; x++) {
        for (int y = 0; y < tw->h; y++) {
            TmPixel(tw, x, y, bg);
        }
    }

    char formattedTime[100];
    sprintf(formattedTime, "%02d:%02d", (int)diff / 60, (int)diff % 60);

    TmRGB textColour;
    if (running) {
        textColour = onBreak ? TmCreateRGB(TOMARTO_COLOUR_BREAK) : TmCreateRGB(TOMARTO_COLOUR_WORK);
    } else {
        textColour = TmCreateRGB(TOMARTO_COLOUR_PAUSED);
    }
    TmText(tw, TOMARTO_WIN_WIDTH / 2, TOMARTO_WIN_HEIGHT / 2, formattedTime, textColour, 1, 1);

    if (TmIsKeyDown(tw, 'S')) {
        running = !running;
    }
    if (TmIsKeyDown(tw, 'R')) {
        running = false;
        onBreak = false;
        startTime = time(NULL);
    }
    if (TmIsKeyDown(tw, 'Q')) {
        TmQuit(tw);
    }
    if (TmIsKeyDown(tw, '=')) {
        *currentTimeToRun += 10;
    }
    if (TmIsKeyDown(tw, '-')) {
        *currentTimeToRun -= 10;
        if (*currentTimeToRun < 0) {
            *currentTimeToRun = 0;
        }
    }
    if (TmIsKeyDown(tw, 'B')) {
        running = false;
        onBreak = true;
        startTime = time(NULL);
    }

    if ((int)diff == 0) {
        running = false;
        TmFlashWindow(tw);
    }
}

void TomartoInit() {
    TmWindow *tw = TmCreateWindow(TOMARTO_WIN_WIDTH, TOMARTO_WIN_HEIGHT, "Tomarto");

    if (!tw) {
        return;
    }

    startTime = frameStart = time(NULL);
    TmStartLoop(tw, TomartoDraw);
}
