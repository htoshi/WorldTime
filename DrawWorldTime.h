/*
 * 世界の時間を取得
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
*/
#ifndef __DRAW_WORLDTIME_H__
#define __DRAW_WORLDTIME_H__

#define WORLDMAP_WIDTH 356
#define WORLDMAP_HEIGHT 184

#include <windows.h>
#include <commctrl.h>
#include "WorldTimeConfig.h"    // datetimeinfo_t readAreaInfo()
#include "WorldTimeCalc.h"

// #define __DEBUG__ 1

/* 世界時刻描画クラス */
class DrawWorldTime {

    private:
        HFONT hFont;            // フォントオブジェクト
        HWND hTool;             // ツールチップハンドル
        TOOLINFO toolInfo;      // ツールチップ情報構造体
        int iMaxDateTime;       // 日時情報構造体に格納された数

        // 日時情報構造体配列
        datetimeinfo_t mDateTime[MAX_AREA];

        // 世界時刻計算クラス
        WorldTimeCalc* mWorldTimeCalc;

        // 現在日時をセット
        void setCurrentTime(datetimeinfo_t*, datetime_t*);

        // ツールチップをアクティベート
        void activateToolTip(TCHAR*);

        // タイムゾーン名を分割して取得
        int splitTZName(TCHAR*, TCHAR*, TCHAR*);

#ifdef __DEBUG__
        void debug(TCHAR*);
#endif

    public:

        DrawWorldTime(HWND, LPARAM); // コンストラクタ
        ~DrawWorldTime();            // デストラクタ

        void drawBitmap(HDC, HBITMAP);
        void drawText(HDC);
        void setCurrentTime(void);

        // ツールチップ表示
        void showToolTip(POINT*);

        // ツールチップをディアクティベート
        void deActivateToolTip();
};

#endif /* __DRAW_WORLDTIME_H__ */
