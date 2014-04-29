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


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


mkdir -p "$DIR/output"
kdialog --passivepopup "Building""$DIR/*.cursor" 1


for CUR in `ls "$DIR"/*.cursor`; do
	BASENAME=$CUR
	BASENAME=${BASENAME##*/}
	BASENAME=${BASENAME%.*}

	xcursorgen "$CUR" "$DIR/output/$BASENAME";
done;


kdialog --passivepopup "Done." 1