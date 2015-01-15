#!/usr/bin/perl

open(MYFILE, 'pan.abs.png');

while (<MYFILE>) {
 	chomp;
 	print "$_\n";
	@val = split(/:/, $_);
	foreach $k (@val) {
          print "$k ";
        }
	print "\n";
	$sno = @val[1];
	$fno = @val[2]; 
        $commandes = "ln -s -f f_$fno.shp sid_$sno.shp ; ".
	"shp2dot.sh sid_$sno.shp 1> sid_$sno.dot ; ".
	"dot -Tpng -osid_$sno.png sid_$sno.dot";

	print $commandes."\n";
	system $commandes;
}

close(MYFILE);


