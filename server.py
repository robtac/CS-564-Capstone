import hashlib
import math
import socket
import select
import threading
import os
import sys
import time

# Set up the server
host = '192.168.40.134'
command_port = 12345
data_port = 12346

command_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
data_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

command_socket.bind((host, command_port))
data_socket.bind((host, data_port))

command_socket.listen(1)
data_socket.listen(1)
key_dict = {1: 'Left mouse button', 2: 'Right mouse button', 3: 'Control-break processing', 4: 'Middle mouse button (three-button mouse)', 5: 'X1 mouse button', 6: 'X2 mouse button', 
            8: 'BACKSPACE key', 9: 'TAB key', 12: 'CLEAR key', 13: 'ENTER key', 16: 'SHIFT key', 17: 'CTRL key', 18: 'ALT key', 19: 'PAUSE key', 20: 'CAPS LOCK key', 21: 'IME Hangul mode', 
            22: 'IME On', 23: 'IME Junja mode', 24: 'IME final mode', 25: 'IME Kanji mode', 26: 'IME Off', 27: 'ESC key', 28: 'IME convert', 29: 'IME nonconvert', 30: 'IME accept', 
            31: 'IME mode change request', 32: 'SPACEBAR', 33: 'PAGE UP key', 34: 'PAGE DOWN key', 35: 'END key', 36: 'HOME key', 37: 'LEFT ARROW key', 38: 'UP ARROW key', 39: 'RIGHT ARROW key', 
            40: 'DOWN ARROW key', 41: 'SELECT key', 42: 'PRINT key', 43: 'EXECUTE key', 44: 'PRINT SCREEN key', 45: 'INS key', 46: 'DEL key', 47: 'HELP key', 48: '0 key', 49: '1 key', 
            50: '2 key', 51: '3 key', 52: '4 key', 53: '5 key', 54: '6 key', 55: '7 key', 56: '8 key', 57: '9 key', 65: 'A key', 66: 'B key', 67: 'C key', 68: 'D key', 69: 'E key', 70: 'F key', 
            71: 'G key', 72: 'H key', 73: 'I key', 74: 'J key', 75: 'K key', 76: 'L key', 77: 'M key', 78: 'N key', 79: 'O key', 80: 'P key', 81: 'Q key', 82: 'R key', 83: 'S key', 84: 'T key', 
            85: 'U key', 86: 'V key', 87: 'W key', 88: 'X key', 89: 'Y key', 90: 'Z key', 91: 'Left Windows key (Natural keyboard)', 92: 'Right Windows key (Natural keyboard)', 
            93: 'Applications key (Natural keyboard)', 95: 'Computer Sleep key', 96: 'Numeric keypad 0 key', 97: 'Numeric keypad 1 key', 98: 'Numeric keypad 2 key', 99: 'Numeric keypad 3 key', 
            100: 'Numeric keypad 4 key', 101: 'Numeric keypad 5 key', 102: 'Numeric keypad 6 key', 103: 'Numeric keypad 7 key', 104: 'Numeric keypad 8 key', 105: 'Numeric keypad 9 key', 
            106: 'Multiply key', 107: 'Add key', 108: 'Separator key', 109: 'Subtract key', 110: 'Decimal key', 111: 'Divide key', 112: 'F1 key', 113: 'F2 key', 114: 'F3 key', 115: 'F4 key', 
            116: 'F5 key', 117: 'F6 key', 118: 'F7 key', 119: 'F8 key', 120: 'F9 key', 121: 'F10 key', 122: 'F11 key', 123: 'F12 key', 124: 'F13 key', 125: 'F14 key', 126: 'F15 key', 127: 'F16 key', 
            128: 'F17 key', 129: 'F18 key', 130: 'F19 key', 131: 'F20 key', 132: 'F21 key', 133: 'F22 key', 134: 'F23 key', 135: 'F24 key', 144: 'NUM LOCK key', 145: 'SCROLL LOCK key', 
            160: 'Left SHIFT key', 161: 'Right SHIFT key', 162: 'Left CONTROL key', 163: 'Right CONTROL key', 164: 'Left ALT key', 165: 'Right ALT key', 166: 'Browser Back key', 167: 'Browser Forward key', 
            168: 'Browser Refresh key', 169: 'Browser Stop key', 170: 'Browser Search key', 171: 'Browser Favorites key', 172: 'Browser Start and Home key', 173: 'Volume Mute key', 174: 'Volume Down key', 
            175: 'Volume Up key', 176: 'Next Track key', 177: 'Previous Track key', 178: 'Stop Media key', 179: 'Play/Pause Media key', 180: 'Start Mail key', 181: 'Select Media key', 182: 'Start Application 1 key', 
            183: 'Start Application 2 key', 186: "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key", 187: "For any country/region, the '+' key", 
            188: "For any country/region, the ',' key", 189: "For any country/region, the '-' key", 190: "For any country/region, the '.' key", 
            191: "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key", 192: "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key", 
            219: "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key", 220: "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\\|' key", 
            221: "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key", 
            222: "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key", 223: 'Used for miscellaneous characters; it can vary by keyboard.',
            226: "The <> keys on the US standard keyboard, or the '\\|' key on the non-US 102-key keyboard", 229: 'IME PROCESS key', 
            231: 'Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods.', 246: 'Attn key', 247: 'CrSel key', 248: 'ExSel key', 
            249: 'Erase EOF key', 250: 'Play key', 251: 'Zoom key', 252: 'Reserved', 253: 'PA1 key', 254: 'Clear key'}

