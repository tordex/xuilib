#pragma once
#include "xuielement.h"

class CXUITabBox : public CXUIElement
{
public:
	CXUITabBox(CXUIElement* parent, CXUIEngine* engine);
	~CXUITabBox(void);
	IMPLEMENT_INTERFACE(L"tabbox")

	void selectTab(int tabID);
	virtual BOOL	onNotify(int idCtrl, LPNMHDR pnmh);
	virtual void	getMinSize(SIZE& minSize);
	virtual void	render(int x, int y, int width, int height);
	virtual void	Init();
	virtual HWND	get_parentWnd();
	virtual void	value_INT(INT val);
	virtual INT		value_INT();
};
