Experimental graphics engine
Builds under Windows with VisualStudio using SDL.

The algorithms work on raw byte buffers, and should run anywhere.

Implemented:
------------
* Immediate-mode anti-aliased lines
* Multi-threaded scan-buffer rendering
    - basic shapes, solid colors
    - layer support (issue draw commands in any order)
    - compact internal representation
    - needs no external libraries (beyond malloc and free)
    - draw commands can run in separate thread from rendering

Planned:
--------
* [x] More efficient scan buffer memory layout (speed up sorting; *maybe* also split scan buffers into per-row lists)
* [ ] Try deferred rendering (buffer the object ID, then paint in a second pass)
* [ ] Antialiasing for scan buffer (start and end points have a 'fade' length, based on gradient of curve)
* [ ] Alpha-layers in scan buffer (two pass)
* [ ] Image loading
* [ ] 'rewind' scan buffer (to allow drawing a background once, and draw dynamic elems multiple times from it)
* [x] Basic bitmap font
* [ ] Font reader and renderer (with scan-buffer switch point caching and rendering)
* [ ] Gourad (linear) shading (might be able to use the same mechanism as antialiasing)
* [ ] logical geometry for shapes in scan buffer (union, subtract, intersect, x-or)
* [ ] Texturing, with 1-bit alpha (use peek next layer -- alpha would only work for 1 layer, no alpha-on-alpha)
* [ ] Affine texturing (rotation etc)
* [ ] Perspective texturing
* [ ] Auto buffer scaling
* [ ] Pick object id from point
* [ ] merge two scanline buffers 
* [ ] overlap detection (given one object ID, output list of ids intersecting) 



Stress test measurement (release mode)
Sort is critical to performance. Here's some measurements:

Winner so far: merge, but there is very little in it

Iterative merge: FPS ave = 64.1026 Idle % = 87      | FPS ave = 64.1409 Idle % = 86.7689 | FPS ave = 64.3777 Idle % = 86.68
Quicksort:       FPS ave = 64.3363 Idle % = 84.8711 | FPS ave = 64.0014 Idle % = 86.88   | FPS ave = 64.3418 Idle % = 85.0089
Radix sort:      FPS ave = 64.1053 Idle % = 87.0356 | FPS ave = 64.4745 Idle % = 86.2489 | FPS ave = 64.0424 Idle % = 85.52

