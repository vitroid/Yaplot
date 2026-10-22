# Sample: SVG overlay (2D HUD on top of the 3D view)
global
reality 3
r 1
<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">

  <!-- shared by every frame: title and legend -->
  <text x="256" y="490" font-family="Helvetica" font-size="20"
        text-anchor="middle" fill="white">SVG overlay demo</text>
  <circle cx="400" cy="30" r="7" fill="#ff6060"/>
  <text x="414" y="36" font-family="Helvetica" font-size="16" fill="white">atom A</text>
</svg>

@ 3 255 96 96
c -5 0 -3
@ 4 96 160 255
s -5 0 -3 5 0 0
c 5 0 0
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
  <rect x="330" y="360" width="160" height="100" fill="black" fill-opacity="0.5" stroke="white"/>
  <polyline points="340,440 370,420 400,430 430,390 460,400 480,370"
            fill="none" stroke="#80c0ff" stroke-width="2"/>
  <line x1="340" y1="365" x2="340" y2="455" stroke="#ff6060" stroke-width="2"/>
  <text x="340" y="355" font-family="Helvetica" font-size="14" fill="white">t = 0</text>
</svg>

@ 3 255 96 96
c -5 0 0
@ 4 96 160 255
s -5 0 0 5 0 0
c 5 0 0
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
  <rect x="330" y="360" width="160" height="100" fill="black" fill-opacity="0.5" stroke="white"/>
  <polyline points="340,440 370,420 400,430 430,390 460,400 480,370"
            fill="none" stroke="#80c0ff" stroke-width="2"/>

  <line x1="410" y1="365" x2="410" y2="455" stroke="#ff6060" stroke-width="2"/>
  <text x="410" y="355" font-family="Helvetica" font-size="14" fill="white">t = 1</text>
</svg>

@ 3 255 96 96
c -5 0 3
@ 4 96 160 255
s -5 0 3 5 0 0
c 5 0 0
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
  <rect x="330" y="360" width="160" height="100" fill="black" fill-opacity="0.5" stroke="white"/>
  <polyline points="340,440 370,420 400,430 430,390 460,400 480,370"
            fill="none" stroke="#80c0ff" stroke-width="2"/>
  <line x1="480" y1="365" x2="480" y2="455" stroke="#ff6060" stroke-width="2"/>
  <text x="440" y="355" font-family="Helvetica" font-size="14" fill="white">t = 2</text>
</svg>
