/*
 * 世界の時間を取得
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#ifndef __DRAW_WORLDTIME_H__
#define __DRAW_WORLDTIME_H__

#define WORLDMAP_WIDTH 356
#define WORLDMAP_HEIGHT 184

#include <windows.h>
#include <commctrl.h>
#include "WorldTimeDLL.h"

// #define __DEBUG__ 1

/* 日時情報格納構造体 */
typedef struct {
    TCHAR szDate[6];   // 日付 (01/01)
    TCHAR szTime[6];   // 時刻 (12:00)
    TCHAR szName[5];   // ゾーン名
    bool isDST;        // 夏時間フラグ (true: 夏時間 false: 標準時間)
    int x;             // 表示位置 X 座標
    int y;             // 表示位置 Y 座標
    TCHAR szInfo[128]; // ツールチップに表示する情報
} datetimeinfo_t;

/* 世界時刻描画クラス */
class DrawWorldTime {

    private:
        HFONT hFont;            // フォントオブジェクト
        HWND hTool;             // ツールチップハンドル
        TOOLINFO toolInfo;      // ツールチップ情報構造体

        // 日時情報構造体配列
        datetimeinfo_t mDateTime[8];

        // 現在日時をセット
        void setCurrentTime(datetimeinfo_t*, datetime_t*);

        // ツールチップをアクティベート
        void activateToolTip(TCHAR*);

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
