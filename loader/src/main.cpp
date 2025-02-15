//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <iostream>
#include <string>
#include <windows.h>

static bool InjectModDll(PROCESS_INFORMATION& pi, std::string_view dllName) {
    LPVOID dllPathAddr = VirtualAllocEx(pi.hProcess, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!dllPathAddr) {
        std::cerr << "VirtualAllocEx failed: " << GetLastError() << "\n";
        TerminateProcess(pi.hProcess, 1);
        return false;
    }

    // Write DLL path to target process
    if (!WriteProcessMemory(pi.hProcess, dllPathAddr, dllName.data(), dllName.size(), nullptr)) {
        std::cerr << "WriteProcessMemory failed: " << GetLastError() << "\n";
        VirtualFreeEx(pi.hProcess, dllPathAddr, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }

    // Get LoadLibraryW address (same in all processes)
    LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    if (!loadLibraryAddr) {
        std::cerr << "GetProcAddress failed: " << GetLastError() << "\n";
        VirtualFreeEx(pi.hProcess, dllPathAddr, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }

    // Create remote thread to load the DLL
    HANDLE hThread =
        CreateRemoteThread(pi.hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, dllPathAddr, 0, nullptr);

    if (!hThread) {
        std::cerr << "CreateRemoteThread failed: " << GetLastError() << "\n";
        VirtualFreeEx(pi.hProcess, dllPathAddr, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }

    // Wait for DLL to load and hooks to install
    WaitForSingleObject(hThread, INFINITE);

    // Cleanup
    VirtualFreeEx(pi.hProcess, dllPathAddr, 0, MEM_RELEASE);
    CloseHandle(hThread);

    return true;
}

static bool startSteamGame() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(nullptr,                   // No module name (use command line)
                       (LPSTR) "Hades.exe",  // Command line
                       NULL,                      // Process handle not inheritable
                       NULL,                      // Thread handle not inheritable
                       FALSE,                     // Set handle inheritance to FALSE
                       CREATE_SUSPENDED,          // Don't run the proggramm
                       NULL,                      // Use parent's environment block
                       NULL,                      // Use parent's starting directory
                       &si,                       // Pointer to STARTUPINFO structure
                       &pi)                       // Pointer to PROCESS_INFORMATION structure
    ) {
        std::cerr << "CreateProcess failed: " << GetLastError() << "\n";
        return false;
    }

    InjectModDll(pi, "HadesModCore.dll");

    // Resume the main thread
    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hThread, INFINITE);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

int main() {
    if (!startSteamGame()) {
        MessageBoxA(nullptr, "Cannot start the game", "Fail", MB_OK);
        return 1;
    }

    return 0;
}
