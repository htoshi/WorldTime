/*
 * 初期設定ファイルクラス
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#include "WorldTimeConfig.h"

// コンストラクタ
WorldTimeConfig::WorldTimeConfig(){}

// デストラクタ
WorldTimeConfig::~WorldTimeConfig(){}

/* 設定ファイル名をフルパスで返す
    TCHAR* 設定ファイル名
    DWORD 設定ファイル名バッファのサイズ
*/
int WorldTimeConfig::getIniFileName(TCHAR* szFullPathName, DWORD nSize){

    TCHAR szDrive[_MAX_DRIVE];
    TCHAR szDir[_MAX_DIR];

    // 実行ファイルのフルパスとファイル名を取得
    if(!GetModuleFileName(NULL, szFullPathName, nSize))
        return ERR;

    // フルパスを分解
    _tsplitpath(szFullPathName, szDrive, szDir, NULL, NULL);

    // szFullPathName に INI ファイル名をフルパスで格納
    _tcscpy(szFullPathName, szDrive);
    _tcscat(szFullPathName, szDir);
    _tcscat(szFullPathName, INIFILENAME);

    return OK;
}

/* 設定ファイルのWindow位置を読み込んで返す
    LPRECT rect ウィンドウの座標値
    戻り値 OK 成功
*/
int WorldTimeConfig::readWindowRect(RECT* rect){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[100];

    // INI ファイル名をフルパスで取得
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // Top 位置の取得
    if(GetPrivateProfileString(
         TEXT("General"), TEXT("Top"), DEFAULT_WINDOW_TOP,
         szBuf, sizeof(szBuf), szIniFileName) == 0)
        return ERR;

    rect->top = (_tstoi(szBuf) == 0)?(int)DEFAULT_WINDOW_TOP:_tstoi(szBuf);

    // Left 位置の取得
    if(GetPrivateProfileString(
         TEXT("General"), TEXT("Left"), DEFAULT_WINDOW_LEFT,
         szBuf, sizeof(szBuf), szIniFileName) == 0)
        return ERR;

    rect->left = (_tstoi(szBuf) == 0)?(int)DEFAULT_WINDOW_LEFT:_tstoi(szBuf);

    return OK;
}

/* 現在のWindow位置を設定ファイルに書き出す
    LPRECT rect ウィンドウの座標値
    戻り値 OK 成功
*/
int WorldTimeConfig::writeWindowRect(RECT* rect){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[100];

    // INI ファイル名をフルパスで取得
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // Top 位置の保存
    wsprintf(szBuf, "%d", rect->top);

    if(!WritePrivateProfileString(
         TEXT("General"), TEXT("Top"), szBuf, szIniFileName))
        return ERR;

    // Left 位置の保存
    wsprintf(szBuf, "%d", rect->left);

    if(!WritePrivateProfileString(
         TEXT("General"), TEXT("Left"), szBuf, szIniFileName))
        return ERR;

    return OK;
}

/* 常に最前面表示情報読み出し
    戻り値 OK 成功
*/
int WorldTimeConfig::readTopMostFlag(bool* isTop){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[5];

    // INI ファイル名をフルパスで取得
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // 常に最前面表示情報読み出し (1: 常に最前面)
    if(GetPrivateProfileString(
         TEXT("General"), TEXT("AlwaysTop"), TEXT("0"),
         szBuf, sizeof(szBuf), szIniFileName) == 0)
        return ERR;

    *isTop = (_tstoi(szBuf) == 1)?true:false;

    return OK;
}

/* 常に最前面表示情報書き出し
    戻り値 OK 成功
*/
int WorldTimeConfig::writeTopMostFlag(bool isTop){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[10];

    // INI ファイル名をフルパスで取得
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // 最前面状態の保存
    wsprintf(szBuf, "%d", isTop?1:0);

    if(!WritePrivateProfileString(
         TEXT("General"), TEXT("AlwaysTop"), szBuf, szIniFileName))
        return ERR;

    return OK;
}

/* ツールチップ表示情報読み出し
    bool* isShow
    戻り値 OK 成功
*/
int WorldTimeConfig::readToolTipFlag(bool* isShow){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[5];

    // INI ファイル名をフルパスで取得
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // ツールチップ表示情報読み出し (1: 表示)
    if(GetPrivateProfileString(
         TEXT("General"), TEXT("ToolTip"), TEXT("0"),
         szBuf, sizeof(szBuf), szIniFileName) == 0)
        return ERR;

    *isShow = (_tstoi(szBuf) == 1)?true:false;

    return OK;
}

/* ツールチップ表示情報書き出し
    bool isShow
    戻り値 OK 成功
*/
int WorldTimeConfig::writeToolTipFlag(bool isShow){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[10];

    // INI ファイル名をフルパスで取得
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // ツールチップ表示情報保存
    wsprintf(szBuf, "%d", isShow?1:0);

    if(!WritePrivateProfileString(
         TEXT("General"), TEXT("ToolTip"), szBuf, szIniFileName))
        return ERR;

    return OK;
}

/* デバッグログ
   char* szBuf デバッグメッセージ
   戻り値: なし
*/
#ifdef __DEBUG__
void WorldTimeConfig::debug(TCHAR* szBuf){
    HANDLE hFile;
    BOOL bRet;
    DWORD dwWritten;
    SYSTEMTIME stTime;

    TCHAR* szLogMsg = (TCHAR*)HeapAlloc(GetProcessHeap(),
                                      0, sizeof(TCHAR)*lstrlen(szBuf) + 20);

    if(szLogMsg == NULL){
        MessageBox(0, TEXT("debug() ヒープ確保エラー"),
                      TEXT("エラー"), MB_ICONSTOP);
        return;
    }

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
        goto lblHeapFree;
    }

    // ファイル最終端にポインタを移動
    SetFilePointer(hFile , 0 , NULL , FILE_END);

    // 書き込み
    bRet = WriteFile(hFile, szLogMsg, (DWORD)strlen(szLogMsg), &dwWritten, NULL);

    CloseHandle(hFile);

lblHeapFree:
    HeapFree(GetProcessHeap(), 0, szLogMsg);
}

#endif /* __DEBUG__ */
