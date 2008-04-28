// HTMLViewer
//
// © 2004 Karl Runmo ,runmo@hotmail.com
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.

#include "htmlviewer.h"
#include "HTMLCODES.h"
#include <math.h>
#include <stdio.h>
#include <comdef.h>
#include <TCHAR.h>

#define ABSOLUTE_ALIGN

struct HTMLTAGS
{
	TCHAR szName[25];
	int bEndRequired;
	int nParent;
};
TCHAR *pszTags[] = {
	TEXT("<br"),			//  0
	TEXT("<i"),			//  1
	TEXT("</i"),			//  2
	TEXT("<b"),			//  3
	TEXT("</b"),			//  4
	TEXT("<strong"),		//  5
	TEXT("</strong"),		//  6
	TEXT("<u"),			//  7
	TEXT("</u"),			//  8
	TEXT("<a"),			//  9
	TEXT("</a"),			// 10
	TEXT("<small"),		// 11
	TEXT("</small"),		// 12
	TEXT("<big"),			// 13
	TEXT("</big"),		// 14
	TEXT("<center"),		// 15
	TEXT("</center"),		// 16
	TEXT("<p"),			// 17
	TEXT("<font"),		// 18
	TEXT("</font"),		// 19
	TEXT("<img"),			// 20
	TEXT("<h1"),			// 21
	TEXT("</h1"),			// 22
	TEXT("<div"),			// 23
	TEXT("</div"),		// 24
	TEXT("<hr"),			// 25
	TEXT("<table"),		// 26
	TEXT("</table"),		// 27
	TEXT("<tr"),			// 28
	TEXT("</tr"),			// 29
	TEXT("<td"),			// 30
	TEXT("</td"),			// 31
	TEXT("<h2"),			// 32
	TEXT("</h2"),			// 33
	TEXT("<h3"),			// 34
	TEXT("</h3"),			// 35
	TEXT("<pre"),			// 36
	TEXT("</pre"),		// 37
	TEXT("<input"),		// 38
	TEXT("<ul"),			// 39
	TEXT("<ol"),			// 40
	TEXT("<li"),			// 41
	TEXT("</ul"),			// 42
	TEXT("</ol"),			// 43
	TEXT("<sub"),			// 44
	TEXT("</sub"),		// 45
	TEXT("<sup"),			// 46
	TEXT("</sup"),		// 47
	TEXT("<strike"),		// 48
	TEXT("</strike"),		// 49
	TEXT("<select"),		// 50
	TEXT("</select"),		// 51
	TEXT("<option"),		// 52
	TEXT("<textarea"),	// 53
	TEXT("</textarea"),	// 54
	TEXT("<th"),			// 55
	TEXT("</th"),			// 56
	TEXT("<form"),		// 57
	TEXT("</form"),		// 58
	TEXT("<blockquote"),	// 59
	TEXT("</blockquote"),	// 60
	TEXT("</p"),	        // 61
	NULL};

void CHTMLItem::GetStringSize(TCHAR *szFont,int nFontSize, int bBold, int bItalic, SIZE *sc,TCHAR *szString,int nMaxWidth,int nLen)
{
	HDC hDC = GetDC(NULL);
	HFONT hf = CreateFont(nFontSize,0,0,0,bBold?FW_BOLD:FW_NORMAL,bItalic,0,0,!_tcsicmp(szFont,TEXT("Wingdings"))?SYMBOL_CHARSET:0,0,0,0,0,szFont);
	HFONT hfOld = (HFONT)SelectObject(hDC,hf);
	GetTextExtentPoint(hDC,szString,nLen==-1?lstrlen(szString):nLen,sc);
//	sc->cy+=2;
	SelectObject(hDC,hfOld);
	DeleteObject(hf);
	ReleaseDC(NULL,hDC);
}
void CHTMLItem::Offset(int nX, int nY)
{
	int i;
	for(i=0;i<m_nSubItems;i++){
		m_ppSubItems[i]->m_pPos.x+=nX;
		m_ppSubItems[i]->m_pPos.y+=nY;
		m_ppSubItems[i]->Offset(nX, nY);
	}

}
int g_nTexts=0;
// Text-object
// Represents texts
CHTMLItem *CHTMLItem::NewText(TCHAR *szText,int nFrames,int nFrameSize, COLORREF colFrame,TCHAR *szFont,int nFontSize,
	int nFontBold,int nFontItalic,COLORREF colText,COLORREF colBkg,int nHFormat,int nVFormat,int nLink,BOOL bNoBreak)
{
	CHTMLItem *pRet = new CHTMLItem;
	pRet->m_nType = TEXT;
	pRet->m_nFrames = nFrames;
	pRet->m_nFrameSize = nFrameSize;
	pRet->m_colFrame = colFrame;
	lstrcpy(pRet->m_szFont,szFont);
	if(_tcschr(pRet->m_szFont,TEXT(',')))
		*_tcschr(pRet->m_szFont,TEXT(','))=0;
	pRet->m_nFontSize = nFontSize;
	pRet->m_nFontBold = nFontBold;
	pRet->m_nFontItalic = nFontItalic;
	pRet->m_colText = colText;
	pRet->m_colBkg = colBkg;
	pRet->m_nHFormat = nHFormat;
	pRet->m_nVFormat = nVFormat;
	pRet->m_szText = new TCHAR[lstrlen(szText)+1];
	pRet->m_nLink = nLink;
	pRet->m_bNoBreak = bNoBreak;
	lstrcpy(pRet->m_szText,szText);
	ConvertFromHTMLCodes(pRet->m_szText);
	GetStringSize(pRet->m_szFont,nFontSize,nFontBold,nFontItalic,&pRet->m_scBounds,pRet->m_szText,0,lstrlen(pRet->m_szText));

	g_nTexts++;
	return pRet;
}
// Break-object
// Represents <BR> and <HR> and are also used for the outer frame of tables
CHTMLItem *CHTMLItem::NewBreak(int nHeight,int nWidth,int nHFormat,int bClear)
{
	CHTMLItem *pRet = new CHTMLItem();
	pRet->m_nType = BREAK;
	pRet->m_scBounds.cy = nHeight;
	pRet->m_scBounds.cx = nWidth;
	pRet->m_nHFormat = nHFormat;
	if(nWidth)
		pRet->m_nFrames=15;
	pRet->m_nFrameSize=1;
	pRet->m_colFrame = -1;
	pRet->m_bClear = bClear;
	return pRet;
}
// COMPOUND-object
// Represents <TD> and <DIV> as a new HTML document
CHTMLItem *CHTMLItem::NewCompound(int nRow, int nCol,int nWidth,int nHFormat,int nVFormat,COLORREF colBkg,int bBorder,int nColSpan,int nRowSpan,int nCellPadding,int nCellSpacing,int nTableWidth,int nBmpBkg,int nHeight,int nTableHeight,COLORREF colBorder,int nAbsoluteAlign)
{
	CHTMLItem *pRet = new CHTMLItem();
	pRet->m_nType = COMPOUND;
	pRet->m_nFrames = (bBorder?15:0);
	pRet->m_colFrame = colBorder;
	pRet->m_nFrameSize = (bBorder?1:0);
	pRet->m_colBkg = colBkg;
	pRet->m_nRow = nRow;
	pRet->m_nCol = nCol;
	pRet->m_nWidth = nWidth;
	pRet->m_nColSpan = nColSpan;
	if(pRet->m_nColSpan<1)
		pRet->m_nColSpan=1;
	else if(pRet->m_nColSpan>1000)
		pRet->m_nColSpan = 1000;
	pRet->m_nRowSpan = nRowSpan;
	if(pRet->m_nRowSpan<1)
		pRet->m_nRowSpan=1;
	else if(pRet->m_nRowSpan>1000)
		pRet->m_nRowSpan = 1000;
	pRet->m_nCellPadding = nCellPadding + (bBorder?1:0);
	pRet->m_nCellSpacing = nCellSpacing;
	pRet->m_nTableWidth = nTableWidth;
	pRet->m_nTableFrameSize = bBorder;
	pRet->m_nHFormat = nHFormat;
	pRet->m_nVFormat = nVFormat;
	pRet->m_nBmpBkg = nBmpBkg;
	pRet->m_nHeight = nHeight;
	pRet->m_nTableHeight = nTableHeight;
	pRet->m_nAbsoluteAlign = nAbsoluteAlign;
	return pRet;
}

CHTMLItem::CHTMLItem()
{
	m_nType = -1;
	m_scBounds.cx = m_scBounds.cy = 0;
	m_pPos.x=m_pPos.y=0;
	m_nFrames = 0;
	m_nFrameSize = 0;
	m_colFrame = 0;
	*m_szFont = 0;
	m_nFontSize = 0;
	m_nFontBold = 0;
	m_nFontItalic = 0;
	m_colText = 0;
	m_colBkg = -1;
	m_nHFormat = 0;
	m_nVFormat = 0;
	m_szText = NULL;
	m_nBmp = -1;
	m_nBmpBkg = -1;
	m_ppSubItems = NULL;
	m_nSubItems = 0;
	m_nRow = 0;
	m_nCol = 0;
	m_nWidth = 0;
	m_nColSpan = 0;
	m_nRowSpan = 0;
	m_nCellPadding = 0;
	m_nCellSpacing = 0;
	m_nLink = -1;
	m_nTableWidth = 0;
	m_nTableFrameSize = 0;
	m_hEvent = NULL;
	m_nHeight=0;
	m_nTableHeight=0;
	m_nMinColumnWidth = -1;
	m_nMaxColumnWidth = -1;
	m_nAMinColumnWidth = -1;
	m_nColumnWidth = -1;
	m_bSelected = FALSE;
	m_bNoBreak = FALSE;
	m_nControlID = -1;
}
CHTMLItem::~CHTMLItem()
{
	int i;
	for(i=0;i<m_nSubItems;i++)
		delete m_ppSubItems[i];
	free(m_ppSubItems);
	m_nSubItems = 0;
	m_ppSubItems = NULL;
	if(m_szText)
		delete m_szText;
}

