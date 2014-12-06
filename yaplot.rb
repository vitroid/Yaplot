require 'formula'

class Yaplot < Formula
  homepage ''
  url 'https://github.com/vitroid/Yaplot/zipball/v3.4.001'
  md5 '5350acb8dfddfead0c95e0a6ff57ed64'

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
