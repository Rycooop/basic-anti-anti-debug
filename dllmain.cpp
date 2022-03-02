#include <iostream>
#include <Windows.h>
#include <winternl.h>

//Simple anti-anti debug, will work for games with shit anticheat that check isDebuggerPresent(), dont try and use on BE or EAC unless you have disabled ObRegisterCallbacks().

DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create a console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "conout$", "w", stdout);
    system("Color 0B");
    std::cout << "[*] Injected..." << std::endl;

    //Locate the peb and cast to a structure
    std::cout << "[*] Locating the PEB..." << std::endl;
    PPEB peb = (PPEB)__readfsdword(0x30);
    //for 64 bit games
    //PPEB peb = (PPEB)__readgsqword(0x60);

    
    //Or manually patch the KERNELBASE IsDebuggerPresentFunction:
     HMODULE hDLL = GetModuleHandle(L"KERNELBASE.dll");
    DWORD* isDebuggerAddr = (DWORD*)GetProcAddress(hDLL, "IsDebuggerPresent");
   
    //Allow write permissions so we can patch
    DWORD oldProtect;
    VirtualProtect(isDebuggerAddr, 0xA, PAGE_EXECUTE_READWRITE, &oldProtect);
    
    //Zero the memory
    memset(isDebuggerAddr, 0x90, 0xA);

    //Bytes to write
    BYTE shellcode[] = { 0x64, 0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3 };

    //Copy shellcode into function and restore page protections
    memcpy(isDebuggerAddr, &shellcode, sizeof(shellcode));
    VirtualProtect(isDebuggerAddr, 0xA, oldProtect, &oldProtect);
    
    //F3 to uninject
    while (!GetAsyncKeyState(VK_F3) &1)
    {
        //Continually set PEB being debugged flag to 0
        peb->BeingDebugged = 0;
    }

    //Shut down and uninject
    FreeConsole();
    fclose(f);
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