void CHTMLViewer::AddBaseToUrl(TCHAR *szUrl,TCHAR *szFullUrl,TCHAR *szBaseUrl)
{
	lstrcpy(szFullUrl,szUrl);
/*	int nPPos;
	for(nPPos=0;szUrl[nPPos] && szUrl[nPPos]!=TEXT('/');nPPos++)
		if(!_tcsncmp(&szUrl[nPPos],TEXT("://"),3))
			break;
	if(!szUrl[nPPos] || szUrl[nPPos]==TEXT('/'))
		nPPos=-1;
	if(nPPos>=0)
		lstrcpy(szFullUrl,szUrl);
	else{
		lstrcpy(szFullUrl,szBaseUrl);
		if(_tcsrchr(szFullUrl,TEXT('?')))
			*_tcsrchr(szFullUrl,TEXT('?'))=0;
		for(nPPos=0;szFullUrl[nPPos];nPPos++)
			if(!_tcsncmp(&szFullUrl[nPPos],TEXT("://"),3))
				break;
		if(!szFullUrl[nPPos])
			nPPos=0;
		int i;
		for(i=0;szFullUrl[i];i++)
			if(szFullUrl[i]==TEXT('\\'))
				szFullUrl[i]=TEXT('/');
		if(!_tcschr(&szFullUrl[nPPos+3],TEXT('/')))
			lstrcat(szFullUrl,TEXT("/"));
		if(_tcsrchr(szFullUrl,TEXT('/')) && _tcsncmp(_tcsrchr(szFullUrl,TEXT('/'))-3,TEXT("://"),3))
			*(_tcsrchr(szFullUrl,TEXT('/'))+1)=0;
		else if(szFullUrl[lstrlen(szFullUrl)-1]!=TEXT('/'))
			lstrcat(szFullUrl,TEXT("/"));
		while(!_tcsncmp(szUrl,TEXT("../"),3)){
			if(szFullUrl[lstrlen(szFullUrl)-1]==TEXT('/')){
				szFullUrl[lstrlen(szFullUrl)-1]=0;
			}
			if(_tcsrchr(szFullUrl,TEXT('/')) && (int)_tcsrchr(szFullUrl,TEXT('/'))-(int)szFullUrl>8)
				lstrcpy(_tcsrchr(szFullUrl,TEXT('/'))+1,TEXT("\0"));
			szUrl+=3;
		}
		if(*szUrl==TEXT('/') && _tcschr(&szFullUrl[nPPos+4],TEXT('/')))
			*_tcschr(&szFullUrl[nPPos+3],TEXT('/'))=0;
		if(*szUrl!=TEXT('/') && szFullUrl[lstrlen(szFullUrl)-1]!=TEXT('/'))
			lstrcat(szFullUrl,TEXT("/"));
		for(i=lstrlen(szFullUrl);i<1023 && *szUrl;i++)
			szFullUrl[i] = *(szUrl++);
		szFullUrl[i]=0;
		if(!_tcsncmp(szFullUrl,TEXT("file://"),nPPos+3))
			for(i=nPPos+3;szFullUrl[i];i++)
				if(szFullUrl[i]==TEXT('/'))
					szFullUrl[i]=TEXT('\\');
	}*/
}
void CHTMLViewer::GetProperties(TCHAR *szData,CHTMLStringTable &st)
{
	int i, j, k, e;
	st.Reset();
	for(e=0;szData[e] && szData[e]!=TEXT('>');e++);
	for(i=0;i<e && szData[i]!=TEXT(' ');i++);
	while(i<e){
		while(szData[i]==TEXT(' ') && i<e)
			i++;
		j=i;
		for(;i<e && szData[i]!=TEXT('=') && szData[i]!=TEXT(' ');i++);
		st.AddRow();
		st.AddString(st.GetCount()-1,&szData[j],i-j);

		if(szData[i]==TEXT('=')){
			i++;
			while(szData[i]==TEXT(' ') && i<e)
				i++;
			if(szData[i]==TEXT('\'') || szData[i]==TEXT('"')){
				k=szData[i];
				i++;
			}
			else
				k=0;
			j=i;
			for(;i<e && ((k && szData[i]!=k) || (!k && szData[i]!=TEXT(' ')));i++);
			st.AddString(st.GetCount()-1,&szData[j],i-j);
			if(k && i<e)
				i++;
		}
	}
}
void CHTMLViewer::AddItem(CHTMLItem **&ppItems, int &nItems)
{
	nItems++;
	m_nItemCount++;
	ppItems = (CHTMLItem**)realloc(ppItems,sizeof(CHTMLItem*)*nItems);
	if(!ppItems)
		Beep(1000,10);
}
COLORREF CHTMLViewer::GetCol(TCHAR *szData)
{
	int i;
	for(i=0;g_ColTable[i].szName;i++)
		if(!_tcsicmp(szData,g_ColTable[i].szName))
			return g_ColTable[i].colVal;
	TCHAR *s = szData;
	if(*s==TEXT('#'))
		s++;
	TCHAR szTmp[3]={0};
	int r, g, b;
	szTmp[0] = s[0];
	szTmp[1] = s[1];
	_stscanf(szTmp,TEXT("%x"),&r);
	szTmp[0] = s[2];
	szTmp[1] = s[3];
	_stscanf(szTmp,TEXT("%x"),&g);
	szTmp[0] = s[4];
	szTmp[1] = s[5];
	_stscanf(szTmp,TEXT("%x"),&b);
	return RGB(r,g,b);
}
void CHTMLViewer::AddHTML(TCHAR *szHTMLData, CHTMLItem **&ppItems, int &nItems,TCHAR *szFont,int nFontSize,COLORREF colText,TCHAR *szBaseUrl,int nLevel,int nHFormat,BOOL bNoBreak)
{
	BOOL bTextStarted=0;
	if(nLevel>m_nLevelCount)
		m_nLevelCount = nLevel;
	BOOL bBold=0, bItalic=0, bUnderline=0, bLink=0, bSmall=0, bBig=0, bCenter=0, bSubText=0, bSupText=0, bStrike=0;
	int nCurrLink=-1;
	int nCurrFontSize = nFontSize;
	COLORREF colCurText = colText;
	TCHAR szCurrFont[100];
	lstrcpy(szCurrFont,szFont);
	int i=0, t;
	int nLen;
	if(!nLevel)
		nLen = lstrlen(szHTMLData);
	while(szHTMLData[i]==TEXT(' '))
		i++;
	for(i=0;szHTMLData[i];i++){
		if(szHTMLData[i]==TEXT('<')){
			if((!_tcsnicmp(&szHTMLData[i],TEXT("<div"),4) && szHTMLData[i+4]==TEXT(' ')) || (!_tcsnicmp(&szHTMLData[i],TEXT("<p"),2) && szHTMLData[i+2]==TEXT(' '))){
				BOOL bP=FALSE;
				int nRestorePos = i;
				if(!_tcsnicmp(&szHTMLData[i],TEXT("<p"),2))
					bP=TRUE;
				CHTMLStringTable test;
				GetProperties(&szHTMLData[i],test);
				COLORREF colTableBkg=-1;
				BOOL bTableBorder=0;
				int nTableHFormat=nHFormat;
				if(bCenter)
					nTableHFormat=1;
				for(int a=0;a<test.GetCount();a++){
					if(!_tcsicmp(test[a][0],TEXT("bgcolor")))
						colTableBkg = GetCol(test[a][1]);
					else if(!_tcsicmp(test[a][0],TEXT("border")))
						bTableBorder = _tstoi(test[a][1]);
					else if(!_tcsicmp(test[a][0],TEXT("align"))){
						if(!_tcsicmp(test[a][1],TEXT("left")))
							nTableHFormat = 0;
						else if(!_tcsicmp(test[a][1],TEXT("right")))
							nTableHFormat = 1;
						else if(!_tcsicmp(test[a][1],TEXT("center")) || !_tcsicmp(test[a][1],TEXT("middle")))
							nTableHFormat = 2;
						else if(!_tcsicmp(test[a][1],TEXT("justify")))
							nTableHFormat = 3;
					}
				}
				int e;
				for(;szHTMLData[i] && szHTMLData[i]!=TEXT('>');i++);
				if(szHTMLData[i]==TEXT('>'))
					i++;
				int bSub=0;
				for(e=i;szHTMLData[e];e++){
					if(!_tcsnicmp(&szHTMLData[e],TEXT("</table"),7) || (!bP && !_tcsnicmp(&szHTMLData[e],TEXT("</div"),5))){
						if(bSub)
							bSub--;
						else
							break;
					}
					else if(!_tcsnicmp(&szHTMLData[e],TEXT("<table"),6) || (!bP && !_tcsnicmp(&szHTMLData[e],TEXT("<div"),4))){
						bSub++;
					}
					else if(bP && !bSub && (!_tcsnicmp(&szHTMLData[e],TEXT("</p"),3) || !_tcsnicmp(&szHTMLData[e],TEXT("<p"),2) || !_tcsnicmp(&szHTMLData[e],TEXT("<hr"),3)))
						break;
				}
				bSub=0;
				if(!szHTMLData[e])
					for(e=i;szHTMLData[e];e++){
						if(!_tcsnicmp(&szHTMLData[e],TEXT("</table"),7)){
							if(bSub)
								bSub--;
						}
						else if(!_tcsnicmp(&szHTMLData[e],TEXT("<table"),6)){
							bSub++;
						}
						if(!bSub)
						if((!bP && !_tcsnicmp(&szHTMLData[e],TEXT("<div"),4)) || (bP && !_tcsnicmp(&szHTMLData[e],TEXT("<p"),2))){
							CHTMLStringTable st;
							GetProperties(&szHTMLData[e],st);
							int a;
							for(a=0;a<st.GetCount();a++)
								if(!_tcsicmp(st[a][0],TEXT("align")))
									break;
							if(a<st.GetCount())
								break;
							bSub++;
						}
					}
				if(nTableHFormat!=nHFormat || (bCenter && nTableHFormat!=1)){
					int Restore = szHTMLData[e];
					szHTMLData[e] = 0;

					AddItem(ppItems, nItems);
					ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,nTableHFormat,0);
					if(bP){
/*						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewText(TEXT(" "),15,2,-1,TEXT("Courier New"),15,
							0,0,0,RGB(255,255,255),nTableHFormat,0,-1,bNoBreak);
*/						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,nTableHFormat,0);
					}
					
					AddHTML(&szHTMLData[i],ppItems,nItems,szCurrFont,nCurrFontSize,colCurText,szBaseUrl,nLevel+1,nTableHFormat,bNoBreak);

					AddItem(ppItems, nItems);
					ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);

					szHTMLData[e] = Restore;
					if(szHTMLData[e] && !(szHTMLData[e]==TEXT('<') && szHTMLData[e+1]!=TEXT('/'))){
						e++;
						for(i=e;szHTMLData[i] && szHTMLData[i]!=TEXT('>');i++);
						if(!szHTMLData[i])
							i--;
					}
					else
						i=e-1;
				}
				else
					i=nRestorePos;
			}
			else if(!_tcsnicmp(&szHTMLData[i],TEXT("<pre"),4)){
				int e;
				for(;szHTMLData[i] && szHTMLData[i]!=TEXT('>');i++);
				if(szHTMLData[i]==TEXT('>'))
					i++;
				int bSub=0;
				for(e=i;szHTMLData[e];e++){
					if(!_tcsnicmp(&szHTMLData[e],TEXT("</pre"),5)){
						if(bSub)
							bSub--;
						else
							break;
					}
					else if(!_tcsnicmp(&szHTMLData[e],TEXT("<pre"),4))
						bSub++;
				}
				int Restore = szHTMLData[e];
				szHTMLData[e] = 0;

				AddItem(ppItems, nItems);
				ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);
				
				AddItem(ppItems, nItems);
				ppItems[nItems-1] = CHTMLItem::NewCompound(0,0,0,0,0,-1,0,1,1,0,0,0,-1,0,0,0,-1);

				AddHTML(&szHTMLData[i],ppItems[nItems-1]->m_ppSubItems,ppItems[nItems-1]->m_nSubItems,TEXT("Courier New"),nFontSize,colText,szBaseUrl,nLevel+1,0,TRUE);

				AddItem(ppItems, nItems);
				ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);

				szHTMLData[e] = Restore;
				if(szHTMLData[e])
					e++;
				for(i=e;szHTMLData[i] && szHTMLData[i]!=TEXT('>');i++);
				while(szHTMLData[i] && szHTMLData[i+1]==TEXT(' '))
					i++;
				if(!szHTMLData[i])
					i--;
			}
			if(!_tcsnicmp(&szHTMLData[i],TEXT("<table"),6)){
				CHTMLStringTable test;
				GetProperties(&szHTMLData[i],test);
				int nTableWidth=0;
				int nTableHeight=0;
				COLORREF colTableBkg=-1;
				BOOL bTableBorder=0;
				int nCellPadding=4;
				int nCellSpacing=4;
				int nTableBmp=-1;
				BOOL bPrevCenter = bCenter;
				int nPrevHFormat = nHFormat;
				COLORREF colTableBorder=-1;
				int nAbsoluteAlign=-1;
				for(int a=0;a<test.GetCount();a++){
					if(!_tcsicmp(test[a][0],TEXT("bgcolor")))
						colTableBkg = GetCol(test[a][1]);
					else if(!_tcsicmp(test[a][0],TEXT("bordercolor")))
						colTableBorder = GetCol(test[a][1]);
					else if(!_tcsicmp(test[a][0],TEXT("width"))){
						if(!_tcsstr(test[a][1],TEXT("%")))
							nTableWidth = _tstoi(test[a][1]);
						else
							nTableWidth = -_tstoi(test[a][1]);
					}
					else if(!_tcsicmp(test[a][0],TEXT("height")) && !_tcsstr(test[a][1],TEXT("%"))){
						nTableHeight = _tstoi(test[a][1]);
					}
					else if(!_tcsicmp(test[a][0],TEXT("border")))
						bTableBorder = _tstoi(test[a][1]);
					else if(!_tcsicmp(test[a][0],TEXT("cellpadding")))
						nCellPadding = _tstoi(test[a][1]);
					else if(!_tcsicmp(test[a][0],TEXT("cellspacing")))
						nCellSpacing = _tstoi(test[a][1]);
					else if(!_tcsicmp(test[a][0],TEXT("align")) && (!_tcsicmp(test[a][1],TEXT("center")) || !_tcsicmp(test[a][1],TEXT("middle")))){
						bCenter=1;
						nHFormat=2;
					}
					else if(!_tcsicmp(test[a][0],TEXT("align")) && !_tcsicmp(test[a][1],TEXT("right"))){
						nHFormat=1;
						nAbsoluteAlign=1;
					}
					else if(!_tcsicmp(test[a][0],TEXT("align")) && !_tcsicmp(test[a][1],TEXT("left"))){
						nHFormat=0;
						nAbsoluteAlign=0;
					}
				}
				AddItem(ppItems, nItems);
				ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);
				ppItems[nItems-1]->m_nBmpBkg=nTableBmp;
				ppItems[nItems-1]->m_colBkg=colTableBkg;

				int e;
				int nTables=0;
				for(e=i+1;szHTMLData[e];e++){
					if(!_tcsnicmp(&szHTMLData[e],TEXT("<table"),6))
						nTables++;
					if(!_tcsnicmp(&szHTMLData[e],TEXT("</table"),7)){
						if(!nTables)
							break;
						nTables--;
					}
				}
				int nRow=0, nCol=0, nWidth=0, bgCol=-1, nColSpan=1,nRowSpan=1, bgTrCol=-1, TrHFormat=0;
				do{
					for(;i<e && szHTMLData[i] && _tcsnicmp(&szHTMLData[i],TEXT("<tr"),3) && _tcsnicmp(&szHTMLData[i],TEXT("<td"),3) && _tcsnicmp(&szHTMLData[i],TEXT("<th"),3);i++)
					{
					}
					nCol = 0;
					bgCol = bgTrCol = -1;
					do{
						if(!_tcsnicmp(&szHTMLData[i],TEXT("<tr"),3)){
							CHTMLStringTable st;
							GetProperties(&szHTMLData[i],st);
							for(int a=0;a<st.GetCount();a++){
								if(!_tcsicmp(st[a][0],TEXT("bgcolor")))
									bgCol = bgTrCol = GetCol(st[a][1]);
								else if(!_tcsicmp(st[a][0],TEXT("align"))){
									if(!_tcsicmp(st[a][1],TEXT("center")) || !_tcsicmp(st[a][1],TEXT("middle")))
										TrHFormat = 2;
									else if(!_tcsicmp(st[a][1],TEXT("right")))
										TrHFormat = 1;
									else if(!_tcsicmp(st[a][1],TEXT("justify")))
										TrHFormat = 3;
								}
							}
							i++;
						}
						int nTr = 0;
						for(;i<e && szHTMLData[i] && _tcsnicmp(&szHTMLData[i],TEXT("<tr"),3) && _tcsnicmp(&szHTMLData[i],TEXT("</tr"),4) && _tcsnicmp(&szHTMLData[i],TEXT("<td"),3) && _tcsnicmp(&szHTMLData[i],TEXT("<th"),3);i++)
						{
						}
						GetProperties(&szHTMLData[i],test);
						int nTD_HFormat=TrHFormat;
						int nTD_VFormat=0;
						int nBmpBkg=-1;
						int nTD_Height=0;
						int nTD_NoBreak = 0;
						int nTD_Header = 0;
						if(!_tcsnicmp(&szHTMLData[i],TEXT("<th"),3)){
							nTD_Header = 1;
							nTD_HFormat = 2;
						}
						for(int tt=0;tt<test.GetCount();tt++){
							if(!_tcsicmp(test[tt][0],TEXT("width")) && !_tcsstr(test[tt][1],TEXT("%")))
								nWidth = _tstoi(test[tt][1]);
							else if(!_tcsicmp(test[tt][0],TEXT("width")) && _tcsstr(test[tt][1],TEXT("%")) && nTableWidth>0)
								nWidth = _tstoi(test[tt][1])*nTableWidth/100;
							else if(!_tcsicmp(test[tt][0],TEXT("height")) && !_tcsstr(test[tt][1],TEXT("%")))
								nTD_Height = _tstoi(test[tt][1]);
							else if(!_tcsicmp(test[tt][0],TEXT("bgcolor")))
								bgCol = GetCol(test[tt][1]);
							else if(!_tcsicmp(test[tt][0],TEXT("colspan")))
								nColSpan = _tstoi(test[tt][1]);
							else if(!_tcsicmp(test[tt][0],TEXT("rowspan")))
								nRowSpan = _tstoi(test[tt][1]);
							else if(!_tcsicmp(test[tt][0],TEXT("align"))){
								if(!_tcsicmp(test[tt][1],TEXT("right")))
									nTD_HFormat = 1;
								else if(!_tcsicmp(test[tt][1],TEXT("center")) || !_tcsicmp(test[tt][1],TEXT("middle")))
									nTD_HFormat = 2;
								else if(!_tcsicmp(test[tt][1],TEXT("justify")))
									nTD_HFormat = 3;
							}
							else if(!_tcsicmp(test[tt][0],TEXT("valign"))){
								if(!_tcsicmp(test[tt][1],TEXT("bottom")))
									nTD_VFormat = 1;
								if(!_tcsicmp(test[tt][1],TEXT("center")) || !_tcsicmp(test[tt][1],TEXT("middle")))
									nTD_VFormat = 2;
							}
							else if(!_tcsicmp(test[tt][0],TEXT("nobreak")) || !_tcsicmp(test[tt][0],TEXT("nowrap")))
								nTD_NoBreak=1;
						}
						if(nWidth && nTD_NoBreak)
							nTD_NoBreak=0;
						if(!_tcsnicmp(&szHTMLData[i],TEXT("</tr"),4) || !_tcsnicmp(&szHTMLData[i],TEXT("<tr"),3))
							break;
						for(;i<e && szHTMLData[i] && szHTMLData[i]!=TEXT('>');i++);
						if(szHTMLData[i]==TEXT('>'))
							i++;
						int nStart = i;
						int nTd = 0, nSubTable=0;
						for(;i<e && szHTMLData[i];i++){
							if(!_tcsnicmp(&szHTMLData[i],TEXT("<table"),6))
								nSubTable++;
							else if(!_tcsnicmp(&szHTMLData[i],TEXT("</table"),7))
								nSubTable--;
							if(nSubTable)
								continue;
							if(!_tcsnicmp(&szHTMLData[i],TEXT("<td"),3) || !_tcsnicmp(&szHTMLData[i],TEXT("</td"),4) || !_tcsnicmp(&szHTMLData[i],TEXT("<th"),3) || !_tcsnicmp(&szHTMLData[i],TEXT("</th"),4) || !_tcsnicmp(&szHTMLData[i],TEXT("</tr"),4) || !_tcsnicmp(&szHTMLData[i],TEXT("<tr"),3))
								break;
						}
						int Restore = szHTMLData[i];
						if(nStart==e)
							break;
						szHTMLData[i] = 0;
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewCompound(nRow,nCol,nWidth,bCenter?2:nHFormat,nTD_VFormat,bgCol,bTableBorder,nColSpan,nRowSpan,nCellPadding,nCellSpacing,nTableWidth,nBmpBkg,nTD_Height,nTableHeight,colTableBorder,nAbsoluteAlign);
						while(szHTMLData[nStart]==TEXT(' '))
							nStart++;
						AddHTML(&szHTMLData[nStart],ppItems[nItems-1]->m_ppSubItems,ppItems[nItems-1]->m_nSubItems,szFont,nFontSize+(nTD_Header?4:0),colText,szBaseUrl,nLevel+1,nTD_HFormat,nTD_NoBreak);
						nCol++;
						if(nColSpan)
							nCol+=nColSpan-1;
						nWidth = 0;
						bgCol = bgTrCol;
						nColSpan=1;
						nRowSpan=1;
						szHTMLData[i] = Restore;
						if(!_tcsnicmp(&szHTMLData[i],TEXT("</tr"),4) || !_tcsnicmp(&szHTMLData[i],TEXT("<tr"),3))
							break;
					}while(i<e);
					if(nCol){
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);
						nRow++;
					}
				}while(i<e);
				bCenter = bPrevCenter;
				nHFormat = nPrevHFormat;
				AddItem(ppItems, nItems);
				ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);

				for(i=e;szHTMLData[i] && szHTMLData[i]!=TEXT('>');i++);
				if(!szHTMLData[i])
					i--;
				else
					while(szHTMLData[i] && szHTMLData[i+1]==TEXT(' '))
						i++;
				continue;
			}
			for(t=0;pszTags[t];t++){
				int nTL = lstrlen(pszTags[t]);
				if(!_tcsnicmp(&szHTMLData[i],pszTags[t],nTL) && 
					(!szHTMLData[i+nTL] || szHTMLData[i+nTL]==TEXT(' ') || szHTMLData[i+nTL]==TEXT('>')))
					break;
			}
			if(pszTags[t]){
				if(t==18){ // Font
					CHTMLStringTable st;
					GetProperties(&szHTMLData[i],st);
					for(int a=0;a<st.GetCount();a++){
						if(!_tcsicmp(st[a][0],TEXT("face")))
							lstrcpy(szCurrFont,st[a][1]);
						else if(!_tcsicmp(st[a][0],TEXT("color")))
							colCurText = GetCol(st[a][1]);
						else if(!_tcsicmp(st[a][0],TEXT("size")) && _tstoi(st[a][1])){
							if(st[a][1][0]==TEXT('+') || st[a][1][0]==TEXT('-'))
								nCurrFontSize=nFontSize+_tstoi(st[a][1]);
							else if(st[a][1][0])
								nCurrFontSize = 4*_tstoi(st[a][1])+8;
						}
					}
				}
				else if(t==9){ // Anchor
					CHTMLStringTable st;
					GetProperties(&szHTMLData[i],st);
					if(m_Links.GetCount()==0)
						m_Links.AddRow();
					for(int a=0;a<st.GetCount();a++){
						if(!_tcsicmp(st[a][0],TEXT("href"))){
							TCHAR szFullUrl[2048];
							AddBaseToUrl(st[a][1],szFullUrl,szBaseUrl);
							for(nCurrLink=0;nCurrLink<m_Links[0].GetCount();nCurrLink++)
								if(!_tcsicmp(m_Links[0][nCurrLink],szFullUrl))
									break;
							if(nCurrLink==m_Links[0].GetCount())
								m_Links[0].AddString(szFullUrl);
							bLink=1;
						}
					}
				}

				i+=lstrlen(pszTags[t]);
				if(t==1) // Italic
					bItalic=1;
				else if(t==2)
					bItalic=0;
				else if(t==3 || t==5) // Bold
					bBold=1;
				else if(t==4 || t==6)
					bBold=0;
				else if(t==7) // Underline
					bUnderline=1;
				else if(t==8)
					bUnderline=0;
				else if(t==10){
					bLink=0;
					nCurrLink=-1;
				}
				else if(t==11){ // Small
					bSmall=1;
					nCurrFontSize-=nCurrFontSize/3;
				}
				else if(t==12){
					bSmall=0;
					nCurrFontSize=nFontSize;
				}
				else if(t==13){ // Big
					bBig=1;
					nCurrFontSize+=nCurrFontSize/3;
				}
				else if(t==14){
					bBig=0;
					nCurrFontSize=nFontSize;
				}
				else if(t==15) // Center
					bCenter=1;
				else if(t==16)
					bCenter=0;
				else if(t==19){ // End of Font
					nCurrFontSize = nFontSize;
					colCurText = colText;
					lstrcpy(szCurrFont,szFont);
				}
				else if(t==21){ // Heading text 1
					nCurrFontSize+=nCurrFontSize;
					bBold=1;
				}
				else if(t==32){ // Heading text 2
					nCurrFontSize+=nCurrFontSize/2;
					bBold=1;
				}
				else if(t==34){ // Heading text 3
					nCurrFontSize+=nCurrFontSize/3;
					bBold=1;
				}
				else if(t==22 || t==33 || t==35){ // End of Heading text
					nCurrFontSize=nFontSize;
					bBold=0;
				}
				else if(t==39 || t==40){ // Lists
					int nEnd;
					int nSub=0;
					int nRow=0;
					AddItem(ppItems, nItems);
					ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);
					// Find the ending of this list, allowing sub lists
					for(nEnd=i;szHTMLData[nEnd];nEnd++){
						if(!_tcsnicmp(&szHTMLData[nEnd],t==39?TEXT("<ul"):TEXT("<ol"),3))
							nSub++;
						if(!_tcsnicmp(&szHTMLData[nEnd],t==39?TEXT("</ul"):TEXT("</ol"),4)){
							if(!nSub)
								break;
							else
								nSub--;
						}
					}
					// Go to beginning of list items
					for(;i<nEnd && szHTMLData[i]!=TEXT('>');i++);
					if(szHTMLData[i]==TEXT('>'))
						i++;
					// Find first list item
					for(;i<nEnd && _tcsnicmp(&szHTMLData[i],TEXT("<li"),3);i++);
					nSub=0;
					while(i<nEnd){
						for(;i<nEnd && szHTMLData[i-1]!=TEXT('>');i++);
						// Find next list item
						int e=i+1;
						for(;e<nEnd;e++){
							if(!_tcsnicmp(&szHTMLData[e],TEXT("<ul"),3) || !_tcsnicmp(&szHTMLData[e],TEXT("<ol"),3))
								nSub++;
							if(!_tcsnicmp(&szHTMLData[e],TEXT("</ul"),4) || !_tcsnicmp(&szHTMLData[e],TEXT("</ol"),4))
								nSub--;
							if(!nSub && !_tcsnicmp(&szHTMLData[e],TEXT("<li"),3))
								break;
						}

						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewCompound(nRow,0,0,0,0,-1,0,1,1,2,0,0,-1,0,0,0,-1);
						TCHAR szRow[135];
						if(t==39){
							if(m_nListLevel%2==0)
								lstrcpy(szRow,TEXT(" l "));
							else
								lstrcpy(szRow,TEXT(" m "));
							AddHTML(szRow,ppItems[nItems-1]->m_ppSubItems,ppItems[nItems-1]->m_nSubItems,TEXT("Wingdings"),nFontSize,colText,szBaseUrl,nLevel+1,0,1);
						}
						else{
							lstrcpy(szRow,TEXT("   "));
							if(m_nListLevel%2==0)
								_itot(nRow+1,&szRow[lstrlen(szRow)],10);
							else{
								int n = lstrlen(szRow);
								szRow[n] = TEXT('a') + nRow;
								szRow[n+1]=0;
							}
							lstrcat(szRow,TEXT(".    "));
							AddHTML(szRow,ppItems[nItems-1]->m_ppSubItems,ppItems[nItems-1]->m_nSubItems,szFont,nFontSize,colText,szBaseUrl,nLevel+1,0,1);
						}
						m_nListLevel++;
						int Restore = szHTMLData[e];
						szHTMLData[e]=0;
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewCompound(nRow,1,0,0,0,-1,0,1,1,2,0,0,-1,0,0,0,-1);
						AddHTML(&szHTMLData[i],ppItems[nItems-1]->m_ppSubItems,ppItems[nItems-1]->m_nSubItems,szFont,nFontSize,colText,szBaseUrl,nLevel+1,nHFormat,bNoBreak);
						szHTMLData[e] = Restore;
						i=e;
						nRow++;
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);
						m_nListLevel--;
						bTextStarted=0;
					}
				}
				else if(t==44) // Sub
					bSubText=1;
				else if(t==45)
					bSubText=0;
				else if(t==46) // Sup
					bSupText=1;
				else if(t==47)
					bSupText=0;
				else if(t==48) // Strike
					bStrike=1;
				else if(t==49)
					bStrike=0;
				else if(t==59){
					int e;
					for(;szHTMLData[i] && szHTMLData[i]!=TEXT('>');i++);
					if(szHTMLData[i]==TEXT('>'))
						i++;
					for(e=i;szHTMLData[e] && _tcsnicmp(&szHTMLData[e],TEXT("</blockquote"),12);e++);
					int Restore = szHTMLData[e];
					szHTMLData[e] = 0;

					AddItem(ppItems, nItems);
					ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);
					
					AddItem(ppItems, nItems);
					ppItems[nItems-1] = CHTMLItem::NewCompound(0,0,0,bCenter?2:nHFormat,0,-1,0,1,1,15,0,0,-1,0,0,-1,-1);

					AddHTML(&szHTMLData[i],ppItems[nItems-1]->m_ppSubItems,ppItems[nItems-1]->m_nSubItems,szFont,nFontSize,colText,szBaseUrl,nLevel+1,nHFormat,bNoBreak);

					AddItem(ppItems, nItems);
					ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);

					szHTMLData[e] = Restore;
					i=e;
					if(szHTMLData[i])
						i--;
					bTextStarted=0;
				}
				if(t==15 || t==16 || t==17 || t==0 || t==21 || t==22 || t==23 || t==24 || t==25 || t==32 || t==33 || t==34 || t==35){
					int bClear=0;
#ifdef ABSOLUTE_ALIGN
					if(t==0){
						CHTMLStringTable st;
						GetProperties(&szHTMLData[i],st);
						for(int a=0;a<st.GetCount();a++){
							if(!_tcsicmp(st[a][0],TEXT("clear"))){
								bClear=1;
								break;
							}
						}
					}
#endif
					if(t==21 || t==32 || t==34){
/*
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewText(TEXT(" "),(bLink || bUnderline)?8:0,(bLink || bUnderline)?1:0,bLink?RGB(0,0,255):0,szCurrFont,nCurrFontSize,
							bBold,bItalic,bLink?RGB(0,0,255):colCurText,-1,bCenter?2:nHFormat,0,nCurrLink,bNoBreak);
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,bClear);
*/
/*
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewText(TEXT(" "),(bLink || bUnderline)?8:0,(bLink || bUnderline)?1:0,bLink?RGB(0,0,255):0,szCurrFont,nCurrFontSize,
							bBold,bItalic,bLink?RGB(0,0,255):colCurText,-1,bCenter?2:nHFormat,0,nCurrLink,bNoBreak);
*/
					}
					if(bTextStarted || t==0){
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewText(TEXT(" "),(bLink || bUnderline)?8:0,(bLink || bUnderline)?1:0,bLink?RGB(0,0,255):0,szCurrFont,nCurrFontSize,
							bBold,bItalic,bLink?RGB(0,0,255):colCurText,-1,bCenter?2:nHFormat,0,nCurrLink,bNoBreak);
					}

					AddItem(ppItems, nItems);
					if(t==25)
						ppItems[nItems-1] = CHTMLItem::NewBreak(2,1,bCenter?2:nHFormat,0);
					else
						ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,bClear);

					if((bTextStarted && t==17) || t==25){
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewText(TEXT(" "),(bLink || bUnderline)?8:0,(bLink || bUnderline)?1:0,bLink?RGB(0,0,255):0,szCurrFont,nCurrFontSize,
							bBold,bItalic,bLink?RGB(0,0,255):colCurText,-1,bCenter?2:nHFormat,0,nCurrLink,bNoBreak);
						AddItem(ppItems, nItems);
						ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,bCenter?2:nHFormat,0);
					}

					if(!bNoBreak && (t==0 || t==17))
						bTextStarted=0;
				}
				while(szHTMLData[i] && szHTMLData[i]!=TEXT('>'))
					i++;
				if(szHTMLData[i]==TEXT('>'))
					i++;
				i--;
				continue;
			}
		}
		else{
			if(szHTMLData[i]==TEXT(' ') || szHTMLData[i]==TEXT('\r') ||
					szHTMLData[i]==TEXT('\n') || szHTMLData[i]==TEXT('\t')){
				if(szHTMLData[i]==TEXT('\t')){
					AddItem(ppItems, nItems);
					int nTab;
					int nChars=0;
					for(nTab=nItems-1;nTab>0 && ppItems[nTab-1]->m_nType==TEXT;nTab--){
						nChars+=lstrlen(ppItems[nTab-1]->m_szText);
					}
					nChars=5-nChars%5;
					TCHAR szTab[6];
					for(nTab=0;nTab<nChars;nTab++)
						szTab[nTab]=TEXT(' ');
					szTab[nTab]=0;
					ppItems[nItems-1] = CHTMLItem::NewText(szTab,(bLink || bUnderline)?8:0,(bLink || bUnderline)?1:0,bLink?RGB(0,0,255):0,szCurrFont,bSubText?2*nCurrFontSize/3:nCurrFontSize,
						bBold,bItalic,bLink?RGB(0,0,255):colCurText,-1,bCenter?2:nHFormat,0,nCurrLink,bNoBreak);
					if(bSupText)
						ppItems[nItems-1]->m_nFontSize = 2*nCurrFontSize/3;
					if(bStrike){
						ppItems[nItems-1]->m_nFrames |= 16;
						ppItems[nItems-1]->m_nFrameSize=1;
					}
				}
				else if(bTextStarted || bNoBreak){
					AddItem(ppItems, nItems);
					ppItems[nItems-1] = CHTMLItem::NewText(TEXT(" "),(bLink || bUnderline)?8:0,(bLink || bUnderline)?1:0,bLink?RGB(0,0,255):0,szCurrFont,bSubText?2*nCurrFontSize/3:nCurrFontSize,
						bBold,bItalic,bLink?RGB(0,0,255):colCurText,-1,bCenter?2:nHFormat,0,nCurrLink,bNoBreak);
					if(bSupText)
						ppItems[nItems-1]->m_nFontSize = 2*nCurrFontSize/3;
					if(bStrike){
						ppItems[nItems-1]->m_nFrames |= 16;
						ppItems[nItems-1]->m_nFrameSize=1;
					}
				}
				if(szHTMLData[i]==TEXT('\r') || szHTMLData[i]==TEXT('\n')){
					AddItem(ppItems, nItems);
					ppItems[nItems-1] = CHTMLItem::NewBreak(0,0,nHFormat,0);
					if(szHTMLData[i]==TEXT('\r') && szHTMLData[i+1]==TEXT('\n'))
						i++;
				}
			}
			else{
				int j;
				for(j=i;szHTMLData[j] && szHTMLData[j]!=TEXT(' ') && szHTMLData[j]!=TEXT('\r') &&
					szHTMLData[j]!=TEXT('\n') && szHTMLData[j]!=TEXT('\t') && szHTMLData[j]!=TEXT('<');j++);
				TCHAR Restore = szHTMLData[j];
				szHTMLData[j] = 0;
				AddItem(ppItems, nItems);
				ppItems[nItems-1] = CHTMLItem::NewText(&szHTMLData[i],(bLink || bUnderline)?8:0,(bLink || bUnderline)?1:0,bLink?RGB(0,0,255):0,szCurrFont,bSubText?2*nCurrFontSize/3:nCurrFontSize,
					bBold,bItalic,bLink?RGB(0,0,255):colCurText,-1,bCenter?2:nHFormat,0,nCurrLink,bNoBreak);
				if(bSupText)
					ppItems[nItems-1]->m_nFontSize = 2*nCurrFontSize/3;
				if(bStrike){
					ppItems[nItems-1]->m_nFrames |= 16;
					ppItems[nItems-1]->m_nFrameSize=1;
				}
				szHTMLData[j] = Restore;
				i=j-1;
				bTextStarted=TRUE;
			}
		}
	}
}
void CHTMLViewer::CalcPositions(RECT &rPos, CHTMLItem **&ppItems, int &nItems)
{
	POINT pStart = {rPos.left,rPos.top};
	int nPrevLine = 0, i;
	int nColumnWidths=0;
	int *pnMinColumnWidths=NULL;
	int *pnAMinColumnWidths=NULL;
	int *pnMaxColumnWidths=NULL;
	int *pnFixedColumnWidths=NULL;
	int *pnRowSpans=NULL;
	int nCurrWidth=0;
	int nRowSpanOffset=0;
	int nRowSpanPosOffset=0;
	int *pnRowHeights=NULL;
	int *pnFixedRowHeights=NULL;
	int nRowHeights=0;
	int nCurrTableWidth=0;
	int nRestoredTopPos;
	int nBreakBeforeTable;
	int nAbsoluteAlign=0;
	int *pnAbsoluteAlignLeftOffset=NULL;
	int *pnAbsoluteAlignRightOffset=NULL;
	int *pnAbsoluteAlignBottom=NULL;
	for(i=0;i<nItems+1;i++){
		// Set <HR> width to something small
		if(i<nItems && ppItems[i]->m_nType==BREAK && ppItems[i]->m_scBounds.cx)
			ppItems[i]->m_scBounds.cx=1;

		// New table, create list with max/min column widths, fixed widths and rowspans
		if(i<nItems-1 && ppItems[i]->m_nType==COMPOUND && !nColumnWidths){
			nRestoredTopPos = pStart.y;
			nBreakBeforeTable = i-1;
			int nTest;
			for(nTest=0;nTest<2;nTest++){
				int k=i, t;
				int nX=0, nY=0, nXR=0;
				do{
					for(k;k<nItems;k++){
						// Check if end of row in table
						if(ppItems[k]->m_nType!=COMPOUND || ppItems[k]->m_nCol!=nX || ppItems[k]->m_nRow!=nY)
							break;
						// Handle Rowspan
						while(nX+nXR<nColumnWidths && pnRowSpans[nX+nXR]){
							pnRowSpans[nX+nXR]--;
							nXR++;
						}
						nX+=ppItems[k]->m_nColSpan-1;
						// Create more columns if we passed the edge
						if(nX+nXR>=nColumnWidths){
							pnMinColumnWidths = (int*)realloc(pnMinColumnWidths,sizeof(int)*(nX+nXR+1));
							pnAMinColumnWidths = (int*)realloc(pnAMinColumnWidths,sizeof(int)*(nX+nXR+1));
							pnMaxColumnWidths = (int*)realloc(pnMaxColumnWidths,sizeof(int)*(nX+nXR+1));
							pnFixedColumnWidths = (int*)realloc(pnFixedColumnWidths,sizeof(int)*(nX+nXR+1));
							pnRowSpans = (int*)realloc(pnRowSpans,sizeof(int)*(nX+nXR+1));
							for(;nColumnWidths<=nX+nXR;nColumnWidths++){
								pnMinColumnWidths[nColumnWidths]=0;
								pnAMinColumnWidths[nColumnWidths]=0;
								pnMaxColumnWidths[nColumnWidths]=0;
								pnFixedColumnWidths[nColumnWidths]=0;
								pnRowSpans[nColumnWidths]=0;
							}
						}
						// Save Rowspan
						if(ppItems[k]->m_nRowSpan>1){
							for(int col=0;col<ppItems[k]->m_nColSpan;col++)
								pnRowSpans[ppItems[k]->m_nCol+nXR+col] = ppItems[k]->m_nRowSpan-1;
						}
						
						// Cached widths (doesn't improve performance as much as I wanted...)
						if(ppItems[k]->m_nColSpan==1 && ppItems[k]->m_nMinColumnWidth>=0 && ppItems[k]->m_nMaxColumnWidth>=0){
							if(pnMaxColumnWidths[nX+nXR]<ppItems[k]->m_nMaxColumnWidth)
								pnMaxColumnWidths[nX+nXR]=ppItems[k]->m_nMaxColumnWidth;
							if(pnMinColumnWidths[nX+nXR]<ppItems[k]->m_nMinColumnWidth)
								pnMinColumnWidths[nX+nXR]=ppItems[k]->m_nMinColumnWidth;
							if(pnAMinColumnWidths[nX+nXR]<ppItems[k]->m_nAMinColumnWidth)
								pnAMinColumnWidths[nX+nXR]=ppItems[k]->m_nAMinColumnWidth;
							if(ppItems[k]->m_nWidth)
								pnFixedColumnWidths[nX+nXR] = 1;
						}
						else{
							// Handle TD with fixed width without colspan
							if(!nTest && ppItems[k]->m_nWidth && ppItems[k]->m_nColSpan==1){
								RECT min={0};
								CalcPositions(min,ppItems[k]->m_ppSubItems,ppItems[k]->m_nSubItems);
								for(t=0;t<ppItems[k]->m_nSubItems;t++){
									if(!ppItems[k]->m_ppSubItems[t]->m_bNoBreak && ppItems[k]->m_ppSubItems[t]->m_nType==TEXT && !lstrcmp(ppItems[k]->m_ppSubItems[t]->m_szText,TEXT(" ")))
										continue;
									if(nX+nXR<nColumnWidths && pnMinColumnWidths[nX+nXR]<ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx)
										pnMinColumnWidths[nX+nXR]=ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx;
									if(nX+nXR<nColumnWidths && pnAMinColumnWidths[nX+nXR]<ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx)
										pnAMinColumnWidths[nX+nXR]=ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx;
								}
								if(ppItems[k]->m_nWidth>pnMinColumnWidths[nX+nXR])
									pnMinColumnWidths[nX+nXR] = ppItems[k]->m_nWidth;
								pnMaxColumnWidths[nX+nXR] = pnMinColumnWidths[nX+nXR];
								pnFixedColumnWidths[nX+nXR] = 1;
							}
							// Handle TD with fixed width with colspan
							else if(nTest && ppItems[k]->m_nWidth && ppItems[k]->m_nColSpan>1){
								RECT max = {0,0,0,0};
								CalcPositions(max,ppItems[k]->m_ppSubItems,ppItems[k]->m_nSubItems);
								int nTdW=0;
								int nMaxR=0, nMinL=0;
								for(t=0;t<ppItems[k]->m_nSubItems;t++){
									if(ppItems[k]->m_ppSubItems[t]->m_nType==BREAK && (t==ppItems[k]->m_nSubItems-1 || ppItems[k]->m_ppSubItems[t+1]->m_nType!=COMPOUND))
										continue;
									if(nMaxR<ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx)
										nMaxR=ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx;
									if(t==0 || nMinL>ppItems[k]->m_ppSubItems[t]->m_pPos.x)
										nMinL=ppItems[k]->m_ppSubItems[t]->m_pPos.x;
								}
								nTdW = nMaxR-nMinL;
								int nW=0;
								int q;
								for(q=nX+nXR-ppItems[k]->m_nColSpan+1;q<nX+nXR + 1 && q<nColumnWidths;q++)
									nW+=pnMinColumnWidths[q] + ppItems[k]->m_nCellPadding*2 + ppItems[k]->m_nCellSpacing;
								nW-=ppItems[k]->m_nCellPadding*2 + ppItems[k]->m_nCellSpacing;
								if(nTdW<ppItems[k]->m_nWidth)
									nTdW=ppItems[k]->m_nWidth;
								while(nW<nTdW){
									int nTest=0;
									for(q=nX+nXR-ppItems[k]->m_nColSpan+1;q<nX+nXR + 1 && q<nColumnWidths;q++){
										if(pnMinColumnWidths[q] && nW<nTdW && !pnFixedColumnWidths[q]){
											nTest++;
											pnMinColumnWidths[q]++;
											if(pnMaxColumnWidths[q]<pnMinColumnWidths[q])
												pnMaxColumnWidths[q]=pnMinColumnWidths[q];
											nW++;
										}
									}
									if(!nTest){
										for(q=nX+nXR-ppItems[k]->m_nColSpan+1;q<nX+nXR + 1 && q<nColumnWidths && nW<nTdW;q++){
											if(!pnFixedColumnWidths[q]){
												pnMaxColumnWidths[q]++;
												pnMinColumnWidths[q]++;
												pnAMinColumnWidths[q] = pnMinColumnWidths[q];
												nW++;
												nTest++;
											}
										}
									}
									if(!nTest)
										break;
								}
							}
							// Handle TD without fixed width without colspan
							else if(!nTest && ppItems[k]->m_nColSpan==1 && !ppItems[k]->m_nWidth){
								RECT min = {0}, max = {0,0,2000,rPos.bottom};
								CalcPositions(min,ppItems[k]->m_ppSubItems,ppItems[k]->m_nSubItems);
								for(t=0;t<ppItems[k]->m_nSubItems;t++){
									if(!ppItems[k]->m_ppSubItems[t]->m_bNoBreak && ppItems[k]->m_ppSubItems[t]->m_nType==TEXT && !lstrcmp(ppItems[k]->m_ppSubItems[t]->m_szText,TEXT(" ")))
										continue;
									if(nX+nXR<nColumnWidths && pnMinColumnWidths[nX+nXR]<ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx)
										pnMinColumnWidths[nX+nXR]=ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx;
									if(nX+nXR<nColumnWidths && pnAMinColumnWidths[nX+nXR]<ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx)
										pnAMinColumnWidths[nX+nXR]=ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx;
								}
								if(rPos.right){
									CalcPositions(max,ppItems[k]->m_ppSubItems,ppItems[k]->m_nSubItems);
									int nMaxR=0, nMinL=max.right;
									for(t=0;t<ppItems[k]->m_nSubItems;t++){
										if(ppItems[k]->m_ppSubItems[t]->m_nType==BREAK && (t==ppItems[k]->m_nSubItems-1 || ppItems[k]->m_ppSubItems[t+1]->m_nType!=COMPOUND))
											continue;
										if(!ppItems[k]->m_ppSubItems[t]->m_bNoBreak && ppItems[k]->m_ppSubItems[t]->m_nType==TEXT && !lstrcmp(ppItems[k]->m_ppSubItems[t]->m_szText,TEXT(" ")))
											continue;
										if(nMaxR<ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx)
											nMaxR=ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx;
										if(t==0 || nMinL>ppItems[k]->m_ppSubItems[t]->m_pPos.x)
											nMinL=ppItems[k]->m_ppSubItems[t]->m_pPos.x;
									}
									if(nX+nXR<nColumnWidths && pnMaxColumnWidths[nX+nXR]<nMaxR-nMinL)
										pnMaxColumnWidths[nX+nXR]=nMaxR-nMinL;
								}
								else if(pnMaxColumnWidths[nX+nXR]<pnMinColumnWidths[nX+nXR])
									pnMaxColumnWidths[nX+nXR]=pnMinColumnWidths[nX+nXR];

							}
							// Handle TD without fixed width with colspan
							else if(nTest && ppItems[k]->m_nColSpan>1 && !ppItems[k]->m_nWidth){
								RECT max = {0,0,0,0};
								CalcPositions(max,ppItems[k]->m_ppSubItems,ppItems[k]->m_nSubItems);
								int nTdW=0;
								int nMaxR=0, nMinL=0;
								for(t=0;t<ppItems[k]->m_nSubItems;t++){
									if(ppItems[k]->m_ppSubItems[t]->m_nType==BREAK && (t==ppItems[k]->m_nSubItems-1 || ppItems[k]->m_ppSubItems[t+1]->m_nType!=COMPOUND))
										continue;
									if(nMaxR<ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx)
										nMaxR=ppItems[k]->m_ppSubItems[t]->m_pPos.x+ppItems[k]->m_ppSubItems[t]->m_scBounds.cx;
									if(t==0 || nMinL>ppItems[k]->m_ppSubItems[t]->m_pPos.x)
										nMinL=ppItems[k]->m_ppSubItems[t]->m_pPos.x;
								}
								nTdW = nMaxR-nMinL;
								int nW=0;
								int q;
								for(q=nX+nXR-ppItems[k]->m_nColSpan+1;q<nX+nXR + 1 && q<nColumnWidths;q++)
									nW+=pnMinColumnWidths[q] + ppItems[k]->m_nCellPadding*2 + ppItems[k]->m_nCellSpacing;
								nW-=ppItems[k]->m_nCellPadding*2 + ppItems[k]->m_nCellSpacing;
								while(nW<nTdW){
									int nTest=0;
									for(q=nX+nXR-ppItems[k]->m_nColSpan+1;q<nX+nXR + 1 && q<nColumnWidths;q++){
										if(pnMinColumnWidths[q] && nW<nTdW && !pnFixedColumnWidths[q]){
											nTest++;
											pnMinColumnWidths[q]++;
											if(pnMaxColumnWidths[q]<pnMinColumnWidths[q])
												pnMaxColumnWidths[q]=pnMinColumnWidths[q];
											nW++;
										}
									}
									if(!nTest){
										for(q=nX+nXR-ppItems[k]->m_nColSpan+1;q<nX+nXR + 1 && q<nColumnWidths;q++)
											if(!pnFixedColumnWidths[q]){
												pnMaxColumnWidths[nX+nXR]++;
												pnMinColumnWidths[nX+nXR]++;
												pnAMinColumnWidths[nX+nXR] = pnMinColumnWidths[nX+nXR];
												nW++;
												nTest++;
											}
									}
									if(!nTest)
										break;
								}
							}
							if(rPos.right && nTest && ppItems[k]->m_nColumnWidth==-1){
								ppItems[k]->m_nMinColumnWidth = pnMinColumnWidths[nX+nXR];
								ppItems[k]->m_nAMinColumnWidth = pnAMinColumnWidths[nX+nXR];
								ppItems[k]->m_nMaxColumnWidth = pnMaxColumnWidths[nX+nXR];
								ppItems[k]->m_nColumnWidth = rPos.right;
							}
						}
						nX++;
					}
					nY++;
					while(nX+nXR<nColumnWidths){
						if(pnRowSpans[nX+nXR])
							pnRowSpans[nX+nXR]--;
						nXR++;
					}
					nX=0;
					nXR=0;
					// Check if end of table
					if(k>=nItems-2 || ppItems[k]->m_nType!=BREAK || ppItems[k+1]->m_nType!=COMPOUND)
						break;
					if(ppItems[k+1]->m_nCol!=nX || ppItems[k+1]->m_nRow!=nY)
						break;
					k+=1;
				}while(1);
				for(k=0;k<nColumnWidths;k++)
					pnRowSpans[k]=0;
			}
			// Handle fixed width on the whole table
			int nTableWidth = ppItems[i]->m_nTableWidth;
			if(nTableWidth<0){
				if(rPos.right)
					nTableWidth = -rPos.right*nTableWidth/100;
				else
					nTableWidth=0;
			}
			if(!nTableWidth) // If the table does not have fixed width, set the width to the document width
				nTableWidth = rPos.right;
			if(nTableWidth){
				int nBreak = 1;
				int nW = ppItems[i]->m_nCellSpacing+ppItems[i]->m_nTableFrameSize*2;
				for(int k=0;k<nColumnWidths;k++)
					nW+=pnMinColumnWidths[k]+ppItems[i]->m_nCellPadding*2 + ppItems[i]->m_nCellSpacing;

				do{
					nBreak = 0;
					if(nW<nTableWidth){
						for(int k=0;k<nColumnWidths;k++){
							if(!pnFixedColumnWidths[k] && nW<nTableWidth && pnMinColumnWidths[k]<pnMaxColumnWidths[k]){
								pnMinColumnWidths[k]++;
								nBreak++;
								nW++;
							}
						}
					}
					if(!nBreak && ppItems[i]->m_nTableWidth && nW<nTableWidth){
						for(int k=0;k<nColumnWidths;k++){
							if(nW<nTableWidth && !pnFixedColumnWidths[k]){
								pnMinColumnWidths[k]++;
								nBreak++;
								nW++;
							}
						}
					}
					// Here is a fix for shrinking fixed-width columns to the table width when
					// the sum of the column widths are larger than the table width
					if(nW>nTableWidth){// && (nW-nTableWidth)<20){
						int nMax=-1;
						for(int k=0;k<nColumnWidths;k++){
							if(pnFixedColumnWidths[k] && nW>nTableWidth && pnMinColumnWidths[k]>10){
								if(pnAMinColumnWidths[k]<pnMinColumnWidths[k] && (nMax==-1 || pnMinColumnWidths[k]>pnMinColumnWidths[nMax]))
									nMax = k;
							}
						}
						if(nMax!=-1){
							pnMinColumnWidths[nMax]--;
							pnMaxColumnWidths[nMax] = pnMinColumnWidths[nMax];
							nBreak++;
							nW--;
						}
					}
				}while(nBreak && nW!=nTableWidth);
			}
			for(int k=0;k<nColumnWidths;k++)
				pnRowSpans[k]=0;

			// Get TD height by positioning the TDs
			for(nTest=0;nTest<2;nTest++){
				int k=i;
				int nY=0;
				int nXR=0;
				int nX=0;
				do{
					for(k;k<nItems;k++){
						// Check if end of table row
						if(ppItems[k]->m_nType!=COMPOUND || ppItems[k]->m_nCol!=nX || ppItems[k]->m_nRow!=nY)
							break;
						// Check that we are not over the edge of rowheights
						while(nY+ppItems[k]->m_nRowSpan>nRowHeights){
							nRowHeights++;
							pnRowHeights = (int*)realloc(pnRowHeights,sizeof(int)*nRowHeights);
							pnFixedRowHeights = (int*)realloc(pnFixedRowHeights,sizeof(int)*nRowHeights);
							pnRowHeights[nRowHeights-1]=0;
							pnFixedRowHeights[nRowHeights-1]=0;
						}
						// Handle Rowspan
						while(ppItems[k]->m_nCol+nXR<nColumnWidths && pnRowSpans[ppItems[k]->m_nCol+nXR]){
							pnRowSpans[ppItems[k]->m_nCol+nXR]--;
							nXR++;
						}
						nX+=ppItems[k]->m_nColSpan-1;

						// Save rowspan
						if(ppItems[k]->m_nRowSpan>1){
							for(int col=0;col<ppItems[k]->m_nColSpan;col++)
								pnRowSpans[ppItems[k]->m_nCol+nXR+col] = ppItems[k]->m_nRowSpan-1;
						}

						// Set width according to list of TD-widths
						RECT r={0};
						if(nColumnWidths>ppItems[k]->m_nCol+nXR){
							r.right = pnMinColumnWidths[ppItems[k]->m_nCol+nXR];
							for(int colspan=1;colspan<ppItems[k]->m_nColSpan && nColumnWidths>ppItems[k]->m_nCol+nXR+colspan;colspan++)
								r.right += pnMinColumnWidths[ppItems[k]->m_nCol+nXR+colspan] + 2*ppItems[k]->m_nCellPadding + ppItems[k]->m_nCellSpacing;
						}
						// Make positioning on this TD
						if(!nTest)
							CalcPositions(r,ppItems[k]->m_ppSubItems,ppItems[k]->m_nSubItems);
						// Handle TD without rowspan
						if(!nTest && ppItems[k]->m_nRowSpan==1){
							int nHeight=0;
							for(int a=0;a<ppItems[k]->m_nSubItems;a++)
								if(nHeight<ppItems[k]->m_ppSubItems[a]->m_pPos.y+ppItems[k]->m_ppSubItems[a]->m_scBounds.cy)
									nHeight = ppItems[k]->m_ppSubItems[a]->m_pPos.y+ppItems[k]->m_ppSubItems[a]->m_scBounds.cy;
							if(pnRowHeights[nY]<nHeight)
								pnRowHeights[nY]=nHeight;
						}
						// Handle TD with rowspan
						else if(nTest && ppItems[k]->m_nRowSpan>1){
							int nHeight=0;
							for(int a=0;a<ppItems[k]->m_nSubItems;a++)
								if(nHeight<ppItems[k]->m_ppSubItems[a]->m_pPos.y+ppItems[k]->m_ppSubItems[a]->m_scBounds.cy)
									nHeight = ppItems[k]->m_ppSubItems[a]->m_pPos.y+ppItems[k]->m_ppSubItems[a]->m_scBounds.cy;
							int nRowHeight=0;
							int row;
							for(row=0;nRowHeights>nY+row && row<ppItems[k]->m_nRowSpan;row++){
								nRowHeight+=pnRowHeights[nY+row];
								if(row)
									nRowHeight+=2*ppItems[k]->m_nCellPadding + ppItems[k]->m_nCellSpacing;
							}
							// If the height of this row is to small, increase height on all including rows
							while(nRowHeight<nHeight){
								for(row=0;nRowHeights>nY+row && row<ppItems[k]->m_nRowSpan && nRowHeight<nHeight;row++){
									pnRowHeights[nY+row]++;
									nRowHeight++;
								}
							}
						}
						// Handle fixed TD height without rowspan
						if(!nTest && ppItems[k]->m_nHeight && ppItems[k]->m_nRowSpan<=1){
							if(pnRowHeights[nY]<ppItems[k]->m_nHeight)
								pnRowHeights[nY]=ppItems[k]->m_nHeight;
							pnFixedRowHeights[nY]=1;
						}
						// Handle fixed TD height with rowspan
						else if(nTest && ppItems[k]->m_nHeight && ppItems[k]->m_nRowSpan>1){
							int row, nTDHeight=0;
							for(row=nY;row<nRowHeights && row<nY+ppItems[k]->m_nRowSpan;row++){
								nTDHeight+=pnRowHeights[row];
								pnFixedRowHeights[row]=1;
							}
							while(nTDHeight<ppItems[i]->m_nHeight){
								int nTest=0;
								for(row=nY;row<nRowHeights && row<nY+ppItems[k]->m_nRowSpan && nTDHeight<ppItems[i]->m_nHeight;row++){
									if(pnFixedRowHeights[row])
										continue;
									nTDHeight++;
									pnRowHeights[row]++;
									nTest++;
								}
								if(!nTest){
									for(row=nY;row<nRowHeights && row<nY+ppItems[k]->m_nRowSpan && nTDHeight<ppItems[i]->m_nHeight;row++){
										nTDHeight++;
										pnRowHeights[row]++;
										nTest++;
									}
								}
							}
						}
						nX++;
					}
					nY++;
					while(nX+nXR<nColumnWidths){
						if(pnRowSpans[nX+nXR])
							pnRowSpans[nX+nXR]--;
						nXR++;
					}
					nX=0;
					nXR=0;
					// Check if end of table
					if(k>=nItems-2 || ppItems[k]->m_nType!=BREAK || ppItems[k+1]->m_nType!=COMPOUND)
						break;
					if(ppItems[k+1]->m_nCol!=nX || ppItems[k+1]->m_nRow!=nY)
						break;
					k+=1;
				}while(1);
				for(k=0;k<nColumnWidths;k++)
					pnRowSpans[k]=0;
			}
			nCurrTableWidth=ppItems[i]->m_nCellSpacing+ppItems[i]->m_nTableFrameSize*2;
			for(nTest=0;nTest<nColumnWidths;nTest++)
				nCurrTableWidth+=pnMinColumnWidths[nTest] + ppItems[i]->m_nCellPadding*2 + ppItems[i]->m_nCellSpacing;

			// Handle fixed tabel height
			if(ppItems[i]->m_nTableHeight){
				int nTableHeight=0;
				int k;
				for(k=0;k<nRowHeights;k++)
					nTableHeight+=pnRowHeights[k];
				while(nTableHeight<ppItems[i]->m_nTableHeight){
					int nTest=0;
					for(k=0;k<nRowHeights && nTableHeight<ppItems[i]->m_nTableHeight;k++){
						if(pnFixedRowHeights[k])
							continue;
						nTableHeight++;
						pnRowHeights[k]++;
						nTest++;
					}
					if(!nTest)
						break;
				}
			}
			// Position and size of the break element representing the outer frame of the table
			if(nBreakBeforeTable>=0){
				ppItems[nBreakBeforeTable]->m_pPos = pStart;
				ppItems[nBreakBeforeTable]->m_scBounds.cx=nCurrTableWidth;
				ppItems[nBreakBeforeTable]->m_scBounds.cy=ppItems[i]->m_nCellSpacing + ppItems[i]->m_nTableFrameSize*2;
				int k;
				for(k=0;k<nRowHeights;k++)
					ppItems[nBreakBeforeTable]->m_scBounds.cy+=pnRowHeights[k]+ppItems[i]->m_nCellPadding*2 + ppItems[i]->m_nCellSpacing;
				ppItems[nBreakBeforeTable]->m_nFrames=15;
				if((int)ppItems[i]->m_colFrame==-1)
					ppItems[nBreakBeforeTable]->m_colFrame=-2;
				else
					ppItems[nBreakBeforeTable]->m_colFrame=ppItems[i]->m_colFrame;
				ppItems[nBreakBeforeTable]->m_nFrameSize=ppItems[i]->m_nTableFrameSize;
			}
		}
		// Positioning of TD
		if(i<nItems-1 && ppItems[i]->m_nType==COMPOUND){
			if(ppItems[i]->m_nCol==0){
				pStart.y+=ppItems[i]->m_nCellSpacing;
				if(ppItems[i]->m_nRow==0)
					pStart.y+=ppItems[i]->m_nTableFrameSize;
				pStart.x+=ppItems[i]->m_nCellSpacing + ppItems[i]->m_nTableFrameSize;
			}
			RECT r={0};
			// Handle rowspan
			while(ppItems[i]->m_nCol+nRowSpanOffset<nColumnWidths && pnRowSpans[ppItems[i]->m_nCol+nRowSpanOffset]){
				nRowSpanPosOffset+=pnMinColumnWidths[ppItems[i]->m_nCol+nRowSpanOffset] + ppItems[i]->m_nCellPadding*2 + ppItems[i]->m_nCellSpacing;
				pnRowSpans[ppItems[i]->m_nCol+nRowSpanOffset]--;
				nRowSpanOffset++;
			}
			if(ppItems[i]->m_nRowSpan>1){
				for(int col=0;col<ppItems[i]->m_nColSpan;col++)
					pnRowSpans[ppItems[i]->m_nCol+nRowSpanOffset+col] = ppItems[i]->m_nRowSpan-1;
			}

			// Set width according to list of TD-widths
			if(nColumnWidths>ppItems[i]->m_nCol+nRowSpanOffset){
				r.right = pnMinColumnWidths[ppItems[i]->m_nCol+nRowSpanOffset];
				for(int colspan=1;colspan<ppItems[i]->m_nColSpan && nColumnWidths>ppItems[i]->m_nCol+nRowSpanOffset+colspan;colspan++)
					r.right += pnMinColumnWidths[ppItems[i]->m_nCol+nRowSpanOffset+colspan];
			}
			int nHeight = pnRowHeights[ppItems[i]->m_nRow];
			int row;
			for(row=1;nRowHeights>ppItems[i]->m_nRow+row && row<ppItems[i]->m_nRowSpan;row++)
				nHeight += pnRowHeights[ppItems[i]->m_nRow+row] + 2*ppItems[i]->m_nCellPadding + ppItems[i]->m_nCellSpacing;
			// Move this TD to it's position
			ppItems[i]->m_pPos.x = pStart.x + nRowSpanPosOffset;
			ppItems[i]->m_pPos.y = pStart.y;
			ppItems[i]->m_scBounds.cx = r.right+ppItems[i]->m_nCellPadding*2*ppItems[i]->m_nColSpan + (ppItems[i]->m_nColSpan-1)*ppItems[i]->m_nCellSpacing;
			ppItems[i]->m_scBounds.cy = nHeight+ppItems[i]->m_nCellPadding*2;
			int nTopPos = 0;
			if(ppItems[i]->m_nVFormat){
				nHeight=0;
				for(int a=0;a<ppItems[i]->m_nSubItems;a++)
					if(nHeight<ppItems[i]->m_ppSubItems[a]->m_pPos.y+ppItems[i]->m_ppSubItems[a]->m_scBounds.cy)
						nHeight = ppItems[i]->m_ppSubItems[a]->m_pPos.y+ppItems[i]->m_ppSubItems[a]->m_scBounds.cy;
				if(ppItems[i]->m_nVFormat==1)
					nTopPos = ppItems[i]->m_scBounds.cy-nHeight-ppItems[i]->m_nCellPadding*2;
				else if(ppItems[i]->m_nVFormat==2)
					nTopPos = (ppItems[i]->m_scBounds.cy-nHeight-ppItems[i]->m_nCellPadding*2)/2;
			}
			ppItems[i]->Offset(pStart.x+nRowSpanPosOffset+ppItems[i]->m_nCellPadding,pStart.y+ppItems[i]->m_nCellPadding+nTopPos);

			if(ppItems[i+1]->m_nType==BREAK){
				for(int a=ppItems[i]->m_nCol+nRowSpanOffset+1;a<nColumnWidths;a++)
					if(pnRowSpans[a])
						pnRowSpans[a]--;
			}
		}
		// Row-break
		if((i==nItems || 
				(!(ppItems[i]->m_nType==TEXT && !lstrcmp(ppItems[i]->m_szText,TEXT(" "))) && 
				ppItems[i]->m_scBounds.cx + pStart.x > rPos.right) || 
				ppItems[i]->m_nType==BREAK)){
			if(i<nItems && ppItems[i]->m_nType==BREAK && ppItems[i]->m_scBounds.cy && (i==nItems-1 || ppItems[i+1]->m_nType!=COMPOUND))
				ppItems[i]->m_scBounds.cx = rPos.right-rPos.left;
			if((i==nItems || !ppItems[i]->m_bNoBreak || ppItems[i]->m_nType==BREAK) && (i==nItems || ppItems[i]->m_nType!=COMPOUND)){
				int a, nMaxHeight=0, nWidth=0;
				nRowSpanOffset = 0;
				nRowSpanPosOffset=0;
				BOOL bCenter = (nPrevLine<nItems && ppItems[nPrevLine]->m_nHFormat==2);
				for(a=nPrevLine;a<i;a++){
					if(ppItems[a]->m_nType!=TEXT && ppItems[a]->m_nType!=IMAGE && ppItems[a]->m_nType!=COMPOUND)
						continue;
					if(ppItems[a]->m_nType==COMPOUND && ppItems[a]->m_nRowSpan>1 && nMaxHeight<ppItems[a]->m_nCellPadding*2)
						nMaxHeight = ppItems[a]->m_nCellPadding*2;
					if(ppItems[a]->m_nType==COMPOUND && ppItems[a]->m_nRowSpan>1)
						continue;
					if(ppItems[a]->m_nHFormat)
						nWidth+=ppItems[a]->m_scBounds.cx;
					if(nMaxHeight<ppItems[a]->m_scBounds.cy)
						nMaxHeight=ppItems[a]->m_scBounds.cy;
				}
				if(i && ppItems[i-1]->m_nType==COMPOUND){
					nWidth = nCurrTableWidth;
				}
				int nCenter = 0;
				// Do not include the last space when adjusting for alignment
				if(i && ppItems[i-1]->m_szText && !lstrcmp(ppItems[i-1]->m_szText,TEXT(" ")))
					nWidth-=ppItems[i-1]->m_scBounds.cx;
				// Move elements if center or right alignment
				if(bCenter){
					nCenter = (rPos.right-rPos.left - nWidth)/2;
					if(nCenter<0)
						nCenter=0;
				}
				else if(nPrevLine<nItems && ppItems[nPrevLine]->m_nHFormat==1)
					nCenter = (rPos.right-rPos.left - nWidth);

				for(a=nPrevLine;a<=i && a<nItems;a++){
					// Align elements to underline of row
					if(i>=nItems-1 || ppItems[i+1]->m_nType!=COMPOUND){
						if(ppItems[a]->m_nType!=COMPOUND && !(a<nItems-1 && ppItems[a]->m_nType==BREAK && ppItems[a+1]->m_nType==COMPOUND))
							ppItems[a]->m_pPos.y+=nMaxHeight-ppItems[a]->m_scBounds.cy;
					}
					// Move elements if center or right alignment
					if(ppItems[a]->m_scBounds.cx<rPos.right && (ppItems[a]->m_nType!=BREAK || (a<nItems-1 && ppItems[a]->m_nType==BREAK && ppItems[a+1]->m_nType==COMPOUND))){
						ppItems[a]->m_pPos.x+=nCenter;
						if(ppItems[a]->m_nType==COMPOUND)
							ppItems[a]->Offset(nCenter,0);
					}
				}
				// Justify
				if(rPos.right && nPrevLine<nItems && i<nItems && ppItems[i]->m_nType!=BREAK && ppItems[nPrevLine]->m_nHFormat==3 && i-nPrevLine>2 && nItems-nPrevLine>2){
					int b;
					while(nWidth<rPos.right-rPos.left){
						int bBreak=TRUE;
						for(a=nPrevLine+2;nWidth<rPos.right-rPos.left && a<i && a<nItems;a++){
							if(ppItems[a]->m_szText && !lstrcmp(ppItems[a]->m_szText,TEXT(" ")))
								continue;
							bBreak=FALSE;
							ppItems[a]->m_pPos.x++;
							for(b=a+1;b<=i && b<nItems;b++)
								ppItems[b]->m_pPos.x++;
							nWidth++;
						}
						if(bBreak)
							break;
					}
				}
				nPrevLine = i;
				if(i<nItems && ppItems[i]->m_nType==BREAK){
					ppItems[i]->m_pPos.x = 0;
					ppItems[i]->m_pPos.y = pStart.y+nMaxHeight;
					if(i<nItems-1 && ppItems[i+1]->m_nType==TEXT)
						ppItems[i]->m_pPos.y-=ppItems[i+1]->m_scBounds.cy/2;
				}
				pStart.y+=nMaxHeight;
#ifdef ABSOLUTE_ALIGN
				int q, p;
				int nUnder=0;
				// Check if pStart is below absolute aligned objects or if the break has the clear attribute
				for(q=0;q<nAbsoluteAlign;q++)
					if(!((i && ppItems[i-1]->m_nType==COMPOUND && ppItems[i-1]->m_nAbsoluteAlign!=-1) || (i>1 && ppItems[i-2]->m_nType==COMPOUND && ppItems[i-2]->m_nAbsoluteAlign!=-1)) && 
							(pStart.y>=pnAbsoluteAlignBottom[q] || i==nItems || (i<nItems && ppItems[i]->m_nType==BREAK && ppItems[i]->m_bClear))){
						if(!(i<nItems && ppItems[i]->m_nType==BREAK && ppItems[i]->m_bClear)){
							for(p=0;p<nAbsoluteAlign;p++){
								if(p==q)
									continue;
								if(pnAbsoluteAlignLeftOffset[p] && pnAbsoluteAlignLeftOffset[q] && pnAbsoluteAlignBottom[p]>pStart.y)
									break;
								if(pnAbsoluteAlignRightOffset[p] && pnAbsoluteAlignRightOffset[q] && pnAbsoluteAlignBottom[p]>pStart.y)
									break;
							}
							if(p<nAbsoluteAlign)
								continue;
						}
						rPos.left-=pnAbsoluteAlignLeftOffset[q];
						rPos.right+=pnAbsoluteAlignRightOffset[q];
						if(i<nItems && ppItems[i]->m_nType==BREAK && ppItems[i]->m_bClear){
							if(pStart.y<pnAbsoluteAlignBottom[q])
								pStart.y=pnAbsoluteAlignBottom[q];
						}
						nAbsoluteAlign--;
						if(!nAbsoluteAlign){
							free(pnAbsoluteAlignLeftOffset);
							pnAbsoluteAlignLeftOffset=NULL;
							free(pnAbsoluteAlignBottom);
							pnAbsoluteAlignBottom=NULL;
							free(pnAbsoluteAlignRightOffset);
							pnAbsoluteAlignRightOffset=NULL;
						}
						else{
							for(p=q;p<nAbsoluteAlign;p++){
								pnAbsoluteAlignBottom[p] = pnAbsoluteAlignBottom[p+1];
								pnAbsoluteAlignLeftOffset[p] = pnAbsoluteAlignLeftOffset[p+1];
								pnAbsoluteAlignRightOffset[p] = pnAbsoluteAlignRightOffset[p+1];
							}
							q--;
						}
					}
				// If the content does not fix between absolut aligned objects, move pStart below all those objects
				if(i<nItems && nAbsoluteAlign && ppItems[i]->m_nType==TEXT && *ppItems[i]->m_szText!=TEXT(' ') && ppItems[i]->m_scBounds.cx>rPos.right-rPos.left){
					for(p=0;p<nAbsoluteAlign;p++){
						if(pStart.y<pnAbsoluteAlignBottom[p])
							pStart.y=pnAbsoluteAlignBottom[p];
						rPos.left-=pnAbsoluteAlignLeftOffset[p];
						rPos.right+=pnAbsoluteAlignRightOffset[p];
					}
					free(pnAbsoluteAlignLeftOffset);
					pnAbsoluteAlignLeftOffset=NULL;
					free(pnAbsoluteAlignBottom);
					pnAbsoluteAlignBottom=NULL;
					free(pnAbsoluteAlignRightOffset);
					pnAbsoluteAlignRightOffset=NULL;
					nAbsoluteAlign=0;
				}
#endif
				pStart.x = rPos.left;
			}
		}
		if(i==nItems)
			break;
		// Set position of ordinary texts and images and compound objects
		if(ppItems[i]->m_nType!=BREAK)
		{
			ppItems[i]->m_pPos.x = pStart.x + nRowSpanPosOffset;
			ppItems[i]->m_pPos.y = pStart.y;
			pStart.x+=ppItems[i]->m_scBounds.cx;
			// Offset x-pos with cellspacing
			if(ppItems[i]->m_nType==COMPOUND)
				pStart.x+=ppItems[i]->m_nCellSpacing;
		} else
		{
			ppItems[i]->m_pPos.x = pStart.x + nRowSpanPosOffset;
			//ppItems[i]->m_pPos.y = pStart.y;
			pStart.x+=ppItems[i]->m_scBounds.cx;
		}
		// Just to make sure that those nasty rowspans that exceed the number of rows in a table doesn't cause any misalignment!
		if(i && i<nItems-1 && ppItems[i]->m_nType==BREAK && ppItems[i-1]->m_nType==COMPOUND && !nColumnWidths)
			pStart.y = ppItems[nBreakBeforeTable]->m_pPos.y + ppItems[nBreakBeforeTable]->m_scBounds.cy;
		// Check if end of table and free list of TD widths if so
		if(i<nItems-1 && ppItems[i]->m_nType==COMPOUND){
			if((i>=nItems-2) ||
			   !((ppItems[i+1]->m_nType==BREAK && ppItems[i+2]->m_nType==COMPOUND) || ppItems[i+1]->m_nType==COMPOUND) ||
			   (ppItems[i+1]->m_nType==COMPOUND && ppItems[i]->m_nCol+ppItems[i]->m_nColSpan!=ppItems[i+1]->m_nCol) ||
			   (ppItems[i+1]->m_nType==BREAK && ppItems[i+2]->m_nType==COMPOUND && (ppItems[i+2]->m_nCol!=0 || ppItems[i+2]->m_nRow!=ppItems[i]->m_nRow+1))){

				free(pnMinColumnWidths);
				pnMinColumnWidths=NULL;
				free(pnAMinColumnWidths);
				pnAMinColumnWidths=NULL;
				free(pnMaxColumnWidths);
				pnMaxColumnWidths=NULL;
				free(pnFixedColumnWidths);
				pnFixedColumnWidths=NULL;
				free(pnRowSpans);
				pnRowSpans=NULL;
				nColumnWidths=0;

				free(pnRowHeights);
				free(pnFixedRowHeights);
				pnRowHeights=NULL;
				pnFixedRowHeights=NULL;
				nRowHeights=0;

				pStart.y+=ppItems[i]->m_nCellSpacing;
				pStart.y+=ppItems[i]->m_nTableFrameSize;
			}
		}
#ifdef ABSOLUTE_ALIGN
		if(i>2 && ppItems[i]->m_nType==BREAK && ppItems[i-1]->m_nType==BREAK && ppItems[i-2]->m_nType==COMPOUND && ppItems[i-2]->m_nAbsoluteAlign!=-1){
			int a = i-2;
			for(;a>0;a--)
				if(ppItems[a]->m_nType==BREAK && ppItems[a-1]->m_nType!=COMPOUND)
					break;
			pStart.y = ppItems[a]->m_pPos.y;

			int b = nAbsoluteAlign++;
			pnAbsoluteAlignLeftOffset = (int*)realloc(pnAbsoluteAlignLeftOffset,sizeof(int)*nAbsoluteAlign);
			if(ppItems[i-2]->m_nAbsoluteAlign==0){
				pnAbsoluteAlignLeftOffset[b] = ppItems[a]->m_scBounds.cx;
				rPos.left += ppItems[a]->m_scBounds.cx;
				pStart.x = ppItems[a]->m_scBounds.cx;
			}
			else
				pnAbsoluteAlignLeftOffset[b] = 0;
			pnAbsoluteAlignRightOffset = (int*)realloc(pnAbsoluteAlignRightOffset,sizeof(int)*nAbsoluteAlign);
			pnAbsoluteAlignRightOffset[b] = ppItems[a]->m_scBounds.cx;
			if(ppItems[i-2]->m_nAbsoluteAlign==1){
				pnAbsoluteAlignRightOffset[b] = ppItems[a]->m_scBounds.cx;
				rPos.right -= ppItems[a]->m_scBounds.cx;
			}
			else
				pnAbsoluteAlignRightOffset[b] = 0;
			pnAbsoluteAlignBottom = (int*)realloc(pnAbsoluteAlignBottom,sizeof(int)*nAbsoluteAlign);
			pnAbsoluteAlignBottom[b] = pStart.y + ppItems[a]->m_scBounds.cy;
			pStart.x = rPos.left;
		}
#endif
	}
	if(nAbsoluteAlign){
		free(pnAbsoluteAlignLeftOffset);
		pnAbsoluteAlignLeftOffset=NULL;
		free(pnAbsoluteAlignBottom);
		pnAbsoluteAlignBottom=NULL;
		free(pnAbsoluteAlignRightOffset);
		pnAbsoluteAlignRightOffset=NULL;
		nAbsoluteAlign = 0;
	}
}
CHTMLViewer::CHTMLViewer(CHTMLProgress *pProgress)
{
	m_bWaitForImages = FALSE;
	m_pProgress = pProgress;
	m_ppItems = NULL;
	m_nItems = 0;
	m_bgCol = -1;
	m_nBmpBkg = -1;
	m_nItemCount = 0;
	m_nLevelCount = 0;
	m_nActiveLink = -1;
	m_nListLevel = 0;
	m_bReading = FALSE;
}
CHTMLViewer::~CHTMLViewer()
{
	int i;
	for(i=0;i<m_nItems;i++)
		delete m_ppItems[i];
	free(m_ppItems);
}
void AddItem()
{
}
void CHTMLViewer::SetHTML(TCHAR *szHTMLData,TCHAR *szFont,int nFontSize,COLORREF colText,TCHAR *szBaseUrl)
{
	m_bReading = TRUE;
	m_bgCol = -1;
	m_nBmpBkg = -1;
	m_nItemCount = 0;
	m_nLevelCount = 0;
	m_Links.Reset();
	int i;
	for(i=0;i<m_nItems;i++)
		delete m_ppItems[i];
	free(m_ppItems);
	m_ppItems=NULL;
	m_nItems=0;

	TCHAR *szText = new TCHAR[lstrlen(szHTMLData)+1];
	lstrcpy(szText,szHTMLData);
	int a, b, bTag=0, t;
	int nPre=0;
	BOOL bNoBreak=0;
	TCHAR *szUseFont = szFont;
	if(_tcsrchr(szBaseUrl,TEXT('.')) && !_tcsicmp(_tcsrchr(szBaseUrl,TEXT('.'))+1,TEXT("txt"))){
		bNoBreak = 1;
		szUseFont = TEXT("Courier New");
		nPre = 1;
	}
	for(a=b=0;szText[a];a++){
		if(!_tcsnicmp(&szText[a],TEXT("<script"),7)){
			for(;szText[a] && _tcsnicmp(&szText[a],TEXT("</script"),8);a++);
			for(;szText[a] && szText[a]!=TEXT('>');a++);
			continue;
		}
		else if(!_tcsnicmp(&szText[a],TEXT("<!--"),4)){
			for(;szText[a] && _tcsnicmp(&szText[a],TEXT("-->"),3);a++);
			for(;szText[a] && szText[a]!=TEXT('>');a++);
			continue;
		}
		else if(!_tcsnicmp(&szText[a],TEXT("<title"),6)){
			int w = a+7;
			while(szText[w] && (szText[w]==TEXT(' ') || szText[w]==TEXT('\t') || szText[w]==TEXT('\r') || szText[w]==TEXT('\n')))
				w++;
			int e;
			for(e=w;szText[e] && _tcsnicmp(&szText[e],TEXT("</title>"),8);e++);
			while(e>w && (szText[e-1]==TEXT(' ') || szText[e-1]==TEXT('\t') || szText[e-1]==TEXT('\r') || szText[e-1]==TEXT('\n')))
				e--;
			int q = szText[e];
			szText[e]=0;
			if(m_pProgress){
				TCHAR *szTitle = new TCHAR[lstrlen(&szText[w])+1];
				lstrcpy(szTitle,&szText[w]);
				ConvertFromHTMLCodes(szTitle);
				m_pProgress->SetTitle(szTitle);
				delete szTitle;
			}
			szText[e]=q;
			for(;szText[a] && _tcsnicmp(&szText[a],TEXT("</title"),7);a++);
			for(;szText[a] && szText[a]!=TEXT('>');a++);
			continue;
		}
		else if(!_tcsnicmp(&szText[a],TEXT("<body"),5)){
			CHTMLStringTable st;
			GetProperties(&szText[a],st);
			for(int t=0;t<st.GetCount();t++){
				if(!_tcsicmp(st[t][0],TEXT("bgcolor")))
					m_bgCol = GetCol(st[t][1]);
			}
		}
		else if(!_tcsnicmp(&szText[a],TEXT("<style"),6)){
			int q=a;
			for(;szText[a] && _tcsnicmp(&szText[a],TEXT("</style"),7);a++);
			if(!szText[a])
				a=q;
			for(;szText[a] && szText[a]!=TEXT('>');a++);
			continue;
		}
		else if(!_tcsnicmp(&szText[a],TEXT("<pre"),4))
			nPre++;
		else if(!_tcsnicmp(&szText[a],TEXT("</pre"),5))
			nPre--;
		if(!bTag && szText[a]==TEXT('<')){
			for(t=0;pszTags[t];t++){
				int nTL = lstrlen(pszTags[t]);
				if(!_tcsnicmp(&szText[a],pszTags[t],nTL) && 
					(!szText[a+nTL] || szText[a+nTL]==TEXT(' ') || szText[a+nTL]==TEXT('\t') || szText[a+nTL]==TEXT('\r') || szText[a+nTL]==TEXT('\n') || szText[a+nTL]==TEXT('>')))
					break;
			}
			if(!pszTags[t])
				bTag=1;
		}
		if(bTag && szText[a]==TEXT('>')){
			bTag=0;
			continue;
		}
		if(!bTag){
			if(!nPre && (szText[a]==TEXT('\t') || szText[a]==TEXT('\n') || szText[a]==TEXT('\r') || szText[a]==TEXT(' '))){
				if(b && szText[b-1]==TEXT(' '))
					continue;
				else
					szText[b++]=TEXT(' ');
			}
			else
				szText[b++] = szText[a];
		}
	}
	szText[b]=0;
	AddHTML(szText,m_ppItems,m_nItems,szUseFont,nFontSize,colText,szBaseUrl,0,0,bNoBreak);
	delete szText;

	m_bReading=FALSE;
}
void CHTMLViewer::AdjustHR(int nWidth, CHTMLItem **&ppItems, int &nItems)
{
	int i;
	for(i=0;i<nItems;i++){
		ppItems[i]->m_nMaxColumnWidth=ppItems[i]->m_nMinColumnWidth=ppItems[i]->m_nAMinColumnWidth=ppItems[i]->m_nColumnWidth=-1;
//		if(ppItems[i]->m_nType==BREAK && ppItems[i]->m_scBounds.cy && (i==nItems-1 || ppItems[i+1]->m_nType!=COMPOUND))
//			ppItems[i]->m_scBounds.cx = nWidth;
		if(ppItems[i]->m_ppSubItems)
			AdjustHR(nWidth?ppItems[i]->m_scBounds.cx-2*ppItems[i]->m_nCellPadding:0,ppItems[i]->m_ppSubItems,ppItems[i]->m_nSubItems);
	}
}
void CHTMLViewer::CalcPositions(RECT &rPos)
{
	rPos.right-=2;
	int nWidth = rPos.right;
	AdjustHR(0,m_ppItems,m_nItems);
	CalcPositions(rPos,m_ppItems,m_nItems);
	int i;
	// Measure document size
	rPos.right=rPos.bottom=0;
	for(i=0;i<m_nItems;i++){
		if(m_ppItems[i]->m_nType==TEXT && !lstrcmp(m_ppItems[i]->m_szText,TEXT(" ")))
			continue;
		if(rPos.right<m_ppItems[i]->m_pPos.x+m_ppItems[i]->m_scBounds.cx)
			rPos.right=m_ppItems[i]->m_pPos.x+m_ppItems[i]->m_scBounds.cx;
		if(rPos.bottom<m_ppItems[i]->m_pPos.y+m_ppItems[i]->m_scBounds.cy)
			rPos.bottom=m_ppItems[i]->m_pPos.y+m_ppItems[i]->m_scBounds.cy;
	}
	AdjustHR(nWidth,m_ppItems,m_nItems);
	rPos.right;
	rPos.bottom;
}
void CHTMLViewer::Draw(HDC hDC,RECT &r, int nX, int nY)
{
	if(m_bgCol!=-1){
		HBRUSH br = CreateSolidBrush(m_bgCol);
		FillRect(hDC,&r,br);
		DeleteObject(br);
	}
	Draw(hDC,r, m_ppItems,m_nItems,nX, nY);
}
void CHTMLViewer::Draw(HDC hDC, RECT &rClient, CHTMLItem **&ppItems, int &nItems,int nX, int nY)
{
	SetBkMode(hDC,TRANSPARENT);
	SetTextAlign(hDC,TA_TOP|TA_LEFT);
	int i;
	for(i=0;i<nItems;i++){
		if(ppItems[i]->m_nControlID!=-1)
			continue;
		RECT r = {ppItems[i]->m_pPos.x-nX,ppItems[i]->m_pPos.y-nY,ppItems[i]->m_scBounds.cx,ppItems[i]->m_scBounds.cy};
		r.right+=r.left;
		r.bottom+=r.top;
		if(rClient.bottom<r.top || rClient.top>r.bottom || rClient.left>r.right || rClient.right<r.left)
			continue;
		if(ppItems[i]->m_colBkg!=-1 || ppItems[i]->m_bSelected){
			HBRUSH br = CreateSolidBrush(ppItems[i]->m_bSelected?RGB(0,0,128):ppItems[i]->m_colBkg);
			FillRect(hDC,&r,br);
			DeleteObject(br);
		}
		if(ppItems[i]->m_nType==TEXT && ppItems[i]->m_scBounds.cx){
			HFONT hf = CreateFont(ppItems[i]->m_nFontSize,0,0,0,ppItems[i]->m_nFontBold?FW_BOLD:FW_NORMAL,ppItems[i]->m_nFontItalic,ppItems[i]->m_nFrames==8?1:0,ppItems[i]->m_nFrames & 16?1:0,!_tcsicmp(ppItems[i]->m_szFont,TEXT("Wingdings"))?SYMBOL_CHARSET:0,0,0,0,0,ppItems[i]->m_szFont);
			HFONT hfOld = (HFONT)SelectObject(hDC,hf);
			SetTextColor(hDC,ppItems[i]->m_bSelected?RGB(255,255,255):ppItems[i]->m_colText);
			if(m_nActiveLink>=0 && m_nActiveLink==ppItems[i]->m_nLink)
				SetTextColor(hDC,RGB(255,0,0));
//			ExtTextOut(hDC,r.left+(ppItems[i]->m_nFrames & 1)*ppItems[i]->m_nFrameSize,r.top+(ppItems[i]->m_nFrames & 2)*ppItems[i]->m_nFrameSize,ETO_CLIPPED,&r,ppItems[i]->m_szText,lstrlen(ppItems[i]->m_szText),NULL);
			TextOut(hDC,r.left+(ppItems[i]->m_nFrames & 1)*ppItems[i]->m_nFrameSize,
				r.top+(ppItems[i]->m_nFrames & 2)*ppItems[i]->m_nFrameSize,ppItems[i]->m_szText,lstrlen(ppItems[i]->m_szText));
			SelectObject(hDC,hfOld);
			DeleteObject(hf);
		}
		else if(ppItems[i]->m_nType==COMPOUND){
			Draw(hDC,rClient, ppItems[i]->m_ppSubItems,ppItems[i]->m_nSubItems,nX, nY);
		}

		if(!ppItems[i]->m_nFrames || (ppItems[i]->m_nType==TEXT && (ppItems[i]->m_nFrames==8 || ppItems[i]->m_nFrames==16)))
			continue;
		HPEN pn;
		int n;
		for(n=0;n<ppItems[i]->m_nFrameSize;n++){
			if(ppItems[i]->m_colFrame==-1)
				pn = CreatePen(PS_SOLID,1,RGB(128,128,128));
			else if(ppItems[i]->m_colFrame==-2)
				pn = CreatePen(PS_SOLID,1,RGB(255,255,255));
			else{
				if(m_nActiveLink>=0 && m_nActiveLink==ppItems[i]->m_nLink)
					pn = CreatePen(PS_SOLID,1,RGB(255,0,0));
				else
					pn = CreatePen(PS_SOLID,1,ppItems[i]->m_colFrame);
			}
			HPEN pnOld = (HPEN)SelectObject(hDC,pn);
			if(ppItems[i]->m_nFrames & 1){
				MoveToEx(hDC,r.left,r.top,NULL);
				LineTo(hDC,r.left,r.bottom-1);
			}
			if(ppItems[i]->m_nFrames & 2){
				MoveToEx(hDC,r.left,r.top,NULL);
				LineTo(hDC,r.right,r.top);
			}
			if(ppItems[i]->m_colFrame==-1 || ppItems[i]->m_colFrame==-2){
				SelectObject(hDC,pnOld);
				DeleteObject(pn);
				if(ppItems[i]->m_colFrame==-1)
					pn = CreatePen(PS_SOLID,1,RGB(255,255,255));
				else
					pn = CreatePen(PS_SOLID,1,RGB(128,128,128));
				pnOld = (HPEN)SelectObject(hDC,pn);
			}
			if(ppItems[i]->m_nFrames & 4){
				MoveToEx(hDC,r.right-1,r.top,NULL);
				LineTo(hDC,r.right-1,r.bottom-1);
			}
			if(ppItems[i]->m_nFrames & 8){
				MoveToEx(hDC,r.left,r.bottom-1,NULL);
				LineTo(hDC,r.right,r.bottom-1);
			}
			if(ppItems[i]->m_nFrames & 16){
				MoveToEx(hDC,r.left,r.bottom - 1 - (r.bottom-r.top)/2,NULL);
				LineTo(hDC,r.right,r.bottom - 1 - (r.bottom-r.top)/2);
			}
			r.left++;
			r.top++;
			r.right--;
			r.bottom--;

			SelectObject(hDC,pnOld);
			DeleteObject(pn);
		}
	}
}
int CHTMLViewer::OnMouseOver(CHTMLItem **&ppItems, int &nItems,int nX, int nY,BOOL bClick)
{
	int i;
	for(i=0;i<nItems;i++){
		if(ppItems[i]->m_nType==COMPOUND && OnMouseOver(ppItems[i]->m_ppSubItems,ppItems[i]->m_nSubItems,nX,nY,bClick))
			return 1;
		if(ppItems[i]->m_nLink==-1)
			continue;
		RECT r = {ppItems[i]->m_pPos.x,ppItems[i]->m_pPos.y,ppItems[i]->m_pPos.x+ppItems[i]->m_scBounds.cx,ppItems[i]->m_pPos.y+ppItems[i]->m_scBounds.cy};
		POINT p = {nX,nY};
		if(PtInRect(&r,p)){
			m_nActiveLink = ppItems[i]->m_nLink;
			if(bClick)
				m_pProgress->LinkClicked(m_Links[0][ppItems[i]->m_nLink]);
			else
				m_pProgress->SetCursor(TRUE,m_Links[0][ppItems[i]->m_nLink]);
			return 1;
		}
	}
	m_nActiveLink=-1;
	return 0;
}
void CHTMLViewer::OnMouseOver(int x, int y)
{
	if(!m_pProgress)
		return;
	if(!OnMouseOver(m_ppItems,m_nItems,x,y,0))
		m_pProgress->SetCursor(FALSE,TEXT(""));
}
void CHTMLViewer::OnMouseClick(int x, int y)
{
	if(!m_pProgress)
		return;
	OnMouseOver(m_ppItems,m_nItems,x,y,1);
}

