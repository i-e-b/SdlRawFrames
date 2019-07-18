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
* [ ] Antialiasing for scan buffer (start and end points have a 'fade' length, based on gradient of curve)
* [ ] Alpha-layers in scan buffer (two pass)
* [ ] Image loading
* [ ] 'rewind' scan buffer (to allow drawing a background once, and draw dynamic elems multiple times from it)
* [ ] Font reader and renderer (with scan-buffer switch point caching and rendering)
* [ ] Gourad (linear) shading (might be able to use the same mechanism as antialiasing)
* [ ] logical geometry for shapes in scan buffer (union, subtract, intersect, x-or)
* [ ] Texturing, with 1-bit alpha (use peek next layer)
* [ ] Affine texturing
* [ ] Perspective texturing
* [ ] Auto buffer scaling
* [ ] Pick object id from point
* [ ] merge two scanline buffers 
* [ ] overlap detection (output list of ids?) 
