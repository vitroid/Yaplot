# Homebrew Formula for Yaplot
# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class Yaplot < Formula
  desc "Yet another plotter in 3D - easy 3D modeller and animator with trackpad gesture support"
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v4.2.tar.gz"
  sha256 "2d07b552bc8510e366fc04fbcd5b09191986bd78c21d6edfc3bfa3b0a425594f"
  license "MIT"

  depends_on "gtk+3"
  depends_on "pkg-config" => :build
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
