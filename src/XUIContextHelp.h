#pragma once
#include "html/htmlviewer.h"

#define XUI_CONTEXTHELP_CLASS		L"XUI_CONTEXTHELP_CLASS"

struct CONTEXT_HELP_ITEM
{
	LPWSTR	control;
	LPWSTR	text;
	LPWSTR	img;
	int		imgWidth;
	int		imgHeight;
};

class CXUIContextHelp : public CHTMLProgress
{
	CXUIEngine*			m_engine;
	CONTEXT_HELP_ITEM*	m_items;
	int					m_count;
	int					m_curItem;
	HWND				m_hWnd;

	CHTMLViewer			m_html;
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint(HDC hdc);
	void GetDesktopRect(RECT* rcDsk, HWND hWnd);
public:
	CXUIContextHelp(CXUIEngine* engine);
	virtual ~CXUIContextHelp(void);

	void Clear();
	void loadDATA(IXMLDOMNode* node);
	void show(LPCWSTR control, HWND hWndCtl);

	// CHTMLProgress
	virtual void LinkClicked(TCHAR *szLink);
	virtual void SetCursor(BOOL bLink,TCHAR *szLink);
	virtual void SetTitle(TCHAR *szTitle);
};
