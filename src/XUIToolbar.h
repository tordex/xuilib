#pragma once
#include "xuielement.h"

class CXUIToolbar : public CXUIElement
{
	struct ICONS_MAP
	{
		LPWSTR	icon;
		int		idx;
		HBITMAP	hBitmap;
	};

	ICONS_MAP*	m_icons;
	int			m_iconsCount;

	BOOL	m_bAltDrag;
	BOOL	m_bFlat;
	BOOL	m_bList;
	BOOL	m_bTooltips;
	int		m_btnWidth;
	int		m_btnHeight;
	BOOL	m_bDrawDDArrows;
	BOOL	m_bMixedButtons;
public:
	CXUIToolbar(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIToolbar(void);

	IMPLEMENT_INTERFACE(L"toolbar")

	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual void	Init();
	virtual BOOL	onNotify(int idCtrl, LPNMHDR pnmh);
	virtual BOOL	onCommnd(UINT code, UINT id, HWND hWnd);

	void	saveToolbar(HKEY hkr, LPCWSTR pszSubKey, LPCWSTR pszValueName);
	void	restoreToolbar(HKEY hkr, LPCWSTR pszSubKey, LPCWSTR pszValueName);
	int		addIcon(LPCWSTR icon);
};

class CXUIToolbarButton : public CXUIElement
{
	BYTE	m_fsStyle;
	LPWSTR	m_icon;
	LPWSTR	m_label;
	LPWSTR	m_tip;
	int		m_iconWidth;
	int		m_iconHeight;
	UINT	m_cmdID;
public:
	CXUIToolbarButton(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIToolbarButton();

	IMPLEMENT_INTERFACE(L"toolbarbutton")

	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual void	Init();
	virtual void	updateDisabledState();
	virtual void	value_STR(LPCWSTR val);

	void	getButtonData(LPTBBUTTON lpButton, BOOL withLabel = TRUE);
	LPCWSTR	getIcon() { return m_icon; }
	void	drawButton(HDC hdc, LPRECT rcDraw, UINT uItemState, int btnWidth, int btnHeight);
	LPCWSTR	getLabel()	{ return m_label;	}
	LPCWSTR	getTip()	{ return m_tip ? m_tip : m_label; }
	UINT	getCmdID()	{ return m_cmdID; }
	void	setCmdID(UINT cmdID)	{ m_cmdID = cmdID; }
	void	setIcon(LPCWSTR icon);
};
