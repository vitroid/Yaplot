# Yaplot -- yet another plot in 3-D
## What is yaplot?

# Features

* Simple animation, simple control.
* Simple data format.

Yaplot is an easy 3D modeller and animator  for visualizing the results of computer simulation.
You can browse the motion of the 3 dimentional wire frame model  with text labels and  some marks on the cheap PC based X terminals.
Data format is simple and intuitive.
It can also open and render several files in windows at a time.
It is useful to watch spaciotemporal data.
It is the least beautiful and not durable for presentation, but  enough quick and smooth for daily use.  (Do not expect much to yaplot!)

## Installation
Yaplot requires gtk and pkg-config.

If you are using Mac, install them via [HomeBrew](https://brew.sh) package manager.

            prompt% brew install pkg-config gtk+

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
Currently all options are unavailable on DOS.

Yaplot has the concept of layers. It has 12 layers by default. You can show/hide each layer by function keys.

When "-" is specified as the command file name, commands are read from standard input.

When multiple command files are specified, they are rendered in different windows. There are two modes of control, which are synchronous mode and asynchronous mode. When the mouse focuses to a windows in synchronous mode, your control (with mouse or keyboard) are reflected to all other windows. In asynchronous mode, on the other hand, your control are reflected only to the window where mouse is focusing.

You can alter modes by "s" key. All the windows are in synchronous mode by default.

If no command file is specified, help file is displayed.

By specifying "RECORD" option in Makefile, recording feature becomes available. When "r" key is pressed, hardcopies of the windows are saved in yaplotxx_yyyyy.gif(xx is window No.、yyyyy is frame No.) To stop recording, press "r" again.

If  "u" is pressed, current view information (eye position, field of view, etc.) is saved in ".yapviewstack" file of the current directory. If "o" is pressed, last view information is recovered from ".yapviewstack" file. It is useful to share the view information on multiple yaplot. (This feature is not available on DOS)

When no palette file is specified, yaplot searches the palette file in the following order:

1. yaplot.col file in current directory.
1. the file specified in YAPLOTCOL environment variable.
1. yaplot.col file in your home directory.
1. System default palette file.

Several sample command files are included in source code package.



## User interface
You can control yaplot by mouse and keys.

Key assignments are listed below.
Commands marked with "!" mark are "repeatable commands". Pressing number keys before repeatble command key makes the same effect as pressing the command key repeatedly. For example, pressing  "5" "N" is the same as pressing "NNNNN" (animate in 5fps).
When you mistype the number, press ESC key to escape.
All the repeatable commands ( except "g" and "f" ) are relative, i.e., pressing "10N5P" is the same as "5N".

* Up and down arrow, or "j" and "k":Pitching rotation.
* Left and right arrow, or "h" and "l":Heading rotation.
* Mouse drag with left button:Rotate immediately
* Pause or "!":Stop automatic rotation.
* Tab:Undo rotation.
* "*" and "/":Scale up and down
* "[" and "]":Zoom in and out
* "r":Start/stop recording.
* "u":pUsh; Push current view information in the view stack.
* "o":pOp; Pop current view information from the view stack.
* PageDown or "n":Next frame.
* PageUp or "p":Previous frame.
* Numeric keys followed by PageDown or "n" with shift: Prograde animation. 
* Numeric keys followed by PageUp or "p" with shift:Retrograde animation.
* Numeric keys followed by "g" or Enter:Jump to the frame specified by numeric keys. Single "g" to the first frame, single "G" to the last frame.
* SPACE:Stop all automatic actions(Rotation and animation)
* "+" and "-":Change rendering fidelity.
* Insert and Delete, or "(" and ")":Gradate line thickness.
* Function keys, or numeric keys followed by "F":Toggle show/hide of the layer.
* "v":Change verbosity.
* "s":Toggle synchronous/asynchronous modes.
* "q" or Break:Quit yaplot.

## Command file format

One line of the command files corresponds to one command. A command consists of a single command character followed by parameters separated by at least one space character, Any parameters are not omittable.

    r {radius}:Specify the radius of circles, rings, and sticks.
    l {x} {y} {z} {x} {y} {z}:Draw a line.
    s {x} {y} {z} {x} {y} {z}:Draw a stick,
    p {n} {x} {y} {z} {x} {y} {z} {x} {y} {z} ...:Draw a polygon
    c {x} {y} {z}:Draw a circle.
    o {x} {y} {z}:Draw a ring sign.
    3 {x} {y} {z}:Draw a triangle sign.
    4 {x} {y} {z}:Draw a square sign.
    5 {x} {y} {z}:Draw a pentagon sign.
    6 {x} {y} {z}:Draw a hexagon sign.
    t {x} {y} {z} {text....}:Draw a string.
    @ {n}:Specify palette No. (Palette colors are specified in palette file.)
    @ {n} {r} {g} {b}:Specify color of the n'th palette. r,g,b are integer between 0 to 255.
    # {comment}:Comment. (Ignored.)
    y {layer}:Change current layer(default layer=1)
    a {arrowtype}:Change current arrow type(default type = 0)
    Empty line:End of a frame.

## Palette file format

Each line of the palette file specifies the Red, Green and Blue  intensity of the pallete. First line describes the color for  palette 0. As the palettes 0 to 2 are reserved for system color  (Black, Gray, and White), you should not modify them.


# Yaplot -- yet another plot in 3-D
* 2013-1現在、GitHub https://github.com/vitroid/Yaplot で最新のソース(3.4)を公開しています。

## yaplotとは？

### yaplotの特徴
* シンプルな画面表示、直感的なキー操作とマウス操作
* シンプルで汎用のデータ形式
* 容易にアニメーションを作成可能。

yaplotは、計算機シミュレーションの結果を容易に可視化するための、  3次元モデラです。3次元空間での直線、円、多角形などの単純な図形を  組みあわせることによって、さまざまな情報をグラフィカルに表示し、  実時間で回転させてみたり、時間変化をながめることができます。  データ形式は単純かつ直感的で、ちょうどXYプロッタを扱うような感覚  で3次元の画像を描くことができます。また、複数のファイルを同時に開いて同時にコントロールすることができます。時系列にそった多角的な情報を同時にながめるのに役立ちます。
yaplotの画像は、本格的なプレゼンテーションに使用できるほど美し  いものではありませんが、日常的なデータ解析にはきっと役に立つこと  でしょう。

## インストール

現在、yaplotはgtkを使って書かれています。インストールする前にあらかじめ  GTKをインストールしておいて下さい。Mac OSでは、 [HomeBrew](https://brew.sh)をおすすめします。

            prompt% brew install gtk+ pkg-config

ソースコードを展開して下さい。makeでコンパイルができます。(automakeはあまりに複雑なのでやめます。)

            prompt% make

を試してみて下さい。コンパイルができたら、

            prompt# make install

でインストールされます。

## 使い方

      usage : yaplot [options] infile [infile ...]
        -e x,y,z        視点の座標を指定
        -l x,y,z        lookpointの座標を指定
        -u x,y,z        上方ベクトルの方向を指定
        -c filename     パレットファイルを指定
        -q n            キャッシュの大きさをnフレーム分に指定(default=1)
        -d              デバッグモード
        infile          コマンドファイル

キャッシュを大きくとると、ファイルの読み込み速度が向上する分メモ  リを消費します。

DOS版では今のところすべてのオプションが認識されません。

yaplotにはレイヤの概念があります。標準ではレイヤは1から12までの 12枚あり、それぞれの表示/非表示をファンクションキーで制御できます。同一のデー タを様々な面から解析する場合に役立ちます。

コマンドファイル名に"-"を指定した場合には、標準入力からデータを  読みこみます。ただし、標準入力から読みこむ場合には、アニメーショ  ン表示することはできません。

コマンドファイルを複数指定すると、複数のウィンドウが表示されます。  それぞれの窓の制御方法は同期モードと非同期モードがあります。非同  期モードの窓に対してキー入力やマウス操作を行うと、その結果はその  窓のみに反映されます。一方、同期モードの窓の対して行った操作は、  非同期窓を含むすべての窓に反映されます。ただし、窓のサイズの変更  と、拡大縮小だけは常に非同期で行われます。

同期モード、非同期モードの切り替えは"s"キーで行えます。デフォル  トではすべての窓は同期モードになっています。また、非同期モードか  ら同期モードの戻る場合には、その窓の視野描画情報はほかの同期モー  ドの窓と同一に戻されます。

MakefileでRECORDオプションを追加してあると、録画機能が使えます。"r"キーを押すと、現在表示されているフレーム以降を次々にgifファイルに保存していきます。録画を停止するには、再度"r"を押すか、"q"(yaplotの終了)を押して下さい。ファイル名は、yaplotxx_yyyyy.gif(xxの部分がウィンドウの番号、yyyyyがフレーム番号)となります。

"u"(pUsh)を押すと、現在の視野情報をカレントディレクトリの ".yapviewstack"ファイルの末尾に追加します。また、"o"(pOp)を押すとカレ ントディレクトリの".yapviewstack"ファイルの末尾行を読み込み、現在の視 野情報とします。"u"とくみあわせることで、異なるyaplotや、異なるwindow の間で同じ視点を共有することができます。(DOS版では使えません)

コマンドファイルを指定しなかった場合には、ヘルプファイル /usr/local/matto/etc/yaplot/help.yapが表示されます。

パレットファイルを指定しなかった場合には、まずカレントディレクト  リのyaplot.colをさがし、見付からなかった場合には、環境変数YAPLOTCOL  の指すファイルを捜し、それでも見付からない場合にはホーム以下をさ  がし、どうしても見付からない場合にはシステムのyaplot.colを読みこみます。なお、現在 のバージョンでは、コマンドファイル内で、"@"コマンドを使用してパレット の内容を変更することができますので、パレットファイルは必須ではありません。

パッケージには、いくつかサンプルが添付されていますから、試してみ  て下さい。

## ユーザインターフェース

マウスとキーで制御することができます。!が付いているコマンドにつ いては、各キーボードコマンドの前に数字を入力すると、そのキーを繰り返し 押したのと同じ効果があります。例えば"2" "0" "N"と入力すると、毎秒20フ レームの速度で自動アニメーションを行います。なお、"g"(指定フレームへジャ ンプ)と"f"(レイヤの表示非表示切替)以外はすべて相対指定です。つまり、 "10N5P"は"5N"と同じく、順方向へ毎秒5フレームでアニメーションします。 数字を打ちそこねた場合は、ESCを押して下さい。

ウィンドウマネージャーによってはファンクションキーがのっとられて使えな い場合などもありますが、そのような場合はshiftを押しながらファンクショ ンを押すなどの方法でウィンドウマネージャーの機能を回避して下さい。

* 上下矢印キー、"j"、"k":ピッチ回転(縦回転)。何度も押すとだんだん回転が速くなります。
* 左右矢印キー、"h"、"l":ヘッド回転(横回転)
* マウスの左ボタンを押しながらドラッグ:即時回転
* Pauseまたは"!":回転を停止
* Tab:回転する前の状態にもどす。
* "*"と"/":ズームインとアウト(望遠と広角)
* "["と"]":視野角を広げる/狭める
* "r":録画開始/終了
* "u":pUsh; 現在の視野情報をカレントディレクトリの".yapviewstack"ファイルの末尾に追加します。
* "o":pOp; カレントディレクトリの".yapviewstack"ファイルの末尾行を読み込み、現在の視野情報とします。
* PageDownまたは"n":次のフレーム
* PageUpまたは"p":前のフレーム
* Shiftを押しながらPageDownまたは"n":毎秒nフレームで順方向にアニメーション
* Shiftを押しながらPageUpまたは"p":毎秒nフレームで逆方向にアニメーション
* (数字のあとに)"g"またはEnter:指定のフレームにジャンプ。"g"を単独で押すと最初のフレーム に、"G"を単独で押すと最後のフレームにジャンプします。
* SPACE:すべての動作(回転とアニメーション)を停止
* "+" and "-":!表示品位の変更
* InsertとDelete、または"("と")":線幅の変化の度合を変える。
* ファンクションキー、あるいは数字のあとに"F":各レイヤの表示/非表示をトグル
* "v":詳細情報の画面表示を制御
* "s":同期モード/非同期モードの切りかえ
* "q" or Break:yaplotを終了

## コマンドファイルフォーマット

コマンドファイルの各行が一つのコマンドに対応します。行の最初に1  文字のコマンド、そのあとにパラメータを列挙して下さい。コマンドと  パラメータの間、パラメータとパラメータの間は1つ以上の空白で区切っ  て下さい。パラメータは省略できません。

    r {radius}:円、棒、環の半径を指定する。
    l {x} {y} {z} {x} {y} {z}:線を描く。
    s {x} {y} {z} {x} {y} {z}:棒(太さのある線)を描く。
    p {n} {x} {y} {z} {x} {y} {z} {x} {y} {z} ...:n角形を描  く。
    c {x} {y} {z}:円を描く。
    o {x} {y} {z}:環マークを描く。
    3 {x} {y} {z}:三角形マークを描く。
    4 {x} {y} {z}:四角形マークを描く。
    5 {x} {y} {z}:五角形マークを描く。
    6 {x} {y} {z}:六角形マークを描く。
    t {x} {y} {z} {text....}:文字列を描く。
    @ {palette}:パレット番号を指定する。
    @ {palette} {r} {g} {b}:パレット番号の色を変更する。 r,g,bはそれぞれ0..255の範囲の整数で指定する。
    # {comment}:コメント(無視されます)
    y {layer}:レイヤを切り替えます(default layer=1)
    a {arrowtype}: Stickの描き方を変更します。(0=通常、1=矢印、2=ダート。Default = 0)
    空行:1フレームの終わりをあらわします。

## パレットファイルフォーマット

パレットファイルの各行には、各パレット番号の色彩を、赤、緑、青の  強度(0から255)で記述します。最初の行がパレット番号0に対応します。  0番から2番まのパレットは、yaplot自体も使用するので、それぞれ、黒、  灰色、白にしておいたほうがいいでしょう。

## 内部動作について

yaplotは、ファイル読み込み時に、ハッシュ表を用いてコマンドファイ  ルで指定された座標が縮退しているかどうかを調べ、同一の座標の座標  変換計算を減らしています。このため、読みこみに少々余分な時間がか  かりますが、ユーザは座標が縮退しているかどうかを心配する必要はあ  りません。

キャッシュ戦略にはLRUを使用していますが、先読みは行っていません。  このため、単に順方向/逆方向にアニメーション表示する場合にはあま  りキャッシュは有効に働きません。すべてのフレームを読みこめるぐら  い大きくキャッシュをとる場合、あるいは特定部分のみ繰り返し再生す  るようなケースではキャッシュが有効に機能します。

## MacOSXでのコンパイル

MacPorts, HomeBrew, OSXWSなどのパッケージマネージャを使って関連ライブラリをインストールし、Linuxの場合と同様にコマンドラインからコンパイルすることをお薦めします。


