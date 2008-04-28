#pragma once
#include "xuielement.h"

#define XUI_COLORPICKER_CLASS	L"XUI_COLORPICKER_CLASS"
#define XUI_COLORBUTTON_CLASS	L"XUI_COLORBUTTON_CLASS"

class CXUIColorPicker;

class CXUIColor : public CXUIElement
{
	COLORREF			m_color;
	CXUIColorPicker*	m_picker;
	BOOL				m_enableNone;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint(void);
	void OnClick(void);
	void SelectColor(void);
public:
	CXUIColor(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIColor(void);

	IMPLEMENT_INTERFACE(L"color")

	void Init();
	BOOL loadDATA(IXMLDOMNode* node);

	virtual INT		value_INT();
	virtual void	value_INT(INT val);
};
