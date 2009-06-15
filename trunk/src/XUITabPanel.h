#pragma once
#include "xuielement.h"

class CXUITabPanel : public CXUIDialog
{
	LPWSTR	m_label;
public:
	CXUITabPanel(CXUIElement* parent, CXUIEngine* engine);
	~CXUITabPanel(void);
	IMPLEMENT_INTERFACE(L"tabpanel")

	BOOL loadDATA(IXMLDOMNode* node);
	void OnInitDialog();
	void set_hidden(int hide);
	void Init();
	void render(int x, int y, int width, int height);
	LPCWSTR get_label() { return m_label; }
};
