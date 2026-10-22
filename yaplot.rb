# Homebrew Formula for Yaplot
# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class Yaplot < Formula
  desc "Yet another plotter in 3D - easy 3D modeller and animator"
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v5.4.tar.gz"
  sha256 "6190fc225467c00bdfb8e851de7fba484697f902f11adf2c249cb0513179f513"
  license "MIT"

  depends_on "pkg-config" => :build
  depends_on "sdl2"
  depends_on "cairo"
  depends_on "libpng"
  depends_on "librsvg"

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
