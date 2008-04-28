#pragma once
#include "xuielement.h"

class CXUIChecksList : public CXUIElement
{
	int	m_visibleItems;
public:
	CXUIChecksList(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIChecksList(void);

	IMPLEMENT_INTERFACE(L"checkslist")

	void Init();
	BOOL loadDATA(IXMLDOMNode* node);
	void render(int x, int y, int width, int height);
	
	int  getItemIndex(int value);
	void setItemIndex(int value, int idx);
	void checkItem(int value, BOOL bChecked);
	BOOL isItemChecked(int value);
	int  getCount();
	int  getItemValue(int idx);
	int  getSelected();
};
