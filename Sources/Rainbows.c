#include <RainbowCaption.h>

static void StepColours(void);
static BOOL CALLBACK SetCaptionColour(HWND Window, LPARAM Unused);

#pragma warning(suppress: 4100)
DWORD WINAPI Cycler(void* Unused) {
	MSG Message = {0};
	while(Message.message != WM_QUIT) {
		PeekMessageW(&Message, NULL, 0, 0, PM_REMOVE);
		StepColours();
		Sleep(CycleDelay);
	}
	ExitThread(0);
}

static union {
	COLORREF Colour;
	BYTE RGBX[4];
} Colour = {RGB(250, 0, 0)};
#pragma warning(suppress: 4100)
static BOOL CALLBACK SetCaptionColour(HWND Window, LPARAM Unused) {
	HWND const restrict ForegroundWindow = GetForegroundWindow();
	const COLORREF StoredColour = Colour.Colour;
	if(ForegroundWindow != Window) {
		/* Good Enough. */
		if(Colour.RGBX[0] > 105) Colour.RGBX[0] = 255;
		else Colour.RGBX[0] += 150;
		if(Colour.RGBX[1] > 105) Colour.RGBX[1] = 255;
		else Colour.RGBX[1] += 150;
		if(Colour.RGBX[2] > 105) Colour.RGBX[2] = 255;
		else Colour.RGBX[2] += 150;
	}
	DwmSetWindowAttribute(Window, DWMWA_CAPTION_COLOR, &Colour.Colour, sizeof(Colour.Colour)); /* DWM's complaining a bit in the debug console but everything seems to be OK */
	Colour.Colour = StoredColour;
	return TRUE;
}
static void StepColours(void) {
	static BYTE Stage;
	switch(Stage) {
		case 0: {
			Colour.RGBX[1] += 10;
			if(Colour.RGBX[1] == 250) Stage++;
			break;
		}
		case 1: {
			Colour.RGBX[0] -= 10;
			if(Colour.RGBX[0] == 0) Stage++;
			break;
		}
		case 2: {
			Colour.RGBX[2] += 10;
			if(Colour.RGBX[2] == 250) Stage++;
			break;
		}
		case 3: {
			Colour.RGBX[1] -= 10;
			if(Colour.RGBX[1] == 0) Stage++;
			break;
		}
		case 4: {
			Colour.RGBX[0] += 10;
			if(Colour.RGBX[0] == 250) Stage++;
			break;
		}
		case 5: {
			Colour.RGBX[2] -= 10;
			if(Colour.RGBX[2] == 0) Stage = 0;
			break;
		}
	}
	EnumWindows(SetCaptionColour, 0);
	return;
}