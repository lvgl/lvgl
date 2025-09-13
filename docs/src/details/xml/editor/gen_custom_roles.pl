#--------------------------------------------------------------------------
# Caution:  this Perl script is meant to be used on Windows.
#           If used on another platform, you will need to change the
#           line endings in the first IF/ELSEIF blocks and assign the
#           correct line endings for the ELSE block.
#
# Usage:
#     perl  gen_custom_roles.pl  /path/to/custom.css  >custom_roles.txt
#
# This script takes input from the specified .CSS file that looks like this:
#
# span.bi {
# 	font-weight: bold;
# 	font-style: italic;
# }
# span.ib {
# 	font-weight: bold;
# 	font-style: italic;
# }
#
# ...and generate output that looks like this:
#
# .. role:: bi
#     :class: bi
# .. role:: ib
#     :class: ib
#--------------------------------------------------------------------------
use strict;
use warnings;

my $i;
my $siLen;

# Linux or git-bash version of perl?
#print 'This Perl was built under OS [', $^O, '].';

if ($^O eq 'msys') {
	$/ = "\r\n";                                 # Chg input-record separator to include CR.
} elsif ($^O eq 'cygwin') {
	$/ = "\r\n";                                 # Chg input-record separator to include CR.
} else {
	# This perl was built under Windows or a DOS-ish OS and so CR/LF is already in $/.
}

while (<>) {
	chomp;                                       # Safely remove line ending.

	if (/^span\.(\w+)\s*\{/) {
		if ($1 ne 'pre') {
			print ".. role:: $1\n    :class: $1\n";
		}
	}
}
