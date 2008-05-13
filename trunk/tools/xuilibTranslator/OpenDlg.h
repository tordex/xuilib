#pragma once
#include "arrays.h"

struct XUI_PROJECT
{
	LPWSTR	name;
	LPWSTR	dst;
	LPWSTR	projectFile;
};

class COpenDlg : public CXUIDialog
{
	CXUIComboBox*			m_ctlLangs;
	CXUIListBox*			m_ctlProjects;
	
	CTArray<XUI_PROJECT>	m_projects;
public:
	COpenDlg(CXUIEngine* engine);
	virtual ~COpenDlg(void);

	// public data
	LPWSTR	m_projectFile;
	LPWSTR	m_outFile;
	LCID	m_lcid;

	XUI_BEGIN_BIND_MAP
		XUI_BIND_ITEM(L"lang",			L"combobox",	m_ctlLangs)
		XUI_BIND_ITEM(L"lstProjects",	L"listbox",		m_ctlProjects)
	XUI_END_BIND_MAP

	virtual void OnInitDialog();
	virtual BOOL OnEndDialog(UINT code);

private:
	
};
