#pragma once
#include "xuielement.h"

#define XUI_FREEDRAW_CLASS	L"XUI_FREEDRAW_CLASS"

class CXUIFreeDraw : public CXUIElement
{
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint();
public:
	CXUIFreeDraw(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIFreeDraw(void);

	IMPLEMENT_INTERFACE(L"freedraw")

	void Init();

	void redraw(BOOL eraseBg);
};
