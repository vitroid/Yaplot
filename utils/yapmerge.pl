#!/usr/bin/env perl
#その場設定paletteは、出てきた順に新番号を与えればよい。32番以降を使用することにする。
#パレット番号のその場指定はglobalである。つまり、最初に一度設定すれば、変更されない限り持続する。このため、各ファイルにおけるパレット移し替え表を別個に準備しなければならない。
use strict;
use FileHandle;

sub usage{
    print STDERR "usage: yapmerge 1.yap nlayer1 2.yap\n";
    print STDERR "nlayer1: Specify number of layers in 1.yap in decimal number\n";
    exit(1);
}

my $nf=0;
my @files;
my @offsets;
while(1){
    $files[$nf]=new FileHandle;
    open $files[$nf],$ARGV[0];
    shift(@ARGV);
    #open($files[$nf],shift)||usage;
    $nf ++;
    last if $#ARGV<0;
    $offsets[$nf]=shift(@ARGV)+$offsets[$nf-1];
}

my $palette=32;
my $last=0;
my @userpalette;
for(my $i=0;$i<$nf;$i++){
    $userpalette[$i][0];
}
my $frames=0;
while(1){
    $last=0;
    for(my $i=0;$i<$nf;$i++){
	$last += getput($files[$i],$offsets[$i],$userpalette[$i]);
    }
    print STDERR "[$frames]";
    $frames++;
    last if $last;
    print "\n";
}

sub getput{
    my ($files,$offsets,$table)=@_;
    my $init = 1;
    while(<$files>){
	return 0 if( /^\s*$/ );
	if ( $init ){
	      #もしユーザが独自色を割りあてたら、別番号に割りふる。
	  print "y ", $offsets+1, "\n";
	  $init = 0;
	}
	chomp;
	split;
	if($_[0] =~ /^y/){
	    print "y ";
	    print $_[1]+$offsets,"\n";
	}elsif($_[0] =~ /^\@/ ){
	    my @cmd = split(/\s+/);
	    my $pal = $cmd[1];
	    if ( $#cmd == 4 ){
		$cmd[1] = $table->[$pal] = $palette++;
	    }elsif( $table->[$pal] ){
		$cmd[1] = $table->[$pal];
	    }
	    print join(" ",@cmd,"\n" );
	}else{
	    print $_,"\n";
	}
    }
    #end of file is detected
    return 1;
}

0;
