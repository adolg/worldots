#/bin/bash

FILES_SRC=`find src -name *.c`
SLOC_1=0
SLOC_2=0

for FILE_SRC in $FILES_SRC
do
	let SLOC_1+=`grep -c ";" $FILE_SRC`
	let SLOC_2+=$(wc -l $FILE_SRC | awk '{ print $1 }')
done

echo -e "Instructions:\t$SLOC_1"
echo -e "Newlines:\t$SLOC_2"