void CHTMLViewer::Statistics(int &nItems, int &nImages,int &nLinks,int &nLevel)
{
	nItems = m_nItemCount;
	nImages = 0;
	nLinks = m_Links[0].GetCount();
	nLevel = m_nLevelCount;
}
void CHTMLViewer::Select(RECT &r)
{
	Select(r,m_ppItems, m_nItems);
}
void CHTMLViewer::Select(RECT &r,CHTMLItem **&ppItems, int &nItems)
{
	int i;
	for(i=0;i<nItems;i++){
		if((ppItems[i]->m_nType==TEXT || ppItems[i]->m_nType==BREAK) && !(i<nItems-1 && ppItems[i]->m_nType==BREAK && ppItems[i+1]->m_nType==COMPOUND)){
			if(r.top<ppItems[i]->m_pPos.y && r.bottom>=ppItems[i]->m_pPos.y && r.right>ppItems[i]->m_pPos.x)
				ppItems[i]->m_bSelected=TRUE;
			else if(r.top<ppItems[i]->m_pPos.y+ppItems[i]->m_scBounds.cy && r.bottom>=ppItems[i]->m_pPos.y+ppItems[i]->m_scBounds.cy && r.left<ppItems[i]->m_pPos.x)
				ppItems[i]->m_bSelected=TRUE;
			else if(ppItems[i]->m_pPos.x>r.right || ppItems[i]->m_pPos.x+ppItems[i]->m_scBounds.cx<r.left ||
				ppItems[i]->m_pPos.y>r.bottom || ppItems[i]->m_pPos.y+ppItems[i]->m_scBounds.cy<r.top)
				ppItems[i]->m_bSelected=FALSE;
			else
				ppItems[i]->m_bSelected=TRUE;
		}
		if(ppItems[i]->m_nSubItems)
			Select(r,ppItems[i]->m_ppSubItems,ppItems[i]->m_nSubItems);
	}
}
void CHTMLViewer::CopySelected()
{
	TCHAR *szCopy=NULL;
	int nLen=0;
	CopySelected(m_ppItems, m_nItems,szCopy,nLen);
	if(!nLen)
		return;

	HANDLE hTemp = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,nLen+1);
	TCHAR *szTemp = (TCHAR*)GlobalLock(hTemp);
	lstrcpy(szTemp,szCopy);
	GlobalUnlock(hTemp);
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT,hTemp);
	CloseClipboard();
	free(szCopy);
}
void CHTMLViewer::CopySelected(CHTMLItem **&ppItems, int &nItems,TCHAR *&szCopy,int &nLen)
{
	int i;
	for(i=0;i<nItems;i++){
		if(ppItems[i]->m_bSelected){
			if(ppItems[i]->m_nType==TEXT){
				int nNewLen = nLen + lstrlen(ppItems[i]->m_szText);
				szCopy = (TCHAR*)realloc(szCopy,nNewLen+1);
				lstrcpy(&szCopy[nLen],ppItems[i]->m_szText);
				nLen = nNewLen;
			}
			else if(ppItems[i]->m_nType==BREAK){
				int nNewLen = nLen + 2;
				szCopy = (TCHAR*)realloc(szCopy,nNewLen+1);
				lstrcpy(&szCopy[nLen],TEXT("\r\n"));
				nLen = nNewLen;
			}
		}
		int nPLen = nLen;
		if(ppItems[i]->m_nSubItems)
			CopySelected(ppItems[i]->m_ppSubItems,ppItems[i]->m_nSubItems,szCopy,nLen);
		if(nLen>nPLen){
			int nNewLen = nLen + 1;
			szCopy = (TCHAR*)realloc(szCopy,nNewLen+1);
			lstrcpy(&szCopy[nLen],TEXT(" "));
			nLen = nNewLen;
		}
	}
}

