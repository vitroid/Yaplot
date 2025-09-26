# Homebrew Formula for Yaplot
# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class Yaplot < Formula
  desc "Yet another plotter in 3D - easy 3D modeller and animator for visualizing simulation results"
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v4.1.2.tar.gz"
  sha256 "c363947c8fe380325631a45e73d65ecc0001432ae14c1b9313431345a258c2dd"
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
    # Test that yaplot can display help
    system "#{bin}/yaplot", "-h"
  end
end
