#pragma once
#include "xuielement.h"

#define XUI_PICTURE_CLASS		TEXT("XUI_PICTURE_CLASS")

class CXUIPicture : public CXUIElement
{
	LPWSTR	m_picture;
	int		m_picWidth;
	int		m_picHeight;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint(HDC hdc);
public:
	CXUIPicture(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIPicture(void);

	IMPLEMENT_INTERFACE(L"picture")

	void Init();
	BOOL loadDATA(IXMLDOMNode* node);
};
