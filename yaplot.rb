require "formula"

class Yaplot < Formula
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/tarballs/Yaplot-4.1.2.tar.gz"
  sha1 "4b15af544995c4a8f969dbe17c5178f9d95adace"

  depends_on "gtk+"
  depends_on :x11

  def install
    ENV["PKG_CONFIG_PATH"]='/opt/X11/lib/pkgconfig'
    # system "./configure"
    system "make", "install"
  end

  test do
    system "false"
  end
end
