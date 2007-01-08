/*
 * 初期設定ファイルクラス
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
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

/* エリア情報読み出し
    datetimeinfo_t dtInfo[] 日時情報格納構造体
    戻り値 読み込んだエリア情報数
*/
int WorldTimeConfig::readAreaInfo(datetimeinfo_t dtInfo[]){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[128];

    TCHAR szSectionName[6];     // セクション名
    int iCount = 0;             // カウンター

    // INI ファイル名をフルパスで取得
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // エリア情報読み出し
    for(int i=0; i<MAX_AREA; i++){

        // セクション名作成
        wsprintf(szSectionName, "Area%d", i);

        // エリア名取得
        if(GetPrivateProfileString(
             szSectionName, TEXT("Name"), "",
             szBuf, sizeof(dtInfo[0].szName), szIniFileName) == 0)
            continue;

        lstrcpy(dtInfo[iCount].szName, szBuf);

        // X 座標位置取得
        if(GetPrivateProfileString(
             szSectionName, TEXT("X"), "0", szBuf, 5, szIniFileName) == 0)
            continue;

        dtInfo[iCount].x = _tstoi(szBuf);

        // Y 座標位置取得
        if(GetPrivateProfileString(
             szSectionName, TEXT("Y"), "0", szBuf, 5, szIniFileName) == 0)
            continue;

        dtInfo[iCount].y = _tstoi(szBuf);

        // オフセット取得 (必須)
        if(GetPrivateProfileString(
             szSectionName, TEXT("Offset"), "0", szBuf, 6, szIniFileName) == 0)
            return ERR;

        dtInfo[iCount].offset = _tstof(szBuf);

        // オフセット値チェック
        if(dtInfo[iCount].offset < -12 || dtInfo[iCount].offset > 12)
            return ERR;

        // ツールチップ情報取得
        if(GetPrivateProfileString(
             szSectionName, TEXT("Info"), "",
             szBuf, sizeof(dtInfo[0].szInfo), szIniFileName) == 0)
            continue;

        lstrcpy(dtInfo[iCount].szInfo, szBuf);

        // 夏時間開始情報取得
        if(GetPrivateProfileString(
            szSectionName, TEXT("DSTStart"), "", szBuf, 12, szIniFileName) != 0){

            // 夏時間情報をエリア情報に格納
            if(parseDSTInfo(szBuf, &dtInfo[iCount].iDSTStartMonth, &dtInfo[iCount].szDSTStartType,
                            &dtInfo[iCount].iDSTStartWeekDay, &dtInfo[iCount].iDSTStartTime) != OK)
                return ERR;

            // 夏時間終了情報取得
            if(GetPrivateProfileString(
                szSectionName, TEXT("DSTEnd"), "", szBuf, 12, szIniFileName) != 0){

                // 夏時間情報をエリア情報に格納
                if(parseDSTInfo(szBuf, &dtInfo[iCount].iDSTEndMonth, &dtInfo[iCount].szDSTEndType,
                            &dtInfo[iCount].iDSTEndWeekDay, &dtInfo[iCount].iDSTEndTime) != OK)
                    return ERR;

                dtInfo[iCount].hasDST = true;
            }

        }else{
            // 夏時間情報なし
            dtInfo[iCount].hasDST = false;
        }

        iCount++;
    }

    return iCount;
}

/* 夏時間情報
   TCHAR* szBuf 入力 (形式: 3/E/0/02:10 月/曜日タイプ/曜日/時刻)
   int* iMonth 月
   TCHAR* szType 曜日タイプ
   int* iWeek 曜日
   int* iTime 時刻
   戻り値: OK 成功時 ERR エラー時
*/
int WorldTimeConfig::parseDSTInfo(TCHAR* szBuf, int* iMonth, TCHAR* szType, int* iWeek, int* iTime){

    TCHAR* pToken = NULL;

    int iTmp = 0;

    // 月の解析
    if((pToken = _tcstok(szBuf, "/")) == NULL)
        return ERR;

    iTmp = _tstoi(pToken);

    // 範囲チェック
    if(iTmp <= 0 || iTmp > 12)
        return ERR;

    *iMonth = iTmp;

    // タイプの解析
    if((pToken = _tcstok(NULL, "/")) == NULL)
        return ERR;

    // 値チェック (E でも F でもなく 1～5 以外の範囲が指定)
    if((_tcscmp(pToken, "E") != 0) && (_tcscmp(pToken, "F") != 0) &&
	   (_tstoi(pToken) < 1 || _tstoi(pToken) > 5))
        return ERR;

    _tcscpy(szType, pToken);

    // 曜日の解析
    if((pToken = _tcstok(NULL, "/")) == NULL)
        return ERR;

    iTmp = _tstoi(pToken);

    // 範囲チェック
    if(iTmp < 0 || iTmp > 6)
        return ERR;

    *iWeek = iTmp;

    // 時刻の解析
    if((pToken = _tcstok(NULL, "/")) == NULL)
        return ERR;

    // 時の解析
    if((pToken = _tcstok(pToken, ":")) == NULL)
        return ERR;

    iTmp = _tstoi(pToken);

    // 時の範囲チェック
    if(iTmp < 0 || iTmp > 23)
        return ERR;

    *iTime = iTmp*60;

    // 分の解析
    if((pToken = _tcstok(NULL, ":")) == NULL)
        return ERR;

    iTmp = _tstoi(pToken);

    // 分の範囲チェック
    if(iTmp < 0 || iTmp > 59)
        return ERR;

    *iTime = *iTime + iTmp;

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
    }

    // ファイル最終端にポインタを移動
    SetFilePointer(hFile , 0 , NULL , FILE_END);

    // 書き込み
    bRet = WriteFile(hFile, szLogMsg, (DWORD)strlen(szLogMsg), &dwWritten, NULL);

    CloseHandle(hFile);
}

#endif /* __DEBUG__ */
