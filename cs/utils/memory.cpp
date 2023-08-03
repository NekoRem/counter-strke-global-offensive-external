#include "../includes.hpp"

bool cs::memory::setup()
{
	LI_FN(GetWindowThreadProcessId).safe_cached()(target, &pid);
	if (!pid)
		return false;

	size_t sizes = 0;
	client_dll = get_module_info("client.dll", &sizes);
	engine_dll = get_module_info("engine.dll", &sizes);
	vstdlib_dll = get_module_info("vstdlib.dll", &sizes);

	printf_s(_("client: %p\nengine: %p\nvstdlib: %p\n"), client_dll, engine_dll, vstdlib_dll);

	handle = LI_FN(OpenProcess)(FILE_ALL_ACCESS, false, pid);
	//handle = hijack_existing_handle(pid);
	printf_s(_("handle hijacked: %p\n"), handle);

	if (!input::init())
		return false;

	return true;
}

bool cs::memory::setup_winapi()
{
	//ntopenprocess = (_NtOpenProcess)LI_FN(GetProcAddress).safe_cached()(ntdll, _("NtOpenProcess"));
	ntwritevirtualmemory = (_NtWriteVirtualMemory)LI_FN(GetProcAddress).safe_cached()(ntdll, _("NtWriteVirtualMemory"));
	ntreadvirtualmemory = (_NtReadVirtualMemory)LI_FN(GetProcAddress).safe_cached()(ntdll, _("NtReadVirtualMemory"));
	ntdelayexecution = (_NtDelayExecution)LI_FN(GetProcAddress).safe_cached()(ntdll, _("NtDelayExecution"));
	zwsettimerresolution = (_ZwSetTimerResolution)LI_FN(GetProcAddress).safe_cached()(ntdll, _("ZwSetTimerResolution"));

	if (!ntwritevirtualmemory || !ntreadvirtualmemory /*|| !ntopenprocess*/ || !ntdelayexecution || !zwsettimerresolution)
	{
		std::cerr << (_("failed to get NT functions.\n"));
		return false;
	}

	return true;
}

