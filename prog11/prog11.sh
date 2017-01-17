#!/bin/bash
set -euo pipefail

if [ ! -f cookies.txt ]
then
    echo
    echo Error: a cookies.txt file is needed to connect to HTS correctly
    echo
    exit 1
fi

wget --load-cookies=cookies.txt https://www.hackthissite.org/missions/prog/11/ -O index.html >/dev/null 2>&1
HTML_PAGE="$(cat index.html)"

CODE=$(echo "$HTML_PAGE" | grep "Generated String: " | sed 's/.*Generated String: //' | sed 's,<br />, ,g' | cut -f1 -d\ )

SHIFT=$(echo "$HTML_PAGE" | grep "Generated String: " | sed 's/.*Generated String: //' | sed 's,<br />, ,g' | sed 's/Shift: //g' | awk '{ print $NF }')

SEPARATOR="$(echo $CODE | dd bs=1 count=1 skip=2 2>/dev/null)"

echo "CODE=$CODE"
echo "SHIFT=$SHIFT"

RESULT=$(echo "$CODE" | awk -F"$SEPARATOR" -v SHIFT=$SHIFT $'{ for (i = 1; i < NF; i++) printf "%c", $i - SHIFT } END { print "" }')

echo RESULT=$RESULT

wget --referer=url=https://www.hackthissite.org/missions/prog/11/ --load-cookies=cookies.txt https://www.hackthissite.org/missions/prog/11/index.php -O answer.html  --post-data="solution=$RESULT" > /dev/null 2>&1 

if [ $(grep -c Wrong answer.html) -eq 0 ]
then
echo SUCCESS
else
echo ERROR
fi

