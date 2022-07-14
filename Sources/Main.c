#include <RainbowCaption.h>

HMODULE restrict ProcessModule;
DWORD CycleDelay = 50;
int CyclePriority = THREAD_PRIORITY_IDLE;

static void LoadConfig(void);
static void SaveConfig(void);

__declspec(noreturn) void __cdecl MainEntrypoint(void) {
	LoadConfig();
	ProcessModule = GetModuleHandleW(NULL);
	HANDLE const restrict CyclerThread = CreateThread(NULL, 1, Cycler, NULL, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	SetThreadPriority(CyclerThread, CyclePriority);

	const WNDCLASSW _WndClass = {
		.lpfnWndProc = NotifyIconProc,
		.hInstance = ProcessModule,
		.lpszClassName = L"RainbowCaptionWndClass"
	};
	const ATOM WndClass = RegisterClassW(&_WndClass);
	HWND const restrict NotifyWindow = CreateWindowExW(0, MAKEINTATOM(WndClass), NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, ProcessModule, NULL);

	NOTIFYICONDATAW NotifyIcon;
	NotifyIcon.cbSize = sizeof(NotifyIcon);
	NotifyIcon.hWnd = NotifyWindow;
	NotifyIcon.uID = 1;
	NotifyIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	NotifyIcon.uCallbackMessage = WM_SHLICON;
	NotifyIcon.hIcon = LoadIconW(ProcessModule, MAKEINTRESOURCEW(IDI_APP_ICON1));
	#ifdef NDEBUG
	lstrcpyW(NotifyIcon.szTip, L"RainbowCaption");
	#else
	lstrcpyW(NotifyIcon.szTip, L"RainbowCaption DEBUG BUILD");
	#endif
	Shell_NotifyIconW(NIM_ADD, &NotifyIcon);

	HMENU NotifyIconMenu = CreatePopupMenu();
	InsertMenuW(NotifyIconMenu, MAXUINT, MF_BYPOSITION | MF_STRING | MF_GRAYED, 0, L"RainbowCaption");
	InsertMenuW(NotifyIconMenu, MAXUINT, MF_BYPOSITION | MF_STRING | MF_SEPARATOR, 0, NULL);
	InsertMenuW(NotifyIconMenu, MAXUINT, MF_BYPOSITION | MF_STRING, NID_SETTINGS, L"Settings");
	InsertMenuW(NotifyIconMenu, MAXUINT, MF_BYPOSITION | MF_STRING, NID_ABOUT, L"About");
	InsertMenuW(NotifyIconMenu, MAXUINT, MF_BYPOSITION | MF_STRING, NID_EXIT, L"Exit");
	SetWindowLongPtrW(NotifyWindow, GWLP_USERDATA, (LONG_PTR)NotifyIconMenu);

	HWND restrict SettingsWnd = NULL;
	HWND restrict AboutWnd = NULL;

	MSG Message;
	while(GetMessageW(&Message, NULL, 0, 0)) {
		switch(Message.message) {
			case WM_OPENSETTINGS: {
				if(SettingsWnd) SetForegroundWindow(SettingsWnd);
				else SettingsWnd = CreateDialogParamW(ProcessModule, MAKEINTRESOURCEW(IDD_SETTINGS), NULL, SettingsDialogProc, (LPARAM)ProcessModule);
				break;
			}
			case WM_CLOSESETTINGS: {
				DestroyWindow(SettingsWnd);
				SettingsWnd = NULL;
				SetThreadPriority(CyclerThread, CyclePriority);
				SaveConfig();
				break;
			}
			case WM_OPENABOUT: {
				if(AboutWnd) SetForegroundWindow(AboutWnd);
				else AboutWnd = CreateDialogParamW(ProcessModule, MAKEINTRESOURCEW(IDD_ABOUT), NULL, AboutDialogProc, (LPARAM)ProcessModule);
				break;
			}
			case WM_CLOSEABOUT: {
				DestroyWindow(AboutWnd);
				AboutWnd = NULL;
				break;
			}
			default: {
				TranslateMessage(&Message);
				DispatchMessageW(&Message);
			}
		}
	}

	PostThreadMessageW(GetThreadId(CyclerThread), WM_QUIT, 0, 0);
	Shell_NotifyIconW(NIM_DELETE, &NotifyIcon);
	DestroyWindow(NotifyWindow);
	DestroyMenu(NotifyIconMenu);
	if(SettingsWnd) DestroyWindow(SettingsWnd);
	if(AboutWnd) DestroyWindow(AboutWnd);
	WaitForSingleObject(CyclerThread, 200);
	CloseHandle(CyclerThread);
	ExitProcess(0);
}

static void LoadConfig(void) {
	HKEY restrict CfgKey;
	if(RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Tom Arnesen\\RainbowCaption", 0, KEY_QUERY_VALUE, &CfgKey) == ERROR_SUCCESS) {
		DWORD Size = sizeof(DWORD);
		RegQueryValueExW(CfgKey, L"Delay", NULL, NULL, (BYTE*)&CycleDelay, &Size);
		Size = sizeof(int);
		RegQueryValueExW(CfgKey, L"Priority", NULL, NULL, (BYTE*)&CyclePriority, &Size);
		RegCloseKey(CfgKey);
	}
	return;
}
static void SaveConfig(void) {
	HKEY restrict CfgKey;
	RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Tom Arnesen\\RainbowCaption", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &CfgKey, NULL);
	RegSetValueExW(CfgKey, L"Delay", 0, REG_DWORD, (BYTE*)&CycleDelay, sizeof(DWORD));
	RegSetValueExW(CfgKey, L"Priority", 0, REG_DWORD, (BYTE*)&CyclePriority, sizeof(int));
	RegCloseKey(CfgKey);
	return;
}