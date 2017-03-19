strings app3win.exe | grep GET
gives => GET /missions/application/app3/auth.php?key= 

try the url directly on the web site, you get an empty web page holding "false" as only word.

=> the app seems to check the key validity thanks to this url, thus just need to spoof it:

add hackthissite.org as name of localhost in /etc/hosts

on a console make netcat return true=> $ echo true | nc -l 80

and start app3win.exe from elsewhere, and just click the ok button...

