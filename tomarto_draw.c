#include <Windows.h>
#include <Windowsx.h>
#include <strsafe.h>
#include <string.h>

#include "tomarto_draw.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "resource.h"

#define TM_DEFAULT_FONT_SIZE 12;
#define TM_FONT_WIDTH 10
#define TM_FONT_HEIGHT 20

static char *glyphMap = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+-={}|[]\\:\"'<>?;,.";

static TmFont *font;
static bool running;
static HDC hdc;
static byte keys[256] = { 0 };

static void toWchar(char *str, wchar_t *wstr, size_t len) {
    StringCbPrintf(wstr, len, L"%hs", str);
}

static void Error(HWND hWnd, wchar_t *message) {
    MessageBox(hWnd, message, L"Error!", 0);
    if (hWnd) {
        SendMessage(hWnd, WM_CLOSE, 0, 0);
    }
}


static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            running = false;
            break;
        }
        case WM_KEYDOWN:
        case WM_KEYUP: {
            UINT32 vkCode = wParam;
            if (vkCode == VK_OEM_PLUS) vkCode = '=';
            if (vkCode == VK_OEM_MINUS) vkCode = '-';
            bool down = (lParam & (1 << 31)) == 0;
            if (vkCode < 256) {
                keys[vkCode] = down ? 1 : 0;
            }
        }
        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProc(hWnd, uMsg, wParam, lParam);
            if (hit == HTCLIENT) {
                hit = HTCAPTION;
            }
            return hit;
        }
        /*
        case WM_CONTEXTMENU: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            HMENU menu = CreatePopupMenu();

            MENUITEMINFO test = {0};
            test.cbSize = sizeof(test);
            test.fMask = MIIM_STRING;
            test.dwTypeData = L"test";
            test.cch = 5;

            InsertMenuItem(menu, 0, 1, &test);

            int ret = TrackPopupMenu(
                menu,
                TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NOANIMATION,
                x, y,
                0,
                hWnd,
                0
            );
            int a = 4;
                
        }
        */
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

TmWindow *TmCreateWindow(int width, int height, const char* title) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    wchar_t wTitle[100];
    toWchar(title, wTitle, 100);

    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInst;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = L"tomarto";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        Error(NULL, L"Error registering window class");
        return NULL;
    }

    RECT rect;
    rect.left = rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int screenWidth = workArea.right - workArea.left;
    int screenHeight = workArea.bottom - workArea.top;

    HWND hWnd = CreateWindow(
        L"tomarto",
        wTitle,
        WS_POPUP | WS_SYSMENU,
        // Place the window in the bottom right corner of the screen
        screenWidth - width, screenHeight - height,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL,
        NULL,
        hInst,
        NULL
    );

    if (!hWnd) {
        Error(NULL, L"Error creating window");
        return NULL;
    }

    HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(RESOURCE_ICON));
    if (!hIcon) {
        Error(NULL, L"Error loading icon resource");
        return NULL;
    }
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    
    hdc = GetDC(hWnd);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    TmWindow *tw = malloc(sizeof(TmWindow));
    tw->w = width;
    tw->h = height;
    tw->handle = hWnd;
    tw->buffer = calloc(width * height * 4, sizeof(UINT32)); // 4bpp
    return tw;
}

