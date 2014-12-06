#!/usr/bin/env perl
#指定されたlayerだけを抽出
#ただしglobal settingな命令は通す。
#
#平成15年1月27日(月)global命令のうち、不要なものはできるだけなくす。そ
#の場設定パレットも、使ったものだけを定義する。
#

sub usage{
    print "usage: $0 layers < file.yap\n";
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
my $first=0;
while( $_=shift @ranges ){
    my $from=$_;
    my $to  =shift @ranges;
    for(my $i=$from;$i<=$to;$i++){
	$table[$i] = $i;
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
my $newlayer=$table[$layer];

my $currentRadius = 1.0;
my $currentPalette = 0;
my $radiusChanged = 1;
my $paletteChanged = 1;
my @palette;
while(<STDIN>){
    if(/^\s$/){
	#new page
	$layer=1;
	$newlayer=$table[$layer];
	print $_;
    }elsif(/^y\s/){
	@_ = split(/\s+/);
	$layer = $_[1];
	$newlayer=$table[$layer];
	if($newlayer){
	    print "y $newlayer\n";
	}
    }elsif(/^[\#r\@]/){
	#global in page
	#commentまたはset paletteならそのまま表示
	my $asis=0;
	if(/^\#/){
	    $asis++;
	}
	if ( $asis ){
	    print $_;
	}else{
	    #
	    #それ以外は、記憶しておく。
	    #
	    @_ = split( /\s+/, $_ );
	    if ( $_[0] eq '@' ){
		if ( $#_>1 ){
		    #
		    #paletteをsetした場合はそのまま記憶する。
		    #
		    $palette[$_[1]] = join(" ",$_[2],$_[3],$_[4]);
		}elsif ( $_[1] != $currentPalette ){
		    #
		    #paletteを切りかえる場合もその番号を記憶する。
		    #
		    $currentPalette = $_[1];
		    $paletteChanged = 1;
		}
	    }elsif ( $_[0] eq 'r' ){
		if ( $_[1] != $currentRadius ){
		    $currentRadius = $_[1];
		    $radiusChanged = 1;
		}
	    }
	} 
    }else{
	if( $newlayer ){
	    if( $radiusChanged ){
		print "r $currentRadius\n";
		$radiusChanged = 0;
	    }
	    if ( $palette[$currentPalette] ){
		print join(" ","@",$currentPalette,
			   $palette[$currentPalette])."\n";
		$palette[$currentPalette] = "";
	    }
	    if( $paletteChanged ){
		print "\@ $currentPalette\n";
		$paletteChanged = 0;
	    }
	    print $_;
	}
    }
}







my $current=0;
while(<STDIN>){
    my @el=split(/\s/,$_);
    if(/^\s*$/){
	#empty line
	$current=0;
	print $_;
	next;
    }elsif($el[0] eq "y"){
	$current=$el[1];
    }
    if($current==5 || $el[0] =~ /^[\#r@]$/){
	print $_;
    }
}
