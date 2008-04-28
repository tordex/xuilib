#pragma once
#include "xuielement.h"

class CXUITab : public CXUIElement
{
	LPWSTR	m_label;
	LPWSTR	m_caption;
	LPWSTR	m_image;
	LPWSTR	m_helpTopic;
	BOOL	m_bSetFocus;
	BOOL	m_bInitialized;
	INT		m_value;

	RECT	m_rc;
	int		m_selected;
	int		m_hover;
public:
	CXUITab(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUITab(void);

	IMPLEMENT_INTERFACE(L"tab")

	LPCWSTR get_label()		{ return m_label;	}
	LPCWSTR get_caption()	{ return m_caption; }
	LPCWSTR get_image()		{ return m_image;	}

	void Draw(HDC hdc, int imageAlign, int imageSize, LPRECT rcItem);
	void GetTabSize(HDC hdc, int imageAlign, int imageSize, SIZE& sz);

	void	rect(LPRECT rc)			{ m_rc = *rc; }
	LPCRECT	rect()					{ return &m_rc; }

	int		selected()				{ return m_selected;		}
	void	selected(int bSelected);

	int		hover()				{ return m_hover;		}
	void	hover(int bHover)	{ m_hover = bHover;	}

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void Init();
	virtual BOOL isInitialized();
	virtual INT  value_INT();
	virtual void onCmd(LPWSTR cmd);

	void reinit();
	void init();
};

inline COLORREF tswLightColor(COLORREF clr, long level)
{
	BYTE red   = GetRValue(clr);
	BYTE green = GetGValue(clr);
	BYTE blue  = GetBValue(clr);
	red = (BYTE) max(0, min(255, (int)((red - 128) + 128 + level)));
	green = (BYTE) max(0, min(255, (int)((green - 128) + 128 + level)));
	blue = (BYTE) max(0, min(255, (int)((blue - 128) + 128 + level)));
	return RGB(red, green, blue);
}
