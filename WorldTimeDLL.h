/*
 * 世界の時間を取得
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#ifndef __WORLDTIMEDLL_H__
#define __WORLDTIMEDLL_H__

#include <windows.h>

// DLL 用宣言(コンパイラオプションで /D "_EXPORTING" を指定)
#ifdef _EXPORTING
#define _EXPORT extern "C" __declspec(dllexport)
#else
#define _EXPORT extern "C" __declspec(dllimport)
#endif

#define OK 0
#define ERR -1

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

/* DLL 内部関数のプロトタイプ宣言 */
#ifdef _EXPORTING
int zeller(int iYear, int iMonth, int iDay);
int getDayOfMonth(int iYear, int iMonth);
int getMonthFirstSunday(int iYear, int iMonth);
int getMonthLastSunday(int iYear, int iMonth);
bool isDataInSpecifiedRange(int iMonth, int iDay,
        int iFromMonth, int iFromDay, int iToMonth,
        int iToDay, bool isInclude);
int getDateTimeByOffset(datetime_t* t, int iOffsetMin);
int getTotalMinutesInDay(int iHour, int iMin);
#endif /* _EXPORTING */

/* Export 関数 */
// 現在のグリニッジ標準時
_EXPORT int getCurrentGMT(datetime_t* dt);
// 日本標準時
_EXPORT int getJST(datetime_t* dt, datetime_t* dt_in);
// 豪州東部標準時
_EXPORT int getAEST(datetime_t* dt, datetime_t* dt_in);
// 豪州西部標準時
_EXPORT int getAWST(datetime_t* dt, datetime_t* dt_in);
// 太平洋標準時
_EXPORT int getPST(datetime_t* dt, datetime_t* dt_in);
// 東部標準時
_EXPORT int getEST(datetime_t* dt, datetime_t* dt_in);
// 中央ヨーロッパ標準時(ドイツ)
_EXPORT int getCET(datetime_t* dt, datetime_t* dt_in);
// モスクワ標準時
_EXPORT int getMSK(datetime_t* dt, datetime_t* dt_in);

#endif /* __WORLDTIMEDLL_H__ */
