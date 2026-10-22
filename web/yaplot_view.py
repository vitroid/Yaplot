"""Show yaplotlib drawings inside a Jupyter / Google Colab notebook.

Usage in Colab:

    !pip install -q yaplotlib
    !wget -q -N https://vitroid.github.io/Yaplot/yaplot_view.py
    from yaplotlib import Yaplot
    from yaplot_view import show

    y = Yaplot()
    y.Points([(0, 0, 0), (1, 0, 0), (0, 1, 0)], color=2, size=0.3)
    y.Line((0, 0, 0), (1, 1, 0), color=3)
    show(y)                   # Yaplot or YaplotDocument (multi-frame)
    show(path="movie.yap")    # an existing .yap file

Click the viewer to give it keyboard focus; the keys are the same as the
desktop Yaplot. Dropping another .yap onto the viewer replaces it.
"""

import html
import json

BASE_URL = "https://vitroid.github.io/Yaplot/"


def _page(text, height, base_url):
    data = json.dumps(text).replace("</", "<\\/")
    base = json.dumps(base_url)
    return f"""<!DOCTYPE html>
<html><head><meta charset="utf-8"><style>
html, body {{ margin: 0; background: #000; }}
#canvas {{ display: block; width: 100%; height: {height}px; outline: none; }}
</style></head><body>
<canvas id="canvas" tabindex="0"></canvas>
<script>
var Module = {{
  arguments: ['/input.yap'],
  canvas: document.getElementById('canvas'),
  locateFile: function (p) {{ return {base} + p; }},
  preRun: [function () {{ FS.writeFile('/input.yap', {data}); }}],
  print: function (t) {{ console.log(t); }},
  printErr: function (t) {{ console.log(t); }}
}};
</script>
<script src={json.dumps(base_url + "yaplot.js")}></script>
</body></html>"""


def show(drawing=None, *, path=None, height=600, base_url=BASE_URL):
    """Display a drawing in the notebook.

    drawing: a yaplotlib ``Yaplot`` / ``YaplotDocument`` (anything with
        ``dumps()`` returning .yap text); a plain .yap string also works.
    path: an existing .yap file, instead of ``drawing``.
    height: viewer height in pixels.
    """
    from IPython.display import HTML, display

    if drawing is not None:
        text = drawing.dumps() if hasattr(drawing, "dumps") else str(drawing)
    elif path is not None:
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
    else:
        raise ValueError("give a drawing or path=")
    # An iframe per call keeps each viewer's single #canvas separate.
    srcdoc = html.escape(_page(text, height, base_url), quote=True)
    display(
        HTML(
            f'<iframe srcdoc="{srcdoc}" '
            f'style="width:100%;height:{height + 4}px;border:0"></iframe>'
        )
    )
