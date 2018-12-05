#include <iostream>
#include <SDL.h>
using namespace std;


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
    for (size_t frame = 0; frame < 5000; frame++)
    {

        // draw a simple gradient
        for (auto i = 0; i < size; i += pixBytes)
        {
            char v = (i + frame) % 256;
            base[i] = v;
            base[i + 1] = 255 - (v);
            base[i + 2] = (char)128;
        }

        // draw a line
        BresenhamLine(base, rowBytes, 10, 20, 600, 400);

        //Update the surface -- need to do this every frame.
        SDL_UpdateWindowSurface(window);
    }


    //Wait two seconds
    //SDL_Delay(2000);

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
    return 0;
}