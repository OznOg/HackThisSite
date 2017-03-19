#!/bin/bash
set -euo pipefail

if [ ! -f cookies.txt ]
then
    echo
    echo Error: a cookies.txt file is needed to connect to HTS correctly
    echo
    exit 1
fi

wget --load-cookies=cookies.txt https://www.hackthissite.org/missions/prog/12/ -O index.html >/dev/null 2>&1

HTML_PAGE="$(cat index.html)"

CODE=$(echo "$HTML_PAGE" | grep "String: " | cut -d\" -f4)

RESULT=$(./prog12 $CODE)

echo RESULT=$RESULT

# Wget seems not to work here for a urlencode reason... (AFAIU)
#wget --referer=https://www.hackthissite.org/missions/prog/12/index.php --load-cookies=cookies.txt https://www.hackthissite.org/missions/prog/12/index.php -O answer.html  --post-data="solution=$RESULT" --post-data="submitbutton=Submit (remaining time: 5 seconds)" >/dev/null 2>&1
curl -e https://www.hackthissite.org/missions/prog/12/index.php -b cookies.txt --data "solution=$RESULT&submitbutton=Submit (remaining time: 5 seconds)" https://www.hackthissite.org/missions/prog/12/index.php 2>&1 > answer.html

if [ $(grep -c wrong answer.html) -eq 0 ]
then
echo SUCCESS
else
echo ERROR
fi

