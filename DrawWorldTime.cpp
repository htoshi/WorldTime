/*
 * 世界の時刻を描画
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#include "DrawWorldTime.h"

/* コンストラクタ */
DrawWorldTime::DrawWorldTime(HWND hwnd, LPARAM lparam){

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

    // フォントの作成
    this->hFont = CreateFont(
            12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            VARIABLE_PITCH | FF_ROMAN , "MS Gothic");

    // 日時情報格納構造体・初期化
    mDateTime[0].x = 30; mDateTime[0].y = 20;
    lstrcpy(mDateTime[0].szName, TEXT("GMT"));
    lstrcpy(mDateTime[0].szInfo, "グリニッジ標準時");

    mDateTime[1].x = 170; mDateTime[1].y = 60;
    lstrcpy(mDateTime[1].szName, TEXT("JST"));
    lstrcpy(mDateTime[1].szInfo, TEXT("日本標準時"));

    mDateTime[2].x = 200; mDateTime[2].y = 130;
    lstrcpy(mDateTime[2].szName, TEXT("AEST"));
    lstrcpy(mDateTime[2].szInfo, TEXT("豪州東部標準時 (Sydney・Canberra)"));

    mDateTime[3].x = 130; mDateTime[3].y = 130;
    lstrcpy(mDateTime[3].szName, TEXT("AWST"));
    lstrcpy(mDateTime[3].szInfo, TEXT("豪州西部標準時 (Perth)"));

    mDateTime[4].x = 260; mDateTime[4].y = 60;
    lstrcpy(mDateTime[4].szName, TEXT("PST"));
    lstrcpy(mDateTime[4].szInfo, TEXT("太平洋標準時 (Los Angeles・San Fransisco)"));

    mDateTime[5].x = 310; mDateTime[5].y = 60;
    lstrcpy(mDateTime[5].szName, TEXT("EST"));
    lstrcpy(mDateTime[5].szInfo, TEXT("東海岸標準時 (New York・Washington)"));

    mDateTime[6].x = 50; mDateTime[6].y = 60;
    lstrcpy(mDateTime[6].szName, TEXT("CET"));
    lstrcpy(mDateTime[6].szInfo, TEXT("中央ヨーロッパ時間 (Germany)"));

    mDateTime[7].x = 90; mDateTime[7].y = 40;
    lstrcpy(mDateTime[7].szName, TEXT("MSK"));
    lstrcpy(mDateTime[7].szInfo, TEXT("モスクワ時間 (Moscow)"));
}

/* デストラクタ */
DrawWorldTime::~DrawWorldTime() {

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
    for(int i=0; i<sizeof(mDateTime)/sizeof(mDateTime[0]); i++){

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

    // GMT をセット
    getCurrentGMT(&dtGMT);
    setCurrentTime(&mDateTime[0], &dtGMT);

    // JST をセット
    getJST(&dt, &dtGMT);
    setCurrentTime(&mDateTime[1], &dt);

    // AEST をセット
    getAEST(&dt, &dtGMT);
    setCurrentTime(&mDateTime[2], &dt);

    // AWST をセット
    getAWST(&dt, &dtGMT);
    setCurrentTime(&mDateTime[3], &dt);

    // PST をセット
    getPST(&dt, &dtGMT);
    setCurrentTime(&mDateTime[4], &dt);

    // EST をセット
    getEST(&dt, &dtGMT);
    setCurrentTime(&mDateTime[5], &dt);

    // CET をセット
    getCET(&dt, &dtGMT);
    setCurrentTime(&mDateTime[6], &dt);

    // MSK をセット
    getMSK(&dt, &dtGMT);
    setCurrentTime(&mDateTime[7], &dt);
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
