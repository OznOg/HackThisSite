#!/bin/bash
set -euo pipefail

if [ ! $# -eq 2 ]
then
 echo
 echo "   Usage: $0 [sudoku csv] [Blowfish encrypted string]"
 echo
 exit 1
fi

if [ ! -f "./sudoku9" ]
then
  echo
  echo "Warninig: ./sudoku9 is missing; trying to compile it..."
  set -x
  g++ -O3 sudoku9.cpp -o sudoku9 || exit $?
  set +x
fi

SUDOKU="$1"

ENCRYPTED="$2"

SUDOKU=$(echo $SUDOKU | awk -F, -v ORS=, '{ i=1; while (i < NF) { if ($i == "") print "0"; else print $i; i++; } ORS="\n"; print $NF}');

# add possibly missing '0' in front or at the end
SUDOKU=$(echo $SUDOKU | sed "s/,$/,0/g" | sed "s/^,/0,/g");

RES=$(./sudoku9 $SUDOKU | tail -1)

SHA1=$(echo -n $RES | sha1sum | cut -d\  -f1 )

SOLUTION=$(echo -e "keys(\"$SHA1\");\nprint(blowfish_decrypt('$ENCRYPTED'));" | php  -d auto_prepend_file=$PWD/blowfish.php -a 2> /dev/null | tail -1)

echo
echo "Decrypted text is:"
echo $SOLUTION
echo

# test data:
# 9,1,,5,4,3,,7,6,,4,5,8,7,,,1,9,,,8,,1,9,5,4,3,7,8,,3,2,1,6,,4,,2,3,6,5,,9,8,7,4,5,,9,8,7,3,2,,8,9,1,,3,2,7,,5,2,3,,7,,5,1,9,8,5,,7,1,9,8,4,3,
# WDq4vQAHhejcu0/swuPMyg==
# should give "5DURn)" (quotes are NOT part of answer)
