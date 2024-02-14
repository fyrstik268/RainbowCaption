#include <RainbowCaption.h>

static INT_PTR CALLBACK LicenseDialogProc(HWND Dialog, UINT Message, WPARAM WParam, LPARAM LParam);

LRESULT CALLBACK NotifyIconProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
	switch(Message) {
		case WM_SHLICON: {
			if(LOWORD(LParam) != WM_LBUTTONDOWN && LOWORD(LParam) != WM_RBUTTONDOWN) return 0;
			POINT CursorPos;
			GetCursorPos(&CursorPos);
			SetForegroundWindow(Window);
			const BOOL MenuItem = TrackPopupMenu((HMENU)GetWindowLongPtrW(Window, GWLP_USERDATA), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_HORPOSANIMATION | TPM_VERPOSANIMATION,
												 CursorPos.x, CursorPos.y, 0, Window, NULL);
			switch(MenuItem) {
				case NID_SETTINGS: {
					PostMessageW(NULL, WM_OPENSETTINGS, 0, 0);
					return 0;
				}
				case NID_ABOUT: {
					PostMessageW(NULL, WM_OPENABOUT, 0, 0);
					return 0;
				}
				case NID_EXIT: {
					PostQuitMessage(0);
					return 0;
				}
			}
		}
		default: {
			return DefWindowProcW(Window, Message, WParam, LParam);
		}
	}
}
#pragma warning(suppress: 4100)
INT_PTR CALLBACK SettingsDialogProc(HWND Dialog, UINT Message, WPARAM WParam, LPARAM LParam) {
	switch(Message) {
		case WM_INITDIALOG: {
			HICON const restrict Icon = LoadIconW(ProcessModule, MAKEINTRESOURCEW(IDI_APP_ICON1));
			SendMessageW(Dialog, WM_SETICON, ICON_BIG, (LPARAM)Icon);
			SendMessageW(Dialog, WM_SETICON, ICON_SMALL, (LPARAM)Icon);
			SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_ADDSTRING, 0, (LPARAM)L"Idle");
			SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_ADDSTRING, 0, (LPARAM)L"Lowest");
			SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_ADDSTRING, 0, (LPARAM)L"Below Normal");
			SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_ADDSTRING, 0, (LPARAM)L"Normal");
			SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_ADDSTRING, 0, (LPARAM)L"Above Normal");
			SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_ADDSTRING, 0, (LPARAM)L"Highest");
			SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_ADDSTRING, 0, (LPARAM)L"RealTime");
			SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_SETCURSEL, (CyclePriority < 0 ? CyclePriority | 0b1100 : CyclePriority & ~0b1100) + 3, 0);
			SendDlgItemMessageW(Dialog, IDC_SPIN_DELAY, UDM_SETRANGE32, 0, MAXINT);
			SetDlgItemInt(Dialog, IDC_EDIT_DELAY, CycleDelay, FALSE);
			return TRUE;
		}
		case WM_COMMAND: {
			switch(LOWORD(WParam)) {
				case IDC_RESET: {
					CycleDelay = 50;
					CyclePriority = THREAD_PRIORITY_IDLE;
					PostThreadMessageW(GetCurrentThreadId(), WM_CLOSESETTINGS, 0, 0);
					__fallthrough;
				}
				default: {
					return TRUE;
				}
				case IDOK:
				__fallthrough;
			}
			__fallthrough;
		}
		case WM_CLOSE: {
			CycleDelay = GetDlgItemInt(Dialog, IDC_EDIT_DELAY, NULL, FALSE);
			CyclePriority = (SendDlgItemMessageW(Dialog, IDC_PRIORITY_SEL, CB_GETCURSEL, 0, 0) - 3) & ~0b1100;
			PostThreadMessageW(GetCurrentThreadId(), WM_CLOSESETTINGS, 0, 0);
			return TRUE;
		}
		default: {
			return FALSE;
		}
	}
}
INT_PTR CALLBACK AboutDialogProc(HWND Dialog, UINT Message, WPARAM WParam, LPARAM LParam) {
	switch(Message) {
		case WM_INITDIALOG: {
			HICON const restrict Icon = LoadIconW(ProcessModule, MAKEINTRESOURCEW(IDI_APP_ICON1));
			SendMessageW(Dialog, WM_SETICON, ICON_BIG, (LPARAM)Icon);
			SendMessageW(Dialog, WM_SETICON, ICON_SMALL, (LPARAM)Icon);
			return TRUE;
		}
		case WM_COMMAND: {
			switch(LOWORD(WParam)) {
				case IDC_LICENSE: {
					CreateDialogParamW(ProcessModule, MAKEINTRESOURCEW(IDD_LICENSE), Dialog, LicenseDialogProc, 0);
					__fallthrough;
				}
				default: {
					return TRUE;
				}
				case IDOK:
				__fallthrough;
			}
			__fallthrough;
		}
		case WM_CLOSE: {
			PostThreadMessageW(GetCurrentThreadId(), WM_CLOSEABOUT, 0, 0);
			return TRUE;
		}
		case WM_NOTIFY: {
			if(((NMHDR*)LParam)->code == NM_CLICK && ((NMHDR*)LParam)->idFrom == IDC_GITHUB_LINK) ShellExecuteW(NULL, L"open", L"https://github.com/PersonMedBrukernavn/RainbowCaption", NULL, NULL, SW_SHOWNORMAL);
			return TRUE;
		}
		default: {
			return FALSE;
		}
	}
}

static const WCHAR LicenseText[] = L"MIT License\r\n\
\r\n\
Copyright(c) 2024 Tom Arnesen\r\n\
\r\n\
Permission is hereby granted, free of charge, to any person obtaining a copy \
of this software and associated documentation files (the \"Software\"), to deal \
in the Software without restriction, including without limitation the rights \
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell \
copies of the Software, and to permit persons to whom the Software is \
furnished to do so, subject to the following conditions:\r\n\
\r\n\
The above copyright notice and this permission notice shall be included in all \
copies or substantial portions of the Software.\r\n\
\r\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE \
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, \
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE \
SOFTWARE.";
#pragma warning(suppress: 4100)
static INT_PTR CALLBACK LicenseDialogProc(HWND Dialog, UINT Message, WPARAM WParam, LPARAM LParam) {
	switch(Message) {
		case WM_INITDIALOG: {
			HICON const restrict Icon = LoadIconW(ProcessModule, MAKEINTRESOURCEW(IDI_APP_ICON1));
			SendMessageW(Dialog, WM_SETICON, ICON_BIG, (LPARAM)Icon);
			SendMessageW(Dialog, WM_SETICON, ICON_SMALL, (LPARAM)Icon);
			SetDlgItemTextW(Dialog, IDC_LICENSE, LicenseText);
			return TRUE;
		}
		case WM_COMMAND: {
			if(LOWORD(WParam) != IDOK) return TRUE;
			__fallthrough;
		}
		case WM_CLOSE: {
			EndDialog(Dialog, 0);
			return TRUE;
		}
		default: {
			return FALSE;
		}
	}
}