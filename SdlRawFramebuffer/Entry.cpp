#include "ImmediateDraw.h"
#include "ScanBufferDraw.h"
#include "BinHeap.h"

#include <SDL.h>
#include <SDL_mutex.h>
#include <SDL_thread.h>

#include <iostream>
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Two-thread rendering stuff:
SDL_Thread *thread = NULL; // Thread for multi-pass rendering
SDL_sem* gDataLock = NULL; // Data access semaphore, for the read buffer
ScanBuffer *BufferA, *BufferB; // pair of scanline buffers. One is written while the other is read
volatile bool quit = false; // Quit flag
volatile int writeBuffer = 0; // which buffer is being written (other will be read)
BYTE* base = NULL; // graphics base
int rowBytes = 0;
int frameByteSize = 0;

// Scanline buffer rendering on a separate thread
int worker(void* data)
{
    while (!quit) {
        SDL_Delay(1); // give the other frame a chance to lock
        if (base == NULL) continue;
        SDL_SemWait(gDataLock); //Lock
        
        auto scanBuf = (writeBuffer > 0) ? BufferA : BufferB; // must be opposite way to writing loop

        // Clear screen:
        for (auto i = 0; i < frameByteSize; i++) {
            base[i] = 255;
        }

        RenderBuffer(scanBuf, base, rowBytes, frameByteSize);

        SDL_SemPost(gDataLock); //Unlock
    }
    return 0;
}


int main(int argc, char * argv[])
{
    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
        return 1;
    } else {
        cout << "SDL initialization succeeded!\r\n";
    }

    // Create window
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError();
        return 1;
    }

    // Initialize semaphore, one reader at a time
    gDataLock = SDL_CreateSemaphore(1);
 
    // Get window surface
    screenSurface = SDL_GetWindowSurface(window);

    // Fill the surface white
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    // Update the surface
    SDL_UpdateWindowSurface(window);

    // Try some direct manipulation:
    // should read the format, but just testing.
    // this shows RGB888, but test shows the format is really RGBx8888;

    base = (BYTE*)screenSurface->pixels;
    int w = screenSurface->w;
    int h = screenSurface->h;
    rowBytes = screenSurface->pitch;
    int pixBytes = rowBytes / w;

    cout << "\r\nScreen format: " << SDL_GetPixelFormatName(screenSurface->format->format);
    cout << "\r\nBytesPerPixel: " << (pixBytes) << ", exact? " << (((screenSurface->pitch % pixBytes) == 0) ? "yes" : "no");

    frameByteSize = w * h * pixBytes;
    int animationFrames = 500;

    BufferA = InitScanBuffer(w, h);
    BufferB = InitScanBuffer(w, h);

    // run the rendering thread
    SDL_Thread* threadA = SDL_CreateThread(worker, "RenderThread", NULL);

    // Used to calculate the frames per second
    long startTicks = SDL_GetTicks();
    long idleTime = 0;
    for (auto frame = 0; frame < animationFrames; frame++) {
        long fst = SDL_GetTicks();

        // draw an animated gradient
        /*for (auto i = 0; i < frameByteSize; i += pixBytes) {
            base[i] = 255;// frame;
            base[i + 1] = 255;// i;
            base[i + 2] = 255;// i + frame;
        }

        // draw a test star of lines using immediate mode
        for (int n = 10; n < 300; n += 10) {
            auto na = n + (frame % 10);
            CoverageLine(base, rowBytes, 150, 150, na, 5,       255, 0, 0);   // red lines at top
            CoverageLine(base, rowBytes, 150, 150, na, 300,     0,   0, 255); // blue lines at bottom

            CoverageLine(base, rowBytes, 150, 150, 5,   na,     0,0,0); // black lines at the sides
            CoverageLine(base, rowBytes, 150, 150, 300, na,     0,0,0);
        }*/



        // Wait for frame render to finish, then swap buffers and do next
        SDL_SemWait(gDataLock); //Lock
        writeBuffer = 1 - writeBuffer; // switch buffer
        SDL_UpdateWindowSurface(window); //Update the surface -- need to do this every frame.
        SDL_SemPost(gDataLock); //Unlock

        auto scanBuf = (writeBuffer > 0) ? BufferA : BufferB; // must be opposite way to writing loop
        ClearScanBuffer(scanBuf); // wipe out buffer

        // Test draw with the scan buffer
        FillTrangle(scanBuf,
            430, 170,
            430, 130,
            470, 150,
            5,                              // near
            frame * 4, frame * 2, frame);   // rainbow

        FillTrangle(scanBuf,
            430, 270,
            430, 230,
            470, 250,
            15,                             // far
            frame, frame * 2, frame * 4);   // rainbow

        FillTrangle(scanBuf,
            450, 300,
            450, 100,
            670, 200,    // slight overshoot (tests clipping)
            10,          // middle
            255, 0, 0);  // red

        // a whole bunch of small triangles
        // to torture test. Also wraps top/bottom
        for (int ti = 0; ti < 500; ti++) {
            auto oti = (ti * 10) % 640;
            FillTrangle(scanBuf,
                5  + oti,    0 + ti,
                0  + oti,  -10 + ti,
                10 + oti,  -10 + ti,
                ti + 3,
                ti % 255, ti % 255, 255);
        }

        SDL_PumpEvents(); // Keep Win32 happy
        long ftime = (SDL_GetTicks() - fst);
        if (ftime < 15) SDL_Delay(15 - ftime);
        idleTime += 15 - ftime; // indication of how much slack we have
    }

    quit = true;

    long endTicks = SDL_GetTicks();
    float avgFPS = animationFrames / ((endTicks - startTicks) / 1000.f);
    float idleFraction = idleTime / (15.f*animationFrames);
    cout << "\r\nFPS ave = " << avgFPS << "\r\nIdle % = " << (100 * idleFraction);

    // Wait for user to close the window
    SDL_Event close_event;
    while (SDL_WaitEvent(&close_event)) {
        if (close_event.type == SDL_QUIT) {
            break;
        }
    }

    // Close up shop
    FreeScanBuffer(BufferA);
    FreeScanBuffer(BufferB);
    SDL_WaitThread(threadA, NULL);
    SDL_DestroySemaphore(gDataLock);
    gDataLock = NULL;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}