void Blit(TmWindow *tw) {
    BITMAPINFO info;
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = tw->w;
    info.bmiHeader.biHeight = -tw->h;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(
        hdc,
        0, 0,
        tw->w, tw->h,
        0, 0,
        tw->w, tw->h,
        tw->buffer,
        &info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

void CleanUp(TmWindow *tw) {
    free(tw->buffer);
    free(tw);
    free(font->data);
    ReleaseDC((HWND)tw->handle, hdc);
}

void TmStartLoop(TmWindow *tw, void(*drawCb)(TmWindow*)) {
    running = true;

    while (running) {
        // Process message queue
        MSG msg;
        while (PeekMessage(&msg, (HWND)tw->handle, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        drawCb(tw);
        Blit(tw);

        Sleep(66);
    }

    CleanUp(tw);
}

// TODO: Make this safe
void TmPixel(TmWindow *tw, int x, int y, TmRGB colour) {
    unsigned int *buf = (unsigned int *)tw->buffer;
    buf[x + y * tw->w] =
        (unsigned int)colour.r << 16
        | (unsigned int)colour.g << 8
        | (unsigned int)colour.b;
}

// TODO: Make this safe
void TmText(TmWindow *tw, int x, int y, const char* text, TmRGB colour, int scale, int center) {
    unsigned int *buf = (unsigned int *)tw->buffer;
    int len = strlen(text);

    if (center) {
        x -= (len * font->glyphWidth * scale) / 2;
        y -= (font->glyphHeight * scale) / 2;
    }

    int offset = 0;
    for (char *c = text; *c != '\0'; c++) {
        if (*c != ' ') {
            int index = font->indexes[(byte)*c];
            for (int fontY = 0; fontY < font->glyphHeight; fontY++) {
                for (int fontX = 0; fontX < font->glyphWidth; fontX++) {

                    if (font->data[index + fontX + fontY * font->glyphWidth] > 0) {
                        for (int sx = 0; sx < scale; sx++) {
                            for (int sy = 0; sy < scale; sy++) {
                                TmPixel(tw,
                                    x + (fontX + offset) * scale + sx,
                                    y + (fontY * scale) + sy,
                                    colour
                                );
                            }
                        }
                    }

                }
            }
        }
        offset += font->glyphWidth;
    }
}

TmRGB TmCreateRGB(byte r, byte g, byte b) {
    TmRGB rgb;
    rgb.r = r;
    rgb.g = g;
    rgb.b = b;
    return rgb;
}

bool TmIsKeyDown(TmWindow *tw, char key) {
    bool isDown = keys[key];
    keys[key] = false;
    return isDown;
}

void TmFlashWindow(TmWindow *tw) {
    FLASHWINFO flashInfo;
    flashInfo.cbSize = sizeof(flashInfo);
    flashInfo.hwnd = (HWND)tw->handle;
    flashInfo.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
    flashInfo.uCount = 3;
    flashInfo.dwTimeout = 0;

    FlashWindowEx(&flashInfo);
}

void TmQuit(TmWindow *tw) {
    running = false;
}

int InitFont() {
    int width, height, n;

    HMODULE handle = GetModuleHandle(NULL);
    HRSRC resourceHandle = FindResource(
        handle,
        MAKEINTRESOURCE(RESOURCE_BMP_FONT_ID),
        RT_RCDATA
    );

    if (!resourceHandle) {
        Error(NULL, L"Error getting bitmap font resource handle");
        return 0;
    }

    HGLOBAL resource = LoadResource(handle, resourceHandle);
    byte *rawData = (byte *)LockResource(resource);
    DWORD size = SizeofResource(handle, resource);
    byte *data = stbi_load_from_memory(rawData, size, &width, &height, &n, 1);

    if (!data) {
        Error(NULL, L"Error loading bitmap font");
        return 0;
    }

    font = calloc(1, sizeof(TmFont));
    font->glyphWidth = TM_FONT_WIDTH;
    font->glyphHeight = TM_FONT_HEIGHT;
    font->data = calloc(width * height, 1);

    // Transpose the glyph data.
    // e.g. starting with two 2x2 glyphs:
    //   | 1 2 | 5 6 |
    //   | 3 4 | 7 8 |
    //
    // These end up in the data array like this:
    //   [1, 2, 5, 6, 3, 4, 7, 8]
    //
    // We want them to be like this:
    //  [1, 2, 3, 4, 5, 6, 7, 8]
    // One glyph is contiguous in memory, so all we need to do is store the
    // offset for that glyph to find it later.
    int glyphCount = width / font->glyphWidth;
    for (int i = 0; i < glyphCount; i++) {
        for (int y = 0; y < height; y++) {
            memcpy(
                &font->data[(i * font->glyphWidth * font->glyphHeight) + (y * font->glyphWidth)],
                &data[(i * font->glyphWidth) + (y * width)],
                font->glyphWidth
            );
        }
        font->indexes[glyphMap[i]] = i * font->glyphWidth * font->glyphHeight;
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!InitFont()) {
        Error(NULL, L"Error initializing font");
        return 1;
    }
    TomartoInit();
}
