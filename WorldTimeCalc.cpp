/*
 * 世界の時間を取得
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#include "WorldTimeCalc.h"

// コンストラクタ
WorldTimeCalc::WorldTimeCalc() {}

// デストラクタ
WorldTimeCalc::~WorldTimeCalc() {}

/* zeller の公式により曜日を求める
   int iYear 年
   int iMonth 月
   int iDay 日
   戻り値: 0 日曜日～6 土曜日
*/
int WorldTimeCalc::zeller(int iYear, int iMonth, int iDay){

    // 1月と2月は前年の13月・14月として計算
    if(iMonth <= 2){
        iYear = iYear - 1;
        iMonth = iMonth + 12;
    }

    return ((5*iYear/4 - iYear/100 + iYear/400
             + (26*iMonth + 16)/10 + iDay) % 7);
}

/* 月の日数を求める
   int iYear 年
   int iMonth 月
*/
int WorldTimeCalc::getDayOfMonth(int iYear, int iMonth){

    static const int dayMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(iMonth == 2 && (iYear%4 == 0 && iYear%100 != 0
              || iYear%400 == 0)){
        return 29 ;
    }

    return dayMonth[iMonth-1] ;
}

/* 月の最初の日曜日を求める
   int iYear 年
   int iMonth 月
   戻り値: 月の最初の日曜日の日付
*/
int WorldTimeCalc::getMonthFirstSunday(int iYear, int iMonth){

    // 1日の曜日を求める
    int iDayWeek = zeller(iYear, iMonth, 1);

    // 1日が最初の日曜日
    if(iDayWeek == 0)
        return 1;

    return (8-iDayWeek);
}

/* 月の最後の日曜日を求める
   int iYear 年
   int iMonth 月
   戻り値: 月の最後の日曜日の日付
*/
int WorldTimeCalc::getMonthLastSunday(int iYear, int iMonth){

    int iDayWeek;

    if(iMonth == 12){
        // 12月の場合は翌年1月1日の曜日を求める
        iDayWeek = zeller(iYear+1, 1, 1);
    }else{
        // 翌月1日の曜日を求める
        iDayWeek = zeller(iYear, iMonth+1, 1);
    }

    // 翌月1日が日曜日なので月末の 6 日前が日曜日
    if(iDayWeek == 0)
        return getDayOfMonth(iYear, iMonth) - 6;

    return (getDayOfMonth(iYear, iMonth) + 1 - iDayWeek);
}

/* ある日付が指定された日付の範囲内にあるか
   int iMonth 指定月
   int iDay 指定日
   int iFromMonth 開始月
   int iFromDay 開始日
   int iToMonth 終了月
   int iToDay 終了日
   bool isInclude 指定日を含む場合 true
   戻り値: true 範囲内 false 範囲外
*/
bool WorldTimeCalc::isDataInSpecifiedRange(int iMonth, int iDay,
        int iFromMonth, int iFromDay, int iToMonth, int iToDay,
        bool isInclude){

    // 指定した月の範囲外
    if(iMonth < iFromMonth || iMonth > iToMonth)
        return false;

    // 指定日を含む場合
    if(isInclude){
        // 指定した月が開始月で日が範囲外の場合
        if(iMonth == iFromMonth && iDay < iFromDay)
            return false;

        // 指定した月が終了月で日が範囲外の場合
        if(iMonth == iToMonth && iDay > iToDay)
            return false;

    // 指定日を含まない場合
    }else{
        // 指定した月が開始月で日が範囲外の場合
        if(iMonth == iFromMonth && iDay <= iFromDay)
            return false;

        // 指定した月が終了月で日が範囲外の場合
        if(iMonth == iToMonth && iDay >= iToDay)
            return false;
    }

    return true;
}

/* datetime_t を与えて任意の時差を与えた時の日付と時刻を得る
    datetime_t* dt  日付時刻構造体
    int iOffsetMin  オフセット(分)
*/
int WorldTimeCalc::getDateTimeByOffset(datetime_t* dt, int iOffsetMin){

    int iHour, iMin;

    iHour = int(iOffsetMin/60);
    iMin = iOffsetMin - iHour*60;

    dt->Hour += iHour;
    dt->Minute += iMin;

    // 分の繰上げ・繰り下げ処理
    if(dt->Minute >= 60){      // 分が60以上の場合
        dt->Hour += 1;
        dt->Minute -= 60;
    }else if(dt->Minute < 0){  // 分が0未満の場合
        dt->Hour -= 1;
        dt->Minute += 60;
    }

    // 時の繰上げ・繰り下げ処理
    if(dt->Hour >= 24){     // 時が24以上の場合
        dt->Day += 1;
        dt->Hour -= 24;
    }else if(dt->Hour < 0){ // 時が0未満の場合
        dt->Day -= 1;
        dt->Hour += 24;
    }

    // 日の繰上げ・繰り下げ処理
    // 日が今月末日以上の場合
    if(dt->Day > getDayOfMonth(dt->Year, dt->Month)){

        dt->Month += 1;
        dt->Day = 1;

        // 12 月の場合は翌年の1/1をセットして終了
        if(dt->Month > 12){
            dt->Month = 1;
            dt->Year += 1;
            return OK;
        }

    }else if(dt->Day <= 0){ // 日が0以下の場合

        dt->Month -= 1;

        // 1月の場合昨年の12/31をセットして終了
        if(dt->Month <= 0){
            dt->Month = 12;
            dt->Day = 31;
            dt->Year -= 1;
            return OK;
        }

        // 先月末日をセット
        dt->Day = getDayOfMonth(dt->Year, dt->Month);
    }

    return OK;
}

