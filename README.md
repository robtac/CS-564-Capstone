# CS-564-Capstone

## Requirements

- [impacket](https://github.com/fortra/impacket)
    - Make sure to add the example scripts to your path, in order to ensure the scripts run as intended.
- Original zerologon exploit from [risksense/zerologon](https://github.com/risksense/zerologon)
- Windows Server 2019 VM

## Initial Access
Use the script zerologon.sh in the to get access to the target system. Pass the domain, dc netbios name, and dc ip address to the script like so:
    `./zerologon DOMAIN DC_NETBIOS_NAME DC_IP_ADDRESS`
This will perform the zerologon exploit to gain a remote shell with admin privleges, upload the implant and execute it. Ensure you have the client.c 

## client.c

Client.c is should be compiled and named svchosts.exe and put in the same folder as the zerologon.sh when running it to successfully implant and run the executable. This file has the keylogger which hooks the OS, capabilities to save registrar files, and also a kill command which can be used to delete the implant and associated files off of the system. Here is the command used to compile client.c:
    gcc -O3 -static -s client.c -o svchosts.exe -lws2_32

## server.py

This is the server that sends commands to the implant and also recieves data back. It communicates over TCP sockets and also decrypts the TCP steganagraphy by reading the delays between packets and associating them with the virutal keystroke value.

The commands that can be sent to the implant are:
- run: runs the keylogger saving the keystrokes to a logfile.
- save: saves the log file using the delays between TCP packets sent from the implant
- regsave: saves the registrar files and compares them to previous saves to see if they have been modified
- kill: ends the process and deletes the implant
