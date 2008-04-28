#pragma	   once
#include <atlbase.h>
#include <MsXml.h>

template <typename T> inline BOOL xmlGetAttributeValue(IXMLDOMNode* item, LPCTSTR attrName, CComVariant& var)
{
	var.Clear();
	CComQIPtr<IXMLDOMElement> spXMLChildElement;
	spXMLChildElement = item;
	HRESULT hr = spXMLChildElement->getAttribute(CComBSTR(attrName), &var);
	if(hr == S_OK)
	{
		if(var.ChangeType(CVarTypeInfo<T>::VT) == S_OK)
		{
			return TRUE;
		}
	}
	return FALSE;
}

inline IXMLDOMNode* xmlOpenNode(IXMLDOMNode* root, LPCTSTR path)
{
	if(!root) return NULL;
	IXMLDOMNode* item = NULL;
	CComBSTR bstrSS(path);
	HRESULT hr = root->selectSingleNode(bstrSS, &item);
	if(hr == S_OK) return item;
	return NULL;
}

#define XML_GETVARIANTVALUE(var, type) (var.*CVarTypeInfo<type>::pmField)
#define XML_NEXTNODE(item) if(item){\
	IXMLDOMNode* nextItem = NULL;\
	item->get_nextSibling(&nextItem);\
	item->Release();\
	item = nextItem;\
	}

template <typename T> inline T xmlGetAttributeValueNonSTR(IXMLDOMNode* item, LPCTSTR attrName, T defValue)
{
	CComVariant var;
	CComQIPtr<IXMLDOMElement> spXMLChildElement;
	spXMLChildElement = item;
	HRESULT hr = spXMLChildElement->getAttribute(CComBSTR(attrName), &var);
	if(hr == S_OK)
	{
		if(var.ChangeType(CVarTypeInfo<T>::VT) == S_OK)
		{
			return XML_GETVARIANTVALUE(var, T);
		}
	}
	return defValue;
}

inline LPWSTR xmlGetAttributeSTR(IXMLDOMNode* item, LPCTSTR attrName)
{
	CComVariant var;
	CComQIPtr<IXMLDOMElement> spXMLChildElement;
	spXMLChildElement = item;
	HRESULT hr = spXMLChildElement->getAttribute(CComBSTR(attrName), &var);
	if(hr == S_OK)
	{
		if(var.ChangeType(VT_BSTR) == S_OK)
		{
			LPWSTR ret = new WCHAR[lstrlen(var.bstrVal) + 1];
			lstrcpy(ret, var.bstrVal);
			return ret;
		}
	}
	return NULL;
}

inline LPWSTR xmlGetNodeText(IXMLDOMNode* item)
{
	BSTR txt = NULL;
	item->get_text(&txt);
	if(txt)
	{
		LPWSTR ret = new WCHAR[lstrlen(txt) + 1];
		lstrcpy(ret, txt);
		SysFreeString(txt);
		return ret;
	}
	return NULL;
}


inline LPWSTR xmlGetLocalizedAttributeSTR(IXMLDOMNode* item, LPCTSTR attrName, LPCTSTR loc)
{
	if(!loc)
	{
		CComVariant var;
		CComQIPtr<IXMLDOMElement> spXMLChildElement;
		spXMLChildElement = item;
		HRESULT hr = spXMLChildElement->getAttribute(CComBSTR(attrName), &var);
		if(hr == S_OK)
		{
			if(var.ChangeType(VT_BSTR) == S_OK)
			{
				LPWSTR ret = new WCHAR[lstrlen(var.bstrVal) + 1];
				lstrcpy(ret, var.bstrVal);
				return ret;
			}
		}
	} else
	{
		LPWSTR ret = new WCHAR[lstrlen(loc) + 1];
		lstrcpy(ret, loc);
		return ret;
	}
	return NULL;
}

inline BOOL xmlGetAttributeValueBOOL(IXMLDOMNode* item, LPCTSTR attrName, BOOL defValue)
{
	LPWSTR val = xmlGetAttributeSTR(item, attrName);
	if(val)
	{
		if(!StrCmpI(val, L"true"))
		{
			delete val;
			return TRUE;
		} else if(!StrCmpI(val, L"false"))
		{
			delete val;
			return FALSE;
		}
		delete val;
	}
	return defValue;
}

inline int xmlGetAttributeValueSTRArray(IXMLDOMNode* item, LPCTSTR attrName, int defValue, LPWSTR array)
{
	LPWSTR val = xmlGetAttributeSTR(item, attrName);
	if(val)
	{
		LPWSTR ar = array;
		for(int i=0; ar[0]; i++)
		{
			if(!StrCmpI(val, ar))
			{
				delete val;
				return i;
			}
			ar += lstrlen(ar) + 1;
		}
		delete val;
	}
	return defValue;
}
