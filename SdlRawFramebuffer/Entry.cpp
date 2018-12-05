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
    // this assumes RGB888
    // My first test shows the format is really RGB?8888;

    char* base = (char*)screenSurface->pixels;
    int w = screenSurface->w;
    int h = screenSurface->h;
    int pixBytes = screenSurface->pitch / screenSurface->w;

    cout << "\r\nScreen format: " << SDL_GetPixelFormatName(screenSurface->format->format);
    cout << "\r\nBytesPerPixel: " << (pixBytes) << ", exact? " << (((screenSurface->pitch % pixBytes) == 0) ? "yes" : "no");

    int size = w * h * pixBytes;
    for (size_t frame = 0; frame < 5000; frame++)
    {

        for (size_t i = 0; i < size; i += pixBytes)
        {
            char v = (i + frame) % 256;
            base[i] = v;
            base[i + 1] = 255 - (v);
            base[i + 2] = 128;
        }

        //Update the surface -- need to do this every time we write.
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