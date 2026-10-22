# Homebrew Formula for Yaplot
# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class Yaplot < Formula
  desc "Yet another plotter in 3D - easy 3D modeller and animator"
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v5.6.tar.gz"
  sha256 "c0278ab1741faa65101ba1d9c442ba4829d64df2c5aea085297b328bf5f6e32a"
  license "MIT"

  depends_on "pkg-config" => :build
  depends_on "sdl2"
  depends_on "cairo"
  depends_on "libpng"
  depends_on "librsvg"

  def install
    system "make", "BREW_PREFIX=#{HOMEBREW_PREFIX}", "PKGDATADIR=#{pkgshare}", "BINDIR=#{bin}"
    bin.install "yaplot"
    pkgshare.install "yaplot.col", "help.yap"
  end

  test do
    assert_match "Yaplot ver. #{version}", shell_output("#{bin}/yaplot -V")
  end
end