/* 1日の中で累積分を求める
    int iHour 時
    int iMin 分
*/
int WorldTimeCalc::getTotalMinutesInDay(int iHour, int iMin){

    return iHour*60 + iMin;
}

/* 現在のGMT時刻を取得 (グリニッジ標準時)
    datetime_t* dt  日付時刻構造体
*/
int WorldTimeCalc::getCurrentGMT(datetime_t* dt){

    SYSTEMTIME stTime;

    // システム日時をそのまま使用
    GetSystemTime(&stTime);

    dt->Year = stTime.wYear;
    dt->Month = stTime.wMonth;
    dt->Day = stTime.wDay;
    dt->Hour = stTime.wHour;
    dt->Minute = stTime.wMinute;
    dt->Second = stTime.wSecond;
    dt->isDST = false;

    return OK;
}

/* JST時刻を取得 (日本標準時)
    datetime_t* dt  日付時刻構造体・変換結果
    datetime_t* dt_in  日付時刻構造体・入力値(GMT)
*/
int WorldTimeCalc::getJST(datetime_t* dt, datetime_t* dt_in){

    // 入力値をセット
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // JST = GMT + 9 (+540min)
    getDateTimeByOffset(dt, 540);

    return OK;
}

/* AEST時刻を取得 (豪州東部標準時)
    datetime_t* dt  日付時刻構造体・変換結果
    datetime_t* dt_in  日付時刻構造体・入力値(GMT)
*/
int WorldTimeCalc::getAEST(datetime_t* dt, datetime_t* dt_in){

    // 入力値をセット
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // AEST = GMT + 10 (+600min)
    getDateTimeByOffset(dt, 600);

    // 3月の最終日曜日を取得
    int iMarchLastSundayDay = getMonthLastSunday(dt->Year, 3);

    // 10月の最終日曜日を取得
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 3月の最終日曜日の翌日～10月の最終日曜日の前日の場合
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        3, iMarchLastSundayDay, 10, iOctoberLastSundayDay, false)){

        return OK;    // サマータイムではない
    }

    // 3月の最終日曜日の場合 (03:00 にサマータイム終了)
    if(dt->Month == 3 && dt->Day == iMarchLastSundayDay){

        // 現地標準時で 02:00 より後はサマータイムではない
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120)
            return OK;
    }

    // 10月の最終日曜日の場合 (02:00 にサマータイム開始)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // 現地標準時で 02:00 より前はサマータイムではない
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 120)
            return OK;
    }

    // サマータイム補正 (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}

/* AWST時刻を取得 (豪州西部標準時)
    datetime_t* dt  日付時刻構造体・変換結果
    datetime_t* dt_in  日付時刻構造体・入力値(GMT)
*/
int WorldTimeCalc::getAWST(datetime_t* dt, datetime_t* dt_in){

    // 入力値をセット
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // AWST = GMT + 8 (+480min)
    getDateTimeByOffset(dt, 480);

    return OK;
}

/* PST時刻を取得 (太平洋標準時)
    datetime_t* dt  日付時刻構造体・変換結果
    datetime_t* dt_in  日付時刻構造体・入力値(GMT)
*/
int WorldTimeCalc::getPST(datetime_t* dt, datetime_t* dt_in){

    // 入力値をセット
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // PST = GMT - 8 (-480min)
    getDateTimeByOffset(dt, -480);

    // 4月の最初の日曜日を取得
    int iAprilFirstSundayDay = getMonthFirstSunday(dt->Year, 4);

    // 10月の最終日曜日を取得
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 4月の最初の日曜日の翌日～10月の最終日曜日の前日
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        4, iAprilFirstSundayDay, 10, iOctoberLastSundayDay, false)){

            // サマータイム補正
            goto lblSummerTime;
    }

    // 4月の最終日曜日の場合 (02:00 にサマータイム開始)
    if(dt->Month == 4 && dt->Day == iAprilFirstSundayDay){

        // 現地標準時で 02:00 より後はサマータイム
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120){

            // サマータイム補正
            goto lblSummerTime;
        }
    }

    // 10月の最終日曜日の場合 (01:00 にサマータイム終了)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // 現地標準時で 01:00 より前はサマータイム
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 60){

            // サマータイム補正
            goto lblSummerTime;
        }
    }

    // サマータイムではない
    return OK;

