require 'formula'

class Yaplot < Formula
  homepage ''
  url 'https://github.com/vitroid/Yaplot/archive/v3.5.002.zip'
  md5 'be416b39bbb2d791430fb69a73d9fb34'

  depends_on "gtk+"

  def install
    ENV.x11 # if your formula requires any X11 headers

    system "export PKG_CONFIG_PATH=/opt/X11/lib/pkgconfig ./configure", ""
    system "make"
    system "make install"
  end

  def test
    system "false"
  end
end
