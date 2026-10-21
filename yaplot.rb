# Homebrew Formula for Yaplot
# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class Yaplot < Formula
  desc "Yet another plotter in 3D - easy 3D modeller and animator with trackpad gesture support"
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v5.1.tar.gz"
  sha256 "REPLACE_AFTER_TAG"
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
