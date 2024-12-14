#!/usr/bin/env perl

use strict;

# quick and dirty convert vgm into blocks of data to play at vsync
# format is
#
# <type> ...
# <type> if top-bit set is 1..127 bytes of pokes follow
#        else              # of vsync to wait
#	     0                 end of stream

my $fn_in = shift or die "Missing input filename";

open (my $fh_in, "<:raw:", $fn_in) or die "Cannot open \"$fn_in\" for input : $!";

my $hdr;

read($fh_in, $hdr, 0x40) == 0x40 || die "Header truncated";

my @hdr_longs = unpack("V16", $hdr);



if (@hdr_longs[2] > 0x150) {
	my $data_offs = @hdr_longs[13];

	my $hdr2;
	read($fh_in, $hdr2, $data_offs - 0x40) == $data_offs - 0x40 || die "Header truncated (2)";
}

my @pokes=();
my $wait=0;

while (1) {
	my $cmd;
	read($fh_in, $cmd, 1) == 1 || die "Unexpected EOF";
	($cmd) = unpack("C", $cmd);
	if ($cmd == 0x50) {
		if ($wait > 500) {
			$wait = int($wait / 700);
			while ($wait > 0) {
				my $w = $wait & 0x7F;
				printf "\t\t.byte\t0x%02X\n", $w;
				$wait -= $w;
			}
		}
		my $dat;
		read($fh_in, $dat, 1) == 1 || die "Unexpected EOF";
		($dat) = unpack("C", $dat);
		push @pokes, $dat;
	} elsif ($cmd == 0x61) {
		my $dat;
		read($fh_in, $dat, 2) == 2 || die "Unexpected EOF";
		($dat) = unpack("v", $dat);

		dumppokes();		

		$wait += $dat;
	} elsif ($cmd == 0x62) {
		$wait += 735;
	} elsif ($cmd == 0x63) {
		$wait += 882;
	} elsif (($cmd & 0xF0) == 0x70) {
		$wait += $cmd & 0x0F;
	} elsif ($cmd == 0x66) {
		last;
	} else {
		printf STDERR "UNRECOGNIZED: %X @ %d\n", $cmd, tell($fh_in);
	}

}

dumppokes();

print "\t\t.byte\t0\n";

sub dumppokes {
		while (scalar @pokes > 0) {
			my @dat = splice(@pokes, 0, 127);
			printf "\t\t.byte\t0x%02X, ", 0x80 | scalar @dat;
			printf join(", ", map { sprintf "0x%02X", $_ } @dat);
			print "\n";
		}
}



close $fh_in;