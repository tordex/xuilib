#pragma once
#include "xuielement.h"

class CXUISendCMD : public CXUIElement
{
	LPWSTR	m_sendto;
	LPWSTR	m_cmd;
	BOOL	m_inViewerOnly;
public:
	CXUISendCMD(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUISendCMD(void);

	IMPLEMENT_INTERFACE(L"sendcmd")

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void doDefaultAction(CXUIElement* el);
};
