#include <iostream>
#include <SDL.h>
using namespace std;

#define BYTE unsigned char

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void CoverageLine(
    BYTE* data, int rowBytes,        // target buffer
    int x0, int y0, int x1, int y1,  // line coords
    BYTE r, BYTE g, BYTE b           // draw color
)
{
    int dx = x1 - x0, sx = (dx < 0) ? -1 : 1;
    int dy = y1 - y0, sy = (dy < 0) ? -1 : 1;

    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int e2, err = 0;

    int coverAdj = (dx + dy) / 2;
    
    int pixoff = 0; // target pixel as byte offset from base
    int ds = (dx >= dy ? sy : sx); // error sign

    int pairoff = (dx > dy ? -rowBytes : 4); // paired pixel for AA, as byte offset from main pixel.
    int errOff = (dx > dy ? dx + dy : 0); // error adjustment

    for (;;) {
        // rough approximation of coverage, based on error
        int v = (err + coverAdj - errOff) * ds;
        if (v > 127) v = 127;
        if (v < -127) v = -127;
        int lv = 127 + v;
        int rv = 127 - v;

        // set pixel, mixing original colour with target colour
        pixoff = (y0 * rowBytes) + (x0 * 4);

        data[pixoff + 0] = ((data[pixoff + 0] * lv) >> 8) + ((r * rv) >> 8);
        data[pixoff + 1] = ((data[pixoff + 1] * lv) >> 8) + ((g * rv) >> 8);
        data[pixoff + 2] = ((data[pixoff + 2] * lv) >> 8) + ((b * rv) >> 8);

        pixoff += pairoff;

        data[pixoff + 0] = ((data[pixoff + 0] * rv) >> 8) + ((r * lv) >> 8);
        data[pixoff + 1] = ((data[pixoff + 1] * rv) >> 8) + ((g * lv) >> 8);
        data[pixoff + 2] = ((data[pixoff + 2] * rv) >> 8) + ((b * lv) >> 8);


        // end of line check
        if (x0 == x1 && y0 == y1) break;

        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void BresenhamLine(BYTE* data, int rowBytes, int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0, sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0, sy = y0 < y1 ? 1 : -1;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int e2, err = (dx > dy ? dx : -dy) / 2;
    int pixoff = 0;

    for (;;) {
        // set pixel (hard coded black for now)
        pixoff = (y0 * rowBytes) + (x0 * 4);
        data[pixoff] = 0;
        data[pixoff + 1] = 0;
        data[pixoff + 2] = 0;

        // end of line check
        if (x0 == x1 && y0 == y1) break;

        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

int main(int argc, char * argv[])
{
    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
        return 1;
    }
    else
    {
        cout << "SDL initialization succeeded!";
    }

    //Create window
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError();
        return 1;
    }
 
    //Get window surface
    screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    //Update the surface
    SDL_UpdateWindowSurface(window);


    // Try some direct manipulation:
    // should read the format, but just testing.
    // this shows RGB888, but test shows the format is really RGBx8888;

    BYTE* base = (BYTE*)screenSurface->pixels;
    int w = screenSurface->w;
    int h = screenSurface->h;
    int rowBytes = screenSurface->pitch;
    int pixBytes = rowBytes / w;

    cout << "\r\nScreen format: " << SDL_GetPixelFormatName(screenSurface->format->format);
    cout << "\r\nBytesPerPixel: " << (pixBytes) << ", exact? " << (((screenSurface->pitch % pixBytes) == 0) ? "yes" : "no");

    int size = w * h * pixBytes;
    int animationFrames = 5000;

    // Used to calculate the frames per second
    long startTicks = SDL_GetTicks();
    for (size_t frame = 0; frame < animationFrames; frame++)
    {
        // draw an animated gradient
        for (auto i = 0; i < size; i += pixBytes)
        {
            BYTE v = (i + frame) % 256;
            base[i] = v;
            base[i + 1] = 255 - (v);
            base[i + 2] = (BYTE)128;
        }

        // draw a test star of lines
        for (int n = 10; n < 600; n += 15)
        {
            CoverageLine(base, rowBytes, 320, 240, n, 10,    /* */  255, 0, 0);
            CoverageLine(base, rowBytes, 320, 240, n, 470,   /* */  0, 0, 255);

            if (n > 460) continue;

            CoverageLine(base, rowBytes, 320, 240, 10, n + 10,    /* */ 0,0,0);
            CoverageLine(base, rowBytes, 320, 240, 610, n + 10,   /* */ 0,0,0);
        }

        //Update the surface -- need to do this every frame.
        SDL_UpdateWindowSurface(window);
    }

    long endTicks = SDL_GetTicks();
    float avgFPS = animationFrames / ((endTicks - startTicks) / 1000.f);
    cout << "\r\nFPS ave = " << avgFPS;

    //Wait two seconds
    //SDL_Delay(2000);
    cout << "\r\nDone. Press enter.\r\n";
    char c;
    cin.get(c);

    // Close up shop
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}