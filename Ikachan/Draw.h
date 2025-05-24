#pragma once
#include <windows.h>
#include "System.h"

#define SURFACE_WIDTH  (320)
#define SURFACE_HEIGHT (240)

enum SURFACE_ID
{
	SURFACE_ID_STATUS = 0,
	SURFACE_ID_MYCHAR = 2,
	SURFACE_ID_FIGURE = 4,
	SURFACE_ID_TEXT0 = 6,
	SURFACE_ID_TEXT1 = 7,
	SURFACE_ID_MSGBOX = 8,
	SURFACE_ID_ITEMBOX = 9,
	SURFACE_ID_ITEM = 10,
	SURFACE_ID_BACK = 11,
	SURFACE_ID_LOADING = 12,
	SURFACE_ID_NPCTYPE = 13,
	SURFACE_ID_CURSOR = 14,
	SURFACE_ID_EDITOR = 15,
	SURFACE_ID_YESNO = 16,
	SURFACE_ID_OPENING = 17,
	SURFACE_ID_FADE = 18,
	SURFACE_ID_BACKUP = 19,
	SURFACE_ID_MARUAME = 20,
	SURFACE_ID_MYCHAR3 = 21,
	SURFACE_ID_MYCHAR2 = 22,
	SURFACE_ID_END = 23,
	SURFACE_ID_STAFF = 24,
	SURFACE_ID_LOADING2 = 25,
	SURFACE_ID_WORDS0 = 40,
	SURFACE_ID_WORDS1 = 41,
	SURFACE_ID_WORDS2 = 42,
	SURFACE_ID_WORDS3 = 43,
	SURFACE_ID_WORDS4 = 44,
	SURFACE_ID_WORDS5 = 45,
	SURFACE_ID_WORDS6 = 46,
	SURFACE_ID_WORDS7 = 47,
	SURFACE_ID_WORDS8 = 48,
	SURFACE_ID_WORDS9 = 49,
	SURFACE_ID_WORDS10 = 50,
	SURFACE_ID_STAR = 60,
	SURFACE_ID_BUBBLE = 61,
	SURFACE_ID_DAMAGE = 62,
	SURFACE_ID_LEVELUP = 63,
	SURFACE_ID_PRTBACK = 70,
	SURFACE_ID_PRTFILT = 71,
	SURFACE_ID_PRTITEM = 72,
	SURFACE_ID_PRTDIR = 73,
	SURFACE_ID_PRTBLOCK = 74,
	SURFACE_ID_PRTDMG = 75,
	SURFACE_ID_PRTSNACK = 77,
	SURFACE_ID_HARI = 80,
	SURFACE_ID_ISOGIN = 81,
	SURFACE_ID_KANI = 82,
	SURFACE_ID_SLEEP = 83,
	SURFACE_ID_CHIBI = 84,
	SURFACE_ID_HOSHI = 85,
	SURFACE_ID_DUM = 86,
	SURFACE_ID_CARRY = 87,
	SURFACE_ID_JUEL = 88,
	SURFACE_ID_UFO = 89,
	SURFACE_ID_IRONHEAD = 100,
};

extern RECT grcFull;

void SetClientOffset(int width, int height);
BOOL Flip_SystemTask(HWND hWnd);
BOOL StartDirectDraw(HWND hWnd, int wndSize);
void EndDirectDraw(HWND hWnd);
BOOL MakeSurface_File(LPCTSTR name, int surf_no);
BOOL MakeSurface_Generic(int bxsize, int bysize, int surf_no);
void BackupSurface(int surf_no, const RECT *rect);
void PutBitmap3(const RECT *rcView, int x, int y, const RECT *rect, int surf_no);
void CortBox(const RECT *rect, DWORD col);
void CortBox2(const RECT *rect, DWORD col, int surf_no);
void InitTextObject(LPCTSTR name);
void PutText(int x, int y, LPCTSTR text, DWORD color, BOOL unk);
void PutText2(int x, int y, LPCTSTR text, DWORD color, int surf_no, BOOL unk);
void PutText3(int x, int y, LPCTSTR text, DWORD color, int surf_no, BOOL unk);
void PutText4(int x, int y, int u, DWORD color, int surf_no, BOOL unk);
void EndTextObject();
