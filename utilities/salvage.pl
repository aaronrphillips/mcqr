#!/usr/bin/perl

use strict;

my $orig_task_file = $ARGV[0];
my $progress_file  = $ARGV[1];

if (! -f $orig_task_file || ! -f $progress_file)
{
  print STDERR "USAGE: $0 orig_task_file progress_file\n";
  exit -1;
}

my %completed_tasks;

open FH, $progress_file || die ("ERROR opening progress file");
while (<FH>)
{
  chomp();
  my ($junk, $id, @rest) = split(/[:,]/);
  $completed_tasks{$id}++;
}
close FH;

my $idx = 1;
open FH, $orig_task_file || die ("ERROR opening task file");
while (<FH>)
{
  chomp();

  print "$_\n" if (!exists($completed_tasks{$idx}));

  $idx++;
}
close FH;

