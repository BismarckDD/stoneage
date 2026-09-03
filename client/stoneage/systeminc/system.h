#pragma once

/**** C INCLUDE ****/
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <clocale>

/**** CPP INCLUDE ****/
#include <string>

/**** WIN32 INCLUDE ****/
#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <io.h>
#include <ime.h>  /* WIN输入法 */
#include <imm.h>  /* WIN输入法管理器*/
#include <direct.h>
#include <winnls32.h>  /* 还是输入法 */
#include <tlhelp32.h>  /* TOOL HELPER*/
#endif

/**** USER INCLUDE ****/
#include "version.h"
#include "directDraw.h"
#include "game/main.h"
#include "game/game_main.h"
#include "sprmgr.h"
#include "init.h"
#include "process.h"
#include "action.h"
#include "sprdisp.h"
#include "oft/work.h"
#include "chat.h"
#include "font.h"
#include "mouse.h"
#include "game/radar.h"
#include "gemini.h"
#include "pattern.h"
#include "tools/math2.h"
