#include "globals.h"
#include <commctrl.h>
#include "MainDialog.h"
#include <gdiplus.h>
using namespace Gdiplus;


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CoInitialize(NULL);
	InitCommonControls();

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CXUIEngine		engine(hInstance);
	CMainDialog		dlg(&engine);

	dlg.DoModal(NULL);

	GdiplusShutdown(gdiplusToken);
	return 0;
}

