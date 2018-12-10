#include "ImmediateDraw.h"

#include <iostream>
#include <SDL.h>
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


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
    int animationFrames =  500;

    // Used to calculate the frames per second
    long startTicks = SDL_GetTicks();
    for (auto frame = 0; frame < animationFrames; frame++)
    {
        long fst = SDL_GetTicks();

        // draw an animated gradient
        for (auto i = 0; i < size; i += pixBytes)
        {
            base[i] = 255;// frame;
            base[i + 1] = 255;// i;
            base[i + 2] = 255;// i + frame;
        }

        // draw a test star of lines
        for (int n = 10; n < 600; n += 15)
        {
            auto na = n + (frame % 15);
            CoverageLine(base, rowBytes, 320, 240, na, 10,    /* */  255, 0, 0);
            CoverageLine(base, rowBytes, 320, 240, na, 470,   /* */  0, 0, 255);

            if (na > 470) continue;

            CoverageLine(base, rowBytes, 320, 240, 10, na,    /* */ 0,0,0);
            CoverageLine(base, rowBytes, 320, 240, 610, na,   /* */ 0,0,0);
        }

        //Update the surface -- need to do this every frame.
        SDL_UpdateWindowSurface(window);
        SDL_PumpEvents(); // Keep Win32 happy
        long ftime = (SDL_GetTicks() - fst);
        if (ftime < 15) SDL_Delay(15 - ftime);
    }

    long endTicks = SDL_GetTicks();
    float avgFPS = animationFrames / ((endTicks - startTicks) / 1000.f);
    cout << "\r\nFPS ave = " << avgFPS;

    // Wait for window to be closed
    SDL_Event close_event;
    while (SDL_WaitEvent(&close_event)) {
        if (close_event.type == SDL_QUIT) {
            break;
        }
    }


    // Close up shop
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}