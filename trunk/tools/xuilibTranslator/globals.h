#pragma once

#include <Windows.h>
#include <tchar.h>
#include <xuilib.h>

#define FREE_CLEAR_STR(str) if(str) delete str; str = NULL;
#define MAKE_STR(str, cpstr) FREE_CLEAR_STR(str); if(cpstr) { str = new TCHAR[lstrlen(cpstr) + 1]; lstrcpy(str, cpstr); }
