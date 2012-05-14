#!/usr/local/bin/perl
#Convert x3d format object into yaplot format data and palette.
#yaplot object data is output into stdout. Palette is into x3d2yap.col
# % x3d2yap.pl < inputfile.x3d > output.yap
# % yaplot -c x3d2yap.col output.yap

while(<STDIN>){
    last if !(/^#/||/^[ \t]*$/);
}
chop;
$ncolor=$_;
print "# Ncolor: $ncolor\n";
print STDERR "Ncolor: $ncolor\n";
for($i=0;$i<$ncolor;$i++){
    while(<STDIN>){
	last if !(/^#/||/^[ \t]*$/);
    }
	chop;
    split;
    ($j,$r[$i],$g[$i],$b[$i])=@_;
}
open(palet,"> x3d2yap.col"); # 
    print palet "0 0 0\n100 100 100\n255 255 255\n";
for($i=0;$i<$ncolor;$i++){
    print palet "$r[$i] $g[$i] $b[$i]\n";
}
    close(palet);

while(<STDIN>){
    last if !(/^#/||/^[ \t]*$/);
}
chop;
$npoint=$_;
print "# Npoint: $npoint\n";
print STDERR "Npoint: $npoint\n";
for($i=0;$i<$npoint;$i++){
    while(<STDIN>){
	last if !(/^#/||/^[ \t]*$/);
    }				# 
    chop;
    split;
    ($j,$x[$i],$y[$i],$z[$i])=@_;
	$x[$i]*=0.005;
	$y[$i]*=0.005;
	$z[$i]*=0.005;
}
while(<STDIN>){
    last if !(/^#/||/^[ \t]*$/);
}
chop;
$nline=$_;
print "# Nline: $nline\n";
print STDERR "Nline: $nline\n";
for($i=0;$i<$nline;$i++){
    while(<STDIN>){
	last if !(/^#/||/^[ \t]*$/);
    }				# 
    chop;
    split;
	$_[1]+=3;
	print "@ $_[1]\n";
	print "l $x[$_[2]] $y[$_[2]] $z[$_[2]] $x[$_[3]] $y[$_[3]] $z[$_[3]]\n";
}
