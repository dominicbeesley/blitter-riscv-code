#!/usr/bin/env perl

use strict;

my $offs = shift or die "Missing offset";
$offs = hex($offs);

my $pre = shift or die "Missing prefix";

my %n = ();

while (<>) {

	my $l = $_;
	print $l;

	$l =~ s/[\r\n\s]+$//;

	if ($l =~ /^(\w+)\s+a\s+([0-9a-f]+)\s*$/) {
		my $la = $pre . $1;
		my $aa = hex($2) + $offs;

		$n{$aa} = sprintf("%s t %x", $la, $aa);
	}

}

foreach my $k (sort keys %n) {
	printf "%s\n", $n{$k};
}