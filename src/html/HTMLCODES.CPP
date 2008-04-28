// HTMLCodes
// Provides different conversions
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

#include <windows.h>
#include <TCHAR.h>

struct HTMLCODES{
	TCHAR szCode[10];
	TCHAR Code;
};
HTMLCODES g_HTMLCodes[] = {
	{TEXT("&quot;"),TEXT('"')},
	{TEXT("&amp;"),TEXT('&')},
	{TEXT("&lt;"),TEXT('<')},
	{TEXT("&gt;"),TEXT('>')},
	{TEXT("&nbsp;"),TEXT(' ')},
	{TEXT("&iexcl;"),TEXT('¡')},
	{TEXT("&cent;"),TEXT('¢')},
	{TEXT("&pound;"),TEXT('£')},
	{TEXT("&curren;"),TEXT('¤')},
	{TEXT("&yen;"),TEXT('¥')},
	{TEXT("&brvbar;"),TEXT('¦')},
	{TEXT("&sect;"),TEXT('§')},
	{TEXT("&uml;"),TEXT('¨')},
	{TEXT("&copy;"),TEXT('©')},
	{TEXT("&ordf;"),TEXT('ª')},
	{TEXT("&laquo;"),TEXT('«')},
	{TEXT("&not;"),TEXT('¬')},
	{TEXT("&shy;"),TEXT('­')},
	{TEXT("&reg;"),TEXT('®')},
	{TEXT("&macr;"),TEXT('¯')},
	{TEXT("&deg;"),TEXT('°')},
	{TEXT("&plusmn;"),TEXT('±')},
	{TEXT("&sup2;"),TEXT('²')},
	{TEXT("&sup3;"),TEXT('³')},
	{TEXT("&acute;"),TEXT('´')},
	{TEXT("&micro;"),TEXT('µ')},
	{TEXT("&para;"),TEXT('¶')},
	{TEXT("&middot;"),TEXT('·')},
	{TEXT("&cedil;"),TEXT('¸')},
	{TEXT("&sup1;"),TEXT('¹')},
	{TEXT("&ordm;"),TEXT('º')},
	{TEXT("&raquo;"),TEXT('»')},
	{TEXT("&frac14;"),TEXT('¼')},
	{TEXT("&frac12;"),TEXT('½')},
	{TEXT("&frac34;"),TEXT('¾')},
	{TEXT("&iquest;"),TEXT('¿')},
	{TEXT("&Agrave;"),TEXT('À')},
	{TEXT("&Aacute;"),TEXT('Á')},
	{TEXT("&Acirc;"),TEXT('Â')},
	{TEXT("&Atilde;"),TEXT('Ã')},
	{TEXT("&Auml;"),TEXT('Ä')},
	{TEXT("&Aring;"),TEXT('Å')},
	{TEXT("&AElig;"),TEXT('Æ')},
	{TEXT("&Ccedil;"),TEXT('Ç')},
	{TEXT("&Egrave;"),TEXT('È')},
	{TEXT("&Eacute;"),TEXT('É')},
	{TEXT("&Ecirc;"),TEXT('Ê')},
	{TEXT("&Euml;"),TEXT('Ë')},
	{TEXT("&Igrave;"),TEXT('Ì')},
	{TEXT("&Iacute;"),TEXT('Í')},
	{TEXT("&Icirc;"),TEXT('Î')},
	{TEXT("&Iuml;"),TEXT('Ï')},
	{TEXT("&ETH;"),TEXT('Ð')},
	{TEXT("&Ntilde;"),TEXT('Ñ')},
	{TEXT("&Ograve;"),TEXT('Ò')},
	{TEXT("&Oacute;"),TEXT('Ó')},
	{TEXT("&Ocirc;"),TEXT('Ô')},
	{TEXT("&Otilde;"),TEXT('Õ')},
	{TEXT("&Ouml;"),TEXT('Ö')},
	{TEXT("&times;"),TEXT('×')},
	{TEXT("&Oslash;"),TEXT('Ø')},
	{TEXT("&Ugrave;"),TEXT('Ù')},
	{TEXT("&Uacute;"),TEXT('Ú')},
	{TEXT("&Ucirc;"),TEXT('Û')},
	{TEXT("&Uuml;"),TEXT('Ü')},
	{TEXT("&Yacute;"),TEXT('Ý')},
	{TEXT("&THORN;"),TEXT('Þ')},
	{TEXT("&szlig;"),TEXT('ß')},
	{TEXT("&agrave;"),TEXT('à')},
	{TEXT("&aacute;"),TEXT('á')},
	{TEXT("&acirc;"),TEXT('â')},
	{TEXT("&atilde;"),TEXT('ã')},
	{TEXT("&auml;"),TEXT('ä')},
	{TEXT("&aring;"),TEXT('å')},
	{TEXT("&aelig;"),TEXT('æ')},
	{TEXT("&ccedil;"),TEXT('ç')},
	{TEXT("&egrave;"),TEXT('è')},
	{TEXT("&eacute;"),TEXT('é')},
	{TEXT("&ecirc;"),TEXT('ê')},
	{TEXT("&euml;"),TEXT('ë')},
	{TEXT("&igrave;"),TEXT('ì')},
	{TEXT("&iacute;"),TEXT('í')},
	{TEXT("&icirc;"),TEXT('î')},
	{TEXT("&iuml;"),TEXT('ï')},
	{TEXT("&eth;"),TEXT('ð')},
	{TEXT("&ntilde;"),TEXT('ñ')},
	{TEXT("&ograve;"),TEXT('ò')},
	{TEXT("&oacute;"),TEXT('ó')},
	{TEXT("&ocirc;"),TEXT('ô')},
	{TEXT("&otilde;"),TEXT('õ')},
	{TEXT("&ouml;"),TEXT('ö')},
	{TEXT("&divide;"),TEXT('÷')},
	{TEXT("&oslash;"),TEXT('ø')},
	{TEXT("&ugrave;"),TEXT('ù')},
	{TEXT("&uacute;"),TEXT('ú')},
	{TEXT("&ucirc;"),TEXT('û')},
	{TEXT("&uuml;"),TEXT('ü')},
	{TEXT("&yacute;"),TEXT('ý')},
	{TEXT("&thorn;"),TEXT('þ')},
	{TEXT("&yuml;"),TEXT('ÿ')},
	{TEXT("&OElig;"),TEXT('E')},
	{TEXT("&oelig;"),TEXT('e')},
	{TEXT("&Scaron;"),TEXT('S')},
	{TEXT("&scaron;"),TEXT('s')},
	{TEXT("&Yuml;"),TEXT('ÿ')},
	{TEXT("&fnof;"),TEXT('f')},
	{TEXT("&circ;"),TEXT('^')},
	{TEXT("&tilde;"),TEXT('~')},
	{TEXT("&ndash;"),TEXT('-')},
	{TEXT("&mdash;"),TEXT('-')},
	{TEXT("&lsquo;"),TEXT('‘')},
	{TEXT("&rsquo;"),TEXT('’')},
	{TEXT("&sbquo;"),TEXT(',')},
	{TEXT("&ldquo;"),TEXT('\"')},
	{TEXT("&rdquo;"),TEXT('\"')},
	{TEXT("&bdquo;"),TEXT('„')},
	{TEXT("&dagger;"),TEXT('†')},
	{TEXT("&Dagger;"),TEXT('‡')},
	{TEXT("&bull;"),TEXT('•')},
	{TEXT("&hellip;"),TEXT('…')},
	{TEXT("&permil;"),TEXT('‰')},
	{TEXT("&lsaquo;"),TEXT('‹')},
	{TEXT("&rsaquo;"),TEXT('›')},
	{TEXT("&euro;"),TEXT('€')},
	{TEXT("&trade;"),TEXT('™')},
	{{(char)0x97,(char)0x00},TEXT('-')},
	{{(char)0xA0,(char)0x00},TEXT(' ')},
	{TEXT(""),0}
};

LPTSTR ConvertFromHTMLCodes(LPTSTR szText)
{
	int i, j, k;
	for(k=j=0;szText[j];j++){
		for(i=0;g_HTMLCodes[i].Code;i++){
			if(!_tcsncmp(&szText[j],g_HTMLCodes[i].szCode,lstrlen(g_HTMLCodes[i].szCode))){
				szText[k++] = g_HTMLCodes[i].Code;
				j+=lstrlen(g_HTMLCodes[i].szCode)-1;
				break;
			}
		}
		if(!g_HTMLCodes[i].Code){
			if(!_tcsnicmp(&szText[j], TEXT("&#"),2)){
				j+=2;
				szText[k++] = _ttoi(&szText[j]);
				while(szText[j] && szText[j]!=';' && szText[j]!=' ')
					j++;
				if(!szText[j])
					break;
			}
			else
				szText[k++] = szText[j];
		}
	}
	szText[k]=0;
	return szText;
}

