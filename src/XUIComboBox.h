#pragma once
#include "xuielement.h"

#define XUI_COMBO_OWNERDRAW_NO			0
#define XUI_COMBO_OWNERDRAW_FIXED		1
#define XUI_COMBO_OWNERDRAW_VARIABLE	2

class CXUIComboBox : public CXUIElement
{
	BOOL	m_editable;
	BOOL	m_sorted;
	int		m_size;
	UINT	m_ownerDraw;
	BOOL	m_bHasStrings;
	LPWSTR	m_string;
public:
	CXUIComboBox(CXUIElement* parent, CXUIEngine* engine);
	~CXUIComboBox(void);
	IMPLEMENT_INTERFACE(L"combobox")

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void Init();
	virtual void render(int x, int y, int width, int height);
	virtual BOOL onCommnd(UINT code, UINT id, HWND hWnd);


	void clearItems();
	void addItem(int id, LPCWSTR str);

	virtual INT		value_INT();
	virtual void	value_INT(INT val);
	virtual LPCWSTR	value_STR();
	virtual void	value_STR(LPCWSTR val);
};
