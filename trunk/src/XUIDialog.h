#pragma once
#include "xuielement.h"
#include "XUIContextHelp.h"
#include "XUIButton.h"

#define BIND_TYPE_INT		0
#define BIND_TYPE_PSTR		1
#define BIND_TYPE_STR		2
#define BIND_TYPE_WORD		3
#define BIND_TYPE_BYTE		4
#define BIND_TYPE_BIT		5
#define BIND_TYPE_INTMASK	6
#define BIND_TYPE_INTUNIT	7

//////////////////////////////////////////////////////////////////////////
//						DATA EXCHANGE MAP								//
//////////////////////////////////////////////////////////////////////////

#define XUI_BEGIN_DATA_MAP		virtual void dataExchange(BOOL bSave, CXUIElement* parent) \
							{ \
								DATA_MAP_ITEM item; \

#define XUI_DATA_MAP_INT( elID, val)	item.id = elID; item.isChoice = FALSE; item.type = BIND_TYPE_INT; item.intVal = (LPINT) &(val); \
									if(bSave) \
									{ \
										saveMapItem(&item); \
									} else \
									{ \
										loadMapItem(&item, parent); \
									}

#define XUI_DATA_MAP_WORD( elID, val)	item.id = elID; item.isChoice = FALSE; item.type = BIND_TYPE_WORD; item.wordVal = (LPWORD) &(val); \
									if(bSave) \
									{ \
										saveMapItem(&item); \
									} else \
									{ \
										loadMapItem(&item, parent); \
									}

#define XUI_DATA_MAP_BYTE( elID, val)	item.id = elID; item.isChoice = FALSE; item.type = BIND_TYPE_BYTE; item.byteVal = (LPBYTE) &(val); \
	if(bSave) \
									{ \
										saveMapItem(&item); \
									} else \
									{ \
										loadMapItem(&item, parent); \
									}

#define XUI_DATA_MAP_INT_CHOICE( elID, trueVal, falseVal, val)	item.id = elID; item.isChoice = TRUE; item.type = BIND_TYPE_INT; item.intVal = (LPINT) &(val); item.intTrueVal = (INT) (trueVal); item.intFalseVal = (INT) (falseVal); \
	if(bSave) \
									{ \
									saveMapItem(&item); \
									} else \
									{ \
									loadMapItem(&item, parent); \
									}

#define XUI_DATA_MAP_BIT( elID, mask, val)	item.id = elID; item.type = BIND_TYPE_BIT; item.intVal = (LPINT) &(val); item.intTrueVal = (INT) (mask); \
	if(bSave) \
									{ \
									saveMapItem(&item); \
									} else \
									{ \
									loadMapItem(&item, parent); \
									}

#define XUI_DATA_MAP_INTMASK( elID, mask, val)	item.id = elID; item.type = BIND_TYPE_INTMASK; item.intVal = (LPINT) &(val); item.intTrueVal = (INT) (mask); \
	if(bSave) \
									{ \
									saveMapItem(&item); \
									} else \
									{ \
									loadMapItem(&item, parent); \
									}

#define XUI_DATA_MAP_INTUNIT( elID, mask, val)	item.id = elID; item.type = BIND_TYPE_INTUNIT; item.intVal = (LPINT) &(val); item.intTrueVal = (INT) (mask); \
	if(bSave) \
									{ \
									saveMapItem(&item); \
									} else \
									{ \
									loadMapItem(&item, parent); \
									}

#define XUI_DATA_MAP_PSTR( elID, val)	item.id = elID; item.isChoice = FALSE; item.type = BIND_TYPE_PSTR; item.pstrVal = &val; \
									if(bSave) \
									{ \
										saveMapItem(&item); \
									} else \
									{ \
										loadMapItem(&item, parent); \
									}

#define XUI_DATA_MAP_STR( elID, val)	item.id = elID; item.isChoice = FALSE; item.type = BIND_TYPE_STR; item.strVal = val; \
									if(bSave) \
									{ \
										saveMapItem(&item); \
									} else \
									{ \
										loadMapItem(&item, parent); \
									}

#define XUI_END_DATA_MAP	}

//////////////////////////////////////////////////////////////////////////
//						CONTROLS BINDING MAP							//
//////////////////////////////////////////////////////////////////////////


#define XUI_BEGIN_BIND_MAP	virtual void bindControls() \
							{ \
								CXUIElement* el = NULL;

#define XUI_BIND_ITEM(elID, elType, var)	el = find(elID); \
										if(el) \
										{ \
											el->QueryElement(elType, (LPVOID*) &var); \
										} else \
										{ \
											var = NULL; \
										}
#define XUI_END_BIND_MAP	}

#define XUI_BEGIN_MESSAGE_MAP	virtual BOOL ProcessNotify(UINT uMsg, CXUIElement* el, WPARAM wParam, LPARAM lParam) \
								{

#define XUI_COMMAND_HANDLER(elID, code, func)	if(uMsg == WM_COMMAND && el && HIWORD(wParam) == code && !lstrcmp(elID, el->get_id())) \
												{ \
													return func(el, code); \
												}

#define XUI_NOTIFY_HANDLER(elID, code, func)	if(uMsg == WM_NOTIFY && el && ((LPNMHDR)lParam)->code == code && !lstrcmp(elID, el->get_id())) \
												{ \
													return func(el, (LPNMHDR)lParam); \
												}
#define XUI_END_MESSAGE_MAP			return FALSE; \
								}

//////////////////////////////////////////////////////////////////////////
//						EVENT HANDLERS MAP								//
//////////////////////////////////////////////////////////////////////////

#define XUI_BEGIN_EVENT_MAP					virtual BOOL OnEvent(CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam) \
											{
#define XUI_HANDLE_CHANGED(elID, func)			if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_CHANGED, evID)) \
												{ \
													return func(); \
												}

#define XUI_HANDLE_CHANGED_ALL(func)			if(IS_SAME_STR(XUI_EVENT_CHANGED, evID)) \
												{ \
													if(func(el)) \
													{ \
														return TRUE; \
													} \
												}

#define XUI_HANDLE_CONTEXTMENU(elID, func)			if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_CONTEXTMENU, evID)) \
												{ \
												return func(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
												}

#define XUI_HANDLE_CONTEXTMENU_ALL(func)			if(IS_SAME_STR(XUI_EVENT_CONTEXTMENU, evID)) \
												{ \
												if(func(el, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) \
													{ \
													return TRUE; \
													} \
												}


#define XUI_HANDLE_CLICKED(elID, func)			if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_CLICKED, evID)) \
												{ \
													return func(); \
												}

#define XUI_HANDLE_CLICKED_ALL(func)			if(IS_SAME_STR(XUI_EVENT_CLICKED, evID)) \
												{ \
													return func(el); \
												}

#define XUI_HANDLE_TABINIT(elID, func)			if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_TAB_INIT, evID)) \
												{ \
													CXUITab* tab = NULL; \
													el->QueryElement(L"tab", (LPVOID*) &tab); \
													return func(tab); \
												}

#define XUI_HANDLE_TABINIT_ALL(func)			if(IS_SAME_STR(XUI_EVENT_TAB_INIT, evID)) \
												{ \
													CXUITab* tab = NULL; \
													el->QueryElement(L"tab", (LPVOID*) &tab); \
													return func(tab); \
												}

#define XUI_HANDLE_TABACTIVATE(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_TAB_ACTIVATE, evID)) \
												{ \
													CXUITab* tab = NULL; \
													el->QueryElement(L"tab", (LPVOID*) &tab); \
													return func(tab); \
												}

#define XUI_HANDLE_TABACTIVATE_ALL(func)		if(IS_SAME_STR(XUI_EVENT_TAB_ACTIVATE, evID)) \
												{ \
													CXUITab* tab = NULL; \
													el->QueryElement(L"tab", (LPVOID*) &tab); \
													return func(tab); \
												}

#define XUI_HANDLE_TABDEACTIVATE(elID, func)	if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_TAB_DEACTIVATE, evID)) \
												{ \
													CXUITab* tab = NULL; \
													el->QueryElement(L"tab", (LPVOID*) &tab); \
													return func(tab); \
												}

#define XUI_HANDLE_TABDEACTIVATE_ALL(func)	if(IS_SAME_STR(XUI_EVENT_TAB_DEACTIVATE, evID)) \
												{ \
													CXUITab* tab = NULL; \
													el->QueryElement(L"tab", (LPVOID*) &tab); \
													return func(tab); \
												}

#define XUI_HANDLE_LST_ITEMACTIVATE(elID, func)	if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_ITEMACTIVATE, evID)) \
												{ \
													return func((LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_ITEMACTIVATE_ALL(func)	if(IS_SAME_STR(XUI_EVENT_LST_ITEMACTIVATE, evID)) \
												{ \
													CXUIList* lst = NULL; \
													el->QueryElement(L"list", (LPVOID*) &lst); \
													return func(lst, (LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_RCLICK(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_RCLICK, evID)) \
												{ \
													return func((LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_RCLICK_ALL(func)			if(IS_SAME_STR(XUI_EVENT_LST_RCLICK, evID)) \
												{ \
													CXUIList* lst = NULL; \
													el->QueryElement(L"list", (LPVOID*) &lst); \
													return func(lst, (LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_CLICK(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_CLICK, evID)) \
												{ \
													return func((LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_CLICK_ALL(func)			if(IS_SAME_STR(XUI_EVENT_LST_CLICK, evID)) \
												{ \
													CXUIList* lst = NULL; \
													el->QueryElement(L"list", (LPVOID*) &lst); \
													return func(lst, (LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_ITEMCHANGED(elID, func)	if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_ITEMCHANGED, evID)) \
												{ \
													return func((LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_ITEMCHANGED_ALL(func)	if(IS_SAME_STR(XUI_EVENT_LST_ITEMCHANGED, evID)) \
												{ \
													CXUIList* lst = NULL; \
													el->QueryElement(L"list", (LPVOID*) &lst); \
													return func(lst, (LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_COLUMNCLICK(elID, func)	if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_COLUMNCLICK, evID)) \
												{ \
													return func((LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_CUSTOMDRAW(elID, func)	if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_CUSTOMDRAW, evID)) \
												{ \
												return func((LPNMLVCUSTOMDRAW) lParam); \
												}

#define XUI_HANDLE_LST_COLUMNCLICK_ALL(func)	if(IS_SAME_STR(XUI_EVENT_LST_COLUMNCLICK, evID)) \
												{ \
													CXUIList* lst = NULL; \
													el->QueryElement(L"list", (LPVOID*) &lst); \
													return func(lst, (LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_DBLCLK(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_DBLCLK, evID)) \
												{ \
													return func((LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_DBLCLK_ALL(func)			if(IS_SAME_STR(XUI_EVENT_LST_DBLCLK, evID)) \
												{ \
													CXUIList* lst = NULL; \
													el->QueryElement(L"list", (LPVOID*) &lst); \
													return func(lst, (LPNMHDR) lParam); \
												}

#define XUI_HANDLE_LST_KEYDOWN(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_KEYDOWN, evID)) \
												{ \
												return func((LPNMLVKEYDOWN) lParam); \
												}

#define XUI_HANDLE_LST_COLSCHANGED(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_COLSCHANGED, evID)) \
												{ \
												return func(); \
												}

#define XUI_HANDLE_LST_REQUESTCOLS(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_LST_REQUESTCOLS, evID)) \
												{ \
												return func((XUI_COL_INFO*) lParam, (int) wParam); \
												}

#define XUI_HANDLE_HSCROLL(elID, func)			if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_HSCROLL, evID)) \
												{ \
													return func(el, wParam); \
												}

#define XUI_HANDLE_HSCROLL_ALL(func)			if(IS_SAME_STR(XUI_EVENT_HSCROLL, evID)) \
												{ \
													return func(el, wParam); \
												}

#define XUI_HANDLE_VSCROLL(elID, func)			if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_VSCROLL, evID)) \
												{ \
													return func(el, wParam); \
												}

#define XUI_HANDLE_VSCROLL_ALL(func)			if(IS_SAME_STR(XUI_EVENT_VSCROLL, evID)) \
												{ \
													return func(el, wParam); \
												}

#define XUI_HANDLE_MEASUREITEM(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_MEASUREITEM, evID)) \
												{ \
													return func(el, (UINT) wParam, (LPMEASUREITEMSTRUCT) lParam); \
												}

#define XUI_HANDLE_DRAWITEM(elID, func)			if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_DRAWITEM, evID)) \
												{ \
													return func(el, (UINT) wParam, (LPDRAWITEMSTRUCT) lParam); \
												}

#define XUI_HANDLE_FSSELECTED(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_FSSELECTED, evID)) \
												{ \
													return func((LPWSTR) lParam); \
												}

#define XUI_HANDLE_FREEDRAW(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_FREEDRAW, evID)) \
												{ \
													return func((HDC) wParam, (LPRECT) lParam); \
												}

#define XUI_HANDLE_TOOLBAR_CHANGED(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_TOOLBAR_CHANGED, evID)) \
												{ \
												return func(); \
												}

#define XUI_HANDLE_TOOLBAR_DROPDOWN(elID, func)		if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_TOOLBAR_DROPDOWN, evID)) \
												{ \
												return func((LPRECT) lParam); \
												}

#define XUI_HANDLE_TOOLBAR_CUSTOMDRAW(elID, func)	if(IS_SAME_STR(elID, el->get_id()) && IS_SAME_STR(XUI_EVENT_TOOLBAR_CUSTOMDRAW, evID)) \
												{ \
												return func((LPNMCUSTOMDRAW) lParam); \
												}


#define XUI_END_EVENT_MAP						return FALSE; \
											}


struct DATA_MAP_ITEM
{
	LPWSTR		id;
	UINT		type;
	BOOL		isChoice;
	union
	{
		LPINT		intVal;
		LPWORD		wordVal;
		LPBYTE		byteVal;
		LPWSTR*		pstrVal;
		LPWSTR		strVal;
	};
	int intTrueVal;
	int intFalseVal;
};

#define	XUI_DLG_BORDER_NONE				0
#define	XUI_DLG_BORDER_THIN				1
#define	XUI_DLG_BORDER_RESIZING			2
#define	XUI_DLG_BORDER_DIALOGFRAME		3

class CXUIDialog : public CXUIElement
{
protected:
	LPDLGTEMPLATE	m_hDlg;
	LPWSTR			m_caption;
	DWORD			m_style;
	DWORD			m_exStyle;
	LPWSTR			m_fileName;
	BOOL			m_bCenterScreen;
	UINT			m_nextDlgID;
	BOOL			m_bContextHelp;
	BOOL			m_bClipChildren;
	BOOL			m_bClipSiblings;
	UINT			m_border;
	BOOL			m_bMinimizeBox;
	BOOL			m_bMaximizeBox;
	BOOL			m_bSysMenu;
	BOOL			m_bTitleBar;
	CXUIContextHelp	m_contextHelp;
	CXUIButton*		m_defButton;
	INT				m_minWindowWidth;
	INT				m_minWindowHeight;
protected:
	LPDLGTEMPLATE createDialog(int left, int top, int width, int height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void GetDesktopRect(RECT* rcDsk, HWND hWnd);
	void loadMapItem(DATA_MAP_ITEM* item, CXUIElement* parent);
	void saveMapItem(DATA_MAP_ITEM* item);
public:
	CXUIDialog(LPCTSTR fileName, CXUIEngine* engine, CXUIElement* parent = NULL);
	virtual ~CXUIDialog(void);
	IMPLEMENT_INTERFACE(L"dialog")

	virtual void OnInitDialog();
	virtual BOOL OnEndDialog(UINT code);
	virtual void OnNotify(CXUIElement* el, int idCtrl, LPNMHDR pnmh);
	virtual BOOL OnCommand(CXUIElement* el, UINT code, UINT id, HWND hWnd);
	virtual BOOL PreprocessMessage(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT& retVal);
	virtual CXUIElement* getDefaultControl();
	virtual int	getSavedWidth(int defWidth);
	virtual int	getSavedHeight(int defHeight);
	virtual void saveSizes(int width, int height);
	virtual void showTipMessage(LPCWSTR elID, LPCWSTR tag);
	
	BOOL loadDATA(IXMLDOMNode* node);
	UINT DoModal(HWND hWndParent);
	HWND Create(HWND hWndParent);
	virtual void dataExchange(BOOL bSave, CXUIElement* parent);
	virtual void bindControls();
	virtual BOOL ProcessNotify(UINT uMsg, CXUIElement* el, WPARAM wParam, LPARAM lParam);
	virtual UINT get_newDlgID();
	
	void set_defButton(LPCWSTR btnID);
};
