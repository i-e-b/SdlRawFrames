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
* [ ] Antialiasing for scan-buffer
* [ ] Alpha-layers in scan buffer
* [ ] Font reader and renderer (with scan-buffer switch point caching and rendering)
* [ ] logical geometry for shapes in scan buffer (union, subtract, intersect, x-or)
* [ ] Affine texturing
* [ ] Perspective texturing