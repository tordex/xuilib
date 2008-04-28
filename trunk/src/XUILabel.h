#pragma once
#include "xuielement.h"

#define XUI_LABEL_ALIGN_LEFT	0
#define XUI_LABEL_ALIGN_RIGHT	1
#define XUI_LABEL_ALIGN_CENTER	2

class CXUILabel : public CXUIElement
{
	LPWSTR	m_value;
	BOOL	m_bVCenter;
	BOOL	m_bMultiline;
	UINT	m_textAlign;
public:
	CXUILabel(CXUIElement* parent, CXUIEngine* engine);
	~CXUILabel(void);
	IMPLEMENT_INTERFACE(L"label")

	BOOL loadDATA(IXMLDOMNode* node);
	void Init();
	
	virtual LPCWSTR	value_STR();
	virtual void	value_STR(LPCWSTR val);
	virtual void	recalcHeight(int width);
};
