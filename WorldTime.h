/*
 * 世界時計メインヘッダ
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
*/
#include "DrawWorldTime.h"
#include "WorldTimeConfig.h"
#include "resource.h"

// #define __DEBUG__ 1

/* プロトタイプ宣言 */
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitInstance(HINSTANCE, int);

void addMenu(HWND, bool, bool);
void deleteMenu(HWND);
void toggleMenuStayTop(HWND);
void toggleShowToolTip(HWND);

void OnMouseMove(HWND, DrawWorldTime*);
void OnTimer(HWND, DrawWorldTime*);

#ifdef __DEBUG__
void debug(TCHAR* szBuf);
#endif
