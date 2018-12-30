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
* [ ] Image loading
* [ ] 'rewind' scan buffer (to allow drawing a background once, and draw dynamic elems multiple times from it)
* [ ] Antialiasing for scan buffer
* [ ] Alpha-layers in scan buffer
* [ ] Font reader and renderer (with scan-buffer switch point caching and rendering)
* [ ] Gourad (linear) shading
* [ ] logical geometry for shapes in scan buffer (union, subtract, intersect, x-or)
* [ ] Texturing, with 1-bit alpha (use peek next layer)
* [ ] Affine texturing
* [ ] Perspective texturing
* [ ] Auto buffer scaling
* [ ] Pick object id from point
* [ ] merge two scanline buffers 
* [ ] overlap detection (output list of ids?) 
