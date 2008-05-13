#include "globals.h"
#include "OpenDlg.h"
#include <xmltools.h>

COpenDlg::COpenDlg(CXUIEngine* engine) : CXUIDialog(L"res:open.xml", engine)
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(engine->get_hInstance(), path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	lstrcat(path, TEXT("projects\\"));

	WIN32_FIND_DATA fd;
	TCHAR findMask[MAX_PATH];
	lstrcpy(findMask, path);
	lstrcat(findMask, TEXT("*"));

	HANDLE hFind = FindFirstFile(findMask, &fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if(lstrcmp(fd.cFileName, TEXT(".")) && lstrcmp(fd.cFileName, TEXT("..")) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				XUI_PROJECT proj;
				ZeroMemory(&proj, sizeof(XUI_PROJECT));

				TCHAR projFile[MAX_PATH];
				lstrcpy(projFile, path);
				lstrcat(projFile, fd.cFileName);
				lstrcat(projFile, L"\\xuiproject.xml");

				CComPtr<IXMLDOMDocument> doc;
				doc.CoCreateInstance(CLSID_DOMFreeThreadedDocument, NULL, CLSCTX_INPROC_SERVER);
				doc->put_async(VARIANT_FALSE);
				doc->put_validateOnParse(VARIANT_FALSE);
				doc->put_resolveExternals(VARIANT_FALSE);
				VARIANT_BOOL bSuccess = VARIANT_FALSE;
				if(doc->load(CComVariant(projFile), &bSuccess) == S_OK && bSuccess == VARIANT_TRUE)
				{
					IXMLDOMNode* root = xmlOpenNode(doc, TEXT("xuiproject"));
					if(root)
					{
						proj.name			= xmlGetAttributeSTR(root, L"name");
						proj.dst			= xmlGetAttributeSTR(root, L"dst");
						MAKE_STR(proj.projectFile, projFile);
						root->Release();
					}
				}
				if(proj.dst && proj.name)
				{
					m_projects.Add(&proj);
				} else
				{
					FREE_CLEAR_STR(proj.dst);
					FREE_CLEAR_STR(proj.name);
					FREE_CLEAR_STR(proj.projectFile);
				}
				doc.Release();
			}
		} while(FindNextFile(hFind, &fd));
		FindClose(hFind);
	}

	m_projectFile	= NULL;
	m_outFile		= NULL;
	m_lcid			= 0;
}

COpenDlg::~COpenDlg(void)
{
	for(int i=0; i < m_projects.GetCount(); i++)
	{
		FREE_CLEAR_STR(m_projects[i].dst);
		FREE_CLEAR_STR(m_projects[i].name);
		FREE_CLEAR_STR(m_projects[i].projectFile);
	}
	m_projects.Clear();
	FREE_CLEAR_STR(m_projectFile);
	FREE_CLEAR_STR(m_outFile);
}

void COpenDlg::OnInitDialog()
{
	CXUIDialog::OnInitDialog();

	for(int primLang = 1; primLang <= 0x8d; primLang++)
	{
		for(int subLang = 1; subLang < 0x20; subLang++)
		{
			WCHAR langName[255];
			LCID lcid = MAKELCID(MAKELANGID(primLang, subLang), 0);
			if(GetLocaleInfo(lcid, LOCALE_SLANGUAGE, langName, sizeof(langName)))
			{
				m_ctlLangs->addItem(lcid, langName);
			}
		}
	}
	m_ctlLangs->value_INT(GetUserDefaultLCID());

	for(int i=0; i < m_projects.GetCount(); i++)
	{
		m_ctlProjects->addItem(i, m_projects[i].name);
	}
	m_ctlProjects->value_INT(0);
}

BOOL COpenDlg::OnEndDialog( UINT code )
{
	if(code == IDOK)
	{
		TCHAR iniPath[MAX_PATH];
		GetModuleFileName(m_engine->get_hInstance(), iniPath, MAX_PATH);
		PathRemoveFileSpec(iniPath);
		PathAddBackslash(iniPath);
		lstrcat(iniPath, TEXT("settings.ini"));

		m_lcid = m_ctlLangs->value_INT();
		int idx = m_ctlProjects->value_INT();
		if(idx >= 0 && idx < m_projects.GetCount())
		{
			MAKE_STR(m_projectFile, m_projects[idx].projectFile);



			TCHAR path[MAX_PATH];
			GetModuleFileName(m_engine->get_hInstance(), path, MAX_PATH);
			PathRemoveFileSpec(path);
			PathAddBackslash(path);
			TCHAR outPath[MAX_PATH];
			outPath[0] = 0;
			GetPrivateProfileString(L"settings", L"outpath", L"", outPath, MAX_PATH, iniPath);
			if(!outPath[0])
			{
				lstrcpy(outPath, path);
			}
			PathAddBackslash(outPath);
			TCHAR outFile[MAX_PATH];
			wsprintf(outFile, L"%s%d\\%s", outPath, m_lcid, m_projects[idx].dst);
			MAKE_STR(m_outFile, outFile);
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}
