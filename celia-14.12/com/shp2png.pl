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
	"shp2shp.sh sid_$sno.shp  ; ".
	"shp2dot.sh ssid_$sno.shp  ; ".
	"dot -Tpng -osid_$sno.png ssid_$sno.shp.dot";

	print $commandes."\n";
	system $commandes;
}

close(MYFILE);


