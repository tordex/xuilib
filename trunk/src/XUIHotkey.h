#pragma once
#include "xuielement.h"

#define XUI_HOTKEY_CLASS		TEXT("XUI_HOTKEY_CLASS")

class CXUIHotkey : public CXUIElement
{
	TCHAR	m_text[500];
	WORD	m_hotKey;
	LPWSTR	m_clrBg;
	LPWSTR	m_clrIcon;
	int		m_btnState;
	BOOL	m_trackInstalled;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint(void);
	void FillString(void);
	void UpdateCarret(void);
public:
	CXUIHotkey(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIHotkey(void);

	IMPLEMENT_INTERFACE(L"hotkey")

	virtual void Init();
	virtual BOOL loadDATA(IXMLDOMNode* node);

	virtual INT		value_INT();
	virtual void	value_INT(INT val);
};
