#pragma once
#include "xuielement.h"

#define XUI_URL_CLASS	TEXT("XUI_URL_CLASS")

#define XUI_URL_TEXT_ALIGN_LEFT		0
#define XUI_URL_TEXT_ALIGN_RIGHT	1

class CXUIUrl : public CXUIElement
{
	LPTSTR	m_text;
	LPTSTR	m_url;
	UINT	m_textalign;
public:
	CXUIUrl(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIUrl(void);

	IMPLEMENT_INTERFACE(L"url")

	void Init();
	BOOL loadDATA(IXMLDOMNode* node);
	void setData(LPCWSTR text, LPCWSTR url);
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnCreate(void);
	void OnLButtonDown(int x, int y);
	void OnPaint(HDC hdc);
	void OnMouseMove(int x, int y);
	void OnMouseLeave(void);
//data
	HFONT	m_TextFont;
	HFONT	m_OverFont;
	BOOL	m_trackLeave;
	RECT	m_rectLink;

	BOOL GetNonClientMetrics(NONCLIENTMETRICS* ncm);
	void wlTrackMouseLeave(HWND hWnd, int remove);
	static VOID CALLBACK wlTrackMouseTimerProc(HWND hWnd,UINT uMsg,UINT idEvent,DWORD dwTime);
};
