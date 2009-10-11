/*
 * 世界の時間を取得
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
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
    戻り値: 月の日数
*/
int WorldTimeCalc::getDayOfMonth(int iYear, int iMonth){

    static const int dayMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(iMonth == 2 && (iYear%4 == 0 && iYear%100 != 0
              || iYear%400 == 0)){
        return 29 ;
    }

    return dayMonth[iMonth-1] ;
}

/* 基準日より対象日が後にあるかどうかチェックする
   int iBaseMonth 基準月
   int iBaseDay 基準日
   int iMonth 対象月
   int iDay 対象日
   戻り値: 1 後にある場合 0 同一 -1 前にある場合
*/
int WorldTimeCalc::isDateAfterSpecifiedDate(int iBaseMonth, int iBaseDay, int iMonth, int iDay){

    if(iBaseMonth < iMonth){
        // 基準日よりも後
        return 1;
    }else if(iBaseMonth > iMonth){
        // 基準日よりも前
        return -1;
    }

    if(iBaseDay < iDay){
        // 基準日よりも後
        return 1;
    }else if(iBaseDay > iDay){
        // 基準日よりも前
        return -1;
    }

    return 0;
}

/* 月の最初のN曜日の日付を求める
   int iYear 年
   int iMonth 月
   int iWeekDay N曜日 (0 日曜日～6 土曜日)
   戻り値: 月の最初のN曜日の日付
*/
int WorldTimeCalc::getMonthFirstWeekDay(int iYear, int iMonth, int iWeekDay){

    // 1日の曜日を求める
    int iWeek = zeller(iYear, iMonth, 1);

    // 1日が最初のN曜日
    if(iWeek == iWeekDay)
        return 1;

    // (N+1)曜日から1日の曜日を引いたものが求める日付
    int iRet = iWeekDay + 1 - iWeek;

    // 0以下の場合は7を足す
    if(iRet <= 0)
        iRet = iRet + 7;

    return iRet;
}

/* 月の最後のN曜日の日付を求める
   int iYear 年
   int iMonth 月
   int iWeekDay N曜日 (0 日曜日～6 土曜日)
   戻り値: 月の最後のN曜日の日付
*/
int WorldTimeCalc::getMonthLastWeekDay(int iYear, int iMonth, int iWeekDay){

    // 月の最後の日を求める
    int iMonthLastDay = getDayOfMonth(iYear, iMonth);

    // 月の最後の日の曜日を求める
    int iWeek = zeller(iYear, iMonth, iMonthLastDay);

    // N曜日から月の最後の日の曜日を引いた値を求める
    int iRet = iWeekDay - iWeek ;

    // 0より大きい場合は7を引く
    if(iRet > 0)
        iRet = iRet - 7;

    return (iMonthLastDay + iRet);
}

