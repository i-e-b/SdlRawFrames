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

    int coverAdj = (dx > dy ? dx : dy) / 2;
    
    int pixoff = 0;
    int v;
    int ds = sx * 2;

    for (;;) {
        // SUPER rough approximation of coverage, based on error
        // only really works for tall lines in this state
        v = (err + coverAdj) / ds;

        // set pixel (hard coded black for now)
        pixoff = (y0 * rowBytes) + ((x0-1) * 4);
        data[pixoff] = 128 + v;
        data[pixoff + 1] = 128 + v;
        data[pixoff + 2] = 128 + v;

        data[pixoff + 4] = 128 - v;
        data[pixoff + 5] = 128 - v;
        data[pixoff + 6] = 128 - v;

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
    // this assumes RGB888
    // My first test shows the format is really RGB?8888;

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

        // draw an animated gradient
        /*for (auto i = 0; i < size; i += pixBytes)
        {
            char v = (i + frame) % 256;
            base[i] = v;
            base[i + 1] = 255 - (v);
            base[i + 2] = (char)128;
        }*/

        // draw some lines
        BresenhamLine(base, rowBytes, 10, 20, 600, 400);
        BresenhamLine(base, rowBytes, 30, 20, 600, 25);

        BresenhamLine(base, rowBytes, 20, 10, 25, 400);

        CoverageLine(base, rowBytes, 30, 10, 31, 400); // very small dx/dy
        CoverageLine(base, rowBytes, 50, 10, 35, 400); // reverse gradient
        CoverageLine(base, rowBytes, 50, 10, 150, 400); // steeper
        CoverageLine(base, rowBytes, 30, 50, 600, 55); // flat line (not working yet)

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