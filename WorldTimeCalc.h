/*
 * 世界の時間を取得
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#ifndef __WORLDTIMECALC_H__
#define __WORLDTIMECALC_H__

#include <windows.h>

// DLL 用宣言
#define DllExport __declspec(dllexport)

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

/* 世界時刻計算クラス */
class WorldTimeCalc {
    private:
        int zeller(int iYear, int iMonth, int iDay);
        int getDayOfMonth(int iYear, int iMonth);
        int getMonthFirstSunday(int iYear, int iMonth);
        int getMonthLastSunday(int iYear, int iMonth);
        bool isDataInSpecifiedRange(int iMonth, int iDay,
                int iFromMonth, int iFromDay, int iToMonth,
                int iToDay, bool isInclude);
        int getDateTimeByOffset(datetime_t* t, int iOffsetMin);
        int getTotalMinutesInDay(int iHour, int iMin);

    public:

        WorldTimeCalc();      // コンストラクタ
        ~WorldTimeCalc();     // デストラクタ

        // 現在のグリニッジ標準時
        int getCurrentGMT(datetime_t* dt);
        // 日本標準時
        int getJST(datetime_t* dt, datetime_t* dt_in);
        // 豪州東部標準時
        int getAEST(datetime_t* dt, datetime_t* dt_in);
        // 豪州西部標準時
        int getAWST(datetime_t* dt, datetime_t* dt_in);
        // 太平洋標準時
        int getPST(datetime_t* dt, datetime_t* dt_in);
        // 東部標準時
        int getEST(datetime_t* dt, datetime_t* dt_in);
        // 中央ヨーロッパ標準時(ドイツ)
        int getCET(datetime_t* dt, datetime_t* dt_in);
        // モスクワ標準時
        int getMSK(datetime_t* dt, datetime_t* dt_in);

};

#endif /* __WORLDTIMECALC_H__ */
