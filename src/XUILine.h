#pragma once
#include "xuielement.h"

#define XUI_LINE_TYPE_ETCHED_HORZ		0
#define XUI_LINE_TYPE_ETCHED_VERT		1

class CXUILine : public CXUIElement
{
	UINT	m_type;
public:
	CXUILine(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUILine(void);

	IMPLEMENT_INTERFACE(L"line")

	void Init();
	BOOL loadDATA(IXMLDOMNode* node);
};
