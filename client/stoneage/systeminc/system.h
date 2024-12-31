#pragma once

/**** SYSTEM INCLUDE ****/
#include <windows.h>
#include <windowsx.h>
#include <cstring>
#include <cstdio>

/**** DirectDraw Include ****/
#include <ddraw.h>

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

#ifdef _STONDEBUG_
// defined in game_main.cpp
extern unsigned int iTotalUseTime;
extern unsigned int iTotalProcTime;
extern unsigned int iTotalRunCount;
// defined in winnt.h >> windows.h
extern LARGE_INTEGER tf;
#endif

