#pragma once

#define true 1
#define false 0

typedef int bool;
typedef unsigned char byte;

typedef struct TmWindow {
    int w;
    int h;
    void *handle; // HWND on windows
    void *buffer;
} TmWindow;

typedef struct TmRGB {
    byte r, g, b;
} TmRGB;

typedef struct TmFont {
    byte *data;
    int glyphWidth;
    int glyphHeight;
    unsigned int indexes[256];
} TmFont;

TmWindow *TmCreateWindow(int width, int height, const char* title);
void TmStartLoop(TmWindow *tw, void(*drawCb)(TmWindow*));
void TmPixel(TmWindow *tw, int x, int y, TmRGB colour);
void TmText(TmWindow *tw, int x, int y, const char* text, TmRGB colour, int scale, int centered);
TmRGB TmCreateRGB(byte r, byte g, byte b);
bool TmIsKeyDown(TmWindow *tw, char key);
void TmFlashWindow(TmWindow *tw);
void TmQuit(TmWindow *tw);