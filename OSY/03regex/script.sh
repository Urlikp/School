#!/bin/bash

A=0
B=0
C=0
D=0
HELP="Usage: $0 [OPTIONS]

$(basename "$0") is a script that has lot of utility as described bellow.

Options:
	-h	Print help and exit script
	-a	List all pdf files in current directory
	-b	Print all lines that begin with signed number from stdin
	-c	Print all sentences (beginning with [[:upper:]] and ending with [.!?]) 
		from stdin
	-d <prefix>
		Insert <prefix> into include directive"

while getopts ":habcd:" OPT
do
    case $OPT in
    h) 
    	echo "$HELP"
    	exit 0
        ;;
    a) 
    	A=1
    	break
        ;;
    b) 
    	B=1
    	break
        ;;
    c) 
    	C=1
    	break
        ;;
    d)
    	D=1
    	PREFIX=${OPTARG}
    	break
    	;;
    ?)
    	echo "$HELP"
    	exit 1	
    	;;
    esac
done

if [ $A -eq 1 ]
then
	ls -a | grep -i '\.pdf$'
elif [ $B -eq 1 ]
then
	while read VSTUP
	do
		if [[ "$VSTUP" =~ ^[+-]?[[:digit:]] ]]
		then
			echo "$VSTUP" | sed 's/^[+-]\?[[:digit:]]*//'
		fi
	done
elif [ $C -eq 1 ]
then
	STRING=""
	while IFS= read VSTUP
	do
		STRING+="$VSTUP"$'\n'
	done
	echo "$STRING" | tr '\n' ' ' | sed -e 's/^[^[:upper:]]*//' -e 's/\.[^[:upper:]]*/\.\n/g' -e 's/?[^[:upper:]]*/?\n/g' -e 's/![^[:upper:]]*/!\n/g'
elif [ $D -eq 1 ]
then
	PREFIX="$(echo "$PREFIX" | sed 's#\/#\\/#g')"
	while IFS= read VSTUP
	do
		if [[ "$VSTUP" =~ '#'[[:blank:]]*include[[:blank:]]*'<'.*'>' ]] || [[ "$VSTUP" =~ '#'[[:blank:]]*include[[:blank:]]*'"'.*'"' ]]
		then
			echo "$VSTUP" | sed 's/#[[:blank:]]*include[[:blank:]]*[<"]/&'$PREFIX'/g'
		else
			echo "$VSTUP"
		fi
	done
fi

exit 0
