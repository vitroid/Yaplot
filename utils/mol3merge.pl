#!/usr/local/bin/perl
$nf=0;
open(file0,@ARGV[0])||die;
open(file1,@ARGV[1])||die;

while(1){
    while(<file0>){
	if( $_ ne "\n" ){print;}
	else{ goto label1;}
    }
    exit 0;
  label1:
    while(<file1>){
	if( $_ eq "\n" ){ goto label2;}
	print;
    }
    exit 0;
  label2:
    print "\n";
}