void CHTMLViewer::UpdateImage(CHTMLItem **&ppItems, int &nItems,int nImage)
{
	if(!m_pProgress)
		return;
	int i;
	for(i=0;i<nItems;i++){
		if(ppItems[i]->m_nType==IMAGE && ppItems[i]->m_nBmp==nImage){
			RECT r={ppItems[i]->m_pPos.x,ppItems[i]->m_pPos.y,ppItems[i]->m_scBounds.cx+ppItems[i]->m_pPos.x,ppItems[i]->m_scBounds.cy+ppItems[i]->m_pPos.y};
			m_pProgress->UpdateScreen(r);
		}
		else if(ppItems[i]->m_nType==COMPOUND)
			UpdateImage(ppItems[i]->m_ppSubItems,ppItems[i]->m_nSubItems,nImage);
	}
}
void CHTMLViewer::UpdateImage(int nImage)
{
	UpdateImage(m_ppItems,m_nItems,nImage);
}
void CHTMLViewer::UpdateLink(CHTMLItem **&ppItems, int &nItems,int nLink)
{
	if(!m_pProgress)
		return;
	int i;
	for(i=0;i<nItems;i++){
		if(ppItems[i]->m_nLink==nLink){
			RECT r={ppItems[i]->m_pPos.x,ppItems[i]->m_pPos.y,ppItems[i]->m_scBounds.cx+ppItems[i]->m_pPos.x,ppItems[i]->m_scBounds.cy+ppItems[i]->m_pPos.y};
			m_pProgress->UpdateScreen(r);
		}
		else if(ppItems[i]->m_nType==COMPOUND)
			UpdateLink(ppItems[i]->m_ppSubItems,ppItems[i]->m_nSubItems,nLink);
	}
}
void CHTMLViewer::UpdateLink(TCHAR *szLink)
{
	int i;
	for(i=0;i<m_Links[0].GetCount();i++)
		if(!lstrcmp(m_Links[0][i],szLink)){
			UpdateLink(m_ppItems,m_nItems,i);
			break;
		}
}

