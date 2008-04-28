#pragma once
#include "xuielement.h"

#define XUI_TAB_SWITCHER		L"XUI_TAB_SWITCHER_WINDOW"
#define XUI_TAB_CAPTION			L"XUI_TAB_CAPTION_WINDOW"

#define XUI_TAB_IMAGE_ALIGN_LEFT	0
#define XUI_TAB_IMAGE_ALIGN_TOP		1

#define TIMER_LEAVE		100

class CXUITab;

class CXUITabSwitcher :	public CXUIElement
{
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcCaption(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	HWND	m_hWndCaption;
	HFONT	m_hCaptionFont;
	
	BOOL	m_mouseTrackInstalled;

	void trackMouseLeave(BOOL bInstall);
	void DrawGradient(HDC dc, const RECT& rectClient, COLORREF m_clrStart, COLORREF m_clrEnd);
	BOOL GetNonClientMetrics(NONCLIENTMETRICS* ncm);

	void OnPaint(HDC hdc, LPRECT rcDraw);
	void OnPaintCaption(HDC hdc, LPRECT rcDraw);
	void OnMouseMove(int x, int y);
	void OnMouseLeave();
	void OnLButtonDown(int x, int y);
	
	CXUITab* findTab(int x, int y);
	CXUITab* findSelectedTab();
	CXUITab* findHoverTab();

	UINT	m_imageAlign;
	int		m_imageSize;
	int		m_defTabHeight;
	int		m_ctlsMarginX;
	int		m_ctlsMarginY;
	int		m_captionHeight;
	BOOL	m_bShowSwitcher;
	BOOL	m_bShowCaption;
public:
	CXUITabSwitcher(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUITabSwitcher(void);

	IMPLEMENT_INTERFACE(L"tabswitcher")

	void Init();
	void render(int x, int y, int width, int height);
	void getMinSize(SIZE& minSize);
	BOOL loadDATA(IXMLDOMNode* node);
	HWND get_parentWnd();
	void value_INT(INT val);
	INT  value_INT();
	LPCWSTR value_STR();

	void recalcLayout();
	virtual void onCmd(LPWSTR cmd);
	void reinitTabs();
};
