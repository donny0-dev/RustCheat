#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <Psapi.h>
#include <Windows.h>
#include <TlHelp32.h>

#include "HyperV/HyperV.h"

#define STR_BUFFER_SIZE 64
#define WSTR_BUFFER_SIZE 1024

class c_memory
{
private:

	HyperV* hyper_v = new HyperV();
	DWORD rust_pid = NULL;

public:

	uintptr_t game_assembly = NULL;
	uintptr_t unity_player = NULL;

	HWND rust_window = NULL;

    uintptr_t pid;
    HANDLE proc_handle;

    bool setup()
    {
        while (!rust_window)
            rust_window = FindWindowA(NULL, "Rust");

        GetWindowThreadProcessId(rust_window, &rust_pid);

        pid = find_process_id("RustClient.exe");
        proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        std::cout << "Process ID: " << pid << std::endl;
        std::cout << "Process Handle: " << std::hex << proc_handle << "\n";

        if (pid != NULL && proc_handle != NULL)
            return true;

        return false;
    }

    int find_process_id(const char* procname) {

        HANDLE hSnapshot;
        PROCESSENTRY32 pe;
        int pid = 0;
        BOOL hResult;

        std::string proc_name(procname);

        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

        pe.dwSize = sizeof(PROCESSENTRY32);

        hResult = Process32First(hSnapshot, &pe);

        while (hResult) {
            if (strcmp(proc_name.c_str(), pe.szExeFile) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
            hResult = Process32Next(hSnapshot, &pe);
        }

        CloseHandle(hSnapshot);
        return pid;
    }

    DWORD_PTR get_module_base_address(DWORD dwProcID, std::string szModuleName)
    {
        DWORD_PTR dwModuleBaseAddress = 0;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
        if (hSnapshot != INVALID_HANDLE_VALUE)
        {
            MODULEENTRY32 ModuleEntry32;
            ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
            if (Module32First(hSnapshot, &ModuleEntry32))
            {
                do
                {
                    if (strcmp(ModuleEntry32.szModule, szModuleName.c_str()) == 0)
                    {
                        dwModuleBaseAddress = (DWORD_PTR)ModuleEntry32.modBaseAddr;
                        break;
                    }
                } while (Module32Next(hSnapshot, &ModuleEntry32));
            }
            CloseHandle(hSnapshot);
        }
        return dwModuleBaseAddress;
    }

    uintptr_t module_base(const char* module_name)
    {
        return get_module_base_address(pid, module_name);
    }

    uintptr_t get_class(uintptr_t typeinfo)
    {
        return chain<uintptr_t>(game_assembly, { typeinfo, 0xB8, 0x0 });
    }

    void read(uintptr_t address, void* buffer, size_t size)
    {
        ReadProcessMemory(proc_handle, (void*)address, buffer, size, NULL);
    }

    template <typename t>
    t read(uintptr_t address)
    {
        t buffer;
        ReadProcessMemory(proc_handle, (void*)address, &buffer, sizeof(t), NULL);
        return buffer;
    }

    std::string read_str(uintptr_t address, int size = STR_BUFFER_SIZE)
    {
        std::unique_ptr<char[]> buffer(new char[size]);
        read(address, buffer.get(), size);
        return std::string(buffer.get());
    }

    std::wstring read_wstr(uintptr_t address)
    {
        wchar_t buffer[WSTR_BUFFER_SIZE * sizeof(wchar_t)];
        read(address, &buffer, WSTR_BUFFER_SIZE * sizeof(wchar_t));
        return std::wstring(buffer);
    }

    template <typename t>
    t chain(uintptr_t address, std::vector<uintptr_t> chain)
    {
        uintptr_t cur_read = address;

        for (int i = 0; i < chain.size() - 1; ++i)
            cur_read = read<uintptr_t>(cur_read + chain[i]);

        return read<t>(cur_read + chain[chain.size() - 1]);
    }

    template<typename v>
    bool write(uintptr_t address, const v& value)
    {
        DWORD old_protection = 0;
        VirtualProtectEx(proc_handle, (LPVOID)address, sizeof(v), PAGE_EXECUTE_READWRITE, &old_protection);
        WriteProcessMemory(proc_handle, (LPVOID)address, &value, sizeof(v), NULL);
        VirtualProtectEx(proc_handle, (LPVOID)address, sizeof(v), old_protection, NULL);
        return true;
    }

}; inline c_memory memory;