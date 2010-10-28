#pragma once
#include "xuielement.h"

class CXUIChecksList : public CXUIElement
{
	int		m_visibleItems;
	BOOL	m_lockChanged;
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
	void checkItemIdx(int idx, BOOL bChecked);
	BOOL isItemChecked(int value);
	BOOL isItemCheckedIdx(int idx);
	int  getCount();
	int  getItemValue(int idx);
	int  getSelected();
	void addItem(LPWSTR text, int data);
	void clearItems();
	virtual BOOL onNotify(int idCtrl, LPNMHDR pnmh);
};