BOOL CHTMLViewer::IsReading()
{
	return m_bReading;
}

CHTMLStringVector::CHTMLStringVector() : m_nStrings(0), m_pszStrings(NULL), m_pnStrings(NULL), m_pnIndexValues(NULL)
{
}
CHTMLStringVector::~CHTMLStringVector()
{
	Clean();
}
void CHTMLStringVector::Clean()
{
	int i;
	for(i=0;i<m_nStrings;i++)
		delete m_pszStrings[i];
	free(m_pszStrings);
	m_nStrings=0;
	m_pszStrings=NULL;
	free(m_pnStrings);
	m_pnStrings=NULL;
	free(m_pnIndexValues);
	m_pnIndexValues=NULL;
}
int CHTMLStringVector::MakeIndex(TCHAR *szString, int nLenght)
{
	int nIndex=0, i;
	for(i=0;i<nLenght;i++)
		nIndex = (255*nIndex+szString[i]);
	return nIndex;
}

int CHTMLStringVector::AddString(TCHAR *szString,int nSize)
{
	if(!szString)
		return 0;
	int i=m_nStrings++;
	m_pnStrings = (int*)realloc(m_pnStrings,sizeof(int)*m_nStrings);
	m_pnIndexValues = (int*)realloc(m_pnIndexValues,sizeof(int)*m_nStrings);
	if(nSize==-1)
		m_pnStrings[i] = lstrlen(szString);
	else
		m_pnStrings[i] = nSize;
	m_pszStrings = (TCHAR**)realloc(m_pszStrings,sizeof(TCHAR*)*m_nStrings);
	m_pszStrings[i] = new TCHAR[m_pnStrings[i]+1];
	if(m_pnStrings[i])
		memcpy(m_pszStrings[i],szString,m_pnStrings[i] * sizeof(TCHAR));
	m_pszStrings[i][m_pnStrings[i]]=0;
	if(m_pnStrings[i])
		m_pnIndexValues[i] = MakeIndex(szString, m_pnStrings[i]);
	else
		m_pnIndexValues[i]=0;
	return 1;
}
int CHTMLStringVector::SetString(int nIndex, TCHAR *szString, int nSize)
{
	if(nIndex<0 || nIndex>=m_nStrings)
		return 0;
	TCHAR *szTmp = m_pszStrings[nIndex];
	if(nSize==-1)
		m_pnStrings[nIndex] = lstrlen(szString);
	else
		m_pnStrings[nIndex] = nSize;
	m_pszStrings[nIndex] = new TCHAR[m_pnStrings[nIndex]+1];
	memcpy(m_pszStrings[nIndex],szString,(m_pnStrings[nIndex]+1)  * sizeof(TCHAR));
	m_pnIndexValues[nIndex] = MakeIndex(szString, m_pnStrings[nIndex]);
	delete szTmp;
	return 1;
}
TCHAR *CHTMLStringVector::GetString(int nIndex,int *pnSize)
{
	if(nIndex<0 || nIndex>=m_nStrings){
		if(pnSize)
			*pnSize=0;
		return TEXT("");
	}
	if(pnSize)
		*pnSize = m_pnStrings[nIndex];
	return m_pszStrings[nIndex]?m_pszStrings[nIndex]:TEXT("");
}
int CHTMLStringVector::GetIndex(int nIndex)
{
	if(nIndex<0 || nIndex>=m_nStrings)
		return -1;
	return m_pnIndexValues[nIndex];
}
int CHTMLStringVector::GetCount()
{
	return m_nStrings;
}
int CHTMLStringVector::SetCount(int nCount)
{
	int i;
	if(nCount<m_nStrings){
		for(i=nCount;i<m_nStrings;i++)
			delete m_pszStrings[i];
	}
	else{
		m_pszStrings = (TCHAR**)realloc(m_pszStrings,sizeof(TCHAR*)*nCount);
		m_pnStrings = (int*)realloc(m_pnStrings,sizeof(int)*nCount);
		m_pnIndexValues = (int*)realloc(m_pnIndexValues,sizeof(int)*nCount);
		for(i=m_nStrings;i<nCount;i++){
			m_pszStrings[i] = new TCHAR[1];
			*m_pszStrings[i]=0;
			m_pnStrings[i]=0;
			m_pnIndexValues[i]=-1;
		}
	}
	return (m_nStrings = nCount);
}
TCHAR *CHTMLStringVector::operator [] (int nIndex)
{
	return GetString(nIndex);
}
CHTMLStringVector &CHTMLStringVector::operator =(CHTMLStringVector &s)
{
	Clean();
	int i, n;
	for(i=0;i<s.GetCount();i++){
		TCHAR *sz = s.GetString(i,&n);
		AddString(sz,n);
	}
	return *this;
}
void CHTMLStringVector::DeleteString(int nIndex)
{
	if(nIndex<0 || nIndex>=m_nStrings)
		return;
	delete m_pszStrings[nIndex];
	m_nStrings--;
	for(int i=nIndex;i<m_nStrings;i++){
		m_pszStrings[i] = m_pszStrings[i+1];
		m_pnStrings[i] = m_pnStrings[i+1];
		m_pnIndexValues[i] = m_pnIndexValues[i+1];
	}
}
int CHTMLStringVector::MoveString(int nFrom, int nTo)
{
	if(nFrom<0 ||nFrom>=m_nStrings)
		return 0;
	if(nTo<0 || nTo>=m_nStrings)
		return 0;
	TCHAR *szTmpStr = m_pszStrings[nFrom];
	m_pszStrings[nFrom] = m_pszStrings[nTo];
	m_pszStrings[nTo] = szTmpStr;
	int nTmp = m_pnStrings[nFrom];
	m_pnStrings[nFrom] = m_pnStrings[nTo];
	m_pnStrings[nTo] = nTmp;
	nTmp = m_pnIndexValues[nFrom];
	m_pnIndexValues[nFrom] = m_pnIndexValues[nTo];
	m_pnIndexValues[nTo] = nTmp;
	return 1;
}
CHTMLStringTable::CHTMLStringTable() : m_nVektors(0), m_pVektors(NULL), m_nRowSize(0)
{
}
CHTMLStringTable::~CHTMLStringTable()
{
	int i;
	for(i=0;i<m_nVektors;i++)
		delete m_pVektors[i];
	free(m_pVektors);
}
int CHTMLStringTable::AddRow(TCHAR **pszRow, int nRow)
{
	if(nRow>m_nRowSize)
		m_nRowSize = nRow;
	int j, i = AddRow()-1;
	for(j=0;j<nRow;j++)
		m_pVektors[i]->AddString(pszRow[j]);
	return m_nVektors;
}
int CHTMLStringTable::AddRow()
{
	int i = m_nVektors++;
	m_pVektors = (CHTMLStringVector**)realloc(m_pVektors,sizeof(CHTMLStringVector*)*m_nVektors);
	m_pVektors[i] = new CHTMLStringVector();
	return m_nVektors;
}
int CHTMLStringTable::InsertRow(int nRow)
{
	if(nRow<0 || nRow>m_nVektors)
		return 0;
	if(nRow==m_nVektors)
		AddRow();
	int i;
	m_nVektors++;
	m_pVektors = (CHTMLStringVector**)realloc(m_pVektors,sizeof(CHTMLStringVector*)*m_nVektors);
	for(i=m_nVektors-1;i>nRow;i--)
		m_pVektors[i] = m_pVektors[i-1];
	m_pVektors[nRow] = new CHTMLStringVector();
	return 1;
}
int CHTMLStringTable::AddString(int nRow, TCHAR *szString, int nString)
{
	if(nRow<0 || nRow>=m_nVektors)
		return 0;
	m_pVektors[nRow]->AddString(szString,nString);
	if(nRow>m_pVektors[nRow]->GetCount())
		m_nRowSize = m_pVektors[nRow]->GetCount();
	return 1;
}
int CHTMLStringTable::DeleteRow(int nRow)
{
	if(nRow<0 || nRow>m_nVektors)
		return 0;
	CHTMLStringVector *p = m_pVektors[nRow];
	m_nVektors--;
	for(;nRow<m_nVektors;nRow++)
		m_pVektors[nRow] = m_pVektors[nRow+1];
	delete p;
	return 1;
}
int CHTMLStringTable::SetString(int nRow, int nIndex, TCHAR *szString, int nString)
{
	if(nRow<0 || nRow>=m_nVektors)
		return 0;
	return m_pVektors[nRow]->SetString(nIndex,szString,nString);
}
TCHAR *CHTMLStringTable::GetString(int nRow, int nIndex,int *pnSize)
{
	if(nRow<0 || nRow>=m_nVektors)
		return TEXT("");
	return m_pVektors[nRow]->GetString(nIndex,pnSize);
}
int CHTMLStringTable::GetCount()		{ return m_nVektors;}

