#include "System.h"
#include "Draw.h"
#include "Sound.h"
#include "Generic.h"
#include "PiyoPiyo.h"
#include "Dialog.h"
#include "Game.h"
#include <stdio.h>

//Keys
DWORD gKey;
DWORD gMouse;

//Window name
LPCTSTR lpCaption = "Ikachan";

//Windows objects
HWND ghWnd;
HACCEL hAccel;

//Window size
WND_SIZE gWndSize;
int gWndWidth, gWndHeight;

//Filepath
TCHAR gModulePath[MAX_PATH];

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int Random(int min, int max)
{
	const int range = max - min + 1;
	return (rand() % range) + min;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	LPCTSTR lpMutexName = "Ikachan";
	RECT unused_rect = { 0, 0, SURFACE_WIDTH, SURFACE_HEIGHT };

	//Create Ikachan mutex (prevent program from running multiple instances)
	HANDLE hObject, hMutex;
	if ((hObject = OpenMutex(MUTEX_ALL_ACCESS, FALSE, lpMutexName)) != NULL)
	{
		CloseHandle(hObject);
		return 1;
	}
	hMutex = CreateMutex(0, TRUE, lpMutexName);

	//Get module path
	size_t i;
	GetModuleFileName(NULL, gModulePath, MAX_PATH);
	for (i = strlen(gModulePath); gModulePath[i] != '\\'; i--);
	gModulePath[i] = 0;
	
	//Open start dialog
	if (DialogBoxParam(hInstance, "DLG_START", NULL, DlgProc, NULL))
	{
		//Define window class
		WNDCLASS wndClass;
		memset(&wndClass, 0, sizeof(WNDCLASS));
		wndClass.lpfnWndProc = WndProc;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = (HICON)LoadImage(hInstance, "0", IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		wndClass.hCursor = NULL;
		wndClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
		wndClass.lpszClassName = lpCaption;

		//Create window (full screen or windowed)
		HWND hWnd;
		HMENU hMenu;

		switch (gWndSize)
		{
			case WS_320x240:
			case WS_640x480:
			{
				//Register class
				wndClass.lpszMenuName = "MENU_MAIN";
				if (!RegisterClass(&wndClass))
				{
					//Release Ikachan mutex
					ReleaseMutex(hMutex);
					return 0;
				}

				//Get window dimensions
				if (gWndSize == WS_320x240)
				{
					gWndWidth = SURFACE_WIDTH;
					gWndHeight = SURFACE_HEIGHT;
				}
				else
				{
					gWndWidth = SURFACE_WIDTH * 2;
					gWndHeight = SURFACE_HEIGHT * 2;
				}

				//Get window size and position
				int nWidth, nHeight, X, Y;
				nWidth = (GetSystemMetrics(SM_CXFIXEDFRAME) * 2) + gWndWidth + 2;
				nHeight = (GetSystemMetrics(SM_CYFIXEDFRAME) * 2) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) + gWndHeight + 2;
				X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
				Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;

				SetClientOffset(GetSystemMetrics(SM_CXFIXEDFRAME) + 1, GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) + 1);

				//Create window
				hWnd = CreateWindowEx(NULL, lpCaption, lpCaption, WS_VISIBLE | WS_BORDER | WS_DLGFRAME | WS_SYSMENU | WS_GROUP, X, Y, nWidth, nHeight, NULL, NULL, hInstance, NULL);
				ghWnd = hWnd;
				if (hWnd == NULL)
				{
					//Release Ikachan mutex
					ReleaseMutex(hMutex);
					return 0;
				}

				//Get accel and menu
				hAccel = LoadAccelerators(hInstance, "ACCEL");
				hMenu = GetMenu(hWnd);

				//Start DirectDraw
				if (gWndSize == WS_320x240)
					StartDirectDraw(hWnd, WS_320x240);
				else
					StartDirectDraw(hWnd, WS_640x480);
				break;
			}
			case WS_FULLSCREEN:
			{
				//Register class
				if (!RegisterClass(&wndClass))
				{
					//Release Ikachan mutex
					ReleaseMutex(hMutex);
					return 0;
				}

				//Get window dimensions
				gWndWidth = SURFACE_WIDTH * 2;
				gWndHeight = SURFACE_HEIGHT * 2;

				//Get window area
				SetClientOffset(0, 0);

				//Create window
				hWnd = CreateWindowEx(NULL, lpCaption, lpCaption, WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
				ghWnd = hWnd;
				if (hWnd == NULL)
				{
					//Release Ikachan mutex
					ReleaseMutex(hMutex);
					return 0;
				}

				//Set cursor position
				StartDirectDraw(hWnd, WS_FULLSCREEN);
				SetCursorPos(gWndWidth / 2, gWndHeight / 2);

				//Start DirectDraw
				SetCursorPos(0, 0);
				break;
			}
		}

		//Load cursor
		HCURSOR hCursor = LoadCursor(hInstance, "PIXELCURSOR");
		SetCursor(hCursor);

		//Set rects
		RECT rcLoading = { 0, 0, 64, 8 };
		RECT rcFull = { 0, 0, 0, 0 };
		rcFull.right = SURFACE_WIDTH;
		rcFull.bottom = SURFACE_HEIGHT;

		//Load the "LOADING" text
		BOOL b = MakeSurface_File("Pbm\\Loading2.pbm", SURFACE_ID_LOADING2);

		//Draw loading screen
		CortBox(&rcFull, 0x000000);
		PutBitmap3(&rcFull, (SURFACE_WIDTH / 2) - 32, (SURFACE_HEIGHT / 2) - 4, &rcLoading, SURFACE_ID_LOADING2);
		if (!Flip_SystemTask(hWnd))
		{
			//Release Ikachan mutex
			ReleaseMutex(hMutex);
			return 1;
		}
		
		//Initialize DirectSound
		if (InitDirectSound(hWnd))
		{
			//Initialize game
			InitTextObject(NULL);
			InitPiyoPiyo();

			//Run game loop
			Game(hWnd);
			
			//End game
			EndPiyoPiyo();
			EndTextObject();

			//End DirectDraw and DirectSound
			EndDirectSound();
			EndDirectDraw(hWnd);

			//Release Ikachan mutex (twice?)
			ReleaseMutex(hMutex);
		}
		else
		{
			MessageBox(hWnd, "Audio initialisation failed", lpCaption, MB_OK);
		}
	}

	//Release Ikachan mutex
	ReleaseMutex(hMutex);
	return 1;
}

