#pragma once
#include "xuielement.h"

#define XUI_LB_SELECTION_SINGLE			0
#define XUI_LB_SELECTION_MULTIPLE		1
#define XUI_LB_SELECTION_EXTENDED		2
#define XUI_LB_SELECTION_NONE			3

class CXUIListBox : public CXUIElement
{
	BOOL	m_bMulticolumn;
	int		m_colWidth;
	UINT	m_selection;
	BOOL	m_bSort;
	BOOL	m_listRows;
	BOOL	m_bNoIntegralHeight;
public:
	CXUIListBox(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIListBox(void);

	IMPLEMENT_INTERFACE(L"listbox")

	void clearItems();
	void addItem(int id, LPCWSTR str);
	void setItemText(int id, LPCWSTR str);
	void delItem(int id);

	virtual void	Init();
	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual BOOL	onCommnd(UINT code, UINT id, HWND hWnd);
	virtual INT		value_INT();
	virtual void	value_INT(INT val);
};
