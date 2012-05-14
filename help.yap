@ 2
t -10 0 10 Welcome to yaplot (Yet another plot)
t -10 0 8  1. Introduction
t -10 0 6  2. Key assignment
t -10 0 4  3. Primitive elements
t -10 0 2  4. Data format
t -10 0 0  5. Command line usage
t -10 0 -2  6. Palette file format
t -10 0 -4  7. Sample image
@ 3
t -10 0 -10 Press "g" to the first page.
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.
t -10 0 -13 Press TAB to reset rotations.

@ 2
t -10 0 10 1. Introduction
t -10 0  8 Yaplot (yet another plot) is designed to handle 
t -10 0  7 3-dimensional wireframe-like objects and their 
t -10 0  6 animation easily.
t -10 0  5 A set of primitive drawing commands, including lines,
t -10 0  4 polygons, circles, texts, etc., is available (corresponds
t -10 0  3 to plotter commands for 2-d, like HPGL etc.). You can also
t -10 0  2 make an animation simply by listing these commands.
t -10 0 -7 (This document is also a yaplot data. Drag it by mouse!) 
@ 3
t -10 0 -10 Press "g" to the first page.
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.
t -10 0 -13 Press TAB to reset rotations.

@ 2
t -10 0 10 2.1 Key assignment 1/2
t -10 0  8 The browsing objects can be rotated by keyboard or mouse.
t -10 0  7 Up and Down keys:     Pitching rotation 
t -10 0  6 Left and Right keys:  Heading rotation
t -10 0  5 mouse dragging        also causes immediate rotation.
t -10 0  4 Pause                 Stop rotation.
t -10 0  3 Tab                   Reset rotation.
t -10 0  2 asterisque and slash:  Zoom in and out(telescopic and wide sight)
t -10 0  1 PageDown and PageUp:  Next and previous frame
t -10 0  0 "n" and "p":          Next and previous frame
t -10 0 -1                      (Animated automatically if SHIFT is pressed.)
t -10 0 -2 "g" and "Home"        Jump to the first frame.
t -10 0 -3 "G" and "End"         Jump to the last frame.
t -10 0 -4 "s"                   Synchronous and asynchronous mode
t -10 0 -5 "v"                   Toggle verbosity
t -10 0 -6 F1..F8                Toggle layers
t -10 0 -7 SPACE                 Stop all motion.
t -10 0 -8 "+" and "-"           Change quality of drawing
t -10 0 -9 Insert and Delete     Control the line thickness
t -10 0 -10 "q" or Break          Quit
#t -10 0 -6 (10-key pad is also available if configured.)
@ 3
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.

@ 2
t -10 0 10 2.2 Key assignment 2/2
t -10 0  8 Commands preceded by number keys provides quick action (like vi).
t -10 0  7 Up and Down keys:     Pitching rotation 
t -10 0  6 Left and Right keys:  Heading rotation
t -10 0  2 asterisque and slash: Zoom in and out(telescopic and wide sight)
t -10 0  1 PageDown and PageUp:  Skip N frames.
t -10 0  0 "n" and "p":          Skip N frames.
t -10 0 -1 "N" and "P"           Automatic animation in frame rate N/sec
t -10 0 -2 "g","G", or Enter:    Jump to the frame N.
t -10 0 -3 Insert and Delete     Control the line thickness
@ 3
t -10 0 -10 Press "g" to the first page.
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.
t -10 0 -13 Press TAB to reset rotations.

