require "formula"

class Yaplot < Formula
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/raw/master/Yaplot-4.1.2.tar.gz"
  sha1 "a48aec031080572bfdf7af994115c46263398531"

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
