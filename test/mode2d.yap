# Sample: global mode 2d + 2-arg coordinates on the xy plane
global
mode 2d
@ 3 80 160 255
r 0.25
y 1

# Frame 0: axes and a triangle (all 2-arg coords)
@ 2
l -2 0 2 0
l 0 -2 0 2
@ 3
p 3 0 1 -1 -0.5 1 -0.5
c 0 0
t -1.8 1.6 "mode 2d"

# Frame 1: move the triangle and add a stick arrow
@ 2
l -2 0 2 0
l 0 -2 0 2
@ 4 255 120 60
a 1
s 0 0 1.2 0.8
@ 3
p 3 0.5 1.2 -0.5 0 1.5 0
c 0.5 0.4
t -1.8 1.6 "frame 1"
