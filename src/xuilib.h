#pragma		once
#include <ObjBase.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <shlwapi.h>
#include "XUIElement.h"
#include "XUIDialog.h"
#include "XUIWindow.h"
#include "XUITextBox.h"
#include "XUIBox.h"
#include "XUIButton.h"
#include "XUIGroupbox.h"
#include "XUILabel.h"
#include "XUICheckbox.h"
#include "XUIRadio.h"
#include "XUIComboBox.h"
#include "XUIMenuPopup.h"
#include "XUIMenuItem.h"
#include "XUIRadioGroup.h"
#include "XUISpin.h"
#include "XUITabBox.h"
#include "XUITabPanel.h"
#include "XUISpacer.h"
#include "XUIMenuBar.h"
#include "XUIComboItem.h"
#include "XUIMenuSeparator.h"
#include "XUITabSwitcher.h"
#include "XUITab.h"
#include "XUIEngine.h"
#include "XUIColor.h"
#include "XUISlider.h"
#include "XUIHotkey.h"
#include "XUIPicture.h"
#include "XUIListbox.h"
#include "XUILine.h"
#include "XUIUrl.h"
#include "XUIList.h"
#include "XUISelectFile.h"
#include "XUIExt.h"
#include "XUIListCol.h"
#include "XUIChecksList.h"
#include "XUICheckItem.h"
#include "XUIFreeDraw.h"
#include "XUIDisabler.h"
#include "XUISendCMD.h"
#include "XUIProgress.h"
#include "XUIOpenHelp.h"
#include "XUIDatePicker.h"
#include "XUISelectFolder.h"
#include "XUIToolbar.h"
#include "XUIRTF.h"
#include "XUIStatusBar.h"
#include "XUISplitter.h"


// standard events
#define XUI_EVENT_CHANGED				L"changed"
#define XUI_EVENT_CLICKED				L"clicked"
#define XUI_EVENT_CONTEXTMENU			L"contextmenu"
#define XUI_EVENT_DBL_CLICK				L"dbl-click"
// Tab switcher events
#define XUI_EVENT_TAB_INIT				L"tab-init"
#define XUI_EVENT_TAB_ACTIVATE			L"tab-activate"
#define XUI_EVENT_TAB_DEACTIVATE		L"tab-deactivate"
// List events
#define XUI_EVENT_LST_ITEMACTIVATE		L"lst-itemactivate"
#define XUI_EVENT_LST_ITEMCHANGED		L"lst-itemchanged"
#define XUI_EVENT_LST_RCLICK			L"lst-rclick"
#define XUI_EVENT_LST_CLICK				L"lst-click"
#define XUI_EVENT_LST_DBLCLK			L"lst-dblclk"
#define XUI_EVENT_LST_COLUMNCLICK		L"lst-columnclick"
#define XUI_EVENT_LST_CUSTOMDRAW		L"lst-customdraw"
#define XUI_EVENT_LST_KEYDOWN			L"lst-keydown"
#define XUI_EVENT_LST_COLSCHANGED		L"lst-colchanged"
#define XUI_EVENT_LST_REQUESTCOLS		L"lst-requestcols"
// Scrollbar and trackbar events
#define XUI_EVENT_HSCROLL				L"hscroll"
#define XUI_EVENT_VSCROLL				L"vscroll"
// owner draw support
#define XUI_EVENT_MEASUREITEM			L"measureitem"
#define XUI_EVENT_DRAWITEM				L"drawitem"
// select file
#define XUI_EVENT_FSSELECTED			L"fsselected"
#define XUI_EVENT_FSINITIALDIR			L"fsinitialdir"
// free draw events
#define XUI_EVENT_FREEDRAW				L"freedraw"
#define XUI_EVENT_MOUSEMOVE				L"mousemove"
#define XUI_EVENT_LBUTTONDOWN			L"lbuttondown"
#define XUI_EVENT_RBUTTONDOWN			L"rbuttondown"
#define XUI_EVENT_LBUTTONUP				L"lbuttonup"
#define XUI_EVENT_RBUTTONUP				L"rbuttonup"
#define XUI_EVENT_SIZE					L"size"
// toolbars events
#define XUI_EVENT_TOOLBAR_CHANGED		L"toolbar-changed"
#define XUI_EVENT_TOOLBAR_DROPDOWN		L"toolbar-dropdown"
#define XUI_EVENT_TOOLBAR_CUSTOMDRAW	L"toolbar-customdraw"


// utils
inline BOOL IS_SAME_STR(LPCWSTR str1, LPCWSTR str2)
{
	if(!str1 && !str2) return TRUE;
	if(str1 && str2)
	{
		if (!StrCmpIW(str1, str2))
		{
			return TRUE;
		}
	}
	return FALSE;
}