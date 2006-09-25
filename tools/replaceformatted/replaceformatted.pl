#!/usr/bin/perl
use strict;

open(IN, $ARGV[0]) || die "$ARGV[0]: $!";
my $val;

while(<IN>){
#  if (/$pattern/) {
    $val = $_;
    $val =~ s/ ([a-zA-Z0-9]*)\.SetFormatted/ printf/g;
    $val =~ s/ ([a-zA-Z0-9]*)\.AppendFormatted/ printf/g;
    $val =~ s/return printf/printf/g;
    print "$val";
#  }
}
close(IN);
