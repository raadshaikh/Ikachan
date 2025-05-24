#include "Dialog.h"
#include "System.h"
#include <stdio.h>

//Options
struct OPTION
{
	int garbage; //unused and not written to
	int wnd_size;
};

LPCTSTR gOptionName = "Option.bin";

BOOL SaveOption(OPTION *option)
{
	//Open option file
	TCHAR path[MAX_PATH];
	sprintf(path, "%s\\%s", gModulePath, gOptionName);

	FILE *fp = fopen(path, "wb");
	if (fp == NULL)
		return FALSE;

	//Write options
	fwrite(option, sizeof(OPTION), 1, fp);
	fclose(fp);
	return TRUE;
}

BOOL LoadOption(OPTION *option)
{
	//Clear option struct
	memset(option, 0, sizeof(OPTION));

	//Open option file
	TCHAR path[MAX_PATH];
	sprintf(path, "%s\\%s", gModulePath, gOptionName);

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	//Read options
	fread(option, sizeof(OPTION), 1, fp);
	fclose(fp);
	return TRUE;
}

//Dialog functions
void InitDialog(HWND hDlg)
{
	//Window size labels
	const LPCTSTR size_names[3] = {
		"Fullscreen",
		"320x240 Windowed",
		"640x480 Windowed",
	};

	//Insert window sizes to size selector
	for (int i = 0; i < 3; i++)
		SendDlgItemMessage(hDlg, 1003, CB_ADDSTRING, 0, (LPARAM)size_names[i]);

	//Read options
	OPTION option;
	if (!LoadOption(&option))
	{
		//Default options
		option.wnd_size = WS_FULLSCREEN;
	}

	//Select options initially on dialog
	SendDlgItemMessage(hDlg, 1003, CB_SETCURSEL, (WPARAM)option.wnd_size, 0);
}

void UseDialog(HWND hDlg)
{
	//Set properties from dialog
	gWndSize = (WND_SIZE)SendDlgItemMessage(hDlg, 1003, CB_GETCURSEL, 2, 0);

	//Write options
	OPTION option;
	option.wnd_size = (int)gWndSize;
	SaveOption(&option);
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_INITDIALOG:
			InitDialog(hDlg);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case 2:
					EndDialog(hDlg, 0);
					break;
				case 1:
					UseDialog(hDlg);
					EndDialog(hDlg, 1);
					break;
			}
		default:
			return FALSE;
	}
	return TRUE;
}
