#pragma once
#include "xuielement.h"

#define XUI_LISTCOL_ALIGN_LEFT		0
#define XUI_LISTCOL_ALIGN_RIGHT		1
#define XUI_LISTCOL_ALIGN_CENTER	3

class CXUIListCol : public CXUIElement
{
	LPWSTR	m_caption;
	LPWSTR	m_name;
	LPWSTR	m_icon;
	int		m_size;
	int		m_flex;
	int		m_align;
	BOOL	m_colHasImages;
	BOOL	m_image;
	BOOL	m_bitmapOnRight;
	int		m_colID;
	BOOL	m_bVisible;
	BOOL	m_isSorted;
	BOOL	m_sortDown;
public:
	CXUIListCol(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIListCol(void);

	IMPLEMENT_INTERFACE(L"listcol")

	virtual BOOL loadDATA(IXMLDOMNode* node);

	int		get_size()		{ return m_size;		}
	void	set_size(int sz){ m_size = sz;			}
	int		get_flex()		{ return m_flex;		}
	LPWSTR	get_caption()	{ return m_caption;		}
	LPWSTR	get_name()		{ return m_name;		}
	void	get_colData(LPLVCOLUMN plvi);
	int 	get_ColID()		{ return m_colID;		}
	BOOL	isVisible()		{ return m_bVisible;	}
	void	setColID(int id)		{ m_colID = id;	}
	void	setVisible(BOOL visible)		{ m_bVisible = visible;	}
	void	setSort(BOOL isSorted, BOOL down);
	BOOL	isSorted()		{ return m_isSorted; }
	BOOL	isSortDown()	{ return m_sortDown; }
};
