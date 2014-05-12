#!/bin/bash
# Open initial output.
# Prefer konsole if its there, otherwise fall back to xterminal.
#tty -s; if [ $? -ne 0 ]; then
#	if command -v konsole &>/dev/null; then
#		konsole -e "$0"; exit;
#		else
#		xterm -e "$0"; exit;
#	fi
#fi

cd "$( dirname "${BASH_SOURCE[0]}" )"

mkdir -p "big"

kdialog --passivepopup "Building $PWD/*.png" 1


for CUR in *.cursor; do
	BASENAME=$CUR
	BASENAME=${BASENAME##*/}
	BASENAME=${BASENAME%.*}

	inkscape -i $BASENAME -f ../raw.svg -e $BASENAME.png
	inkscape -i $BASENAME -d 180 -f ../raw.svg -e big/$BASENAME.png
	
done

for i in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
do
   inkscape -i progress-$i -f ../raw.svg -e progress-$i.png
   inkscape -i progress-$i -d 180 -f ../raw.svg -e big/progress-$i.png

   inkscape -i wait-$i -f ../raw.svg -e wait-$i.png
   inkscape -i wait-$i -d 180 -f ../raw.svg -e big/wait-$i.png
done

kdialog --passivepopup "Done." 1