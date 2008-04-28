#pragma once
#include "xuielement.h"

class CXUIExt : public CXUIElement
{
	LPWSTR	m_text;
	LPWSTR	m_extension;
public:
	CXUIExt(CXUIElement* parent, CXUIEngine* engine, LPWSTR text = NULL, LPWSTR ext = NULL);
	virtual ~CXUIExt(void);

	IMPLEMENT_INTERFACE(L"ext")

	BOOL loadDATA(IXMLDOMNode* node);

	LPCWSTR get_text() { return m_text; }
	LPCWSTR get_extension() { return m_extension; }
};
