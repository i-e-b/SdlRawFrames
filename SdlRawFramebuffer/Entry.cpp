#include <iostream>
#include <SDL.h>
using namespace std;


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void CoverageLine(char* data, int rowBytes, int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0, sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0, sy = y0 < y1 ? 1 : -1;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int e2, err = 0;

    int coverAdj = (dx + dy) / 2;
    
    int pixoff = 0; // pixel as offset from base
    int v;
    int ds = (dx > dy ? sy : sx) * 2;

    int pairoff = (dx > dy ? -rowBytes : 4); // paired pixel for AA, as offset from main pixel.
    int errOff = (dx > dy ? dx + dy : 0); // error adjustment

    for (;;) {
        // rough approximation of coverage, based on error
        v = (err + coverAdj - errOff) / ds;

        // very slight int overshoot?
        if (v > 128) v = 0;
        if (v < -128) v = 0;

        // set pixel (hard coded black for now)
        pixoff = (y0 * rowBytes) + (x0 * 4);
        data[pixoff + 0] = 128 + v;
        data[pixoff + 1] = 128 + v;
        data[pixoff + 2] = 128 + v;

        pixoff += pairoff;
        data[pixoff + 0] = 128 - v;
        data[pixoff + 1] = 128 - v;
        data[pixoff + 2] = 128 - v;

        // end of line check
        if (x0 == x1 && y0 == y1) break;

        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void BresenhamLine(char* data, int rowBytes, int x0, int y0, int x1, int y1)
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

    char* base = (char*)screenSurface->pixels;
    int w = screenSurface->w;
    int h = screenSurface->h;
    int rowBytes = screenSurface->pitch;
    int pixBytes = rowBytes / w;

    cout << "\r\nScreen format: " << SDL_GetPixelFormatName(screenSurface->format->format);
    cout << "\r\nBytesPerPixel: " << (pixBytes) << ", exact? " << (((screenSurface->pitch % pixBytes) == 0) ? "yes" : "no");

    int size = w * h * pixBytes;
    int animationFrames = 1;

    for (size_t frame = 0; frame < animationFrames; frame++)
    {


        // clear to white
        for (auto i = 0; i < size; i += pixBytes)
        {
            base[i] = 255;base[i + 1] = 255;base[i + 2] = 255;
        }
        /*
        // draw an animated gradient
        for (auto i = 0; i < size; i += pixBytes)
        {
            char v = (i + frame) % 256;
            base[i] = v;
            base[i + 1] = 255 - (v);
            base[i + 2] = (char)128;
        }*/

        // draw a test star of lines
        for (int n = 10; n < 600; n += 15)
        {
            CoverageLine(base, rowBytes, 320, 240, n, 10);
            CoverageLine(base, rowBytes, 320, 240, n, 470);

            if (n > 470) continue;

            CoverageLine(base, rowBytes, 320, 240, 10, n);
            CoverageLine(base, rowBytes, 320, 240, 610, n);
        }

        //Update the surface -- need to do this every frame.
        SDL_UpdateWindowSurface(window);
    }


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