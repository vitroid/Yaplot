# frame 0: CSS '#' rule and a blank line inside the SVG, '#' comment outside
@ 3
c -5 0 0
# this is a yap comment
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
<style>
#label { fill: yellow; font-size: 30px; font-family: Helvetica; }
</style>

<text id="label" x="20" y="480">frame 0: css + blank line</text>
</svg>
c 5 0 0

# frame 1: a stray XML comment line must not swallow the following commands
<!-- just a note -->
@ 4
c 0 0 3
c 0 0 -3

# frame 2: nested <svg>
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
  <svg x="20" y="20" width="200" height="100" viewBox="0 0 2 1">
    <rect width="2" height="1" fill="orange"/>
  </svg>
  <text x="20" y="480" fill="white" font-size="30" font-family="Helvetica">frame 2: nested svg</text>
</svg>
c 0 0 0

# frame 3: matplotlib-style prolog with a two-line DOCTYPE
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
  <text x="20" y="480" fill="cyan" font-size="30" font-family="Helvetica">frame 3: doctype</text>
</svg>
c 0 0 0
