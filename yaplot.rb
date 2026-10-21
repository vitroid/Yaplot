# Homebrew Formula for Yaplot
# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class Yaplot < Formula
  desc "Yet another plotter in 3D - easy 3D modeller and animator"
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v5.1.1.tar.gz"
  sha256 "5be0e306ee4eed8f241f8af7d8d03580b8adb44fc041528cf999fe4e70c25466"
  license "MIT"

  depends_on "pkg-config" => :build
  depends_on "sdl2"
  depends_on "cairo"
  depends_on "libpng"

  def install
    system "make", "PKGDATADIR=#{share}/yaplot", "BINDIR=#{bin}"
    bin.install "yaplot"
    (share/"yaplot").install "yaplot.col", "help.yap"
  end

  test do
    # Test that yaplot binary exists and is executable
    assert_predicate bin/"yaplot", :exist?
    assert_predicate bin/"yaplot", :executable?
  end
end