@ 2
t -10 0  10 3.1 Primitive elements
r 1
c  -8 0   7
t -9 0   5 circle
l  -6 0   8 -4 0 6 
t  -6 0   5 line
r 0.2
s  -3 0   8 -1 0 6 
t  -3 0   5 stick
p 3 0 0 8 1 0 6 2 0 7
t 0 0 5 polygon
t -10 0   0 3.2 Marker symbols
r 1
o  -8 0   -2
t  -9 0   -4 ring
3  -5 0   -3
t  -6 0   -5 triangle
4  -2 0   -2
t  -3 0   -4 square
5   1 0   -3
t   0 0   -5 pentagon
6   4 0   -2
t   3 0   -4 hexagon
@ 3
r 1
t   7 0   -3 text
@ 2
t   6 0   -5 text string
t -10 0  -6 Line thickness is changed automatically.
t -10 0  -7 Pressing keys "+" / "-" changes the quality of drawing;
t -10 0  -8 {Skeleton, Outlined, Translucent, and Opaque}
@ 3
t -10 0 -10 Press "g" to the first page.
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.
t -10 0 -13 Press TAB to reset rotations.

@ 2
t -10 0  14 3.3 Source data of the previous page
@ 3
t -10 0  12  @ 2
t -10 0  11 t -10 0  10 3.1 Primitive elements
t -10 0  10 r 1
t -10 0   9 c  -8 0   7
t -10 0   8 t -9 0   5 circle
t -10 0   7 l  -6 0   8 -4 0 6 
t -10 0   6 t  -6 0   5 line
t -10 0   5 r 0.2
t -10 0   4 s  -3 0   8 -1 0 6 
t -10 0   3 t  -3 0   5 stick
t -10 0   2 p 3 0 0 8 1 0 6 2 0 7
t -10 0   1 t 0 0 5 polygon
t -10 0   0 t -10 0   0 3.2 Marker symbols
t -10 0  -1 r 1
t -10 0  -2 o  -8 0   -2
t -10 0  -3 t  -9 0   -4 ring
t -10 0  -4 @ 3
t -10 0  -5 r 1
t -10 0  -6 t  -6 0   -2 text
t -10 0  -7 @ 2
t -10 0  -8 t  -6 0   -4 text string
t -10 0  -9 t -10 0  -6 Line thickness is changed automatically.
t -10 0 -10 t -10 0  -7 Pressing keys "+" / "-" changes the quality of drawing;
t -10 0 -11 t -10 0  -8 {Skeleton, Outlined, Translucent, and Opaque}
t -10 0 -12 @ 3
t -10 0 -10 Press "g" to the first page.
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.
t -10 0 -13 Press TAB to reset rotations.

@ 2
t -10 0  10 4. Data format
t -10 0   8 All command lines in the data file are made of the single
t -10 0   7 command character at the first column, followed by
t -10 0   6 parameters. No parameters can be omitted.
t -10 0   5 You can easily make the data file by Fortran or other
t -10 0   4 scripting languages.
t -10 0   2 r {radius} Specify the radius of the circle, ring, and stick
t -10 0   1 l {x} {y} {z} {x} {y} {z} Draw a line
t -10 0   0 s {x} {y} {z} {x} {y} {z} Draw a stick. (=thick line)
t -10 0  -1 p {x} {y} {z} {x} {y} {z} {x} {y} {z} ...Draw a polygon
t -10 0  -2 c {x} {y} {z} Draw a circle
t -10 0  -3 o {x} {y} {z} Draw a ring
t -10 0  -4 t {x} {y} {z} {text....} Draw a string
t -10 0  -5 @ {palette} Change the current palette
t -10 0  -6 y {layer} Change the current layer
t -10 0  -7 # {comment} Comment(ignored)
t -10 0  -8 NULL line: End of a frame
@ 3
t -10 0 -10 Press "g" to the first page.
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.
t -10 0 -13 Press TAB to reset rotations.

