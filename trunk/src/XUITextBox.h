#pragma once
#include "xuielement.h"

#define XUI_TEXTBOX_ALIGN_LEFT		0
#define XUI_TEXTBOX_ALIGN_RIGHT		1
#define XUI_TEXTBOX_ALIGN_CENTER	3

class CXUITextBox :	public CXUIElement
{
	LPWSTR	m_value;
	BOOL	m_readonly;
	BOOL	m_multiline;
	int		m_maxlength;
	int		m_rows;
	int		m_cols;
	int		m_size;
	BOOL	m_numbers;
	BOOL	m_password;
	int		m_textAlign;

	BOOL	m_bLockEvents;
public:
	CXUITextBox(CXUIElement* parent, CXUIEngine* engine);
	~CXUITextBox(void);
	IMPLEMENT_INTERFACE(L"textbox")

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void Init();
	virtual HWND get_parentWnd();
	virtual BOOL onCommnd(UINT code, UINT id, HWND hWnd);

	virtual INT		value_INT();
	virtual void	value_INT(INT val);
	virtual LPCWSTR	value_STR();
	virtual void	value_STR(LPCWSTR val);
};
