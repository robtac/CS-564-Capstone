# CS-564-Capstone

## Requirements

- [impacket](https://github.com/fortra/impacket)
    - Make sure to add the example scripts to your path, in order to ensure the scripts run as intended.
- Original zerologon exploit from [risksense/zerologon](https://github.com/risksense/zerologon)

## Initial Access
Use the script zerologon.sh in the to get access to the target system. Pass the domain, dc netbios name, and dc ip address to the script like so:
    `./zerologon DOMAIN DC_NETBIOS_NAME DC_IP_ADDRESS`
It will drop you into a privleged shell if it executes succesfully
