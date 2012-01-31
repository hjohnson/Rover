#!/bin/bash
rm -rf gerber
mkdir gerber
mv *.GTL gerber
mv *.GTO gerber
mv *.GTS gerber
mv *.GBL gerber
mv *.GBO gerber
mv *.GBS gerber
mv *.TXT gerber
rm *.gpi 
rm *.GTP
rm *.dri
rm *.GML
cd gerber
echo "Enter zip file name for gerbers"
read filename
zip $filename *
cd ..
