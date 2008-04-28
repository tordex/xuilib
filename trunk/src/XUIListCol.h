#pragma once
#include "xuielement.h"

class CXUIListCol : public CXUIElement
{
	LPWSTR	m_caption;
	int		m_size;
	int		m_flex;
public:
	CXUIListCol(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIListCol(void);

	IMPLEMENT_INTERFACE(L"listcol")

	BOOL loadDATA(IXMLDOMNode* node);

	int		get_size()		{ return m_size;	}
	int		get_flex()		{ return m_flex;	}
	LPWSTR	get_caption()	{ return m_caption;	}
};
