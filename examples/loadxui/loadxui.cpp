// xuitest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "loadxui.h"
#include "xuilib.h"
#include <commctrl.h>
#include <GdiPlus.h>

#define FREE_CLEAR_STR(str) if(str) delete str; str = NULL;
#define MAKE_STR(str, cpstr) FREE_CLEAR_STR(str); if(cpstr) { str = new TCHAR[lstrlen(cpstr) + 1]; lstrcpy(str, cpstr); }

WCHAR imagesPath[MAX_PATH];

class MyXUIEngine : public CXUIEngine
{
public:
	MyXUIEngine(HINSTANCE hInst);
	~MyXUIEngine();
	void DrawImage(HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl);
};

MyXUIEngine::MyXUIEngine(HINSTANCE hInst) : CXUIEngine(hInst)
{
	
}

MyXUIEngine::~MyXUIEngine()
{

}

void MyXUIEngine::DrawImage(HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl)
{
	WCHAR imgPath[MAX_PATH];
	lstrcpy(imgPath, imagesPath);
	lstrcat(imgPath, imgUrl);

	Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(imgPath, FALSE);
	if(bmp)
	{
		Gdiplus::Graphics gdi(hdc);
		gdi.DrawImage(bmp, (Gdiplus::REAL) x, (Gdiplus::REAL) y, (Gdiplus::REAL) width, (Gdiplus::REAL) height);
		delete bmp;
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CoInitialize(NULL);

	InitCommonControls();
	LoadLibrary(L"Riched20.dll");

	ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	lstrcpy(imagesPath, lpCmdLine);
	PathRemoveFileSpec(imagesPath);
	PathAddBackslash(imagesPath);
	lstrcat(imagesPath, L"images\\");
	
	MyXUIEngine engine(hInstance);

	CXUIDialog dlg(lpCmdLine, &engine);

	dlg.DoModal(NULL);

	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return 0;
}

