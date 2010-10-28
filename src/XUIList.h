#pragma once
#include "xuielement.h"
#include <CommCtrl.h>
#include <vector>
#include "XUIListCol.h"

#define XUI_LIST_VIEW_ICON			0
#define XUI_LIST_VIEW_SMALLICON		1
#define XUI_LIST_VIEW_LIST			2
#define XUI_LIST_VIEW_REPORT		3

struct XUI_COL_INFO
{
	WCHAR	id[255];
	int		size;
	BOOL	visible;
	int		order;
};

class CXUIList : public CXUIElement
{
	UINT		m_view;
	BOOL		m_bNoColHeader;
	BOOL		m_bAutoArrange;
	BOOL		m_bAlwaysShowSelection;
	BOOL		m_bEditLabels;
	BOOL		m_bNoSortHeader;
	BOOL		m_bNoLabelWrap;
	BOOL		m_bNoScroll;
	BOOL		m_bSingleSelection;
	BOOL		m_bOwnerDraw;

	BOOL		m_bCheckBoxes;
	BOOL		m_bFullRowSelect;
	BOOL		m_bGridLines;
	BOOL		m_bHeaderDragDrop;
	BOOL		m_bLabelTip;
	BOOL		m_bEditColumns;
	BOOL		m_useIcons;

	BOOL		m_bAutoSizeCols;

	HIMAGELIST			m_hSmallImages;
	int					m_smallIconsWidth;
	int					m_smallIconsHeight;
	std::vector<LPWSTR>	m_smallIcons;
public:

	enum ICON_TYPE
	{
		ICON_TYPE_NORMAL,
		ICON_TYPE_SMALL,
		ICON_TYPE_STATE,
		ICON_TYPE_GROUPHEADER,
	};

	CXUIList(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIList(void);

	IMPLEMENT_INTERFACE(L"list")

	int		insertItem(LPCWSTR text, LPARAM data, int iconID = -1);
	void	setItemText(int iItem, int iSubitem, LPCWSTR text);
	void	getItemText(int iItem, int iSubitem, LPCWSTR text, int cbText);
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

	int addIcon(LPCWSTR icon, ICON_TYPE iconType);
	int addIcon(HBITMAP bmp, ICON_TYPE iconType);
	CXUIListCol* getColByID(int id);
	void getColsInfo(XUI_COL_INFO* ci);
	void setColSort(int colID, BOOL down);
private:
	void clearSmallIcons();
	void assignColumnsSize();
	void updateColsSortState();
};
