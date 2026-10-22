# Homebrew Formula for Yaplot
# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class Yaplot < Formula
  desc "Yet another plotter in 3D - easy 3D modeller and animator"
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot/archive/v5.4.1.tar.gz"
  sha256 "a6532b12d4b02238044946b8893021103b62eafc2d14564f1dec0b7061330fad"
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