/* 月の第M N曜日の日付を求める
   int iYear 年
   int iMonth 月
   int iNum 第 M 曜日の M を指定
   int iWeekDay N曜日 (0 日曜日～6 土曜日)
   戻り値: 月の最初のN曜日の日付 (存在しない場合は -1)
*/
int WorldTimeCalc::getMonthMWeekDay(int iYear, int iMonth, int iNum, int iWeekDay){

    // 月の最初のN曜日の日付を求める
    int iDay = getMonthFirstWeekDay(iYear, iMonth, iWeekDay);

    // M番目のN曜日の日付を求める
    iDay = iDay + (iNum - 1)*7;

    // 存在しない
    if(iDay > getDayOfMonth(iYear, iMonth))
        return -1;

    return iDay;
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
    戻り値: OK
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
    戻り値: 1日の中での累積分
*/
int WorldTimeCalc::getTotalMinutesInDay(int iHour, int iMin){

    return iHour*60 + iMin;
}

/* 現在のGMT時刻を取得 (グリニッジ標準時)
    datetime_t* dt  日付時刻構造体
    戻り値: OK
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

/* DST 日付を設定
    dstinfo_t* dstInfo 夏時間情報構造体
    int iYear 年
    戻り値: OK
*/
int WorldTimeCalc::setDSTDay(dstinfo_t* dstInfo, int iYear){

    // 月の第一週の曜日の日付を設定
    if(dstInfo->Type == 'F'){

        dstInfo->Day = getMonthFirstWeekDay(iYear,
            dstInfo->Month, dstInfo->WeekDay);

    // 月の最終週の曜日の日付を設定
    }else if(dstInfo->Type == 'E'){

        dstInfo->Day = getMonthLastWeekDay(iYear,
            dstInfo->Month, dstInfo->WeekDay);

	// 月の第M N曜日の日付を設定
	}else{
		int iNum = _tstoi(&(dstInfo->Type));
		if(iNum >= 1 && iNum <= 5){
			dstInfo->Day = getMonthMWeekDay(iYear,
            dstInfo->Month, iNum, dstInfo->WeekDay);
		}
	}

    return OK;
}

/* 世界の任意時差・指定されたサマータイムで時刻を取得
    datetime_t* dt  日付時刻構造体・変換結果
    datetime_t* dt_in  日付時刻構造体・入力値(GMT)
    double offset オフセット (時間)
    bool hasDST 夏時間保持フラグ (true: 夏時間あり false: 夏時間なし)
    dstinfo_t* dstStart 夏時間開始情報
    dstinfo_t* dstEnd 夏時間終了情報
    bool* isDST 夏時間フラグ (true: 夏時間 false: 標準時間)
    戻り値: OK
*/
int WorldTimeCalc::getWorldTime(datetime_t* dt, datetime_t* dt_in, double offset,
        bool hasDST, dstinfo_t* dstStart, dstinfo_t* dstEnd){

    // 入力値をセット
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // オフセット補正
    getDateTimeByOffset(dt, (int)(offset*60));

    // 夏時間フラグ
    dt->isDST = false;

    // 夏時間情報なし
    if(!hasDST)
        return OK;

    // DST 日付を設定
    setDSTDay(dstStart, dt->Year);
    setDSTDay(dstEnd, dt->Year);

    // 1年中でサマータイム開始日が終了日よりも前にある(北半球)
    if(isDateAfterSpecifiedDate(dstStart->Month, dstStart->Day, dstEnd->Month, dstEnd->Day) > 0){

        // サマータイム開始日の翌日～サマータイム終了日の前日
        if(isDataInSpecifiedRange(dt->Month, dt->Day,
            dstStart->Month, dstStart->Day, dstEnd->Month, dstEnd->Day, false)){

                // サマータイム補正
                goto lblSummerTime;
        }

        // サマータイム開始日の場合 (dstStart->Time にサマータイム開始)
        if(dt->Month == dstStart->Month && dt->Day == dstStart->Day){

            // 現地標準時で dstStart->Time より後はサマータイム
            if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= dstStart->Time){

                // サマータイム補正
                goto lblSummerTime;
            }
        }

        // サマータイム終了日の場合 (dstEnd->Time にサマータイム終了)
        if(dt->Month == dstEnd->Month && dt->Day == dstEnd->Day){

            // 現地標準時で dstEnd->Time より前はサマータイム
            if(getTotalMinutesInDay(dt->Hour, dt->Minute) < dstEnd->Time){

                // サマータイム補正
                goto lblSummerTime;
            }
        }

        // サマータイムではない
        return OK;

    // 1年中でサマータイム開始日が終了日よりも後にある(南半球)
    }else{

        // サマータイム終了日の翌日～サマータイム開始日の前日の場合
        if(isDataInSpecifiedRange(dt->Month, dt->Day,
            dstEnd->Month, dstEnd->Day, dstStart->Month, dstStart->Day, false)){

            return OK;    // サマータイムではない
        }

        // サマータイム終了日の場合 (dstEnd->Time にサマータイム終了)
        if(dt->Month == dstEnd->Month && dt->Day == dstEnd->Day){

            // 現地標準時で dstEnd->Time より後はサマータイムではない
            if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= dstEnd->Time)
                return OK;
        }

        // サマータイム開始日の場合 (dstStart->Time にサマータイム開始)
        if(dt->Month == dstStart->Month && dt->Day == dstStart->Day){

            // 現地標準時で dstStart->Time より前はサマータイムではない
            if(getTotalMinutesInDay(dt->Hour, dt->Minute) < dstStart->Time)
                return OK;
        }

        // それ以外はサマータイム
    }

lblSummerTime:
    // サマータイム補正 (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}
