#pragma once
#include "xuielement.h"
#include <CommCtrl.h>

#define XUI_LIST_VIEW_ICON			0
#define XUI_LIST_VIEW_SMALLICON		1
#define XUI_LIST_VIEW_LIST			2
#define XUI_LIST_VIEW_REPORT		3

class CXUIList : public CXUIElement
{
	UINT	m_view;
	BOOL	m_bNoColHeader;
	BOOL	m_bAutoArrange;
	BOOL	m_bAlwaysShowSelection;
	BOOL	m_bEditLabels;
	BOOL	m_bNoSortHeader;
	BOOL	m_bNoLabelWrap;
	BOOL	m_bNoScroll;
	BOOL	m_bSingleSelection;

	BOOL	m_bCheckBoxes;
	BOOL	m_bFullRowSelect;
	BOOL	m_bGridLines;
	BOOL	m_bHeaderDragDrop;
	BOOL	m_bLabelTip;

	BOOL	m_bAutoSizeCols;
public:
	CXUIList(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIList(void);

	IMPLEMENT_INTERFACE(L"list")

	int		insertItem(LPWSTR text, LPARAM data, int iconID = -1);
	void	setItemText(int iItem, int iSubitem, LPWSTR text);
	void	getItemText(int iItem, int iSubitem, LPWSTR text, int cbText);
	void	selectItem(int idx);
	void	clearItems();
	int		getItemsCount();
	BOOL	isSelected(int idx);
	LPARAM	getSelected();
	INT		getSelectedIndex();
	LPARAM	getItemData(int idx);
	void	deleteItemByIndex(int idx);
	void	setImageList(HIMAGELIST himl, int iImageList);

	virtual void Init();
	virtual BOOL loadDATA( IXMLDOMNode* node );
	virtual void render(int x, int y, int width, int height);
	virtual BOOL onNotify(int idCtrl, LPNMHDR pnmh);
};
