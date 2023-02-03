# require "formula"

# class Yaplot < Formula
#   homepage "https://github.com/vitroid/Yaplot"
#   # url "https://github.com/vitroid/Yaplot/raw/master/Yaplot-4.1.2.tar.gz"
#   url "https://github.com/vitroid/Yaplot/archive/refs/heads/master.zip"
#   sha256 "f9c386b4f5c8ee3051c633f4b5a8fb05f54a4961804667ffca73cc808c2512ca"

#   depends_on "gtk+"
#   # depends_on :x11

#   def install
#     ENV["PKG_CONFIG_PATH"]='/opt/X11/lib/pkgconfig'
#     system "./configure"
#     system "make", "install"
#   end

#   test do
#     system "false"
#   end
# end

# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
# PLEASE REMOVE ALL GENERATED COMMENTS BEFORE SUBMITTING YOUR PULL REQUEST!
class Yaplot < Formula
  desc ""
  homepage "https://github.com/vitroid/Yaplot"
  url "https://github.com/vitroid/Yaplot"
  version "4.1.2"
  sha256 "24acaee0fc46c54ded39fc2254f74d0ebf229cf58926997594373cfb1cd8854d"
  license "MIT"

  # depends_on "cmake" => :build

  def install
    # ENV.deparallelize  # if your formula fails when building in parallel
    # Remove unrecognized options if warned by configure
    # https://rubydoc.brew.sh/Formula.html#std_configure_args-instance_method
    system "./configure", *std_configure_args, "--disable-silent-rules"
    system "make", "install"
    # system "cmake", "-S", ".", "-B", "build", *std_cmake_args
  end

  test do
    # `test do` will create, run in and delete a temporary directory.
    #
    # This test will fail and we won't accept that! For Homebrew/homebrew-core
    # this will need to be a test that verifies the functionality of the
    # software. Run the test with `brew test yaplot`. Options passed
    # to `brew install` such as `--HEAD` also need to be provided to `brew test`.
    #
    # The installed folder is not in the path, so use the entire path to any
    # executables being tested: `system "#{bin}/program", "do", "something"`.
    system "false"
  end
end