lblSummerTime:
    // サマータイム補正 (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}

/* EST時刻を取得 (東部標準時)
    datetime_t* dt  日付時刻構造体・変換結果
    datetime_t* dt_in  日付時刻構造体・入力値(GMT)
*/
int WorldTimeCalc::getEST(datetime_t* dt, datetime_t* dt_in){

    // 入力値をセット
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    dt->isDST = false;

    // PST = GMT - 5 (-300min)
    getDateTimeByOffset(dt, -300);

    // 4月の最初の日曜日を取得
    int iAprilFirstSundayDay = getMonthFirstSunday(dt->Year, 4);

    // 10月の最終日曜日を取得
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 4月の最初の日曜日の翌日～10月の最終日曜日の前日
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        4, iAprilFirstSundayDay, 10, iOctoberLastSundayDay, false)){

            // サマータイム補正
            goto lblSummerTime;
    }

    // 4月の最初の日曜日の場合 (02:00 にサマータイム開始)
    if(dt->Month == 4 && dt->Day == iAprilFirstSundayDay){

        // 現地標準時で 02:00 より後はサマータイム
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120){

            // サマータイム補正
            goto lblSummerTime;
        }
    }

    // 10月の最終日曜日の場合 (01:00 にサマータイム終了)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // 現地標準時で 01:00 より前はサマータイム
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 60){

            // サマータイム補正
            goto lblSummerTime;
        }
    }

    // サマータイムではない
    return OK;

lblSummerTime:
    // サマータイム補正 (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}

/* CET時刻を取得 (中央ヨーロッパ標準時・ドイツ)
    datetime_t* dt  日付時刻構造体・変換結果
    datetime_t* dt_in  日付時刻構造体・入力値(GMT)
*/
int WorldTimeCalc::getCET(datetime_t* dt, datetime_t* dt_in){

    // 入力値をセット
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // CET = GMT + 1 (+60min)
    getDateTimeByOffset(dt, 60);

    // 3月の最終日曜日を取得
    int iMarchLastSundayDay = getMonthLastSunday(dt->Year, 3);

    // 10月の最終日曜日を取得
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 3月の最終日曜日の翌日～10月の最終日曜日の前日
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        3, iMarchLastSundayDay, 10, iOctoberLastSundayDay, false)){

            // サマータイム補正
            goto lblSummerTime;
    }

    // 3月の最終日曜日の場合 (02:00 にサマータイム開始)
    if(dt->Month == 3 && dt->Day == iMarchLastSundayDay){

        // 現地標準時で 02:00 より後はサマータイム
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120){

            // サマータイム補正
            goto lblSummerTime;
        }
    }

    // 10月の最終日曜日の場合 (01:00 にサマータイム終了)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // 現地標準時で 01:00 より前はサマータイム
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 60){

            // サマータイム補正
            goto lblSummerTime;
        }
    }

    // サマータイムではない
    return OK;

lblSummerTime:
    // サマータイム補正 (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}

/* MSK時刻を取得 (モスクワ標準時)
    datetime_t* dt  日付時刻構造体・変換結果
    datetime_t* dt_in  日付時刻構造体・入力値(GMT)
*/
int WorldTimeCalc::getMSK(datetime_t* dt, datetime_t* dt_in){

    // 入力値をセット
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // MSK = GMT + 3 (+180min)
    getDateTimeByOffset(dt, 180);

    // 3月の最終日曜日を取得
    int iMarchLastSundayDay = getMonthLastSunday(dt->Year, 3);

    // 10月の最終日曜日を取得
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 3月の最終日曜日の翌日～10月の最終日曜日の前日
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        3, iMarchLastSundayDay, 10, iOctoberLastSundayDay, false)){

            // サマータイム補正
            goto lblSummerTime;
    }

    // 3月の最終日曜日の場合 (02:00 にサマータイム開始)
    if(dt->Month == 3 && dt->Day == iMarchLastSundayDay){

        // 現地標準時で 02:00 より後はサマータイム
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120){

            // サマータイム補正
            goto lblSummerTime;
        }
    }

    // 10月の最終日曜日の場合 (02:00 にサマータイム終了)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // 現地標準時で 02:00 より前はサマータイム
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 120){

            // サマータイム補正
            goto lblSummerTime;
        }
    }

    // サマータイムではない
    return OK;

lblSummerTime:
    // サマータイム補正 (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}
