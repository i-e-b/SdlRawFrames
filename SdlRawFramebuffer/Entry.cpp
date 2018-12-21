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
SDL_mutex* gDataLock = NULL; // Data access semaphore, for the read buffer
ScanBuffer *BufferA, *BufferB; // pair of scanline buffers. One is written while the other is read
volatile bool quit = false; // Quit flag
volatile int writeBuffer = 0; // which buffer is being written (other will be read)
BYTE* base = NULL; // graphics base
int rowBytes = 0;
int frameByteSize = 0;

// Scanline buffer to pixel buffer rendering on a separate thread
int RenderWorker(void* data)
{
    while (!quit) {
        long fst = SDL_GetTicks();
        SDL_Delay(5); // give the other frame a chance to lock
        if (base == NULL) continue;

        SDL_LockMutex(gDataLock); //Lock
        
        auto scanBuf = (writeBuffer > 0) ? BufferA : BufferB; // must be opposite way to writing loop

        RenderBuffer(scanBuf, base, rowBytes, frameByteSize);

        SDL_UnlockMutex(gDataLock); //Unlock

        // pause for the frame remainder
        long ftime = (SDL_GetTicks() - fst);
        if (ftime < 15) SDL_Delay(15 - ftime);
    }
    return 0;
}

// Drawing commands to scan buffer
void DrawToScanBuffer(ScanBuffer *scanBuf, int frame) {
    ClearScanBuffer(scanBuf); // wipe out buffer

    SetBackground(scanBuf, 10000, 0, 0, 0);

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
    for (int ti = 0; ti < 4000; ti++) {
        auto oti = (frame + ti * 7) % 640;
        auto yti = (ti >> 3);
        FillTrangle(scanBuf,
            5 + oti, 0 + yti,
            0 + oti, -10 + yti,
            10 + oti, -10 + yti,
            ti + 3,
            ti % 255, ti % 255, 255);
    }
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

    gDataLock = SDL_CreateMutex(); // Initialize lock, one reader at a time
    screenSurface = SDL_GetWindowSurface(window); // Get window surface
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF)); // Fill the surface white
    SDL_UpdateWindowSurface(window); // Update the surface

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
    SDL_Thread* threadA = SDL_CreateThread(RenderWorker, "RenderThread", NULL);

    // Used to calculate the frames per second
    long startTicks = SDL_GetTicks();
    long idleTime = 0;
    for (auto frame = 0; frame < animationFrames; frame++) {
        long fst = SDL_GetTicks();

        // Wait for frame render to finish, then swap buffers and do next
        SDL_LockMutex(gDataLock);                               // lock
        SDL_UpdateWindowSurface(window);                        // update the surface -- need to do this every frame.
        writeBuffer = 1 - writeBuffer;                          // switch buffer
        auto scanBuf = (writeBuffer > 0) ? BufferB : BufferA;   // must be opposite way to writing loop
        SDL_UnlockMutex(gDataLock);                             // unlock

        // Pick the write buffer and set switch points:
        DrawToScanBuffer(scanBuf, frame);

        // Event loop and frame delay
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
    SDL_DestroyMutex(gDataLock);
    gDataLock = NULL;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}