CHTMLStringVector &CHTMLStringTable::operator [] (int nRow)
{
	if(nRow<0 || nRow>=m_nVektors)
		return m_err;
	return *m_pVektors[nRow];
}
CHTMLStringTable &CHTMLStringTable::operator = (CHTMLStringTable &st)
{
	while(GetCount())
		DeleteRow(0);
	int i, j, n;
	for(i=0;i<st.GetCount();i++){
		AddRow();
		for(j=0;j<st[i].GetCount();j++){
			TCHAR *sz = st.GetString(i,j,&n);
			AddString(i,sz,n);
		}
	}
	return *this;
}


int CHTMLStringTable::MoveRow(int nFrom, int nTo)
{
	if(nFrom<0 || nFrom>=m_nVektors || nTo<0 || nTo>=m_nVektors)
		return 0;
	CHTMLStringVector *pTmp = m_pVektors[nFrom];
	m_pVektors[nFrom] = m_pVektors[nTo];
	m_pVektors[nTo] = pTmp;
	return 1;
}

void CHTMLStringTable::Reset()
{
	while(GetCount())
		DeleteRow(GetCount()-1);
}


int CHTMLStringTable::SplitString(TCHAR *szData,TCHAR *szFieldSep, TCHAR *szRowSep)
{
	AddRow();
	int q, p;
	int nFieldSep=0;
	if(szFieldSep)
		nFieldSep = lstrlen(szFieldSep);
	int nRowSep=0;
	if(szRowSep)
		nRowSep = lstrlen(szRowSep);
	for(p=q=0;szData[q];q++){
		if(!szData[q] || (nFieldSep && !_tcsncmp(&szData[q],szFieldSep,nFieldSep)) || 
				(nRowSep && !_tcsncmp(&szData[q],szRowSep,nRowSep))){
			AddString(GetCount()-1,&szData[p],q-p);
			if(nRowSep && !_tcsncmp(&szData[q],szRowSep,nRowSep)){
				q+=nRowSep-1;
				AddRow();
			}
			else
				q+=nFieldSep-1;
			p=q+1;
		}
	}
	if(szData[p])
		AddString(GetCount()-1,&szData[p]);
	else if(m_pVektors[GetCount()-1]->GetCount()==0)
		DeleteRow(GetCount()-1);
	return 1;
}


