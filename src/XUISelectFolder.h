#pragma once
#include "xuielement.h"

class CXUISelectFolder : public CXUIElement
{
	LPWSTR	m_title;
	LPWSTR	m_saveto;
public:
	CXUISelectFolder(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUISelectFolder(void);

	IMPLEMENT_INTERFACE(L"selectfolder")

	BOOL loadDATA(IXMLDOMNode* node);
	void Init();

	virtual void doDefaultAction(CXUIElement* el);
};