uintptr_t cs::memory::get_module_info(std::string module, size_t* size)
{
	auto snapshot = LI_FN(CreateToolhelp32Snapshot).safe_cached()(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	auto entry = MODULEENTRY32{};
	entry.dwSize = sizeof(entry);

	while (LI_FN(Module32Next).safe_cached()(snapshot, &entry)) {
		if (!strcmp(entry.szModule, module.c_str())) {
			LI_FN(CloseHandle).safe_cached()(snapshot);
			*size = (size_t)entry.modBaseSize;
			return reinterpret_cast<uintptr_t>(entry.modBaseAddr);
		}
	}
	return 0;
}

bool cs::memory::compare_memory(const byte* data, const char* pattern)
{
	for (; *pattern; *pattern != ' ' ? ++data : data, ++pattern) {
		if (*pattern == ' ' || *pattern == '?') continue;
		if (*data != get_byte(pattern)) return false;
		++pattern;
	}
	return true;
}

uintptr_t cs::memory::pattern_scan(std::string mod, const char* pattern, int offset, int extra, bool relative, bool subtract)
{
	size_t mod_size;
	auto module = get_module_info(mod, &mod_size);

	uintptr_t address = 0;

	byte* data = new byte[mod_size];
	ntreadvirtualmemory(handle, (LPVOID)module, data, mod_size, NULL);

	for (DWORD i = 0x1000; i < mod_size; i++) {
		if (compare_memory((const byte*)(data + i), pattern)) {
			address = module + i + offset;
			if (relative) { ntreadvirtualmemory(handle, (LPVOID)address, &address, sizeof(uintptr_t), NULL); }
			if (subtract) { address -= module; }
			address += extra;
			break;
		}
	}

	delete[] data;
	return address;
}

OBJECT_ATTRIBUTES cs::memory::init_object_attributes(PUNICODE_STRING name, ULONG attributes, HANDLE hRoot, PSECURITY_DESCRIPTOR security)
{
	OBJECT_ATTRIBUTES object;
	object.Length = sizeof(OBJECT_ATTRIBUTES);
	object.ObjectName = name;
	object.Attributes = attributes;
	object.RootDirectory = hRoot;
	object.SecurityDescriptor = security;
	return object;
}

bool is_valid_handle(HANDLE handle)
{
	if (handle && handle != INVALID_HANDLE_VALUE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void clean_up_and_exit(HANDLE process_handle, std::string error_message)
{
	delete[] cs::memory::handle_info;

	process_handle ? LI_FN(CloseHandle).safe_cached()(process_handle) : 0;
	std::cout << error_message << std::endl;
}

HANDLE cs::memory::hijack_existing_handle(DWORD pid)
{
	HANDLE process_handle = 0;
	HANDLE hijacked_handle = 0;

	_RtlAdjustPrivilege RtlAdjustPrivilege = (_RtlAdjustPrivilege)LI_FN(GetProcAddress).safe_cached()(memory::ntdll, _("RtlAdjustPrivilege"));

	boolean old_privilege;
	RtlAdjustPrivilege(SeDebugPriv, TRUE, FALSE, &old_privilege);

	_NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)LI_FN(GetProcAddress).safe_cached()(memory::ntdll, _("NtQuerySystemInformation"));

	_NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)LI_FN(GetProcAddress).safe_cached()(memory::ntdll, _("NtDuplicateObject"));

	_NtOpenProcess NtOpenProcess = (_NtOpenProcess)LI_FN(GetProcAddress).safe_cached()(memory::ntdll, _("NtOpenProcess"));

	OBJECT_ATTRIBUTES obj_attribute = init_object_attributes(NULL, NULL, NULL, NULL);

	CLIENT_ID client_id = { 0 };

	DWORD size = sizeof(SYSTEM_HANDLE_INFORMATION);

	handle_info = (SYSTEM_HANDLE_INFORMATION*) new byte[size];

	ZeroMemory(handle_info, size);

	NTSTATUS nt_ret = NULL;

	do
	{
		delete[] handle_info;

		size *= 1.5;
		try
		{
			handle_info = (PSYSTEM_HANDLE_INFORMATION) new byte[size];
		}
		catch (std::bad_alloc)
		{
			clean_up_and_exit(process_handle, _("bad heap allocation"));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	} while ((nt_ret = NtQuerySystemInformation(SystemHandleInformation, handle_info, size, NULL)) == STATUS_INFO_LENGTH_MISMATCH);

	if (!NT_SUCCESS(nt_ret))
	{
		clean_up_and_exit(process_handle, _("ntquerysysteminformation failed"));
	}

	for (unsigned int i = 0; i < handle_info->HandleCount; ++i)
	{
		static DWORD num_of_open_handles;

		LI_FN(GetProcessHandleCount).safe_cached()(LI_FN(GetCurrentProcess).safe_cached()(), &num_of_open_handles);

		/*if (NumOfOpenHandles > 65)
		{
			clean_up_and_exit("Error Handle Leakage Detected");
		}*/

		if (!is_valid_handle((HANDLE)handle_info->Handles[i].Handle))
			continue;

		if (handle_info->Handles[i].ObjectTypeNumber != ProcessHandleType)
			continue;

		client_id.UniqueProcess = (DWORD*)handle_info->Handles[i].ProcessId;

		process_handle ? LI_FN(CloseHandle).safe_cached()(process_handle) : 0;

		nt_ret = NtOpenProcess(&process_handle, PROCESS_DUP_HANDLE, &obj_attribute, &client_id);
		if (!is_valid_handle(process_handle) || !NT_SUCCESS(nt_ret))
			continue;

		nt_ret = NtDuplicateObject(process_handle, (HANDLE)handle_info->Handles[i].Handle, NtCurrentProcess, &hijacked_handle, PROCESS_ALL_ACCESS, 0, 0);
		if (!is_valid_handle(hijacked_handle) || !NT_SUCCESS(nt_ret))
			continue;

		if (LI_FN(GetProcessId).safe_cached()(hijacked_handle) != pid)
		{
			LI_FN(CloseHandle).safe_cached()(hijacked_handle);
			continue;
		}
		break;
	}

	clean_up_and_exit(process_handle, _("success"));

	return hijacked_handle;
}
