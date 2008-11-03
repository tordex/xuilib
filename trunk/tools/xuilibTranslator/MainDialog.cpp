#include "globals.h"
#include "MainDialog.h"
#include <xmltools.h>
#include "TranslateDlg.h"
#include "OpenDlg.h"
#include <shlobj.h>
#include "MyXUIEngine.h"

//////////////////////////////////////////////////////////////////////////

CDialogs::CDialogs()
{
	m_fileName	= NULL;
	m_Name		= NULL;
}

CDialogs::~CDialogs()
{
	FREE_CLEAR_STR(m_fileName);
	FREE_CLEAR_STR(m_Name);
	for(int i=0; i < m_strings.GetCount(); i++)
	{
		FREE_CLEAR_STR(m_strings[i]);
	}
}

void CDialogs::processNode(IXMLDOMNode* root)
{
	LPWSTR locid = xmlGetAttributeSTR(root, L"locid");
	if(locid)
	{
		m_strings.Add(&locid);
	}

	IXMLDOMNode* child = NULL;
	root->get_firstChild(&child);
	while(child)
	{
		processNode(child);
		XML_NEXTNODE(child);
	}
}


BOOL CDialogs::loadDialog( LPWSTR fileName )
{
	MAKE_STR(m_fileName, fileName);

	BOOL ret = FALSE;
	CComPtr<IXMLDOMDocument> doc;
	doc.CoCreateInstance(CLSID_DOMFreeThreadedDocument, NULL, CLSCTX_INPROC_SERVER);
	doc->put_async(VARIANT_FALSE);
	doc->put_validateOnParse(VARIANT_FALSE);
	doc->put_resolveExternals(VARIANT_FALSE);
	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	if(doc->load(CComVariant(fileName), &bSuccess) == S_OK && bSuccess == VARIANT_TRUE)
	{
		IXMLDOMNode* root = xmlOpenNode(doc, TEXT("dialog"));
		if(root)
		{
			m_Name = xmlGetAttributeSTR(root, L"caption");
			if(!m_Name)
			{
				MAKE_STR(m_Name, PathFindFileName(fileName));
			}

			processNode(root);
			root->Release();
			ret = TRUE;
		}
	}
	return ret;
}

void CDialogs::testDialog( CXUIEngine* engine, HWND hWndParent )
{
	CXUIDialog dlg(m_fileName, engine);
	dlg.DoModal(hWndParent);
}

