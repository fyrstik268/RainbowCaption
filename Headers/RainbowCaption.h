#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include <Resources.h>

enum {
	WM_SHLICON = WM_APP,
	WM_OPENSETTINGS,
	WM_OPENABOUT,
	WM_CYCLECOLOUR,
	WM_CLOSESETTINGS,
	WM_CLOSEABOUT
};

enum {
	NID_SETTINGS = 1,
	NID_ABOUT,
	NID_EXIT
};

extern HMODULE restrict ProcessModule;
extern DWORD CycleDelay;
extern int CyclePriority;

DWORD WINAPI Cycler(void* Unused);
LRESULT CALLBACK NotifyIconProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);
INT_PTR CALLBACK SettingsDialogProc(HWND Dialog, UINT Message, WPARAM WParam, LPARAM LParam);
INT_PTR CALLBACK AboutDialogProc(HWND Dialog, UINT Message, WPARAM WParam, LPARAM LParam);