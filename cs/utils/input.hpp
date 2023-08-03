#pragma once

namespace input
{
	inline BYTE nt_user_send_input_bytes[30];

	__forceinline bool init()
	{
		LPVOID ntusersendinput_address = LI_FN(GetProcAddress).safe_cached()(LI_FN(GetModuleHandleA).safe_cached()(_("win32u")), _("NtUserSendInput"));
		if (!ntusersendinput_address)
		{
			std::cout << _("failed to find NtUserSendInput (win32u)\n");
			LPVOID ntusersendinput_address = LI_FN(GetProcAddress).safe_cached()(LI_FN(GetModuleHandleA).safe_cached()(_("user32")), _("NtUserSendInput"));
			if (!ntusersendinput_address)
			{
				std::cout << _("failed to find NtUserSendInput (user32)\n");
				LPVOID ntusersendinput_address = LI_FN(GetProcAddress).safe_cached()(LI_FN(GetModuleHandleA).safe_cached()(_("user32")), _("SendInput"));
				if (!ntusersendinput_address)
				{
					std::cout << _("failed to find SendInput (user32)\n");
					return false;
				}
			}
		}

		memcpy(nt_user_send_input_bytes, ntusersendinput_address, 30);
		return true;
	}

	__forceinline bool NtUserSendInput(UINT cInputs, LPINPUT pInputs, int cbSize)
	{
		LPVOID ntusersendinput_spoofed = LI_FN(VirtualAlloc).safe_cached()(0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!ntusersendinput_spoofed)
			return false;

		memcpy(ntusersendinput_spoofed, nt_user_send_input_bytes, 30);
		NTSTATUS result = reinterpret_cast<NTSTATUS(NTAPI*)(UINT, LPINPUT, int)>(ntusersendinput_spoofed)(cInputs, pInputs, cbSize);
		ZeroMemory(ntusersendinput_spoofed, 0x1000);
		LI_FN(VirtualFree).safe_cached()(ntusersendinput_spoofed, 0, MEM_RELEASE);
		return (result > 0);
	}

	__forceinline bool move_mouse(int x, int y)
	{
		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi.mouseData = 0;
		input.mi.time = 0;
		input.mi.dx = x;
		input.mi.dy = y;
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
		return NtUserSendInput(1, &input, sizeof(input));
	}
}