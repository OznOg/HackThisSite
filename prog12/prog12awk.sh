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

RESULT=$(echo $CODE | awk '
BEGIN {
   for (n = 0; n <= 127; n++)
      ord[sprintf("%c", n)] = n
}

function isdigit(a) {
     return (a ~ /^[0-9]$/);
}

function isprime(a)
{
     return a == 2 || a == 3 || a == 5 || a == 7
}

{ 
  len = 0;
  prime_sum = 0; composite_sum = 0;
  for (i = 1; i <= length($0); i++) {
       char = substr($0, i, 1);
       if (char == "1" || char == "0") {
           continue;
       }
       if (isdigit(char)) {
            val = char - '0';
            if (isprime(val))
                  prime_sum += val;
            else
                  composite_sum += val;
       } else {
           if (len++ < 25)
              printf "%c", ord[char] + 1
       }
  }
  print composite_sum * prime_sum;
}')

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

