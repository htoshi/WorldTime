/*
 * 世界の時刻を描画
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
*/
#include "resource.h"
#include "DrawWorldTime.h"

/* コンストラクタ */
DrawWorldTime::DrawWorldTime(HWND hwnd, LPARAM lparam){

    WorldTimeConfig* config;    // 設定ファイルクラス

    // コモンコントロール初期化
    InitCommonControls();

    // ツールチップ作成
    hTool = CreateWindowEx(0, TOOLTIPS_CLASS,
                NULL, TTS_ALWAYSTIP,
                CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, CW_USEDEFAULT,
                hwnd, NULL, ((LPCREATESTRUCT)(lparam))->hInstance,
                NULL);

    // クライアント領域を設定
    GetClientRect(hwnd, (LPRECT)&toolInfo.rect);

    toolInfo.cbSize = sizeof(TOOLINFO);
    toolInfo.uFlags = TTF_SUBCLASS;
    toolInfo.hwnd = hwnd;
    toolInfo.lpszText = "";

    SendMessage(hTool, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

    // 世界時刻計算クラス
    mWorldTimeCalc = new WorldTimeCalc();

    // フォントの作成
    this->hFont = CreateFont(
            12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            VARIABLE_PITCH | FF_ROMAN , "MS Gothic");

    // 設定ファイルから設定を読み込み
    config = new WorldTimeConfig();

    int iRet = config->readAreaInfo(mDateTime);

    // INI ファイル異常
    if(iRet == ERR){
        MessageBox(NULL, TEXT("INIファイル異常"),
                         IDC_STATIC_APPNAME, MB_ICONSTOP | MB_OK);

        exit(ERR);
    }

    iMaxDateTime = iRet;    // 日時情報構造体に格納された数を設定

    delete(config);
}

/* デストラクタ */
DrawWorldTime::~DrawWorldTime() {

    // 世界時刻計算クラス削除
    if(mWorldTimeCalc != NULL)
        delete(mWorldTimeCalc);

    // フォント削除
    DeleteObject(hFont);
}

/* ビットマップを描画
   HDC hdc デバイスコンテキストハンドル
   HBITMAP hBitmap ビットマップハンドル
   戻り値: なし
*/
void DrawWorldTime::drawBitmap(HDC hdc, HBITMAP hBitmap){
    HDC hBuffer;

    // DC 取得
    hBuffer = CreateCompatibleDC(hdc);
    SelectObject(hBuffer, hBitmap);

    // BitBltで描画
    BitBlt(hdc, 0, 0, WORLDMAP_WIDTH, WORLDMAP_HEIGHT, hBuffer, 0, 0, SRCCOPY);
    DeleteDC(hBuffer);
}

/* テキストを描画
   HDC hdc デバイスコンテキストハンドル
   戻り値: なし
*/
void DrawWorldTime::drawText(HDC hdc){

    int iTZOffset;

    SetBkMode(hdc , TRANSPARENT);           // 背景色を透明にする
    SelectObject(hdc , this->hFont);        // フォントの選択

    // それぞれのタイムゾーンの日時を表示
    for(int i=0; i<iMaxDateTime; i++){

        // タイムゾーン名の微妙な位置調整
        iTZOffset = (lstrlen(mDateTime[i].szName) == 3)?8:4;

        // 夏時間
        if(mDateTime[i].isDST){
            SetTextColor(hdc, RGB(255, 255, 0));    // テキスト色は黄
        }else{
            SetTextColor(hdc, RGB(255, 255, 255));  // テキスト色は白
        }

        // タイムゾーン名
        TextOut(hdc, mDateTime[i].x+iTZOffset, mDateTime[i].y,
                mDateTime[i].szName, lstrlen(mDateTime[i].szName));

        // 日付と時刻
        TextOut(hdc, mDateTime[i].x, mDateTime[i].y+10,
                mDateTime[i].szDate, lstrlen(mDateTime[i].szDate));
        TextOut(hdc, mDateTime[i].x, mDateTime[i].y+20,
                mDateTime[i].szTime, lstrlen(mDateTime[i].szTime));
    }
}

/* 現在日時をセットする
   datetimeinfo_t* dtinfo 日時情報格納構造体
   datetime_t* dt 日時情報格納構造体
   戻り値: なし
*/
void DrawWorldTime::setCurrentTime(datetimeinfo_t* dtinfo, datetime_t* dt){

    wsprintf(dtinfo->szDate, "%02d/%02d", dt->Month, dt->Day);
    wsprintf(dtinfo->szTime, "%02d:%02d", dt->Hour, dt->Minute);
    dtinfo->isDST = dt->isDST;

}

/* 現在日時をセットする
   戻り値: なし
*/
void DrawWorldTime::setCurrentTime(){

    datetime_t dtGMT, dt;
    dstinfo_t dstStart, dstEnd;

    // GMT を取得
    mWorldTimeCalc->getCurrentGMT(&dtGMT);

    // 各エリアの時刻を取得してセット
    for(int i=0; i<iMaxDateTime; i++){

        // 夏時間情報をセット
        if(mDateTime[i].hasDST){

            // 夏時間開始情報
            dstStart.Month = mDateTime[i].iDSTStartMonth;
            dstStart.Type = mDateTime[i].szDSTStartType;
            dstStart.WeekDay = mDateTime[i].iDSTStartWeekDay;
            dstStart.Time = mDateTime[i].iDSTStartTime;

            // 夏時間終了情報
            dstEnd.Month = mDateTime[i].iDSTEndMonth;
            dstEnd.Type = mDateTime[i].szDSTEndType;
            dstEnd.WeekDay = mDateTime[i].iDSTEndWeekDay;
            dstEnd.Time = mDateTime[i].iDSTEndTime;
        }

        // 世界時刻取得
        mWorldTimeCalc->getWorldTime(&dt, &dtGMT, mDateTime[i].offset,
            mDateTime[i].hasDST, &dstStart, &dstEnd);

        setCurrentTime(&mDateTime[i], &dt);
    }
}

/* ツールチップをアクティベート
   TCHAR* szBuf テキスト
*/
void DrawWorldTime::activateToolTip(TCHAR* szBuf){

    // テキストをセット
    this->toolInfo.lpszText = szBuf;

    // アクティベート
    SendMessage(hTool, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
    SendMessage(hTool, TTM_ACTIVATE, TRUE, 0);
}

/* ツールチップをディアクティベート */
void DrawWorldTime::deActivateToolTip(){

    SendMessage(this->hTool, TTM_ACTIVATE, FALSE, 0);
}

/* ツールチップ表示
   POINT* po マウス位置
*/
void DrawWorldTime::showToolTip(POINT* po){

    // それぞれのタイムゾーンのツールチップを表示
    for(int i=0; i<sizeof(mDateTime)/sizeof(mDateTime[0]); i++){

        if((po->x >= mDateTime[i].x) && (po->x <= mDateTime[i].x + 34) &&
           (po->y >= mDateTime[i].y) && (po->y <= mDateTime[i].y + 32)){

            // ツールチップを活性化
            activateToolTip(mDateTime[i].szInfo);

            return;
        }
    }

    // ツールチップを非活性化
    deActivateToolTip();
}

/* デバッグログ
   char* szBuf デバッグメッセージ
   戻り値: なし
*/
#ifdef __DEBUG__
void DrawWorldTime::debug(TCHAR* szBuf){
    HANDLE hFile;
    BOOL bRet;
    DWORD dwWritten;
    SYSTEMTIME stTime;

    TCHAR szLogMsg[1024];

    GetLocalTime(&stTime);

    wsprintf(szLogMsg, "%04d/%02d/%02d %02d:%02d:%02d %s\r\n",
                stTime.wYear, stTime.wMonth, stTime.wDay,
                stTime.wHour, stTime.wMinute, stTime.wSecond, szBuf);

    // ファイル作成
    hFile = CreateFile("debug.log", GENERIC_WRITE,
                        FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE){
        MessageBox(0, TEXT("ファイルオープンエラー"),
                      TEXT("エラー"), MB_ICONSTOP);
        return;
    }

    // ファイル最終端にポインタを移動
    SetFilePointer(hFile , 0 , NULL , FILE_END);

    // 書き込み
    bRet = WriteFile(hFile, szLogMsg, (DWORD)strlen(szLogMsg), &dwWritten, NULL);

    CloseHandle(hFile);
}
#endif /* __DEBUG__ */
