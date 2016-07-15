#!/usr/bin/perl
# Send art frames to the badges
use warnings;
use strict;
use IO::Socket;
use Data::Dumper;
use Time::HiRes 'usleep';
use List::Util 'shuffle';

my @hosts = (
	"172.31.16.109",
	"172.31.24.39",
	"172.31.24.30",
	"172.31.21.131",
	"172.31.24.32",
);

my @socks = map { IO::Socket::INET->new(
	Proto		=> "UDP",
	Blocking	=> 0,
	PeerPort	=> 5453,
	#PeerAddr	=> "255.255.255.255",
	#Broadcast	=> 1,
	PeerAddr	=> $_,
) or die "socket($_) $!\n" } @hosts;


my $magic = 0x54534c50;
my $blend_time = 1000;


sub tx_frame
{
	my $blend_time = shift;

	my $msg = pack("VV", $magic, $blend_time);

	for(@_)
	{
		$msg .= chr($_->[0]). chr($_->[1]) . chr($_->[2]);
	}

    	$_->send($msg) for @socks;
}

my @pixels = map { [0,0,0] } 0..63;

while(1)
{
	# wipe blue by diagonal
	for my $i (0..16)
	{
		for my $j (0..$i)
		{
			my $p = $j * 8 + $i - 8;
			next if $p > 63 or $p < 0;
			$pixels[$p] = [ 0, 0, 255];
		}

		tx_frame($blend_time, @pixels);
		usleep(100e3);
	}

	# wipe red by row
	for my $i (0..7)
	{
		for my $j (0..7)
		{
			my $p = $i * 8 + $j;
			$pixels[$p] = [ 255, 0, 0];
		}

		tx_frame($blend_time, @pixels);
		usleep(200e3);
	}

	# wipe green by col
	for my $i (0..7)
	{
		for my $j (0..7)
		{
			my $p = $j * 8 + $i;
			$pixels[$p] = [ 0, 255, 0];
		}

		tx_frame($blend_time, @pixels);
		usleep(200e3);
	}

	# fade all black
	for my $i (0..7)
	{
		for my $j (0..7)
		{
			my $p = $j * 8 + $i;
			$pixels[$p] = [ 0, 0, 0];
		}
	}

	tx_frame(10000, @pixels);
	usleep(2000e3);

	# white sparkles
	for my $x (shuffle(0..63))
	{
		$pixels[$x] = [ 255, 255, 255 ];
		tx_frame($blend_time, @pixels);
		usleep(100e3);
	}
}
