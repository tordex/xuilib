#pragma once
#include "xuielement.h"

class CXUITabBox : public CXUIElement
{
public:
	CXUITabBox(CXUIElement* parent, CXUIEngine* engine);
	~CXUITabBox(void);
	IMPLEMENT_INTERFACE(L"tabbox")

	void selectTab(int tabID);
	BOOL onNotify(int idCtrl, LPNMHDR pnmh);

	void Init();
	HWND get_parentWnd();
};
