/*
 * 世界の時間を取得
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
*/
#ifndef __WORLDTIMECALC_H__
#define __WORLDTIMECALC_H__

#include <windows.h>

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

// 夏時間情報構造体
typedef struct {
    short Month;    // 月
    short Day;      // 日
    char Type;      // 曜日タイプ (F: 第一週 E: 最終週)
    short WeekDay;  // 曜日 (0:日曜日～6:土曜日)
    short Time;     // 時刻(分)
} dstinfo_t;

/* 世界時刻計算クラス */
class WorldTimeCalc {
    private:
        int zeller(int, int, int);
        int getDayOfMonth(int, int);
        bool isDataInSpecifiedRange(int, int, int, int, int, int, bool);
        int getDateTimeByOffset(datetime_t*, int);
        int getTotalMinutesInDay(int, int);
        int setDSTDay(dstinfo_t*, int);

    public:

        WorldTimeCalc();      // コンストラクタ
        ~WorldTimeCalc();     // デストラクタ

        int getDayOfYear(int, int, int);

        // 月の最初のN曜日の日付を求める
        int getMonthFirstWeekDay(int, int, int);

        // 月の最後のN曜日の日付を求める
        int getMonthLastWeekDay(int, int, int);

        // 基準日より対象日が後にあるかどうかチェックする
        int isDateAfterSpecifiedDate(int, int, int, int);

        // 現在のグリニッジ標準時
        int getCurrentGMT(datetime_t*);

        // 世界の任意時差・指定されたサマータイムで時刻を取得
        int getWorldTime(datetime_t*, datetime_t*, double,
                bool, dstinfo_t*, dstinfo_t*);

};

#endif /* __WORLDTIMECALC_H__ */
