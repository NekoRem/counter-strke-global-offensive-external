#pragma once

#define in_range( x, a, b ) ( x >= a && x <= b )
#define get_bits( x ) ( in_range( x, '0', '9' ) ? ( x - '0' ) : ( ( x & ( ~0x20 ) ) - 'A' + 0xa ) )
#define get_byte( x ) ( get_bits( x[ 0 ] ) << 4 | get_bits( x[ 1 ] ) )

namespace cs
{
	namespace memory
	{
		inline DWORD pid = 0;
		inline HANDLE handle = 0;
		inline HWND target = 0;
		inline uintptr_t client_dll = 0;
		inline uintptr_t engine_dll = 0;
		inline uintptr_t vstdlib_dll = 0;

		inline HMODULE ntdll = LI_FN(GetModuleHandleA).safe_cached()(_("ntdll"));
		inline SYSTEM_HANDLE_INFORMATION* handle_info = {};

		//inline _NtOpenProcess ntopenprocess = 0;
		inline _NtWriteVirtualMemory ntwritevirtualmemory = 0;
		inline _NtReadVirtualMemory ntreadvirtualmemory = 0;
		inline _NtDelayExecution ntdelayexecution = 0;
		inline _ZwSetTimerResolution zwsettimerresolution = 0;

		bool setup();
		bool setup_winapi();
		uintptr_t get_module_info(std::string module, size_t* size);
		bool compare_memory(const byte* data, const char* pattern);
		uintptr_t pattern_scan(std::string mod, const char* pattern, int offset, int extra, bool relative, bool subtract);
		HANDLE hijack_existing_handle(DWORD pid);
		OBJECT_ATTRIBUTES init_object_attributes(PUNICODE_STRING name, ULONG attributes, HANDLE hRoot, PSECURITY_DESCRIPTOR security);

		template<class t>
		inline bool write(uintptr_t address, t value)
		{
			return ntwritevirtualmemory(handle, (PVOID)address, &value, sizeof(t), 0);
		}

		inline bool write(uintptr_t address, PVOID buffer, size_t size)
		{
			return ntwritevirtualmemory(handle, (PVOID)address, buffer, size, 0);
		}

		template<class t>
		inline t read(uintptr_t address)
		{
			t data{};
			ntreadvirtualmemory(handle, (PVOID)address, &data, sizeof(t), 0);
			return data;
		}

		inline bool read(uintptr_t address, LPVOID buffer, size_t size)
		{
			auto ret = ntreadvirtualmemory(handle, (PVOID)address, &buffer, size, 0);
			return ret != 0;
		}
	}
}