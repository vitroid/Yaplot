# Yaplot -- yet another plot in 3-D

## What is yaplot?

# Features

- Simple animation, simple control.
- Simple data format.

Yaplot is an easy 3D modeller and animator for visualizing the results of computer simulation.
You can browse the motion of the 3 dimentional wire frame model with text labels and some marks on the cheap PC based X terminals.
Data format is simple and intuitive.
It can also open and render several files in windows at a time.
It is useful to watch spaciotemporal data.
It is the least beautiful and not durable for presentation, but enough quick and smooth for daily use. (Do not expect much to yaplot!)

## Installation

### Mac

See https://github.com/vitroid/homebrew-yaplot .

### General install procedure

Yaplot requires SDL2, Cairo, libpng, librsvg, and pkg-config.

If you are using Mac, install them via [HomeBrew](https://brew.sh) package manager.

            prompt% brew install pkg-config sdl2 cairo libpng librsvg

Then, compile it.

            prompt% make

When it is compiled successfully, install them to the appropriate places.

            prompt% make install

(You might need to be a superuser to do it.)

## Usage

      usage : yaplot [options] infile [infile ...]
        -e x,y,z        Set the coordinate of the eye point.
        -l x,y,z        Set the coordinate of the look point.
        -u x,y,z        Set the up-vector.
        -c filename     Specify palette file.
        -q n            Specify n frames are cached in memory.(default=1)
        -d              Debug mode.
        infile          Yaplot command file.

Yaplot has the concept of layers. It has 12 layers by default. You can show/hide each layer by function keys.

When "-" is specified as the command file name, commands are read from standard input.

When multiple command files are specified, they are rendered in different windows. There are two modes of control, which are synchronous mode and asynchronous mode. When the mouse focuses to a windows in synchronous mode, your control (with mouse or keyboard) are reflected to all other windows. In asynchronous mode, on the other hand, your control are reflected only to the window where mouse is focusing.

You can alter modes by "s" key. All the windows are in synchronous mode by default.

If no command file is specified, a 3D growing-tree demo (`help.yap`) is shown (layers `F1`: wood, `F2`: foliage, `F3`: blossoms). Regenerate it with `python3 utils/gen_koch_demo.py`. Docs live in this README.

When "r" key is pressed, the current frame and the following frames are saved as vector PDF files yaplotxx_yyyyy.pdf (xx is window No., yyyyy is frame No.); SVG overlays stay as vectors, while depth-splat shadows are embedded as small bitmaps. "R" records an h264 video yaplotxx.mp4 through ffmpeg. To stop recording, press the same key again.

If "u" is pressed, current view information (eye position, field of view, etc.) is saved in ".yapviewstack" file of the current directory. If "o" is pressed, last view information is recovered from ".yapviewstack" file. It is useful to share the view information on multiple yaplot.

When no palette file is specified, yaplot searches the palette file in the following order:

1. yaplot.col file in current directory.
1. the file specified in YAPLOTCOL environment variable.
1. yaplot.col file in your home directory.
1. System default palette file.

Several sample command files are included in source code package.

## User interface

You can control yaplot by mouse and keys.

Key assignments are listed below.
Commands marked with "!" mark are "repeatable commands". Pressing number keys before repeatble command key makes the same effect as pressing the command key repeatedly. For example, pressing "5" "N" is the same as pressing "NNNNN" (animate in 5fps).
When you mistype the number, press ESC key to escape.
All the repeatable commands ( except "g" and "f" ) are relative, i.e., pressing "10N5P" is the same as "5N".

- Up and down arrow, or "j" and "k":Pitching rotation.
- Left and right arrow, or "h" and "l":Heading rotation.
- Mouse drag with left button:Rotate immediately
- macOS trackpad pinch:Scale up and down (same as "*" / "/")
- macOS trackpad rotate gesture:Roll the object around the view axis (clockwise)
- Two-finger scroll / mouse wheel:Change field of view (same as "[" / "]"). On macOS, trackpad inertia is ignored so FOV stops when fingers lift. Pinch and rotate are macOS-only; elsewhere use keys.
- Pause or "!":Stop automatic rotation.
- Tab:Undo rotation.
- "\*" and "/":Scale up and down
- "[" and "]":Zoom in and out
- "r":Start/stop recording snapshots. (PDF)
- "R":Start/stop recording snapshots. (h264 video)
- "u":pUsh; Push current view information in the view stack.
- "o":pOp; Pop current view information from the view stack.
- PageDown or "n":Next frame.
- PageUp or "p":Previous frame.
- Numeric keys followed by PageDown or "n" with shift: Prograde animation.
- Numeric keys followed by PageUp or "p" with shift:Retrograde animation.
- Numeric keys followed by "g" or Enter:Jump to the frame specified by numeric keys. Single "g" to the first frame, single "G" to the last frame.
- SPACE:Stop all automatic actions(Rotation and animation)
- "+" and "-":Change rendering fidelity.
- "d":Cycle depth-splat shadow density (Reality 2–4). Numeric prefix `0d`..`3d` sets the level; `0` is off.
- Insert and Delete, or "(" and ")":Gradate line thickness.
- Function keys, or numeric keys followed by "F":Toggle show/hide of the layer.
- "v":Change verbosity.
- "s":Toggle synchronous/asynchronous modes.
- "%":Toggle sliced view. Move the slice position by "{" and "}".
- "q" or Break:Quit yaplot.

## Command file format

One line of the command files corresponds to one command. A command consists of a single command character followed by parameters separated by at least one space character, Any parameters are not omittable.

    r {radius}:Specify the radius of circles, rings, and sticks.
    l {x} {y} {z} {x} {y} {z}:Draw a line.
    s {x} {y} {z} {x} {y} {z}:Draw a stick. (A styled line.)
    p {n} {x} {y} {z} {x} {y} {z} {x} {y} {z} ...:Draw a polygon
    c {x} {y} {z}:Draw a circle.
    o {x} {y} {z}:Draw a ring sign.
    3 {x} {y} {z}:Draw a triangle sign.
    4 {x} {y} {z}:Draw a square sign.
    5 {x} {y} {z}:Draw a pentagon sign.
    6 {x} {y} {z}:Draw a hexagon sign.
    t {x} {y} {z} {text....}:Draw a string. Lines are read up to 64KB. You can use quoted strings: t 0 0 0 "long text with spaces"; inside quotes, \" and \\ are escaped.
    @ {n}:Specify palette No. (Palette colors are specified in palette file.)
    @ {n} {r} {g} {b}:Specify color of the n'th palette temporarily. R,g, and b are integers between 0 and 255.
    # {comment}:Comment. (Ignored.)
    y {layer}:Change current layer(default layer=1)
    a {arrowtype}:Change current arrow type(default type = 0)
    Empty line:End of a frame.
    global: Optional block at the start of the file. After a line whose first token is `global`, the following `@` / `r` / `y` / `a` (and long names color/palette/radius/layer/arrow) until the next empty line set defaults for every subsequent frame. In addition, UI defaults can be set: `reality {0..4}` (initial rendering quality), `splat {0..3}` (depth-splat shadow level, Reality 2..4), `fov {degrees}` (camera field of view, default 45), `verbose {0..3}` (on-screen info verbosity), `thickness {n}` / `thick {n}` (line width gradation, default 2), `rotation {wh} [{wb} [{wp}]]` / `rotate ...` (initial auto-rotation speeds for heading, banking, roll), `fps {df}` / `autoplay {df}` (initial animation speed in frames/sec), `hide {l1} ...` / `show {l1} ...` / `layers {l1} ...` (initial layer visibility), and `message {text}` (or `msg` / `echo`; quoted `"..."` supported) to print a line to stderr on load. Drawing commands in the block are ignored. Leading `#` comments before `global` are allowed.

You can use long command names instead of single letters: line, stick, polygon, circle, ring, triangle, square, pentagon, hexagon, text, radius, color (or palette), layer, arrow, comment. Example: `line 0 0 0 1 1 1` is the same as `l 0 0 0 1 1 1`.

Without a `global` block, size (radius), palette, layer, and arrow must be complete within each frame; status set in one frame is not carried over to another frame. (This allows random access to any frame.) With `global`, those defaults apply at the start of every frame; a frame may still override them locally. Lines ignored inside the `global` block are reported to stderr.

### SVG overlay (2D HUD)

You can overlay 2D information fixed to the screen (plots, legends, color bars, captions) on top of the 3D view by writing SVG directly in the command file. A line whose first non-space character is `<` starts an SVG block, which continues up to the line containing `</svg>`. Inside the block, empty lines do not end the frame and there is no line-length limit, so the output of matplotlib (`savefig(..., format="svg", transparent=True)`), gnuplot, Inkscape, etc. can be pasted as is, including the `<?xml ...?>` header.

- An SVG block inside the `global` block is shown in every frame; one inside a frame is shown only in that frame. Several blocks may be written; they are drawn in order.
- Each SVG is fitted to the whole window according to its `viewBox` and `preserveAspectRatio` (default: keep the aspect ratio and center). Use a transparent background, or the 3D view will be hidden.
- Drawing order: 3D view, global SVG, frame SVG, status text.

See `test/svg_overlay.yap` for an example.

## Palette file format

Each line of the palette file specifies the Red, Green and Blue intensity of the pallete. First line describes the color for palette 0. As the palettes 0 to 2 are reserved for system color (Black, Gray, and White), you should not modify them.

# Yaplot -- yet another plot in 3-D

- 2013-1 現在、[GitHub](https://github.com/vitroid/Yaplot) で最新のソース(3.4)を公開しています。

## yaplot とは？

### yaplot の特徴

- シンプルな画面表示、直感的なキー操作とマウス操作
- シンプルで汎用のデータ形式
- 容易にアニメーションを作成可能。

yaplot は、計算機シミュレーションの結果を容易に可視化するための、3 次元モデラです。3 次元空間での直線、円、多角形などの単純な図形を組みあわせることによって、さまざまな情報をグラフィカルに表示し、実時間で回転させてみたり、時間変化をながめることができます。データ形式は単純かつ直感的で、ちょうど XY プロッタを扱うような感覚で 3 次元の画像を描くことができます。また、複数のファイルを同時に開いて同時にコントロールすることができます。時系列にそった多角的な情報を同時にながめるのに役立ちます。
yaplot の画像は、本格的なプレゼンテーションに使用できるほど美しいものではありませんが、日常的なデータ解析にはきっと役に立つことでしょう。

## インストール

現在、yaplot は SDL2 と Cairo を使って書かれています。インストールする前にあらかじめ SDL2、Cairo、libpng、librsvg、pkg-config をインストールしておいて下さい。Mac OS では、[HomeBrew](https://brew.sh)をおすすめします。

            prompt% brew install pkg-config sdl2 cairo libpng librsvg

ソースコードを展開して下さい。make でコンパイルができます。(automake はあまりに複雑なのでやめます。)

            prompt% make

を試してみて下さい。コンパイルができたら、

            prompt% make install

でインストールされます。(管理者権限でないとインストールできないかもしれません)

## 使い方

      usage : yaplot [options] infile [infile ...]
        -e x,y,z        視点の座標を指定
        -l x,y,z        lookpointの座標を指定
        -u x,y,z        上方ベクトルの方向を指定
        -c filename     パレットファイルを指定
        -q n            キャッシュの大きさをnフレーム分に指定(default=1)
        -d              デバッグモード
        infile          コマンドファイル

キャッシュを大きくとると、ファイルの読み込み速度が向上する分メモリを消費します。

yaplot にはレイヤの概念があります。標準ではレイヤは 1 から 12 までの 12 枚あり、それぞれの表示/非表示をファンクションキーで制御できます。同一のデータを様々な面から解析する場合に役立ちます。

コマンドファイル名に"-"を指定した場合には、標準入力からデータを 読みこみます。ただし、標準入力から読みこむ場合には、アニメーション表示することはできません。

コマンドファイルを複数指定すると、複数のウィンドウが表示されます。 それぞれの窓の制御方法は同期モードと非同期モードがあります。非同 期モードの窓に対してキー入力やマウス操作を行うと、その結果はその 窓のみに反映されます。一方、同期モードの窓の対して行った操作は、 非同期窓を含むすべての窓に反映されます。ただし、窓のサイズの変更 と、拡大縮小だけは常に非同期で行われます。

同期モード、非同期モードの切り替えは"s"キーで行えます。デフォル トではすべての窓は同期モードになっています。また、非同期モードか ら同期モードの戻る場合には、その窓の視野描画情報はほかの同期モー ドの窓と同一に戻されます。

"r"キーを押すと、現在表示されているフレーム以降を次々にベクタ形式の PDF ファイルに保存していきます。ファイル名は、yaplotxx_yyyyy.pdf(xx の部分がウィンドウの番号、yyyyy がフレーム番号)となります。SVG オーバーレイはベクタのまま保存され、奥行きスプラットの影は小さなビットマップとして埋め込まれます。"R"キーでは ffmpeg を通して h264 動画 yaplotxx.mp4 を保存します。録画を停止するには、同じキーを再度押すか、"q"(yaplot の終了)を押して下さい。

"u"(pUsh)を押すと、現在の視野情報をカレントディレクトリの ".yapviewstack"ファイルの末尾に追加します。また、"o"(pOp)を押すとカレ ントディレクトリの".yapviewstack"ファイルの末尾行を読み込み、現在の視 野情報とします。"u"とくみあわせることで、異なる yaplot や、異なる window の間で同じ視点を共有することができます。

コマンドファイルを指定しなかった場合には、木が伸び葉が付き花が咲いて散る 3D デモ（`help.yap`、レイヤ `F1`: 幹と枝、`F2`: 葉、`F3`: 花と花吹雪）が表示されます。再生成は `python3 utils/gen_koch_demo.py`。操作説明はこの README を参照してください。

パレットファイルを指定しなかった場合には、まずカレントディレクト リの yaplot.col をさがし、見付からなかった場合には、環境変数 YAPLOTCOL の指すファイルを捜し、それでも見付からない場合にはホーム以下をさ がし、どうしても見付からない場合にはシステムの yaplot.col を読みこみます。なお、現在のバージョンでは、コマンドファイル内で、"@"コマンドを使用してパレット の内容を変更することができますので、パレットファイルは必須ではありません。

パッケージには、いくつかサンプルが添付されていますから、試してみ て下さい。

## ユーザインターフェース

マウスとキーで制御することができます。!が付いているコマンドにつ いては、各キーボードコマンドの前に数字を入力すると、そのキーを繰り返し 押したのと同じ効果があります。例えば"2" "0" "N"と入力すると、毎秒 20 フ レームの速度で自動アニメーションを行います。なお、"g"(指定フレームへジャ ンプ)と"f"(レイヤの表示非表示切替)以外はすべて相対指定です。つまり、 "10N5P"は"5N"と同じく、順方向へ毎秒 5 フレームでアニメーションします。 数字を打ちそこねた場合は、ESC を押して下さい。

ウィンドウマネージャーによってはファンクションキーがのっとられて使えな い場合などもありますが、そのような場合は shift を押しながらファンクショ ンを押すなどの方法でウィンドウマネージャーの機能を回避して下さい。

- 上下矢印キー、"j"、"k":ピッチ回転(縦回転)。何度も押すとだんだん回転が速くなります。
- 左右矢印キー、"h"、"l":ヘッド回転(横回転)
- マウスの左ボタンを押しながらドラッグ:即時回転
- Pause または"!":回転を停止
- Tab:回転する前の状態にもどす。
- "\*"と"/":ズームインとアウト(望遠と広角)
- "["と"]":視野角を広げる/狭める
- "r":録画開始/終了 (PDF)
- "R":録画開始/終了 (h264 movie)
- "u":pUsh; 現在の視野情報をカレントディレクトリの".yapviewstack"ファイルの末尾に追加します。
- "o":pOp; カレントディレクトリの".yapviewstack"ファイルの末尾行を読み込み、現在の視野情報とします。
- PageDown または"n":次のフレーム
- PageUp または"p":前のフレーム
- Shift を押しながら PageDown または"n":毎秒 n フレームで順方向にアニメーション
- Shift を押しながら PageUp または"p":毎秒 n フレームで逆方向にアニメーション
- (数字のあとに)"g"または Enter:指定のフレームにジャンプ。"g"を単独で押すと最初のフレーム に、"G"を単独で押すと最後のフレームにジャンプします。
- SPACE:すべての動作(回転とアニメーション)を停止
- "+" and "-":!表示品位の変更
- Insert と Delete、または"("と")":線幅の変化の度合を変える。
- ファンクションキー、あるいは数字のあとに"F":各レイヤの表示/非表示をトグル
- "v":詳細情報の画面表示を制御
- "s":同期モード/非同期モードの切りかえ
- "q" or Break:yaplot を終了

## コマンドファイルフォーマット

コマンドファイルの各行が一つのコマンドに対応します。行の最初に 1 文字のコマンド、そのあとにパラメータを列挙して下さい。コマンドと パラメータの間、パラメータとパラメータの間は 1 つ以上の空白で区切っ て下さい。パラメータは省略できません。

    r {radius}:円、棒、環の半径を指定する。
    l {x} {y} {z} {x} {y} {z}:線を描く。
    s {x} {y} {z} {x} {y} {z}:棒(太さのある線)を描く。
    p {n} {x} {y} {z} {x} {y} {z} {x} {y} {z} ...:n角形を描く。
    c {x} {y} {z}:円を描く。
    o {x} {y} {z}:環マークを描く。
    3 {x} {y} {z}:三角形マークを描く。
    4 {x} {y} {z}:四角形マークを描く。
    5 {x} {y} {z}:五角形マークを描く。
    6 {x} {y} {z}:六角形マークを描く。
    t {x} {y} {z} {text....}:文字列を描く。1行は最大64KBまで。引用符付き t 0 0 0 "空白を含む長い文字列" も使える。\" と \\ はエスケープされる。
    @ {palette}:パレット番号を指定する。
    @ {palette} {r} {g} {b}:パレット番号の色を変更する。r,g,bはそれぞれ0..255の範囲の整数で指定する。
    # {comment}:コメント(無視されます)
    y {layer}:レイヤを切り替えます(default layer=1)
    a {arrowtype}: Stickの描き方を変更します。(0=通常、1=矢印、2=ダート。Default = 0)
    空行:1フレームの終わりをあらわします。
    global: ファイル先頭のオプションブロック。先頭トークンが `global` の行のあと、次の空行までの `@` / `r` / `y` / `a`（および color/palette/radius/layer/arrow）が以降の全フレームの初期値になります。また UI の初期状態として、`reality {0..4}`（表示品位）、`splat {0..3}`（奥行きスプラット影レベル、Reality 2〜4）、`fov {degrees}`（カメラ画角・度数法、既定45）、`verbose {0..3}`（画面ステータス表示の詳細度）、`thickness {n}` / `thick {n}`（線幅変化の度合、既定2）、`rotation {wh} [{wb} [{wp}]]` / `rotate ...`（横・縦・ロールの自動回転速度）、`fps {df}` / `autoplay {df}`（自動アニメーション速度・秒間フレーム数）、`hide {l1} ...` / `show {l1} ...` / `layers {l1} ...`（レイヤの初期表示・非表示）、および `message {text}`（`msg` / `echo` も可。`"..."` 引用符対応、読み込み時に stderr へ出力）を指定できます。ブロック内の描画命令は無視されます。`global` の前の `#` コメントは許可されます。

1文字コマンドの代わりに長い名前も使えます: line, stick, polygon, circle, ring, triangle, square, pentagon, hexagon, text, radius, color（または palette）, layer, arrow, comment。例: `line 0 0 0 1 1 1` は `l 0 0 0 1 1 1` と同じです。

`global` ブロックが無い場合、size(半径)やpaletteやlayerやarrowなどの設定は**各フレームごとに完結**しており、あるフレームでのstatus設定は別のフレームには引き継がれません。(任意のフレームをランダムアクセスできるようにするため) `global` がある場合はその値が各フレーム先頭のデフォルトになり、フレーム内での上書きもできます。global ブロック内で無視された行は stderr に出ます。

### SVG オーバーレイ（2D HUD）

コマンドファイルの中に SVG を直接書くと、3D 描画の上に画面に固定された 2D 情報（グラフ、凡例、カラーバー、キャプションなど）を重ねられます。行頭（空白を除く）が `<` の行から、`</svg>` を含む行までが 1 つの SVG ブロックです。ブロック内では空行でフレームが終わらず、行の長さ制限もないので、matplotlib（`savefig(..., format="svg", transparent=True)`）、gnuplot、Inkscape などの出力を `<?xml ...?>` 行ごとそのまま貼り付けられます。

- `global` ブロック内の SVG は全フレームに、フレーム内の SVG はそのフレームだけに表示されます。複数書いた場合は書いた順に重ねます。
- 各 SVG は `viewBox` と `preserveAspectRatio`（既定は縦横比を保って中央）に従ってウィンドウ全体に合わせて描画されます。背景は透明にして下さい（不透明だと 3D 描画が隠れます）。
- 描画順は 3D 描画、global の SVG、フレームの SVG、ステータス表示の順です。

例は `test/svg_overlay.yap` を参照して下さい。

## パレットファイルフォーマット

パレットファイルの各行には、各パレット番号の色彩を、赤、緑、青の 強度(0 から 255)で記述します。最初の行がパレット番号 0 に対応します。 0 番から 2 番まのパレットは、yaplot 自体も使用するので、それぞれ、黒、 灰色、白にしておいたほうがいいでしょう。

## 内部動作について

yaplot は、ファイル読み込み時に、ハッシュ表を用いてコマンドファイ ルで指定された座標が縮退しているかどうかを調べ、同一の座標の座標 変換計算を減らしています。このため、読みこみに少々余分な時間がか かりますが、ユーザは座標が縮退しているかどうかを心配する必要はあ りません。

キャッシュ戦略には LRU を使用していますが、先読みは行っていません。 このため、単に順方向/逆方向にアニメーション表示する場合にはあま りキャッシュは有効に働きません。すべてのフレームを読みこめるぐら い大きくキャッシュをとる場合、あるいは特定部分のみ繰り返し再生す るようなケースではキャッシュが有効に機能します。

## MacOSX でのコンパイル

MacPorts, HomeBrew, OSXWS などのパッケージマネージャを使って関連ライブラリをインストールし、Linux の場合と同様にコマンドラインからコンパイルすることをお薦めします。
