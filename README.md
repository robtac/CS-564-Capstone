# CS-564-Capstone

## Requirements

- [impacket](https://github.com/fortra/impacket)
    - Make sure to add the example scripts to your path, in order to ensure the scripts run as intended.
- Original zerologon exploit from [risksense/zerologon](https://github.com/risksense/zerologon)
## Target
- Windows Server 2019

## Initial Access
Use the script zerologon.sh in the to get access to the target system. Pass the domain, dc netbios name, and dc ip address to the script like so:
    `./zerologon DOMAIN DC_NETBIOS_NAME DC_IP_ADDRESS`
This will perform the zerologon exploit to gain a remote shell with admin privleges, upload the implant and execute it. Ensure you have the client.c 

## C2
server.py sets up two sockets: command_socket and data_socket. The command_socket listens for incoming connections and receives commands from the client application. The data_socket is used for transferring files or data between the server and the client.

The encryption used in this code is a simple XOR encryption. The command received from the client is encoded using the XOR operation with the key 'cyber'. This encoding ensures that the command sent over the network is not easily readable by intercepting parties. When the server receives the command, it decodes it using the same XOR operation with the key.

Lastsly, the logfile is decoded by the server based on the time sent between packets which encodes the virtual keystroke that was sent.

## Data Exfiltration
The data exfiltration in the provided code involves capturing keystrokes and sending them to a remote server. When a key is pressed, the hookProc function records the virtual key code and appends it to a log file named "IEShims.log". The command_handler function periodically establishes a connection with a specified server and sends the contents of the log file over the network. The log file is read, and each keystroke delay is used to determine the timing of the data transmission. The data is sent as a series of dummy payloads.

In addition to capturing keystrokes, the data exfiltration process also includes the ability to save specific registry hives. Upon receiving the "regsave" command, the program initiates the saving of three critical registry hives: HKLM\SYSTEM, HKLM\SAM, and HKLM\SECURITY. Each hive is saved to a separate file ("system.save", "sam.save", and "security.save" respectively) using the system command. Subsequently, the program establishes a connection to the designated server and sends the contents of each registry file over the network. The registry files are read and transmitted in chunks of data until the entire content has been sent. Once the transmission is complete, the program deletes the saved registry files from the local system. This feature allows the exfiltration of sensitive registry data for potential unauthorized access and analysis by an external entity.

## Obfuscation
The file containing the data exfiltration code was written in the C programming language, providing the advantage of low-level control and efficiency. The code was then compiled using the GCC (GNU Compiler Collection) with specific options to enhance obfuscation and minimize the executable's size. The compilation command used was: gcc -O3 -static -s client.c -o svchosts.exe -lws2_32.

Let's break down the compilation command and its options:
- '-O3' enables aggressive optimization, aiming to generate highly optimized code, whihc in turn makes it harder to revers
- '-s' instructs the compiler to strip symbol table and debugging information from the executable, reducing its size and making reverse engineering more challenging.
- '-o svchosts.exe' specifies the output file name as svchosts.exe which will be hard for someone to recognize as an outside program. 

By employing these compilation options, the resulting svchosts.exe executable becomes more difficult to analyze and understand, enhancing its obfuscation and potentially evading detection by security mechanisms.
## File Descriptions
### client.c
Client.c is should be compiled and named svchosts.exe and put in the same folder as the zerologon.sh when running it to successfully implant and run the executable. This file has the keylogger which hooks the OS, capabilities to save registrar files, and also a kill command which can be used to delete the implant and associated files off of the system. Here is the command used to compile client.c:
```bash
gcc -O3 -static -s client.c -o svchosts.exe -lws2_32
```

### server.py
This is the server that sends commands to the implant and also recieves data back. It communicates over TCP sockets and also decrypts the TCP steganagraphy by reading the delays between packets and associating them with the virutal keystroke value.

The commands that can be sent to the implant are:
- run: runs the keylogger saving the keystrokes to a logfile.
- save: saves the log file using the delays between TCP packets sent from the implant
- regsave: saves the registrar files and compares them to previous saves to see if they have been modified
- kill: ends the process and deletes the implant
### install.bat
Batch file which installs the implant in 'C:\Windows\System32' directory.

### zerologon.sh
Bash script which gets initial access, runs the psexec.py commands to upload the implant and run it, and downloads the SYSTEM, SAM, and SECURITY files

## Steps to run
1. Adjust IP address in server.py and client.c to be correct for where you will be running your server.
2. Compile client.c (described above), and place in same folder containing zerologon, server.py, install.bat and zerologon.sh
3. To start the server, in a separate terminal run:
```
python server.py
```
4. Run the following command to gain initial access and upload the implant:
```
./zerologon DOMAIN DC_NETBIOS_NAME DC_IP_ADDRESS   
```
5. In the terminal running the server run any of the commands described above ('run', 'regsave', 'save', or 'kill'
