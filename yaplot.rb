require "formula"

class Yaplot < Formula
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v3.5.002.tar.gz"
  sha1 "a759c411891701b3b18d89cc297077b73c12bfa4"

  depends_on "gtk+"
  depends_on :x11

  def install
    ENV["PKG_CONFIG_PATH"]='/opt/X11/lib/pkgconfig'
    system "./configure"
    system "make", "install"
  end

  test do
    system "false"
  end
end
