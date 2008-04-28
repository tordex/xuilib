#pragma once
#include "xuielement.h"

class CXUIOpenHelp : public CXUIElement
{
	LPWSTR	m_topic;
public:
	CXUIOpenHelp(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIOpenHelp(void);

	IMPLEMENT_INTERFACE(L"openhelp")

	BOOL loadDATA(IXMLDOMNode* node);
	void doDefaultAction(CXUIElement* el);
};
