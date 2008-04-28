#pragma once
#include "xuielement.h"

class CXUIRadio : public CXUIElement
{
	enum VALIGN
	{
		vaDefault,
		vaTop,
		vaBottom,
		vaCenter
	};
	enum HALIGN
	{
		haDefault,
		haLeft,
		haRight,
		haCenter
	};

	LPWSTR	m_label;
	BOOL	m_selected;
	BOOL	m_bGroup;
	BOOL	m_bMultiline;
	int		m_value;
	VALIGN	m_vAlign;
	HALIGN	m_hAlign;
public:
	CXUIRadio(CXUIElement* parent, CXUIEngine* engine);
	~CXUIRadio(void);

	IMPLEMENT_INTERFACE(L"radio")

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void Init();
	virtual BOOL onCommnd(UINT code, UINT id, HWND hWnd);

	void set_group(BOOL group) { m_bGroup = group; }
	int  get_value() { return m_value; }

	virtual INT		value_INT();
	virtual void	value_INT(INT val);
	virtual void	recalcHeight(int width);
};
