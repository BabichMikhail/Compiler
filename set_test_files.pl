use warnings;
use strict;

use IO::Dir;

my @Python_path = ("Tests\\\\Lexer\\\\", "Tests\\\\Parser\\\\", "Tests\\\\Parser_Declarations\\\\", "Tests\\\\Parser_Statements\\\\", "Tests\\\\Parser_Statements_Errors\\\\");
my @path = ("Tests\\Lexer\\", "Tests\\Parser\\", "Tests\\Parser_Declarations\\", "Tests\\Parser_Statements\\", "Tests\\Parser_Statements_Errors\\");
my @test_keys = ('-l', '-p', '-d', '-s', '-s');
my @lens;
my @dirs; 
foreach my $i (0.. $#path){
	push @dirs, IO::Dir->new($path[$i]);
	push @lens, 0;
}

my @out_names;
open(my $f1, '>tests_run.bat');
foreach my $i (0 .. $#dirs){
	if(defined $dirs[$i]){
		while(defined($_ = $dirs[$i]->read)){
			eval{
				if ($_ =~ qw/^(.*)\.in$/){
					my $name = $1;
					push @out_names, $name;
					#print $name, "\n";
					$lens[$i] += 1;
					print $f1 "Compiler.exe $test_keys[$i] $path[$i]$name.in";
					print $f1 " $path[$i]$name.out\n";
				}
			}	
		}
	}
}

print $f1 "init_tests.py\n";
close($f1);

open (my $f2, ">init_tests.py");
print $f2 "import unittest\n";
print $f2 "import filecmp\n\n";
print $f2 "class ClassForTest(unittest.TestCase):\n";
print $f2 "\tdef setUp(self):\n";
print $f2 "\t\tpass\n\n";
print $f2 "\tdef tearDown(self):\n";
print $f2 "\t\tpass\n\n";
my $l = -1;
foreach my $j (0 .. $#Python_path){
	for (my $i = 0; $i < $lens[$j]; ++$i){
		$l += 1;
		print $f2 "\tdef test_$l(self):\n";
		print $f2 "\t\tself.assertTrue(filecmp.cmp(\"$Python_path[$j]$out_names[$l]\.out\", \"$Python_path[$j]$out_names[$l]\.res\"))\n\n";
	}
}

print $f2 "if __name__ == '__main__':\n\tunittest.main()";
close($f2);