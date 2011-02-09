#pragma once
#include "xuielement.h"

#define XUI_FREEDRAW_CLASS	L"XUI_FREEDRAW_CLASS"

class CXUIFreeDraw : public CXUIElement
{
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint();
	BOOL m_bDoubleBuffered;
public:
	CXUIFreeDraw(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIFreeDraw(void);

	IMPLEMENT_INTERFACE(L"freedraw")

	virtual void Init();
	virtual BOOL loadDATA(IXMLDOMNode* node);

	void redraw(BOOL eraseBg);
};
