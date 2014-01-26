#/bin/bash

FILES_SRC=`find src -type f`
MAXCOLS=72

for FILE_SRC in $FILES_SRC
do
	if [ "$(wc -L $FILE_SRC | awk '{ print $1 }')" -gt "$MAXCOLS" ]
	then
		echo -e "ERROR: Overlong line in $FILE_SRC."
		exit 1
	fi
	if [ "$(grep '[[:space:]]$' $FILE_SRC)" != "" ]
	then
		echo -e "ERROR: Trailing space or tab in $FILE_SRC."
		exit 1
	fi
done

echo -e "Maximum line length $MAXCOLS:\t\t\tSuccess."
echo -e "No trailing white spaces or tabs:\tSuccess."

