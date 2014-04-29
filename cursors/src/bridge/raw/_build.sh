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

mkdir -p "output"

kdialog --passivepopup "Building $PWD/*.cursor" 1


for CUR in *.cursor; do
	BASENAME=$CUR
	BASENAME=${BASENAME##*/}
	BASENAME=${BASENAME%.*}
	
	err="$( xcursorgen "$CUR" "output/$BASENAME" 2>&1 )"
	
	if [[ "$?" -ne "0" ]]; then
		kdialog --icon dialog-warning --passivepopup "$CUR fail: $err" 1
	fi
done


kdialog --passivepopup "Done." 1