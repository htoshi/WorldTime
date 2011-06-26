/*
 * 初期設定ファイルクラス
 * $Id$
 *
 * Copyright (C) 2011, Toshi All rights reserved.
*/
#pragma once

#include <windows.h>
#include <tchar.h>

// 初期設定ファイル名
#define INIFILENAME _T("worldtime.ini")

// ウィンドウのデフォルト位置
#define DEFAULT_WINDOW_TOP _T("10")
#define DEFAULT_WINDOW_LEFT _T("10")

// エリア情報最大値
#define MAX_AREA 15

// パス最大値
#define MAX_PATHLEN 32768

#define OK 0
#define ERR -1

/* 日時情報格納構造体 */
typedef struct {
	TCHAR szDate[6];		// 日付 (01/01)
	TCHAR szTime[6];		// 時刻 (12:00)
	TCHAR szName[10];		// ゾーン名
	bool isDST;				// 夏時間フラグ (true: 夏時間 false: 標準時間)
	int x;					// 表示位置 X 座標
	int y;					// 表示位置 Y 座標
	double offset;			// GMT からのオフセット (時間)
	TCHAR szInfo[128];		// ツールチップに表示する情報
	bool hasDST;			// 夏時間保持フラグ (true: 夏時間あり false: 夏時間なし)
	int iDSTStartMonth;		// 夏時間開始月
	TCHAR szDSTStartType;	// 夏時間開始曜日タイプ(F:第一週 E:最終週)
	int iDSTStartWeekDay;	// 夏時間開始曜日 (0:日曜日～6:土曜日)
	int iDSTStartTime;		// 夏時間開始時刻 (分)
	int iDSTEndMonth;		// 夏時間終了月
	TCHAR szDSTEndType;		// 夏時間終了曜日タイプ(F:第一週 E:最終週)
	int iDSTEndWeekDay;		// 夏時間終了曜日 (0:日曜日～6:土曜日)
	int iDSTEndTime;		// 夏時間終了時刻 (分)
} datetimeinfo_t;

// 日時情報格納構造体
typedef struct {
	short Year;
	short Month;
	short DayOfWeek;
	short Day;
	short Hour;
	short Minute;
	short Second;
	bool  isDST;
} datetime_t;

// 夏時間情報構造体
typedef struct {
	short Month;	// 月
	short Day;		// 日
	TCHAR Type;		// 曜日タイプ (F: 第一週 E: 最終週 1～5: 第N週)
	short WeekDay;	// 曜日 (0:日曜日～6:土曜日)
	short Time;		// 時刻(分)
} dstinfo_t;

/* 設定ファイルクラス */
class Config {

	private:
		int parseDSTInfo(TCHAR*, int*, TCHAR*, int*, int*);

	public:
		Config();	// コンストラクタ
		~Config();	// デストラクタ

		int getIniFileName(TCHAR*, DWORD);

		int readWindowRect(RECT*);	// Window位置を読み込んで返す
		int writeWindowRect(RECT*);	// Window位置を書き出す

		int readTopMostFlag(bool*);	// 常に最前面表示情報読み出し
		int writeTopMostFlag(bool);	// 常に最前面表示情報書き出し

		int readToolTipFlag(bool*);	// ツールチップ表示情報読み出し
		int writeToolTipFlag(bool);	// ツールチップ表示情報書き出し

		int readAlpha(int*);		// 透明度読み出し
		int writeAlpha(int);		// 透明度書き出し

		int readSimpleMode(bool*);	// シンプルモード読み出し
		int writeSimpleMode(bool);	// シンプルモード書き出し

		int readAreaInfo(datetimeinfo_t datetimeinfo[]);	// エリア情報取得
};
