#include "globals.h"
#include "MyXUIEngine.h"
#include <GdiPlus.h>

CMyXUIEngine::CMyXUIEngine(HINSTANCE hInst, LPCWSTR projectFile) : CXUIEngine(hInst)
{
	m_imagesPath = new WCHAR[lstrlen(projectFile) + lstrlen(L"images\\")];
	lstrcpy(m_imagesPath, projectFile);
	PathRemoveFileSpec(m_imagesPath);
	PathAddBackslash(m_imagesPath);
	lstrcat(m_imagesPath, L"images\\");
}

CMyXUIEngine::~CMyXUIEngine(void)
{
	FREE_CLEAR_STR(m_imagesPath);
}

void CMyXUIEngine::DrawImage( HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl )
{
	if(!imgUrl) return;
	LPWSTR imgFile = new WCHAR[lstrlen(m_imagesPath) + lstrlen(imgUrl) + 1];
	lstrcpy(imgFile, m_imagesPath);
	lstrcat(imgFile, imgUrl);
	if(PathFileExists(imgFile))
	{
		Gdiplus::Graphics	gdi(hdc);
		Gdiplus::Image		img(imgFile);
		gdi.DrawImage(&img, (Gdiplus::REAL) x, (Gdiplus::REAL) y, (Gdiplus::REAL) width, (Gdiplus::REAL) height);
	}
}

BOOL CMyXUIEngine::isViewer()
{
	return TRUE;
}
