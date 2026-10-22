"""Show Yaplot (.yap) data inside a Jupyter / Google Colab notebook.

Usage in Colab:

    !wget -q -N https://vitroid.github.io/Yaplot/yaplot_view.py
    from yaplot_view import show
    show(yap_text)            # a string in .yap format
    show(path="foo.yap")      # or a file

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


def show(text=None, path=None, height=600, base_url=BASE_URL):
    """Display .yap data given as a string (text) or a file (path)."""
    from IPython.display import HTML, display

    if text is None:
        if path is None:
            raise ValueError("give either text or path")
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
    # An iframe per call keeps each viewer's single #canvas separate.
    srcdoc = html.escape(_page(text, height, base_url), quote=True)
    display(
        HTML(
            f'<iframe srcdoc="{srcdoc}" '
            f'style="width:100%;height:{height + 4}px;border:0"></iframe>'
        )
    )
