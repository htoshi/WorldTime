/*
 * 初期設定ファイルクラス
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#ifndef __WORLDTIMECONFIG_H__
#define __WORLDTIMECONFIG_H__

#include <windows.h>
#include <tchar.h>

// 初期設定ファイル名
#define INIFILENAME "worldtime.ini"

// ウィンドウのデフォルト位置
#define DEFAULT_WINDOW_TOP "10"
#define DEFAULT_WINDOW_LEFT "10"

#define OK 0
#define ERR -1

// #define __DEBUG__ 1

/* 世界時刻設定ファイルクラス */
class WorldTimeConfig {

    private:
        int getIniFileName(TCHAR*, DWORD);

#ifdef __DEBUG__
        void debug(TCHAR* szBuf);
#endif

    public:

        WorldTimeConfig();      // コンストラクタ
        ~WorldTimeConfig();     // デストラクタ

        int readWindowRect(RECT*);   // Window位置を読み込んで返す
        int writeWindowRect(RECT*);  // Window位置を書き出す

        int readTopMostFlag(bool*);  // 常に最前面表示情報読み出し
        int writeTopMostFlag(bool);  // 常に最前面表示情報書き出し

        int readToolTipFlag(bool*);  // ツールチップ表示情報読み出し
        int writeToolTipFlag(bool);  // ツールチップ表示情報書き出し
};

#endif /* __WORLDTIMECONFIG_H__ */