def compute_checksum(file_path):
    sha256_hash = hashlib.sha256()
    with open(file_path, "rb") as f:
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()

def adjust_logfile_using_key_map(file_path):
    with open(file_path, 'r') as f:
        lines = f.readlines()

    print("".join(lines))

    new_lines = []
    for line in lines:
        line = line.strip()
        if line.isdigit():
            key = int(line)
            if key in key_dict:
                new_lines.append(key_dict[key] + "\n")
            else:
                new_lines.append(line + "\n")
        else:
            new_lines.append(line + "\n")

    content = "".join(new_lines)
    print(content)

    with open(file_path, 'w') as f:
        f.write(content)

# Initialize the command as an empty string
command = ''

# Define a function that gets user input and updates the command
def get_command():
    global command
    while True:
        new_command = input('Enter a command: ')
        if new_command:
            command = new_command

# Create a thread for user input
input_thread = threading.Thread(target=get_command)
input_thread.daemon = True
input_thread.start()

# Directory to store "prev" files
prev_dir = "prev_files"

# Ensure that the directory for "prev" files exists
if not os.path.isdir(prev_dir):
    os.mkdir(prev_dir)

while True:
    # Wait for incoming connections
    command_client_socket, address = command_socket.accept()

    # Send the current command to the other application if there is any
    command_client_socket.sendall(command.encode())

    # Handle the "save" and "regsave" commands
    # Handle the "save" and "regsave" commands
    if command in ['save', 'regsave']:
        file_name = 'received_logfile.log' if command == 'save' else ''
        if command == 'regsave':
            for name in ['s_system.save', 's_sam.save', 's_security.save']:
                prev_file_path = os.path.join(prev_dir, f"{name}_prev")
                new_file_path = name
                
                # Delete any existing previous file
                if os.path.isfile(prev_file_path):
                    os.remove(prev_file_path)
                
                # Move the existing files to "prev" versions before writing new ones
                if os.path.isfile(new_file_path):
                    os.rename(new_file_path, prev_file_path)
                
                with open(new_file_path, 'wb') as f:
                    data_client_socket, _ = data_socket.accept()
                    while True:
                        data = data_client_socket.recv(1024)
                        if not data:
                            break
                        f.write(data)
                    data_client_socket.close()
                
                # Compute checksum for the new version of the file
                checksum_new = compute_checksum(new_file_path)

                # Only compute checksum for the previous version of the file if it exists
                if os.path.isfile(prev_file_path):
                    checksum_prev = compute_checksum(prev_file_path)

                    # Compare the checksums and print the result
                    if checksum_prev == checksum_new:
                        print(f"The file '{name}' has not been modified.")
                    else:
                        print(f"The file '{name}' has been modified.")
                else:
                    print(f"No previous version of the file '{name}' exists.")


            
                    
        else:
            data_client_socket, _ = data_socket.accept()
            prev_time = None
            while True:
                data = data_client_socket.recv(1024)
                if not data:
                    break

                current_time = time.time()  # Record the current time
                if prev_time is not None:
                    time_diff = current_time - prev_time
                    print(f"Time difference: {time_diff}\n".encode())
                    time_diff = (time_diff) * 100 - 0.5
                    k = int(math.floor((time_diff)))
                    k = k if k > 0 else 0
                    while(1):
                        if k in key_dict:
                            break
                        k = k + 1
                    print(f"Character: {key_dict[k]}\n".encode())
                
                prev_time = current_time  # Update the previous time

            data_client_socket.close()

        print(f"Logfile received and saved as '{file_name}'.")
        # adjust_logfile_using_key_map(file_name)
        print("Logfile adjusted using the key_map.")
        command = "run"

    # Shut down the server if the command is "quit"
    if command == 'kill':
        print('Shutting down the server...')
        command_client_socket.close()
        data_client_socket.close()
        command_socket.close()
        data_socket.close()
        sys.exit(0)

    command_client_socket.close()
