#!/bin/bash

COMPRESS=0
ERROR=0
HELP="Usage: $0 [OPTIONS]

$(basename "$0") is a script that reads from standard input, it takes every line that 
starts with substring 'PATH ' and designates the remainder of the line as path 
to file, it checks whether the file is regular file (FILE), directory (DIR), 
symbolic link (LINK) or neither (ERROR) and prints the type of the file, path to 
the file and some other file info.

Options:
  -h	Print help and exit script
  -z	Compress all regular files to 'output.tgz'"

while getopts "hz" OPT
do
    case $OPT in
    h) 
    	echo "$HELP"
    	exit 0
        ;;
    z) 
    	COMPRESS=1
    	FILES=()
        ;;
    *)
    	exit 2	
    	;;
    esac
done

while read VSTUP
do
	if [[ $VSTUP == "PATH "* ]]
	then
		FILE=${VSTUP#PATH }
		if [[ -L "$FILE" ]]
		then
			LINK=$(readlink "$FILE")
			if [[ "$?" -ne 0 ]]
			then 
				exit 2
			fi
			echo "LINK '$FILE' '$LINK'"
		elif [[ -f "$FILE" ]]
		then
			LINES=$(wc -l < "$FILE")
			if [[ "$?" -ne 0 ]]
			then
				exit 2
			fi
			FIRST_LINE=$(head -n 1 "$FILE")
			if [[ "$?" -ne 0 ]]
			then 
				exit 2
			fi
			echo "FILE '$FILE' $LINES '$FIRST_LINE'"
			if [[ $COMPRESS -eq 1 ]]
			then
				FILES+=("$FILE")
			fi
		elif [[ -d "$FILE" ]]
		then
			echo "DIR '$FILE'"
		
		else
			echo "ERROR '$FILE'" >&2
			ERROR=1
		fi
	fi 
done

if [ $COMPRESS -eq 1 -a ${#FILES[*]} -ne 0 ]
then
	tar czf output.tgz "${FILES[@]}"
	if [[ $? -ne 0 ]]
	then
		exit 2
	fi
fi
	
if [[ $ERROR -eq 0 ]]
then
	exit 0
else
	exit 1
fi