@ 2
t -10 0  12 5. Command line usage
t -10 0  10 usage : yaplot [-e x,y,z][-l x,y,z][-u x,y,z][-c palette] infile [infile...]
t -10 0   9    -e x,y,z        set eyepoint (default: {0,-40,0})
t -10 0   8    -l x,y,z        set lookpoint (default: {0,0,0})
t -10 0   7    -u x,y,z        set upvector (default: {0,0,1})
t -10 0   6    -q n            set cache size to n frames (default: 1)
t -10 0   5    -c filename     specify palette file(default: mol3.col)
t -10 0   2 You can specify single "-" for input file, which means
t -10 0   1 stdin. Multiple frame animation is, however, not available
t -10 0   0 because stdin cannot be accessed randomly by fseek().
t -10 0  -2 If you do not specify the palette file, "mol3.col" in current
t -10 0  -3 directory is searched. If not found, the environment variable
t -10 0  -4 "MOL3COL" is used instead. If not found, "mol3.col" in your home
t -10 0  -5 directory will be used.
t -10 0  -7 If multiple infile is given, they are animated in different 
t -10 0  -8 windows and handled at a time. 
t -10 0  -9 (You can also choose asynchronous handling mode.)
@ 3
t -10 0 -10 Press "g" to the first page.
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.
t -10 0 -13 Press TAB to reset rotations.

