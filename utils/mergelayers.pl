#!/usr/bin/env perl
#指定されたレイヤ群を、その最初のレイヤに重ねる。
#残りのレイヤは-cが指定されていれば詰める。
#yの引数を変更するのみ。layer 1の扱いに注意

#例えば、mergelayer.pl -c 3,5,7
#を指定すると、3、5、7番目のlayerは3番目のレイヤに重ねられ、それ以外の
#レイヤは順次前に詰める。(第6レイヤは第4レイヤに移動する。)


sub usage{
    print "usage: $* layers < file.yap\n";
    print "layers: 1,3,5-9,...\n";
    exit 1;
}

sub ParseRanges{
    #min,maxは省略時の範囲
    my ($ranges,$min,$max) = @_;
    my @ranges;
    foreach my $range ( split(/,/,$ranges) ){
	my $lastto=$min-1;
	#範囲指定の場合
	if($range =~ /-/){
	    my ($a,$b)=split(/-/,$range);
	    if($a !~ /^[0-9]+$/){
		$a=$min;
	    }
	    if($b !~ /^[0-9]+$/){
		$b=$max;
	    }
	    usage if $b<$a;
	    usage if $lastto>$a;
	    $lastto=$b;
	    push(@ranges,$a,$b);
	}else{
	    $lastto=$range;
	    push(@ranges,$range,$range);
	}
    }
    @ranges;
}

my $compress=0;
if ( $ARGV[0] eq '-c' ){
    $compress++;
    shift;
}
usage if ($#ARGV != 0);
my @ranges = ParseRanges( shift(),1,20 );

#make conversion table
my @table;
for(my $i=1;$i<=20;$i++){
    $table[$i] = $i;
}
my $first=0;
while( $_=shift @ranges ){
    my $from=$_;
    if ( $first == 0 ){
	$first = $from;
    }
    my $to  =shift @ranges;
    for(my $i=$from;$i<=$to;$i++){
	$table[$i] = $first;
    }
}
if( $compress ){
    #空きレイヤを詰める。
    my $i=$first+1;
    for( my $j = $i; $j<=20; $j ++ ){
	next if $table[$j] == $first;
	$table[$j] = $i;
	$i ++;
    }
}

my $layer=1;
while(<STDIN>){
    if(/^\s$/){
	#new page
	$layer=1;
	print $_;
	if ( $table[$layer] != 1 ){
	    #もし最初のレイヤが射影されていれば
	    #こんなことはおきないはずだが
	    print "y ".$table[$layer]."\n";
	}
    }elsif(/^y\s/){
	split(/\s+/);
	$layer = $_[1];
	print "y ".$table[$layer]."\n";
    }else{
	print $_;
    }
}
