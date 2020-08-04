#include "ImmediateDraw.h"
#include "ScanBufferDraw.h"
#include "ScanBufferFont.h"


#include <SDL.h>
#include <SDL_mutex.h>
#include <SDL_thread.h>

#include <iostream>
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// If defined, renderer will attempt 60fps. Otherwise, drawing will be as fast as possible
#define FRAME_LIMIT 1
// If defined, renderer will run in a parallel thread. Otherwise, draw and render will run in sequence
#define MULTITHREAD 1
// If defined, the output screen will remain visible after the test run is complete
#define WAIT_AT_END 1

// Two-thread rendering stuff:
SDL_Thread *thread = NULL; // Thread for multi-pass rendering
SDL_mutex* gDataLock = NULL; // Data access semaphore, for the read buffer
ScanBuffer *BufferA, *BufferB; // pair of scanline buffers. One is written while the other is read
volatile bool quit = false; // Quit flag
volatile bool drawDone = false; // Quit complete flag
volatile int writeBuffer = 0; // which buffer is being written (other will be read)
volatile int frameWait = 0; // frames waiting
BYTE* base = NULL; // graphics base
int rowBytes = 0;
int frameByteSize = 0;

// Scanline buffer to pixel buffer rendering on a separate thread
int RenderWorker(void* data)
{
    while (base == NULL) {
        SDL_Delay(5);
    }
    SDL_Delay(150); // delay wake up
    while (!quit) {
        while (!quit && frameWait < 1) {
            SDL_Delay(1); // pause the thread until a new scan buffer is ready
        }

        SDL_LockMutex(gDataLock);
        auto scanBuf = (writeBuffer > 0) ? BufferA : BufferB; // must be opposite way to writing loop
        SDL_UnlockMutex(gDataLock);

        RenderBuffer(scanBuf, base);

        SDL_LockMutex(gDataLock);
        frameWait = 0;
        SDL_UnlockMutex(gDataLock);
    }
    drawDone = true;
    return 0;
}

// Drawing commands to scan buffer
void DrawToScanBuffer(ScanBuffer *scanBuf, int frame) {
    ClearScanBuffer(scanBuf); // wipe out buffer

    SetBackground(scanBuf, 10000, 50, 80, 70);
//*
    auto rx = (int)(sin(frame / 128.0f) * 80);
    auto ry = (int)(-cos(frame / 128.0f) * 80);
    FillTrangle(scanBuf, // this triangle alternates between cw and ccw
        230 + rx, 130 + ry,
        230, 170,
        270, 150,
        4, 200, 255, 200);
    
    FillCircle(scanBuf, 230 + rx, 130 + ry, 5, 3, 255, 255, 255);

    FillTrangle(scanBuf, // ccw
        470, 150,
        430, 130,
        430, 170,
        5,                              // near
        frame * 4, frame * 2, frame);   // rainbow

    FillTrangle(scanBuf, // cw
        430, 270,
        430, 230,
        470, 250,
        15,                             // far
        frame, frame * 2, frame * 4);   // rainbow

    FillTrangle(scanBuf,
        450, 300,
        450, 100,
        870, 200,    // slight overshoot (tests clipping)
        10,          // middle
        255, 0, 0);  // red

    FillRect(scanBuf,
        150, 100, 200, 200,
        8,
        0, 200, 0);

    OutlineEllipse(scanBuf,
        300, 300, 100, 35,
        4, 5,
        120, 140, 110);

    FillCircle(scanBuf,
        300, 300, 5,
        3,
        255, 255, 255);

    // a little pseudo 3d box
    FillTriQuad(scanBuf,
        50, 400,
        65, 410,
        55, 500,
        1,
        220, 0, 255);

    FillTriQuad(scanBuf,
        65, 410,
        130, 370,
        70, 510,
        1,
        255, 120, 255);

    FillTriQuad(scanBuf,
        50, 400,
        65, 410,
        115, 360,
        1,
        255, 200, 255);

    DrawLine(scanBuf,
        500, 500,
        500 + rx, 500 + ry,
        5, 15, // width
        255, 255, 0);

    DrawLine(scanBuf,
        500, 500,
        500 + ry, 500 + rx,
        5, 2, // width
        255, 0, 255);
    
 ///*
    // a whole bunch of small triangles
    // as a torture test. Also wraps top/bottom
    for (int ti = 0; ti < 6000; ti++) {
        auto oti = (frame + ti * 18) % 820;
        auto yti = (ti >> 3);
        FillTrangle(scanBuf,
            -5 + oti, 0 + yti,
            -10 + oti, -10 + yti,
            0 + oti, -10 + yti,
            ti + 3,
            ti % 255, ti % 255, 255);
    }

//*/
    auto scale = (frame * 3) % 500;
    EllipseHole(scanBuf,
        400, 300, 10 + scale, 10 + scale,
        2,
        0, 0, 0);

    // test font
    int px = 2;
    auto demo1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    auto demo2 = "Hello, world! 0123456789 !\"#$%&'()*+,-./:;<=>?`{}|~@";
    auto demo3 = "The quick brown fox jumped over the lazy dog []\\^_..";
    for (int i = 0; i < 52; i++) {
        AddGlyph(scanBuf, demo1[i], (2 + i) * 8, 20, 1, (i+1) * 322638);
        AddGlyph(scanBuf, demo2[i], (2 + i) * 8, 28, 1, 0xffffff);
        AddGlyph(scanBuf, demo3[i], (2 + i) * 8, 36, 1, 0x77ffff);

        // stress-test
        //*
        for (int j = 0; j < 70; j++) {
            AddGlyph(scanBuf, demo1[i], (i) * 8, (j + 7) * 8 + ((i+(frame>>2))%5), 3, 0x77ffff);
            AddGlyph(scanBuf, demo1[i], (52 + i) * 8, (j + 7) * 8 + ((i+(frame>>2))%5), 15, 0xffff77);
        }
        //*/
    }


    // quick test: number of points:
    /*int k,j = 0;
    int m = 0;
    for (int i = 0; i < scanBuf->height; i++) {
        k = scanBuf->scanLines[i].count;
        j+=k;
        if (k > m)m = k;
    }
    cout << "\nTotal items drawn: " << scanBuf->itemCount;
    cout << "\nTotal switch points: " << j;
    cout << "\nLargest line: " << m;*/
}

