#pragma once

class CMainDialog;

class CTranslateDlg : public CXUIDialog
{
	CMainDialog*	m_parent;
	int				m_idx;
public:
	CTranslateDlg(CMainDialog* parent, int startIDX, CXUIEngine* engine);
	virtual ~CTranslateDlg(void);

	XUI_BEGIN_EVENT_MAP
		XUI_HANDLE_CLICKED(L"btnSave",	OnSave)
		XUI_HANDLE_CLICKED(L"btnSkip",	OnSkip)
	XUI_END_EVENT_MAP

	virtual void OnInitDialog();
	virtual int  getSavedWidth(int defWidth);
	virtual int  getSavedHeight(int defHeight);
	virtual void saveSizes(int width, int height);

	BOOL loadString(int idx, BOOL skipTranslated);
	BOOL OnSave();
	BOOL OnSkip();
};
