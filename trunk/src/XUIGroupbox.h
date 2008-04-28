#pragma once
#include "xuielement.h"

class CXUIGroupbox : public CXUIElement
{
	LPWSTR m_label;
public:
	CXUIGroupbox(CXUIElement* parent, CXUIEngine* engine);
	~CXUIGroupbox(void);
	IMPLEMENT_INTERFACE(L"groupbox")

	BOOL loadDATA(IXMLDOMNode* node);
	void Init();
	HWND get_parentWnd();
};
