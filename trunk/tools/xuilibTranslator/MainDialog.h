#pragma once

#include "arrays.h"

struct LOC_STRINGS
{
	LPTSTR			id;
	LPTSTR			attr;
	LPTSTR			defString;
	LPTSTR			locString;
};

class CDialogs
{
	LPWSTR				m_fileName;
	LPWSTR				m_Name;
	CTArray<LPWSTR>		m_strings;
public:
	CDialogs();
	~CDialogs();

	BOOL loadDialog(LPWSTR fileName);
	void testDialog(CXUIEngine* engine, HWND hWndParent);
	BOOL checkLOCID(LPCWSTR locid);

	LPCWSTR getName()	{ return m_Name; }

private:
	void processNode(IXMLDOMNode* root);
};

struct STR_INFO
{
	LPCWSTR	strID;
	LPCWSTR	attr;
	LPCWSTR	dialog;
};

class CMainDialog : public CXUIDialog
{
	CTArray<LOC_STRINGS>	m_strings;
	CTArray<CDialogs*>		m_dialogs;

	LPWSTR	m_projectFile;
	LPWSTR	m_outFile;
	LCID	m_lcid;

	BOOL	m_bChanged;

	// controls
	CXUIList*			m_ctlList;
	CXUIComboBox*		m_ctlDialogs;
public:
	CMainDialog(CXUIEngine* engine);
	virtual ~CMainDialog(void);

	virtual void OnInitDialog();
	virtual BOOL OnEndDialog(UINT code);
	virtual int  getSavedWidth(int defWidth);
	virtual int  getSavedHeight(int defHeight);
	virtual void saveSizes(int width, int height);

	XUI_BEGIN_BIND_MAP
		XUI_BIND_ITEM(L"lstStrings",	L"list",		m_ctlList)
		XUI_BIND_ITEM(L"dialog",		L"combobox",	m_ctlDialogs)
	XUI_END_BIND_MAP

	XUI_BEGIN_EVENT_MAP
		XUI_HANDLE_CLICKED			(L"btnOpen",		OnOpen)
		XUI_HANDLE_CLICKED			(L"btnSave",		OnSave)
		XUI_HANDLE_CLICKED			(L"btnTranslate",	OnTranslate)
		XUI_HANDLE_CLICKED			(L"testDlg",		OnTestDlg)
		XUI_HANDLE_CLICKED			(L"showOriginal",	OnShowOriginalDlg)
		XUI_HANDLE_LST_DBLCLK		(L"lstStrings",		OnTranslate)
		XUI_HANDLE_LST_ITEMCHANGED	(L"lstStrings",		OnListSelected)
		XUI_HANDLE_FSSELECTED		(L"import",			OnImport)
		XUI_HANDLE_FSSELECTED		(L"export",			OnExport)
		XUI_HANDLE_FSSELECTED		(L"exportText",		OnExportText)
		XUI_HANDLE_CHANGED			(L"find",			OnFindChanged)
		XUI_HANDLE_CHANGED			(L"dialog",			OnFindChanged)
		XUI_HANDLE_CLICKED			(L"urlTranslated",	OnUrlTranslated)
		XUI_HANDLE_CLICKED			(L"urlOriginal",	OnUrlOriginal)
		XUI_HANDLE_CLICKED			(L"prevDlg",		OnPrevDlg)
		XUI_HANDLE_CLICKED			(L"nextDlg",		OnNextDlg)
	XUI_END_EVENT_MAP

	LOC_STRINGS* getString(int idx);
	void updateString(int idx, LPCWSTR str);
	void updateListText(int idx);
	void getStringInfo(int idx, STR_INFO* info);

protected:
	// Event handlers
	BOOL OnOpen();
	BOOL OnSave();
	BOOL OnImport(LPWSTR fileName);
	BOOL OnExport(LPWSTR fileName);
	BOOL OnExportText(LPWSTR fileName);

	LPWSTR clearStringForImport( LPWSTR src );
	void importString( LPWSTR src, LPWSTR dst );
	BOOL OnFindChanged();
	BOOL OnTestDlg();
	BOOL OnUrlTranslated();
	BOOL OnUrlOriginal();
	BOOL OnPrevDlg();
	BOOL OnNextDlg();

	BOOL OnShowOriginalDlg();

	void showOriginalDialog( int dlgIDX );
	void showTranslatedDialog( int dlgIDX );
	BOOL Save();
	BOOL OnTranslate(LPNMHDR phdr = NULL);
	BOOL OnListSelected(LPNMHDR phdr = NULL);

	// helpers
	void clearStrings();
	void clearDialogs();
	void fillList();
	void fillDialogs();
	void showDlgInfo(int strIDX);
};
