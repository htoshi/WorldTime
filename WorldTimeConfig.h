/*
 * 初期設定ファイルクラス
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
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

// エリア情報最大値
#define MAX_AREA 10

#define OK 0
#define ERR -1

//#define __DEBUG__ 1

/* 日時情報格納構造体 */
typedef struct {
    TCHAR szDate[6];        // 日付 (01/01)
    TCHAR szTime[6];        // 時刻 (12:00)
    TCHAR szName[5];        // ゾーン名
    bool isDST;             // 夏時間フラグ (true: 夏時間 false: 標準時間)
    int x;                  // 表示位置 X 座標
    int y;                  // 表示位置 Y 座標
    double offset;          // GMT からのオフセット (時間)
    TCHAR szInfo[128];      // ツールチップに表示する情報
    bool hasDST;            // 夏時間保持フラグ (true: 夏時間あり false: 夏時間なし)
    int iDSTStartMonth;     // 夏時間開始月
    TCHAR szDSTStartType;   // 夏時間開始曜日タイプ(F:第一週 E:最終週)
    int iDSTStartWeekDay;   // 夏時間開始曜日 (0:日曜日～6:土曜日)
    int iDSTStartTime;      // 夏時間開始時刻 (分)
    int iDSTEndMonth;       // 夏時間終了月
    TCHAR szDSTEndType;     // 夏時間終了曜日タイプ(F:第一週 E:最終週)
    int iDSTEndWeekDay;     // 夏時間終了曜日 (0:日曜日～6:土曜日)
    int iDSTEndTime;        // 夏時間終了時刻 (分)
} datetimeinfo_t;

/* 世界時刻設定ファイルクラス */
class WorldTimeConfig {

    private:
        int getIniFileName(TCHAR*, DWORD);

#ifdef __DEBUG__
        void debug(TCHAR* szBuf);
#endif

        int parseDSTInfo(TCHAR*, int*, TCHAR*, int*, int*);

    public:

        WorldTimeConfig();      // コンストラクタ
        ~WorldTimeConfig();     // デストラクタ

        int readWindowRect(RECT*);   // Window位置を読み込んで返す
        int writeWindowRect(RECT*);  // Window位置を書き出す

        int readTopMostFlag(bool*);  // 常に最前面表示情報読み出し
        int writeTopMostFlag(bool);  // 常に最前面表示情報書き出し

        int readToolTipFlag(bool*);  // ツールチップ表示情報読み出し
        int writeToolTipFlag(bool);  // ツールチップ表示情報書き出し

        int readAreaInfo(datetimeinfo_t datetimeinfo[]);  // エリア情報取得
};

#endif /* __WORLDTIMECONFIG_H__ */
