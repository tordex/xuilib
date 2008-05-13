#include "globals.h"
#include "TranslateDlg.h"
#include "MainDialog.h"

CTranslateDlg::CTranslateDlg(CMainDialog* parent, int startIDX, CXUIEngine* engine) : CXUIDialog(L"res:translate.xml", engine)
{
	m_parent	= parent;
	m_idx		= startIDX;
}

CTranslateDlg::~CTranslateDlg(void)
{
}

void CTranslateDlg::OnInitDialog()
{
	CXUIDialog::OnInitDialog();
	find(L"dstString")->set_TabStopFocus();
	set_defButton(L"btnSave");

	if(!loadString(m_idx, FALSE))
	{
		EndDialog(m_hWnd, IDCANCEL);
	}
}

LPWSTR translateReturns(LPWSTR str)
{
	if(!str) return NULL;
	LPWSTR out = new WCHAR[(lstrlen(str) + 1) * 2];
	int j = 0;
	for(int i=0; str[i]; i++)
	{
		if(str[i] == L'\n')
		{
			out[j++] = L'\r';
		}
		out[j++] = str[i];
	}
	out[j++] = 0;
	return out;
}

LPWSTR translateRN(LPCWSTR str)
{
	if(!str) return NULL;
	LPWSTR out = new WCHAR[lstrlen(str) + 1];
	int j = 0;
	for(int i=0; str[i]; i++)
	{
		if(!(str[i] == L'\r' && str[i + 1] == L'\n'))
		{
			out[j++] = str[i];
		}
	}
	out[j++] = 0;
	return out;
}

BOOL CTranslateDlg::loadString( int idx, BOOL skipTranslated )
{
	LOC_STRINGS* str = NULL;
	if(skipTranslated)
	{
		for(int i = idx; str = m_parent->getString(i); i++)
		{
			if(str && !str->locString)
			{
				m_idx = i;
				break;
			}
		}
	} else
	{
		str = m_parent->getString(idx);
		if(str)
		{
			m_idx = idx;
		}

	}
	if(str)
	{
		{
			STR_INFO strInfo = {0};
			m_parent->getStringInfo(m_idx, &strInfo);

			LPWSTR text = NULL;
			if(strInfo.strID)
			{
				text = new WCHAR[lstrlen(strInfo.strID) + 100];
				text[0] = 0;
				if(strInfo.attr)
				{
					lstrcat(text, L"[");
					lstrcat(text, strInfo.attr);
					lstrcat(text, L"] ");
				}
				lstrcat(text, strInfo.strID);
				find(L"strID")->value_STR(text);
				delete text;
			}

			find(L"dlgName")->value_STR(strInfo.dialog);
		}

		LPWSTR src = translateReturns(str->defString);
		LPWSTR dst = translateReturns(str->locString);

		find(L"srcString")->value_STR(src);
		find(L"dstString")->value_STR(dst);

		FREE_CLEAR_STR(src);
		FREE_CLEAR_STR(dst);

		return TRUE;
	}
	return FALSE;
}

BOOL CTranslateDlg::OnSave()
{
	LPWSTR str = translateRN(find(L"dstString")->value_STR());
	m_parent->updateString(m_idx, str);
	FREE_CLEAR_STR(str);
	loadString(m_idx + 1, find(L"skipTranslated")->value_INT());
	find(L"dstString")->set_TabStopFocus();
	return TRUE;
}

BOOL CTranslateDlg::OnSkip()
{
	loadString(m_idx + 1, find(L"skipTranslated")->value_INT());
	find(L"dstString")->set_TabStopFocus();
	return TRUE;
}

int CTranslateDlg::getSavedWidth( int defWidth )
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(m_engine->get_hInstance(), path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	lstrcat(path, TEXT("settings.ini"));

	int width = GetPrivateProfileInt(L"TranslateWindow", L"width", defWidth, path);
	return max(defWidth, width);
}

int CTranslateDlg::getSavedHeight( int defHeight )
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(m_engine->get_hInstance(), path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	lstrcat(path, TEXT("settings.ini"));

	int height = GetPrivateProfileInt(L"TranslateWindow", L"height", defHeight, path);
	return max(defHeight, height);
}

void CTranslateDlg::saveSizes( int width, int height )
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(m_engine->get_hInstance(), path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	lstrcat(path, TEXT("settings.ini"));

	WCHAR str[50];

	_itow_s(width, str, 50, 10);
	WritePrivateProfileString(L"TranslateWindow", L"width", str, path);
	_itow_s(height, str, 50, 10);
	WritePrivateProfileString(L"TranslateWindow", L"height", str, path);
}
