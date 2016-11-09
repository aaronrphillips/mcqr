#!/usr/bin/perl -w

use Data::Dumper;
use JSON;

my $infile  = $ARGV[0];
#my $outfile = $ARGV[1];
my $outfile = "flare.json";

my %nodes;
my %struct;
my %file;
my $max_time = 0;
my %hunter_times;
my %tasks;
my %hunter_nodes;
my @working_hunters;
my @done_tasks;

#  The following is a sample input line:
#PROC TIME:116,compute-0-9,40,0.539492,18.051604: seconds

open(FH, $infile) || die("Can't open input file, $infile: $!\n");
while (<FH>)
{
  next unless (/^PROC/);

  my($tmp1,$data,$tmp2)               =  split(':', $_);
  my($uuid,$node,$hunter,$wait,$proc) =  split(',', $data);
  #$node                               =~ s/^[-a-zA-Z_]*//;

  $wait = 0 if ($wait < 0);

  $hunter_times{$hunter} += $wait;
  $hunter_times{$hunter} += $proc;
  $hunter_nodes{$node}{$hunter}++;

  my %proctask;
  $proctask{'uuid'} = 0 + $uuid;
  $proctask{'time'} = 0.0 + sprintf("%.2f", $proc);
  $proctask{'type'} = 'proc';
  my %waittask;
  $waittask{'uuid'} = 0 + $uuid;
  $waittask{'time'} = 0.0 + sprintf("%.2f", $wait);
  $waittask{'type'} = 'wait';
  push(@{$tasks{$hunter}}, \%waittask, \%proctask);

  $done_tasks[int($hunter_times{$hunter})]++;
}
close(FH);

foreach my $hunter (keys(%hunter_times))
{
  my @all_nodes = keys(%hunter_nodes);
  my $init_time = 2 * ($hunter % scalar(keys(%{$hunter_nodes{$all_nodes[0]}})));

  if ($max_time < $hunter_times{$hunter} + $init_time)
  {
    $max_time = $hunter_times{$hunter} + $init_time;
  }
}

$done_tasks[1] = 0;
for my $second (1 .. int($max_time) - 1)
{
  my $tasks = $done_tasks[$second];
  $done_tasks[$second + 1] += $tasks;
  $working_hunters[$second] = 0;
}

foreach my $hunter (keys(%hunter_times))
{
  my @all_nodes = keys(%hunter_nodes);
  my $init_time = 2 * ($hunter % scalar(keys(%{$hunter_nodes{$all_nodes[0]}})));

  my %idletask;
  $idletask{'uuid'} = 0;
  $idletask{'time'} = 0.0 + sprintf("%.2f", $max_time - $hunter_times{$hunter} - $init_time);
  $idletask{'type'} = 'idle';

  for ($init_time .. int($hunter_times{$hunter}))
  {
    $working_hunters[$_]++;
  }

  push(@{$tasks{$hunter}}, \%idletask);
}

foreach my $hunter (keys(%hunter_times))
{
  my %init_task;
  my @all_nodes = keys(%hunter_nodes);
  $init_task{'uuid'} = 0;
  $init_task{'time'} = 2 * ($hunter % scalar(keys(%{$hunter_nodes{$all_nodes[0]}}))); # account for the 2*coreid second wait
  $init_task{'type'} = 'wait';
  unshift(@{$tasks{$hunter}}, \%init_task); #front!
}

my @nodes;
foreach my $node (sort {"$a" cmp "$b"} keys(%hunter_nodes))
{
  my %node;
  #$node{'name'} = 0 + $node;
  $node{'name'} = $node;
  $node{'size'} = scalar(keys(%{$hunter_nodes{$node}}));
  my @hunters;
  for my $hunter (sort {$a <=> $b} keys(%{$hunter_nodes{$node}}))
  {
    my %hunter_ob;
    $hunter_ob{'name'}  = $hunter;
    $hunter_ob{'size'}  = scalar(@{$tasks{$hunter}});
    $hunter_ob{'tasks'} = $tasks{$hunter};
    push(@hunters, \%hunter_ob);
  }
  $node{'children'} = \@hunters;
  push(@nodes, \%node);
}

$file{'name'}     = $infile;
$file{'max_time'} = 0.0 + sprintf("%.2f", $max_time);
$file{'children'} = \@nodes;


open(FH, ">$outfile") || die("Can't create output file, $outfile: $!\n");
print(FH encode_json(\%file));
close(FH);

open(FH, ">$outfile.tsv") || die("Can't create output file, $outfile.tsv: $!\n");
print FH "x\ty\ty2\ty3\n";
my $task_cutoff = int($done_tasks[$max_time - 1] * 0.8);
for my $idx (1 .. $max_time - 1)
{
  print FH "$idx\t" . $working_hunters[$idx] . "\t" . $done_tasks[$idx] . "\t" . $task_cutoff . "\n" ;
}
close(FH);

exit(0);
print Data::Dumper->Dump([\%file],['file']);
