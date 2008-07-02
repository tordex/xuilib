#pragma once
#include "xuielement.h"

#define XUI_PICTURE_CLASS		TEXT("XUI_PICTURE_CLASS")

#define XUI_FRAMES_VALIGN		0
#define XUI_FRAMES_HALIGN		1

class CXUIPicture : public CXUIElement
{
	LPWSTR	m_picture;
	int		m_picWidth;
	int		m_picHeight;
	int		m_frames;
	int		m_animInterval;
	int		m_framesAlign;

	int		m_curFrame;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint(HDC hdc);
public:
	CXUIPicture(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIPicture(void);

	IMPLEMENT_INTERFACE(L"picture")

	void Init();
	BOOL loadDATA(IXMLDOMNode* node);
};