//Window process
BOOL bActive;

void InactiveWindow()
{
	bActive = FALSE;
}

void ActiveWindow()
{
	bActive = TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_CREATE:
			return FALSE;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case 40001:
					PostMessage(hWnd, WM_DESTROY, 0, 0);
					break;
				case 40002:
				{
					int year, month, day;
					int v1, v2, v3, v4;
					TCHAR text[0x40];

					GetCompileDate(&year, &month, &day);
					GetCompileVersion(&v1, &v2, &v3, &v4);
					sprintf(text, "version.%d.%d.%d\n1999/04/XX - %04d/%02d/%02d\nby Studio Pixel", v1, v2, v3, year, month, day);
					MessageBoxA(hWnd, text, lpCaption, MB_OK);
					break;
				}
				case 40003:
					if (!OpenSoundVolume(hWnd))
						MessageBox(hWnd, "Volume setting unavailable", lpCaption, MB_OK);
					break;
				case 40004:
					ShowWindow(hWnd, SW_MINIMIZE);
					break;
			}
			break;

		case WM_SYSCOMMAND:
			switch (wParam)
			{
				case SC_MONITORPOWER:
					break;
				case SC_KEYMENU:
					break;
				case SC_SCREENSAVE:
					break;
				default:
					DefWindowProc(hWnd, Msg, wParam, lParam);
					break;
			}
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				case VK_LEFT:
					gKey |= KEY_LEFT;
					break;
				case VK_RIGHT:
					gKey |= KEY_RIGHT;
					break;
				case VK_UP:
					gKey |= KEY_UP;
					break;
				case VK_DOWN:
					gKey |= KEY_DOWN;
					break;
				case VK_SPACE:
					gKey |= KEY_SPACE;
					break;
				case 'X':
					gKey |= KEY_X;
					break;
				case 'Z':
					gKey |= KEY_Z;
					break;
				case 'S':
					gKey |= KEY_S;
					break;
			}
			break;

		case WM_KEYUP:
			switch (wParam)
			{
				case VK_LEFT:
					gKey &= ~KEY_LEFT;
					break;
				case VK_RIGHT:
					gKey &= ~KEY_RIGHT;
					break;
				case VK_UP:
					gKey &= ~KEY_UP;
					break;
				case VK_DOWN:
					gKey &= ~KEY_DOWN;
					break;
				case VK_SPACE:
					gKey &= ~KEY_SPACE;
					break;
				case 'X':
					gKey &= ~KEY_X;
					break;
				case 'Z':
					gKey &= ~KEY_Z;
					break;
				case 'S':
					gKey &= ~KEY_S;
					break;
			}
			break;

		case WM_LBUTTONDOWN:
			gMouse |= MOUSE_LEFT;
			break;

		case WM_LBUTTONUP:
			gMouse &= ~MOUSE_LEFT;
			break;

		case WM_RBUTTONDOWN:
			gMouse |= MOUSE_RIGHT;
			break;

		case WM_RBUTTONUP:
			gMouse &= ~MOUSE_RIGHT;
			break;

		case WM_IME_NOTIFY:
			if (wParam == IMN_SETOPENSTATUS)
			{
				HIMC hImc = ImmGetContext(hWnd);
				ImmSetOpenStatus(hImc, 0);
				ImmReleaseContext(hWnd, hImc);
			}
			break;
			
	#ifndef FIX_BUGS
		case WM_SIZE: //WHAT HAPPENED HERE?
			switch (wParam)
			{
				case WA_INACTIVE:
					ActiveWindow();
					break;
				case WA_ACTIVE:
					InactiveWindow();
					break;
			}
			break;
	#else
		case WM_ACTIVATE:
			switch (LOWORD(wParam))
			{
				case WA_INACTIVE:
					InactiveWindow();
					break;
				case WA_ACTIVE:
					ActiveWindow();
					break;
			}
			break;
	#endif
			
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return TRUE;
}

//System task
BOOL SystemTask()
{
	MSG Msg;
	while (PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE) || bActive == FALSE)
	{
		if (!GetMessage(&Msg, NULL, 0, 0))
			return FALSE;
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return TRUE;
}