@ 2
t -10 0  10 6. Palette file format
t -10 0   8 Each line of the palette file specifies the Red, Green and
t -10 0   7 Blue intensity of the palette in integer between
t -10 0   6 0..255. First line describes the color for palette 0. As
t -10 0   5 the palettes 0 to 2 are  reserved for system color (Black,
t -10 0   4 Gray, and White), you should not modify them. 
t -10 0   2 Here is a sample palette data:
@ 0
t -10 0   0 0 0 0 (Black)
@ 2
t -10 0  -1 100 100 100 (Gray, though it isn't...)
t -10 0  -2 255 255 255 (White)
@ 3
t -10 0  -3 0 255 0 (Green)
@ 3
t -10 0 -10 Press "g" to the first page.
t -10 0 -11 Press "n" to next page.
t -10 0 -12 Press "p" to previous page.
t -10 0 -13 Press TAB to reset rotations.

@ 2
t -10 0 10 7. Sample Image
r 0.1
@ 2
l -4 -4 -4 4 -4 -4
l -4 -4 -4 -4 4 -4
l -4 -4 -4 -4 -4 4
l -4 -4 4 4 -4 4
l 4 -4 -4 4 4 -4
l -4 4 -4 -4 4 4
l -4 4 -4 4 4 -4
l -4 -4 4 -4 4 4
l 4 -4 -4 4 -4 4
l -4 4 4 4 4 4
l 4 -4 4 4 4 4
l 4 4 -4 4 4 4
@ 3
l -4 -4 -3 -4 -4 -2
l -4 -4 -2 -3 -4 -2
l -4 -4 -2 -4 -3 -2
l -4 -4 -2 -4 -4 -1
l -4 -4 3 -4 -3 3
l -4 -2 -4 -3 -2 -4
l -4 -2 0 -4 -1 0
l -4 -2 2 -3 -2 2
l -4 -1 -3 -4 0 -3
l -4 -1 0 -3 -1 0
l -4 0 3 -4 1 3
l -4 1 2 -4 1 3
l -4 1 3 -3 1 3
l -4 3 -1 -4 4 -1
l -4 3 -1 -4 3 0
l -3 -4 2 -3 -3 2
l -3 -3 -4 -2 -3 -4
l -3 -3 -4 -3 -2 -4
l -3 -3 -4 -3 -3 -3
l -3 -3 -3 -2 -3 -3
l -3 -3 2 -2 -3 2
l -3 -3 2 -3 -2 2
l -3 -2 -4 -2 -2 -4
l -3 -2 -2 -3 -2 -1
l -3 -2 2 -3 -2 3
l -3 -2 3 -3 -2 4
l -3 0 -4 -2 0 -4
l -3 1 3 -2 1 3
l -3 2 2 -3 3 2
l -3 3 -2 -3 4 -2
l -3 3 2 -3 4 2
l -2 -4 -4 -1 -4 -4
l -2 -4 -4 -2 -3 -4
l -2 -4 3 -1 -4 3
l -2 -4 3 -2 -4 4
l -2 -3 -4 -1 -3 -4
l -2 -3 -4 -2 -2 -4
l -2 -3 -4 -2 -3 -3
l -2 -2 1 -1 -2 1
l -2 -2 1 -2 -1 1
l -2 0 -4 -2 0 -3
l -2 0 0 -1 0 0
l -2 2 1 -1 2 1
l -2 3 -4 -1 3 -4
l -2 3 -4 -2 4 -4
l -2 3 3 -2 4 3
l -2 3 3 -2 3 4
l -1 -4 -4 -1 -3 -4
l -1 -4 0 0 -4 0
l -1 -4 0 -1 -3 0
l -1 -4 3 -1 -4 4
l -1 -3 -2 0 -3 -2
l -1 -3 0 -1 -3 1
l -1 -3 1 -1 -2 1
l -1 -2 1 -1 -2 2
l -1 -2 2 -1 -2 3
l -1 -1 0 0 -1 0
l -1 -1 0 -1 0 0
l -1 0 0 0 0 0
l -1 0 0 -1 1 0
l -1 0 3 0 0 3
l -1 3 -4 0 3 -4
l -1 3 -4 -1 4 -4
l 0 -4 -2 0 -3 -2
l 0 -4 -2 0 -4 -1
l 0 -4 -1 0 -4 0
l 0 -4 0 1 -4 0
l 0 -3 -2 0 -2 -2
l 0 -2 -2 1 -2 -2
l 0 -2 -2 0 -2 -1
l 0 -1 0 0 0 0
l 0 0 -1 1 0 -1
l 0 0 -1 0 0 0
l 0 0 2 0 1 2
l 0 0 2 0 0 3
l 0 0 3 0 1 3
l 0 1 2 0 2 2
l 0 1 2 0 1 3
l 0 1 3 1 1 3
l 0 1 3 0 2 3
l 0 2 2 1 2 2
l 0 2 2 0 2 3
l 0 3 -4 0 3 -3
l 1 -4 0 2 -4 0
l 1 -4 0 1 -4 1
l 1 -2 -2 1 -1 -2
l 1 -1 -4 2 -1 -4
l 1 -1 -4 1 -1 -3
l 1 -1 -3 1 0 -3
l 1 -1 -3 1 -1 -2
l 1 -1 1 2 -1 1
l 1 0 -1 2 0 -1
l 1 2 1 1 2 2
l 1 2 2 2 2 2
l 1 3 0 1 4 0
l 1 3 3 1 4 3
l 2 -4 0 3 -4 0
l 2 -4 2 3 -4 2
l 2 -2 -4 2 -1 -4
l 2 -1 3 2 -1 4
l 2 1 -3 2 1 -2
l 2 1 -2 2 2 -2
l 2 2 -2 2 3 -2
l 2 2 2 3 2 2
l 2 2 2 2 3 2
l 2 3 -2 3 3 -2
l 2 3 2 2 4 2
l 3 -4 -2 4 -4 -2
l 3 -4 0 3 -3 0
l 3 -4 2 3 -4 3
l 3 -4 3 4 -4 3
l 3 -4 3 3 -3 3
l 3 -4 3 3 -4 4
l 3 -3 0 3 -2 0
l 3 -3 3 4 -3 3
l 3 -3 3 3 -2 3
l 3 -2 0 4 -2 0
l 3 -2 0 3 -1 0
l 3 -1 -3 4 -1 -3
l 3 -1 -3 3 0 -3
l 3 -1 0 4 -1 0
l 3 0 -3 4 0 -3
l 3 1 2 4 1 2
l 3 1 2 3 2 2
l 3 2 1 4 2 1
l 3 2 1 3 3 1
l 3 2 1 3 2 2
l 3 2 2 3 2 3
l 3 2 3 3 3 3
l 3 3 -3 3 3 -2
l 3 3 -2 3 4 -2
l 3 3 3 3 4 3

@ 2
t -5 0 0 That's All, Folks!
