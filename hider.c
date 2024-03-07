#include <Windows.h>
#include <stdio.h>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

int main(int argc, char* argv[]) {
	if (!PathFileExistsW(L"injection.dll")) return 1;

	HMODULE mod_handle = GetModuleHandleW(L"kernel32.dll");

	LPVOID mod_loader = (LPVOID) GetProcAddress(mod_handle, "LoadLibraryW");

	WCHAR buffer[MAX_PATH];
	BOOL result = GetFullPathNameW(L"injection.dll", MAX_PATH, buffer, NULL);
	
	UINT32 pid = strtol(argv[1], NULL, 10);

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (!handle) return 1;

	size_t bytes = (wcslen(buffer) + 1) * sizeof(WCHAR);

	DWORD action = MEM_RESERVE | MEM_COMMIT;

	LPVOID address = (LPVOID) VirtualAllocEx(handle, NULL, bytes, action, PAGE_READWRITE);
		if (!address) return 1;
		if (!WriteProcessMemory(handle, address, buffer, bytes, NULL)) return 1;

	LPTHREAD_START_ROUTINE routine = (LPTHREAD_START_ROUTINE) mod_loader;

	HANDLE thread = CreateRemoteThread(handle, NULL, 0, routine, address, 0, NULL);
		if (!thread) return 1;
		if (WaitForSingleObject(thread, INFINITE) == WAIT_FAILED) return 1;
		
	DWORD ret = 0;
		if (!GetExitCodeThread(thread, &ret)) return 1;
		if (!ret) return 1;
		
	CloseHandle(thread);
	CloseHandle(handle);

	return 0;
}