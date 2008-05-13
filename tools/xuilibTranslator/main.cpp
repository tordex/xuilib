#include "globals.h"
#include <commctrl.h>
#include "MainDialog.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CoInitialize(NULL);
	InitCommonControls();

	CXUIEngine		engine(hInstance);
	CMainDialog		dlg(&engine);

	dlg.DoModal(NULL);

	return 0;
}

