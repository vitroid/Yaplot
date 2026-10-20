# Homebrew Formula for Yaplot
# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class Yaplot < Formula
  desc "Yet another plotter in 3D - easy 3D modeller and animator with trackpad gesture support"
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v5.0.tar.gz"
  sha256 "7cac7134422831af5f199cf45a47b3edc0603f0df2861aa0f18366f996c4b963"
  license "MIT"

  depends_on "pkg-config" => :build
  depends_on "sdl2"
  depends_on "cairo"
  depends_on "libpng"

  def install
    # Generate common.h from template
    system "make", "gen_common", "PKGDATADIR=#{share}/yaplot", "BINDIR=#{bin}"
    
    # Build the program
    system "make", "PKGDATADIR=#{share}/yaplot", "BINDIR=#{bin}"
    
    # Install manually since the Makefile's install target uses hardcoded paths
    bin.install "yaplot"
    (share/"yaplot").install "yaplot.col", "help.yap"
  end

  test do
    # Test that yaplot binary exists and is executable
    assert_predicate bin/"yaplot", :exist?
    assert_predicate bin/"yaplot", :executable?
  end
end