// We undefine the `main` macro in SDL_main.h, because it confuses the linker.
#undef main
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
    int animationFrames = 1500;

    BufferA = InitScanBuffer(w, h);
    BufferB = InitScanBuffer(w, h);

    // run the rendering thread
#ifdef MULTITHREAD
    SDL_Thread* threadA = SDL_CreateThread(RenderWorker, "RenderThread", NULL);
#endif

    // Used to calculate the frames per second
    long startTicks = SDL_GetTicks();
    long idleTime = 0;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw loop                                                                                      //
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    auto scanBuf = BufferA;
    for (auto frame = 0; frame < animationFrames; frame++) {
        long fst = SDL_GetTicks();

        SDL_UpdateWindowSurface(window);                        // update the surface -- need to do this every frame.

        // Wait for frame render to finish, then swap buffers and do next

#ifdef MULTITHREAD
        if (frameWait < 1) {
            // Swap buffers, we will render one to pixels while we're issuing draw commands to the other
            // If render can't keep up with framewait, we skip this frame and draw to the same buffer.
            SDL_LockMutex(gDataLock);                               // lock
            writeBuffer = 1 - writeBuffer;                          // switch buffer
            scanBuf = (writeBuffer > 0) ? BufferB : BufferA;        // MUST be opposite way to writing loop
            frameWait = 1;                                          // signal to the other thread that the buffer has changed
            SDL_UnlockMutex(gDataLock);                             // unlock
        }
#endif

        // Pick the write buffer and set switch points:
        DrawToScanBuffer(scanBuf, frame);

#ifndef MULTITHREAD
        RenderBuffer(scanBuf, base);
#endif

        // Event loop and frame delay
#ifdef FRAME_LIMIT
        SDL_PumpEvents(); // Keep Win32 happy
        long ftime = (SDL_GetTicks() - fst);
        if (ftime < 15) SDL_Delay(15 - ftime);
        idleTime += 15 - ftime; // indication of how much slack we have
#endif
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    quit = true;
    frameWait = 100;

    long endTicks = SDL_GetTicks();
    float avgFPS = animationFrames / ((endTicks - startTicks) / 1000.f);
    float idleFraction = idleTime / (15.f*animationFrames);
    cout << "\r\nFPS ave = " << avgFPS << "\r\nIdle % = " << (100 * idleFraction);

#ifdef MULTITHREAD
    while (!drawDone) { SDL_Delay(100); }// wait for the renderer to finish
#endif

#ifdef WAIT_AT_END
    // Wait for user to close the window
    SDL_Event close_event;
    while (SDL_WaitEvent(&close_event)) {
        if (close_event.type == SDL_QUIT) {
            break;
        }
    }
#endif

    // Close up shop
    FreeScanBuffer(BufferA);
    FreeScanBuffer(BufferB);
#ifdef MULTITHREAD
    SDL_WaitThread(threadA, NULL);
#endif
    SDL_DestroyMutex(gDataLock);
    gDataLock = NULL;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

#pragma comment(linker, "/subsystem:Console")
//#pragma comment( linker, "/entry:\"main\"" )