BOOL CDialogs::checkLOCID( LPCWSTR locid )
{
	for(int i=0; i < m_strings.GetCount(); i++)
	{
		if(!lstrcmp(locid, m_strings[i]))
		{
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

CMainDialog::CMainDialog(CXUIEngine* engine) : CXUIDialog(L"res:main.xml", engine)
{
	m_projectFile	= NULL;
	m_outFile		= NULL;
	m_lcid			= 0;
	m_bChanged		= FALSE;
}

CMainDialog::~CMainDialog(void)
{
	FREE_CLEAR_STR(m_projectFile);
	FREE_CLEAR_STR(m_outFile);
	clearStrings();
	clearDialogs();
}

BOOL CMainDialog::OnOpen()
{
	COpenDlg dlg(m_engine);
	if(dlg.DoModal(m_hWnd) == IDOK)
	{
		MAKE_STR(m_projectFile, dlg.m_projectFile);
		MAKE_STR(m_outFile,		dlg.m_outFile);
		m_lcid = dlg.m_lcid;

		IXMLDOMNode* outStrings = NULL;

		CComPtr<IXMLDOMDocument> docOut;
		docOut.CoCreateInstance(CLSID_DOMFreeThreadedDocument, NULL, CLSCTX_INPROC_SERVER);
		docOut->put_async(VARIANT_FALSE);
		docOut->put_validateOnParse(VARIANT_FALSE);
		docOut->put_resolveExternals(VARIANT_FALSE);
		VARIANT_BOOL bSuccess = VARIANT_FALSE;
		if(docOut->load(CComVariant(m_outFile), &bSuccess) == S_OK && bSuccess == VARIANT_TRUE)
		{
			outStrings = xmlOpenNode(docOut, TEXT("strings"));
		}


		CComPtr<IXMLDOMDocument> doc;
		doc.CoCreateInstance(CLSID_DOMFreeThreadedDocument, NULL, CLSCTX_INPROC_SERVER);
		doc->put_async(VARIANT_FALSE);
		doc->put_validateOnParse(VARIANT_FALSE);
		doc->put_resolveExternals(VARIANT_FALSE);
		bSuccess = VARIANT_FALSE;
		if(doc->load(CComVariant(m_projectFile), &bSuccess) == S_OK && bSuccess == VARIANT_TRUE)
		{
			IXMLDOMNode* root = xmlOpenNode(doc, TEXT("xuiproject"));
			if(root)
			{
				IXMLDOMNode* strings = xmlOpenNode(root, TEXT("strings"));
				if(strings)
				{
					clearStrings();

					IXMLDOMNode* strNode = NULL;
					strings->get_firstChild(&strNode);
					while(strNode)
					{
						LOC_STRINGS locStr;
						ZeroMemory(&locStr, sizeof(locStr));

						locStr.id			= xmlGetAttributeSTR(strNode, L"id");
						locStr.attr			= xmlGetAttributeSTR(strNode, L"attr");
						locStr.defString	= xmlGetAttributeSTR(strNode, L"text");

						if(locStr.id && locStr.defString && locStr.defString[0])
						{
							IXMLDOMNode* outString = NULL;
							TCHAR nodeQuery[300];
							if(locStr.attr)
							{
								wsprintf(nodeQuery, L"string[@id=\"%s\" and @attr=\"%s\"]", locStr.id, locStr.attr);
							} else
							{
								wsprintf(nodeQuery, L"string[@id=\"%s\"]", locStr.id);
							}

							outString = xmlOpenNode(outStrings, nodeQuery);
							if(outString)
							{
								locStr.locString = xmlGetAttributeSTR(outString, L"loc");
								outString->Release();
							}

							m_strings.Add(&locStr);
						} else
						{
							FREE_CLEAR_STR(locStr.id);
							FREE_CLEAR_STR(locStr.attr);
							FREE_CLEAR_STR(locStr.defString);
							FREE_CLEAR_STR(locStr.locString);
						}

						XML_NEXTNODE(strNode);
					}

					fillList();

					strings->Release();
				}

				clearDialogs();

				IXMLDOMNode* dialogs = xmlOpenNode(root, TEXT("files"));
				if(dialogs)
				{
					IXMLDOMNode* dlgNode = NULL;
					dialogs->get_firstChild(&dlgNode);
					while(dlgNode)
					{
						LPWSTR path = new WCHAR[lstrlen(m_projectFile) * 2];
						lstrcpy(path, m_projectFile);
						PathRemoveFileSpec(path);
						PathAddBackslash(path);

						LPWSTR file = xmlGetAttributeSTR(dlgNode, L"name");

						if(file)
						{
							lstrcat(path, file);
							CDialogs* dlg = new CDialogs;
							if(dlg->loadDialog(path))
							{
								m_dialogs.Add(&dlg);
							} else
							{
								delete dlg;
							}
							delete file;
						}
						delete path;


						XML_NEXTNODE(dlgNode);
					}
					fillDialogs();
				}

				root->Release();
			}
		}
		if(outStrings)
		{
			outStrings->Release();
		}
		return TRUE;
	}
	return FALSE;
}

void CMainDialog::clearStrings()
{
	for(int i=0; i < m_strings.GetCount(); i++)
	{
		FREE_CLEAR_STR(m_strings[i].id);
		FREE_CLEAR_STR(m_strings[i].attr);
		FREE_CLEAR_STR(m_strings[i].defString);
		FREE_CLEAR_STR(m_strings[i].locString);
	}
	m_strings.Clear();
}

void CMainDialog::fillList()
{
	m_ctlList->clearItems();

	LPCWSTR filter = find(L"find")->value_STR();

	for(int i=0; i < m_strings.GetCount(); i++)
	{
		BOOL add = FALSE;
		if(filter)
		{
			if(m_strings[i].defString)
			{
				if(StrStrI(m_strings[i].defString, filter))
				{
					add = TRUE;
				}
			}
			if(m_strings[i].locString)
			{
				if(StrStrI(m_strings[i].locString, filter))
				{
					add = TRUE;
				}
			}
		} else
		{
			add = TRUE;
		}

		if(add)
		{
			int dlgID = m_ctlDialogs->value_INT();
			if(dlgID >= 0)
			{
				if(!m_dialogs[dlgID]->checkLOCID(m_strings[i].id))
				{
					add = FALSE;
				}
			}
		}

		if(add)
		{
			int idx = m_ctlList->insertItem(m_strings[i].defString, (LPARAM) i);
			if(m_strings[i].locString)
			{
				m_ctlList->setItemText(idx, 1, m_strings[i].locString);
			}
		}
	}
}

BOOL CMainDialog::OnTranslate(LPNMHDR phdr)
{
	if(!m_ctlList->getItemsCount()) return FALSE;

	int idx = m_ctlList->getSelectedIndex();
	if(idx < 0)
	{
		idx = 0;
	}

	CTranslateDlg dlg(this, idx, m_engine);
	dlg.DoModal(m_hWnd);
	return TRUE;
}

void CMainDialog::OnInitDialog()
{
	CXUIDialog::OnInitDialog();

	OnListSelected();

	if(!OnOpen())
	{
		EndDialog(m_hWnd, IDCANCEL);
	} else
	{
		find(L"btnSave")->set_disabled(TRUE);
		m_bChanged = FALSE;
	}
}

LOC_STRINGS* CMainDialog::getString( int idx )
{
	if(idx >= 0 && idx < m_ctlList->getItemsCount())
	{
		int strIDX = (int) m_ctlList->getItemData(idx);
		return &(m_strings[strIDX]);
	}
	return NULL;
}

void CMainDialog::updateString( int idx, LPCWSTR str )
{
	if(idx >= 0 && idx < m_ctlList->getItemsCount())
	{
		int strIDX = (int) m_ctlList->getItemData(idx);

		MAKE_STR(m_strings[strIDX].locString, str);
		updateListText(strIDX);
		if(!m_bChanged)
		{
			find(L"btnSave")->set_disabled(FALSE);
			m_bChanged = TRUE;
		}
		for(int i=idx + 1; i < m_strings.GetCount(); i++)
		{
			if(!lstrcmp(m_strings[strIDX].defString, m_strings[i].defString) && !m_strings[i].locString)
			{
				MAKE_STR(m_strings[i].locString, str);
				updateListText(i);
			}
		}
	}
}

void CMainDialog::updateListText( int idx )
{
	int count = m_ctlList->getItemsCount();
	for(int i=0; i < count; i++)
	{
		int lstData = (int) m_ctlList->getItemData(i);
		if(lstData == idx)
		{
			m_ctlList->setItemText(i, 1, m_strings[idx].locString);
			break;
		}
	}
}

IXMLDOMNode* xmlAppendNode(IXMLDOMDocument* doc, IXMLDOMNode* root, LPWSTR nodeName)
{
	IXMLDOMNode* newNode = NULL;
	IXMLDOMNode* insertedNode = NULL;
	HRESULT hr = doc->createNode(CComVariant(NODE_ELEMENT), CComBSTR(nodeName), NULL, &newNode);
	if(newNode)
	{
		root->appendChild(newNode, &insertedNode);
		newNode->Release();
	}
	return insertedNode;
}

BOOL CMainDialog::OnSave()
{
	if(!Save())
	{
		WCHAR msg[500];
		wsprintf(msg, L"Impossible to save strings. Make sure you have the write permissions for this file: \n%s", m_outFile);
		MessageBox(m_hWnd, msg, L".:: xuilib translator ::.", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	find(L"btnSave")->set_disabled(TRUE);
	m_bChanged = FALSE;

	return TRUE;
}

BOOL CMainDialog::Save()
{
	LPWSTR path = NULL;
	MAKE_STR(path, m_outFile);
	PathRemoveFileSpec(path);
	PathRemoveBackslash(path);

	SHCreateDirectory(m_hWnd, path);

	FREE_CLEAR_STR(path);

	CComPtr<IXMLDOMDocument> docOut;
	docOut.CoCreateInstance(CLSID_DOMFreeThreadedDocument, NULL, CLSCTX_INPROC_SERVER);
	docOut->put_async(VARIANT_FALSE);
	docOut->put_validateOnParse(VARIANT_FALSE);
	docOut->put_resolveExternals(VARIANT_FALSE);
	VARIANT_BOOL bSuccess = VARIANT_FALSE;

	IXMLDOMProcessingInstruction* pi = NULL;;
	HRESULT hr = docOut->createProcessingInstruction(CComBSTR(TEXT("xml")), CComBSTR(TEXT("version=\"1.0\" encoding=\"UTF-16\"")), &pi);
	if(hr != S_OK) return FALSE;
	IXMLDOMNode* insertedNode = NULL;
	hr = docOut->appendChild(pi, &insertedNode);
	pi->Release();
	if(insertedNode)
	{
		insertedNode->Release();
	}
	if(hr != S_OK) return FALSE;

	IXMLDOMNode* root = xmlAppendNode(docOut, docOut, L"strings");
	if(!insertedNode) return FALSE;

	for(int i=0; i < m_strings.GetCount(); i++)
	{
		if(m_strings[i].locString)
		{
			insertedNode = xmlAppendNode(docOut, root, L"string");
			if(insertedNode)
			{
				CComQIPtr<IXMLDOMElement> spXMLChildElement;
				spXMLChildElement = insertedNode;
				spXMLChildElement->setAttribute(L"id",		CComVariant(m_strings[i].id));
				if(m_strings[i].attr)
				{
					spXMLChildElement->setAttribute(L"attr",	CComVariant(m_strings[i].attr));
				}
				spXMLChildElement->setAttribute(L"text",	CComVariant(m_strings[i].defString));
				spXMLChildElement->setAttribute(L"loc",		CComVariant(m_strings[i].locString));
				insertedNode->Release();
			}
		}
	}

	if(docOut->save(CComVariant(m_outFile)) != S_OK)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMainDialog::OnEndDialog( UINT code )
{
	if(code == IDOK)
	{
		return FALSE;
	}
	if(m_bChanged)
	{
		int ret = MessageBox(m_hWnd, L"Do you want to save changes?", L".:: xuilib translator ::.", MB_YESNOCANCEL | MB_ICONQUESTION);
		if(ret == IDCANCEL) return FALSE;
		if(ret == IDYES)
		{
			if(!OnSave())
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CMainDialog::OnImport( LPWSTR fileName )
{
	CComPtr<IXMLDOMDocument> doc;
	doc.CoCreateInstance(CLSID_DOMFreeThreadedDocument, NULL, CLSCTX_INPROC_SERVER);
	doc->put_async(VARIANT_FALSE);
	doc->put_validateOnParse(VARIANT_FALSE);
	doc->put_resolveExternals(VARIANT_FALSE);
	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	if(doc->load(CComVariant(fileName), &bSuccess) == S_OK && bSuccess == VARIANT_TRUE)
	{
		IXMLDOMNode* root = xmlOpenNode(doc, TEXT("translation"));
		if(root)
		{
			IXMLDOMNode* strNode = NULL;
			root->get_firstChild(&strNode);
			while(strNode)
			{
				IXMLDOMNode* nodeSRC = xmlOpenNode(strNode, TEXT("src"));
				IXMLDOMNode* nodeDST = xmlOpenNode(strNode, TEXT("dst"));

				if(nodeSRC && nodeDST)
				{
					LPWSTR src = xmlGetNodeText(nodeSRC);
					LPWSTR dst = xmlGetNodeText(nodeDST);
					
					if(src && dst)
					{
						importString(src, dst);
					}

					FREE_CLEAR_STR(src);
					FREE_CLEAR_STR(dst);
				}
				if(nodeSRC)		nodeSRC->Release();
				if(nodeDST)		nodeDST->Release();

				XML_NEXTNODE(strNode);
			}

			root->Release();
		}
	} else
	{
		FILE* fl = _tfopen(fileName, TEXT("rb"));
		if(fl)
		{
			while(!feof(fl))
			{
				TCHAR locStr[1024];
				if(_fgetts(locStr, 1024, fl))
				{
					locStr[lstrlen(locStr)-1] = 0;

					TCHAR* p = _tcsstr(locStr, TEXT("="));
					if(p)
					{
						TCHAR* enStr = new TCHAR[p - locStr + 1];
						_tcsncpy(enStr, locStr, p - locStr);
						enStr[p - locStr] = 0;
						
						for(int i=0; i < m_strings.GetCount(); i++)
						{
							if(!m_strings[i].locString && !lstrcmp(enStr, m_strings[i].defString))
							{
								updateString(i, p + 1);
							}
						}
						delete enStr;
					}
				}
			}
			fclose(fl);
		}
	}
	return TRUE;
}

BOOL CMainDialog::OnFindChanged()
{
	fillList();
	return TRUE;
}

void CMainDialog::clearDialogs()
{
	for(int i=0; i < m_dialogs.GetCount(); i++)
	{
		delete m_dialogs[i];
	}
	m_dialogs.Clear();
}

void CMainDialog::fillDialogs()
{
	m_ctlDialogs->clearItems();
	m_ctlDialogs->addItem(-1, L"<All dialogs>");
	for(int i=0; i < m_dialogs.GetCount(); i++)
	{
		m_ctlDialogs->addItem(i, m_dialogs[i]->getName());
	}
	m_ctlDialogs->value_INT(-1);
}

BOOL CMainDialog::OnTestDlg()
{
	int dlgIDX = m_ctlDialogs->value_INT();
	showTranslatedDialog(dlgIDX);
	return TRUE;
}

BOOL CMainDialog::OnShowOriginalDlg()
{
	int dlgIDX = m_ctlDialogs->value_INT();
	showOriginalDialog(dlgIDX);
	return TRUE;
}

int CMainDialog::getSavedWidth( int defWidth )
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(m_engine->get_hInstance(), path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	lstrcat(path, TEXT("settings.ini"));

	int width = GetPrivateProfileInt(L"MainWindow", L"width", defWidth, path);
	return max(defWidth, width);
}

int CMainDialog::getSavedHeight( int defHeight )
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(m_engine->get_hInstance(), path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	lstrcat(path, TEXT("settings.ini"));

	int height = GetPrivateProfileInt(L"MainWindow", L"height", defHeight, path);
	return max(defHeight, height);
}

void CMainDialog::saveSizes( int width, int height )
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(m_engine->get_hInstance(), path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	lstrcat(path, TEXT("settings.ini"));

	WCHAR str[50];

	_itow_s(width, str, 50, 10);
	WritePrivateProfileString(L"MainWindow", L"width", str, path);
	_itow_s(height, str, 50, 10);
	WritePrivateProfileString(L"MainWindow", L"height", str, path);
}

BOOL CMainDialog::OnListSelected( LPNMHDR phdr )
{
	STR_INFO strInfo = {0};
	getStringInfo(m_ctlList->getSelectedIndex(), &strInfo);

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
	find(L"urlTranslated")->set_hidden(strInfo.dialog ? FALSE : TRUE);
	find(L"urlOriginal")->set_hidden(strInfo.dialog ? FALSE : TRUE);

	return TRUE;
}

void CMainDialog::showOriginalDialog( int dlgIDX )
{
	if(dlgIDX >= 0)
	{
		CMyXUIEngine engine(m_engine->get_hInstance(), m_projectFile);
		m_dialogs[dlgIDX]->testDialog(&engine, m_hWnd);
	}
}

void CMainDialog::showTranslatedDialog( int dlgIDX )
{
	if(dlgIDX >= 0)
	{
		if(m_bChanged)
		{
			int ret = MessageBox(m_hWnd, L"Do you want to save changes?", L".:: xuilib translator ::.", MB_YESNOCANCEL | MB_ICONQUESTION);
			if(ret == IDCANCEL) return;
			if(ret == IDYES)
			{
				if(!OnSave())
				{
					return;
				}
			}
		}
		CMyXUIEngine engine(m_engine->get_hInstance(), m_projectFile);
		engine.loadStrings(m_outFile);
		m_dialogs[dlgIDX]->testDialog(&engine, m_hWnd);
	}
}

BOOL CMainDialog::OnUrlTranslated()
{
	int strIDX = (int) m_ctlList->getSelected();
	int idx = -1;
	if(strIDX >= 0)
	{
		for(int i=0; i < m_dialogs.GetCount(); i++)
		{
			if(m_dialogs[i]->checkLOCID(m_strings[strIDX].id))
			{
				idx = i;
				break;
			}
		}
	}
	showTranslatedDialog(idx);

	return TRUE;
}

BOOL CMainDialog::OnUrlOriginal()
{
	int strIDX = (int) m_ctlList->getSelected();
	int idx = -1;
	if(strIDX >= 0)
	{
		for(int i=0; i < m_dialogs.GetCount(); i++)
		{
			if(m_dialogs[i]->checkLOCID(m_strings[strIDX].id))
			{
				idx = i;
				break;
			}
		}
	}
	showOriginalDialog(idx);

	return TRUE;
}

BOOL CMainDialog::OnPrevDlg()
{
	int idx = m_ctlDialogs->value_INT();
	if(idx >= 0)
	{
		m_ctlDialogs->value_INT(idx - 1);
		OnFindChanged();
	}
	return TRUE;
}

BOOL CMainDialog::OnNextDlg()
{
	int idx = m_ctlDialogs->value_INT();
	if(idx < m_dialogs.GetCount() - 1)
	{
		m_ctlDialogs->value_INT(idx + 1);
		OnFindChanged();
	}
	return TRUE;
}

void CMainDialog::getStringInfo( int idx, STR_INFO* info )
{
	ZeroMemory(info, sizeof(STR_INFO));
	int strIDX = (int) m_ctlList->getItemData(idx);
	if(strIDX >= 0)
	{
		info->strID = m_strings[strIDX].id;
		info->attr  = m_strings[strIDX].attr;

		for(int i=0; i < m_dialogs.GetCount(); i++)
		{
			if(m_dialogs[i]->checkLOCID(info->strID))
			{
				info->dialog =  m_dialogs[i]->getName();
				break;
			}
		}
	}
}

void CMainDialog::importString( LPWSTR src, LPWSTR dst )
{
	if(src && dst)
	{
		for(int i=0; i < m_strings.GetCount(); i++)
		{
			LPWSTR strClear1 = clearStringForImport(src);
			LPWSTR strClear2 = clearStringForImport(m_strings[i].defString);

			if(!m_strings[i].locString && !lstrcmp(strClear1, strClear2))
			{
				updateString(i, dst);
			}
			delete strClear1;
			delete strClear2;
		}
	}
}

LPWSTR CMainDialog::clearStringForImport( LPWSTR src )
{
	LPWSTR strNoAmp = new WCHAR[lstrlen(src) + 1];
	int ii = 0;
	for(int i=0; src[i] && src[i] != L'\t'; i++)
	{
		if(src[i] != L'&')
		{
			strNoAmp[ii++] = src[i];
		}
	}
	strNoAmp[ii] = 0;
	return strNoAmp;
}

BOOL CMainDialog::OnExport( LPWSTR fileName )
{
	HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		for(int i=0; i < m_strings.GetCount(); i++)
		{
			int len = lstrlen(m_strings[i].defString);
			LPSTR strA = new CHAR[len + 1];
			WideCharToMultiByte(CP_ACP, 0, m_strings[i].defString, -1, strA, len + 1, NULL, NULL);
			DWORD cbWrite = 0;
			WriteFile(hFile, strA, len, &cbWrite, NULL);
			LPSTR eol = "\r\n";
			WriteFile(hFile, eol, 2, &cbWrite, NULL);
			delete strA;
		}
		CloseHandle(hFile);
	}
	return TRUE;
}