/*
 * 初期設定ファイルクラス
 * $Id$
 *
 * Copyright (C) 2011, Toshi All rights reserved.
*/
#include "stdafx.h"		// for C1010 error
#include "Config.h"

// コンストラクタ
Config::Config(){}

// デストラクタ
Config::~Config(){}

/* 設定ファイル名をフルパスで返す
	TCHAR* 設定ファイル名
	DWORD 設定ファイル名バッファのサイズ
*/
int Config::getIniFileName(TCHAR* szFullPathName, DWORD nSize){

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[MAX_PATHLEN];

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

/* 設定ファイルのWindow位置を読み込んで返す(RightとBottomは無視)
	LPRECT rect ウィンドウの座標値
	戻り値 OK 成功
*/
int Config::readWindowRect(RECT* rect){

	TCHAR szIniFileName[MAX_PATHLEN];
	TCHAR szBuf[100];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// Top 位置の取得
	if(GetPrivateProfileString(
		_T("General"), _T("Top"), DEFAULT_WINDOW_TOP,
		szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	rect->top = (_tstol(szBuf) == 0)?_tstol(DEFAULT_WINDOW_TOP):_tstol(szBuf);

	// Left 位置の取得
	if(GetPrivateProfileString(
	_T("General"), _T("Left"), DEFAULT_WINDOW_LEFT,
	szBuf, sizeof(szBuf), szIniFileName) == 0)
	return ERR;

	rect->left = (_tstol(szBuf) == 0)?_tstol(DEFAULT_WINDOW_LEFT):_tstol(szBuf);

	return OK;
}

/* 現在のWindow位置を設定ファイルに書き出す(RightとBottomは無視)
	LPRECT rect ウィンドウの座標値
	戻り値 OK 成功
*/
int Config::writeWindowRect(RECT* rect){

	TCHAR szIniFileName[MAX_PATHLEN];
	TCHAR szBuf[100];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// Top 位置の保存
	wsprintf(szBuf, _T("%d"), rect->top);

	if(!WritePrivateProfileString(
		_T("General"), _T("Top"), szBuf, szIniFileName))
		return ERR;

	// Left 位置の保存
	wsprintf(szBuf, _T("%d"), rect->left);

	if(!WritePrivateProfileString(
		_T("General"), _T("Left"), szBuf, szIniFileName))
		return ERR;

	return OK;
}

/* 常に最前面表示情報読み出し
    戻り値 OK 成功
*/
int Config::readTopMostFlag(bool* isTop){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[5];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// 常に最前面表示情報読み出し (1: 常に最前面)
	if(GetPrivateProfileString(
		_T("General"), _T("AlwaysTop"), _T("0"),
			szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	*isTop = (_tstoi(szBuf) == 1)?true:false;

	return OK;
}

/* 常に最前面表示情報書き出し
    戻り値 OK 成功
*/
int Config::writeTopMostFlag(bool isTop){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[10];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
	return ERR;

	// 最前面状態の保存
	wsprintf(szBuf, _T("%d"), isTop?1:0);

	if(!WritePrivateProfileString(
		_T("General"), _T("AlwaysTop"), szBuf, szIniFileName))
		return ERR;

	return OK;
}

/* ツールチップ表示情報読み出し
    bool* isShow
    戻り値 OK 成功
*/
int Config::readToolTipFlag(bool* isShow){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[5];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// ツールチップ表示情報読み出し (1: 表示)
	if(GetPrivateProfileString(
		_T("General"), _T("ToolTip"), _T("0"),
		szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	*isShow = (_tstoi(szBuf) == 1)?true:false;

	return OK;
}

/* ツールチップ表示情報書き出し
    bool isShow
    戻り値 OK 成功
*/
int Config::writeToolTipFlag(bool isShow){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[10];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// ツールチップ表示情報保存
	wsprintf(szBuf, _T("%d"), isShow?1:0);

	if(!WritePrivateProfileString(
		_T("General"), _T("ToolTip"), szBuf, szIniFileName))
		return ERR;

	return OK;
}

/* 透明度読み出し
    int* alpha
    戻り値 OK 成功
*/
int Config::readAlpha(int* alpha){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[5];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// 透明度読み出し
	if(GetPrivateProfileString(
		_T("General"), _T("Alpha"), _T("0"),
		szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	*alpha = _tstoi(szBuf);

	// 最小値 20 最大値 100
	if(*alpha < 20 || *alpha > 100){
		*alpha = 100;
	}

	return OK;
}

/* 透明度書き出し
    int alpha
    戻り値 OK 成功
*/
int Config::writeAlpha(int alpha){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[10];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// 透明度保存
	wsprintf(szBuf, _T("%d"), alpha);

	if(!WritePrivateProfileString(
		_T("General"), _T("Alpha"), szBuf, szIniFileName))
		return ERR;

	return OK;
}


/* シンプルモード読み出し
    戻り値 OK 成功
*/
int Config::readSimpleMode(bool* isTop){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[5];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// シンプルモード読み出し (1: シンプルモード)
	if(GetPrivateProfileString(
		_T("General"), _T("SimpleMode"), _T("0"),
			szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	*isTop = (_tstoi(szBuf) == 1)?true:false;

	return OK;
}

/* シンプルモード書き出し
    戻り値 OK 成功
*/
int Config::writeSimpleMode(bool isTop){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[10];

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
	return ERR;

	// シンプルモードの保存
	wsprintf(szBuf, _T("%d"), isTop?1:0);

	if(!WritePrivateProfileString(
		_T("General"), _T("SimpleMode"), szBuf, szIniFileName))
		return ERR;

	return OK;
}

/* エリア情報読み出し
    datetimeinfo_t dtInfo[] 日時情報格納構造体
    戻り値 読み込んだエリア情報数
*/
int Config::readAreaInfo(datetimeinfo_t dtInfo[]){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[128];

	TCHAR szSectionName[16];	// セクション名
	int iCount = 0;				// カウンター

	// INI ファイル名をフルパスで取得
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// エリア情報読み出し
	for(int i=0; i<MAX_AREA; i++){

		// セクション名作成
		wsprintf(szSectionName, _T("Area%d"), i);

		// エリア名取得
		if(GetPrivateProfileString(
			szSectionName, _T("Name"), _T(""),
			szBuf, sizeof(dtInfo[0].szName), szIniFileName) == 0)
			continue;

		lstrcpy(dtInfo[iCount].szName, szBuf);

		// X 座標位置取得
		if(GetPrivateProfileString(
			szSectionName, _T("X"), _T("0"), szBuf, 5, szIniFileName) == 0)
			continue;

		dtInfo[iCount].x = _tstoi(szBuf);

		// Y 座標位置取得
		if(GetPrivateProfileString(
			szSectionName, _T("Y"), _T("0"), szBuf, 5, szIniFileName) == 0)
			continue;

		dtInfo[iCount].y = _tstoi(szBuf);

		// オフセット取得 (必須)
		if(GetPrivateProfileString(
			szSectionName, _T("Offset"), _T("0"), szBuf, 6, szIniFileName) == 0)
			return ERR;

		dtInfo[iCount].offset = _tstof(szBuf);

		// オフセット値チェック
		if(dtInfo[iCount].offset < -12 || dtInfo[iCount].offset > 12)
			return ERR;

		// ツールチップ情報取得
		if(GetPrivateProfileString(
			szSectionName, _T("Info"), _T(""),
			szBuf, sizeof(dtInfo[0].szInfo), szIniFileName) == 0)
			continue;

		lstrcpy(dtInfo[iCount].szInfo, szBuf);

		// 夏時間開始情報取得
		if(GetPrivateProfileString(
			szSectionName, _T("DSTStart"), _T(""), szBuf, 12, szIniFileName) != 0){

				// 夏時間情報をエリア情報に格納
				if(parseDSTInfo(szBuf, &dtInfo[iCount].iDSTStartMonth, &dtInfo[iCount].szDSTStartType,
					&dtInfo[iCount].iDSTStartWeekDay, &dtInfo[iCount].iDSTStartTime) != OK)
					return ERR;

				// 夏時間終了情報取得
				if(GetPrivateProfileString(
					szSectionName, _T("DSTEnd"), _T(""), szBuf, 12, szIniFileName) != 0){

					// 夏時間情報をエリア情報に格納
					if(parseDSTInfo(szBuf, &dtInfo[iCount].iDSTEndMonth, &dtInfo[iCount].szDSTEndType,
						&dtInfo[iCount].iDSTEndWeekDay, &dtInfo[iCount].iDSTEndTime) != OK)
						return ERR;

					dtInfo[iCount].hasDST = true;
				} // if

		}else{
			// 夏時間情報なし
			dtInfo[iCount].hasDST = false;
		} // if

		iCount++;
	} // for

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
int Config::parseDSTInfo(TCHAR* szBuf, int* iMonth, TCHAR* szType, int* iWeek, int* iTime){

	TCHAR* pToken = NULL;

	int iTmp = 0;

	// 月の解析
	if((pToken = _tcstok(szBuf, _T("/"))) == NULL)
		return ERR;

	iTmp = _tstoi(pToken);

	// 範囲チェック
	if(iTmp <= 0 || iTmp > 12)
		return ERR;

	*iMonth = iTmp;

	// タイプの解析
	if((pToken = _tcstok(NULL, _T("/"))) == NULL)
		return ERR;

	// 値チェック (E でも F でもなく 1～5 以外の範囲が指定)
	if((_tcscmp(pToken, _T("E")) != 0) && (_tcscmp(pToken, _T("F")) != 0) &&
		(_tstoi(pToken) < 1 || _tstoi(pToken) > 5))
	return ERR;

	_tcscpy(szType, pToken);

	// 曜日の解析
	if((pToken = _tcstok(NULL, _T("/"))) == NULL)
		return ERR;

	iTmp = _tstoi(pToken);

	// 範囲チェック
		if(iTmp < 0 || iTmp > 6)
			return ERR;

	*iWeek = iTmp;

	// 時刻の解析
	if((pToken = _tcstok(NULL, _T("/"))) == NULL)
		return ERR;

	// 時の解析
	if((pToken = _tcstok(pToken, _T(":"))) == NULL)
		return ERR;

	iTmp = _tstoi(pToken);

	// 時の範囲チェック
	if(iTmp < 0 || iTmp > 23)
		return ERR;

	*iTime = iTmp*60;

	// 分の解析
	if((pToken = _tcstok(NULL, _T(":"))) == NULL)
		return ERR;

	iTmp = _tstoi(pToken);

	// 分の範囲チェック
	if(iTmp < 0 || iTmp > 59)
		return ERR;

	*iTime = *iTime + iTmp;

	return OK;
}
