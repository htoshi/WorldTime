/*
 * 世界時計ダイアログクラス
 * $Id$
 *
 * Copyright (C) 2011, Toshi All rights reserved.
*/
#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "Config.h"

/* ウィンドウメッセージのフィルタ処理
	戻り値: BOOL
*/
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

/* アイドル時
	戻り値 BOOL
*/
BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

/* ダイアログ初期化
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT Rect;
	int iRet;

	// INIファイルから設定を読み込み
	m_config = new Config();
	m_datetime_number = m_config->readAreaInfo(m_datetime);

	iRet = m_config->readWindowRect(&Rect);					// 位置・サイズ
	iRet += m_config->readTopMostFlag(&m_topmost_flag);		// 最前面に表示
	iRet += m_config->readToolTipFlag(&m_tooltip_flag);		// ツールチップ
	iRet += m_config->readAlpha(&m_alpha);					// 透明度
	iRet += m_config->readSimpleMode(&m_simplemode);		// シンプルモード

	if(iRet != OK){
		MessageBox(_T("INIファイルが異常です"), _T("Error"), MB_ICONSTOP | MB_OK);
		delete(m_config);
		return -1;
	}

	// 位置を設定
	SetWindowPos(NULL, Rect.left, Rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// ポップアップメニュー読み込み
	m_PopupMenu.LoadMenu(IDR_RMENU);

	// ツールチップ作成
	m_ToolTip.Create(m_hWnd);

	setToolTipState(false);		// ツールチップ・メニューチェック状態設定
	setAlwaysTopState(false);	// 常に最前面に表示・メニューチェック状態設定
	setAlpha(m_alpha);			// 透過設定
	setSimpleMode();			// シンプルモード(タイトルバー・枠・タスクバーアイコン非表示)

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	HDC dc = GetDC();

	// ビットマップオブジェクトの読み込み
	m_bmp = LoadBitmap(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_WORLDMAP));
	{
		// 幅と高さを取得
		BITMAP bmp;
		GetObject(m_bmp, sizeof(BITMAP), &bmp);
		m_bitmap_width = bmp.bmWidth;
		m_bitmap_height = bmp.bmHeight;
	}

	// ワーキングコンテキスト作成
	m_WorkDC = CreateCompatibleDC(dc);

	// 背景を透過に設定	
	SetBkMode(m_WorkDC , TRANSPARENT);

	SelectObject(m_WorkDC, m_bmp);

	// 裏画面用コンテキスト作成・メモリビットマップ作成
	m_BackDC = CreateCompatibleDC(dc);
	m_BackBmp = CreateCompatibleBitmap(dc, m_bitmap_width, m_bitmap_height);

	SelectObject(m_BackDC, m_BackBmp);

	// フォント設定
	m_font = CreateFont(12, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
				SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				FIXED_PITCH | FF_MODERN, _T("MS Gothic"));
	SelectObject(m_WorkDC , m_font);

	ReleaseDC(dc);

	UIAddChildWindowContainer(m_hWnd);

	// 描画
	draw();

	// インターバル 500ms でタイマーセット
	SetTimer(IDC_TIMER, 500, NULL);

	// 通常表示
//	ShowWindow(SW_SHOWNORMAL);
	SetWindowPos(HWND_TOP, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME | SWP_SHOWWINDOW);

	return TRUE;
}

/* 終了時処理
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// 後始末
	DeleteDC(m_WorkDC);
	DeleteDC(m_BackDC);

	DeleteObject(m_bmp);
	DeleteObject(m_BackBmp);
	DeleteObject(m_font);

	RECT Rect;

	// 現在位置とサイズを取得
	GetWindowRect((LPRECT)&Rect);

	// 設定を保存
	m_config->writeWindowRect(&Rect);
	m_config->writeTopMostFlag(m_topmost_flag);
	m_config->writeToolTipFlag(m_tooltip_flag);
	m_config ->writeAlpha(m_alpha);

	delete(m_config);

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

/* タイマーイベント発生時
	wParam: タイマーID
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(wParam == IDC_TIMER){
		SYSTEMTIME stTime;					// 現在時刻
		static bool isTimeUpdated = false;	// 描画処理完了フラグ

		// 現在時刻を取得
		GetSystemTime(&stTime);

		// 1 分に 1 回実行
		if((stTime.wSecond == 0) && !isTimeUpdated){
			isTimeUpdated = true;
			draw();
		}else if((stTime.wSecond != 0) && isTimeUpdated){
			isTimeUpdated = false;
		}
//ATLTRACE(_T("WM_TIMER %d %d\n"), rand(), wParam);
	}

	return 0;
}

/* 描画イベント発生時
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	// 画面のデザインを変更した場合のサイズ調整
	int frame_width = 2*GetSystemMetrics(SM_CXFIXEDFRAME);

	int width;
	int height;

	// シンプルモード時
	if(m_simplemode){
		width = m_bitmap_width;
		height = m_bitmap_height;
	}else{
		width = m_bitmap_width + frame_width;
		height = m_bitmap_height + GetSystemMetrics(SM_CYCAPTION) + frame_width;
	}

	// サイズを設定
	SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);

	// ビットマップ転送
	BitBlt(dc, 0, 0, m_bitmap_width, m_bitmap_height, m_WorkDC, 0, 0, SRCCOPY);

	return 0;
}

/* 世界時計について・メニュー選択時
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();

	return 0;
}

/* ツールチップ・メニュー状態を設定
	toggle: 状態をトグル
*/
void CMainDlg::setToolTipState(bool toggle)
{
	MENUITEMINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;

	// メニュー情報取得
	if(m_PopupMenu.GetMenuItemInfo(IDM_TOOLTIP, FALSE, &menuInfo) == FALSE){
		return;
	}

	// トグル
	if(toggle){
		// ツールチップ表示時
		if(m_tooltip_flag){
			m_tooltip_flag = false;
			menuInfo.fState = MFS_UNCHECKED;	// アンチェック
			m_ToolTip.DelTool(m_hWnd, 0);		// ツールチップ非表示
		}else{
			m_tooltip_flag = true;
			menuInfo.fState = MFS_CHECKED;		// チェック
			CToolInfo ti(TTF_SUBCLASS, m_hWnd, 0, NULL, _T(" "));	// ツールチップ表示
			m_ToolTip.AddTool(ti);
		}

	// 状態設定のみ
	}else{
		if(m_tooltip_flag){
			menuInfo.fState = MFS_CHECKED;		// チェック
			CToolInfo ti(TTF_SUBCLASS, m_hWnd, 0, NULL, _T(" "));	// ツールチップ表示
			m_ToolTip.AddTool(ti);
		}else{
			menuInfo.fState = MFS_UNCHECKED;	// アンチェック
			m_ToolTip.DelTool(m_hWnd, 0);		// ツールチップ非表示
		}
	}

	// メニュー情報設定
	m_PopupMenu.SetMenuItemInfo(IDM_TOOLTIP, FALSE, &menuInfo);
}

/* ツールチップを表示・メニュー選択時
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnToolTip(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	setToolTipState(true);
	return 0;
}

/* 常に手前に表示・メニュー状態を設定
	toggle: 状態をトグル
*/
void CMainDlg::setAlwaysTopState(bool toggle)
{
	MENUITEMINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;

	// メニュー情報取得
	if(m_PopupMenu.GetMenuItemInfo(IDM_ALWAYSTOP, FALSE, &menuInfo) == FALSE){
		return;
	}

	// トグル
	if(toggle){
		if(m_topmost_flag){
			m_topmost_flag = false;
			menuInfo.fState = MFS_UNCHECKED;	// アンチェック
			SetWindowPos(HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
		}else{
			m_topmost_flag = true;
			menuInfo.fState = MFS_CHECKED;		// チェック
			SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
		}
	// 状態設定のみ
	}else{
		if(m_topmost_flag){
			menuInfo.fState = MFS_CHECKED;		// チェック
			SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
		}else{
			menuInfo.fState = MFS_UNCHECKED;	// アンチェック
			SetWindowPos(HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
		}
	}

	// メニュー情報設定
	m_PopupMenu.SetMenuItemInfo(IDM_ALWAYSTOP, FALSE, &menuInfo);
}

/* 常に手前に表示・メニュー選択時
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnAlwaysTop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	setAlwaysTopState(true);
	return 0;
}

/* 設定ファイルを開く・メニュー選択時
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnOpenInifile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR szIniFileName[MAX_PATHLEN];

	// INI ファイル名をフルパスで取得
	if(m_config->getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// INI ファイルを開く
	ShellExecute(m_hWnd, _T("open"), szIniFileName, _T(""), _T(""), SW_SHOW);

	return 0;
}

/* シンプルモード設定
	戻り値: なし
*/
void CMainDlg::setSimpleMode()
{
	if(m_simplemode){
		MENUITEMINFO menuInfo;
		menuInfo.cbSize = sizeof(MENUITEMINFO);
		menuInfo.fMask = MIIM_STRING;

		TCHAR szBuf[] = TEXT("通常モード");
		menuInfo.dwTypeData = szBuf;

		// コンテキストメニュー設定
		m_PopupMenu.SetMenuItemInfo(IDM_SIMPLEMODE, FALSE, &menuInfo);

		// ウィンドウスタイル設定
		LONG style = GetWindowLong(GWL_EXSTYLE);
		SetWindowLong(GWL_EXSTYLE, style | WS_EX_TOOLWINDOW);
		SetWindowLong(GWL_STYLE, WS_POPUP);
	}
}

/* シンプルモード・メニュー選択時
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnSimpleMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MENUITEMINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE | MIIM_STRING;
	menuInfo.fState = MFS_DISABLED;					// 選択不可

	if(m_simplemode){
		TCHAR szBuf[] = TEXT("シンプルモード");
		menuInfo.dwTypeData = szBuf;
		m_config ->writeSimpleMode(false);
	}else{
		TCHAR szBuf[] = TEXT("通常モード");
		menuInfo.dwTypeData = szBuf;
		m_config ->writeSimpleMode(true);
	}

	// メニュー情報設定
	m_PopupMenu.SetMenuItemInfo(IDM_SIMPLEMODE, FALSE, &menuInfo);

	// メッセージ表示
	TCHAR szMsg[] = TEXT("アプリケーションを再起動してください");

	SetTextColor(m_WorkDC, RGB(255, 0, 0));
	TextOut(m_WorkDC, 70, 155, szMsg, _tcslen(szMsg));
	InvalidateRect(NULL, TRUE);

	return 0;
}

/* クライアント領域をクリックして移動
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnNCHITest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lr = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

	bHandled = false;

	// 左ボタンが押された時
	if(GetAsyncKeyState(VK_LBUTTON) < 0){
		switch(lr){
			case HTCLOSE:
			case HTMINBUTTON:
			case HTSYSMENU:
					  break;
			default:
				bHandled = true;
				return HTCAPTION;
		}
	}

	return 0;
}

/* マウス移動イベント発生時
	LPARAM マウス位置
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_tooltip_flag == false){
		return 0;
	}

	static ULONGLONG LastCount = GetTickCount() - 5000;
	static int iLastIndex = -1;

	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	// 現在の tick count
	ULONGLONG Count = GetTickCount();

	// 繰り上がり対策
	if(Count < LastCount){
		Count+=ULONG_MAX;
		Count++;
	}

	// それぞれのタイムゾーンのツールチップを表示
	for(int i=0; i<sizeof(m_datetime)/sizeof(m_datetime[0]); i++){
		if((x >= m_datetime[i].x) && (x <= m_datetime[i].x + 34) &&
			(y >= m_datetime[i].y) && (y <= m_datetime[i].y + 32)){

			// 0.5 秒以内
			if((Count - LastCount < 500) && (iLastIndex == i)){
				return 0;
			}

			LastCount = Count;
			iLastIndex = i;
			m_ToolTip.UpdateTipText(m_datetime[i].szInfo, m_hWnd, 0);
			m_ToolTip.Update();
			m_ToolTip.Activate(TRUE);

			return 0;
		}
	}

	m_ToolTip.Activate(FALSE);

	return 0;
}

/* キー押下時(デバッグ用)
	WPARAM キーコード
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(wParam != VK_SPACE) return 0;

//ATLTRACE(_T("OnKeyDown\n"));

	return 0;
}

/* 右クリック時コンテキストメニュー表示
	HWND ウィンドウハンドル
	CPoint クリック位置
	戻り値: なし
*/
void CMainDlg::OnContextMenu(HWND hWnd, CPoint pt)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);

	if(rc.PtInRect(pt)){
		m_PopupMenu.GetSubMenu(0).TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, pt.x, pt.y, m_hWnd);
	}else{
		SetMsgHandled(false);
	}
}

/* 描画処理
	戻り値: なし
*/
void CMainDlg::draw()
{
	HDC dc = GetDC();
	datetime_t dtGMT;

	// ワーキングコンテキスト再作成
	if(m_WorkDC != NULL){
		DeleteDC(m_WorkDC);
		m_WorkDC = NULL;
		m_WorkDC = CreateCompatibleDC(dc);

		// 背景を透過に設定
		SetBkMode(m_WorkDC , TRANSPARENT);
		SelectObject(m_WorkDC , m_font);
	}

	// ビットマップ読み直し
	if(m_bmp != NULL){
		DeleteObject(m_bmp);
		m_bmp = NULL;
		m_bmp = LoadBitmap(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_WORLDMAP));
	}

	// ビットマップ再選択
	SelectObject(m_WorkDC, m_bmp);
	// 裏画面初期化
	PatBlt(m_BackDC, 0, 0, m_bitmap_width, m_bitmap_height, BLACKNESS);

	// GMT を取得
	getCurrentGMT(&dtGMT);

	double hour=dtGMT.Hour + (double)(dtGMT.Minute/60.0) + (double)(dtGMT.Second/3600.0);

	int J=0;

	for(int i=0; i<dtGMT.Month-1; i++){
		J+=daymonth[i];
	}

	if((dtGMT.Month > 2) && (dtGMT.Year%4 == 0 && dtGMT.Year%100 != 0 || dtGMT.Year%400 == 0)){
		J+=1;
	}

	J+=dtGMT.Day;

	double A=2.0*PI*J/365.0;
	double B=(2.0*PI*(double)J)/365.0;

	double ET = ((a[0] + a[1]*cos(B) - a[2]*sin(B) - a[3]*cos(2.0*B)
				- a[4]*sin(2.0*B))*12.0)/PI;
	double delta = b[0] - b[1]*cos(A) + b[2]*sin(A)
					- b[3]*cos(2.0*A)+b[4]*sin(2.0*A)-b[5]*cos(3.0*A)+b[6]*sin(3.0*A);

	double lat, lon;

	for(int x=0; x<360; x+=1){
		for(int y=0; y<=169; y+=1){
			// 緯度方向の座標変換
			if(x>=210){
				lon = x-390.0;
			}else{
				lon = x-30.0;
			}
			// 経度方向の座標変換
			lat = -(10.0/11.0)*(double)y+90.0;

			double phi=D2R(lat);
			double MST=hour+lon/15.0;

			double TST = MST + ET;
			double t = D2R(15.0)*(TST-12.0);

			double thetaV=acos(sin(delta)*sin(phi)+cos(delta)*cos(phi)*cos(t));
			double ele = 90.0-180.0*thetaV/PI;

			// 昼間部分のみ
			if(ele>0){
				SetPixel(m_BackDC, x, y, RGB(64, 64, 64));
			}
		} // for
	} // for

/*
	// 経緯度線(チェック用)
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
	HPEN pen2 = CreatePen(PS_SOLID, 2, RGB(255,0,0));
	SelectObject(m_BackDC, pen);

	for(int x=0; x<=360; x+=30){
		MoveToEx(m_BackDC, x, 0, NULL);
		LineTo(m_BackDC, x, 169);
	}

	for(int ll=-60; ll<=75; ll+=15){
		int y = 1.1*(90.0-ll);

		if(ll==0)
			SelectObject(m_BackDC, pen2);
		else
			SelectObject(m_BackDC, pen);

		MoveToEx(m_BackDC, 0, y, NULL);
		LineTo(m_BackDC, 360, y);
	}

	DeleteObject(pen);
	DeleteObject(pen2);
*/

	// 裏コンテキストからワーキングコンテキストに転送
	BitBlt(m_WorkDC, 0, 0, m_bitmap_width, m_bitmap_height, m_BackDC, 0, 0, SRCINVERT);
	ReleaseDC(dc);

	// 現在時刻を設定
	setCurrentTime(dtGMT);

	// 時刻を描画
	drawText();

	InvalidateRect(NULL, FALSE);
}

/* 現在日時をセットする
	datetime_t GMT時刻
	戻り値: なし
*/
void CMainDlg::setCurrentTime(datetime_t dtGMT)
{
	datetime_t dt;
	dstinfo_t dstStart, dstEnd;

	// 各エリアの時刻を取得してセット
	for(int i=0; i<m_datetime_number; i++){

		// 夏時間情報をセット
		if(m_datetime[i].hasDST){

			// 夏時間開始情報
			dstStart.Month = m_datetime[i].iDSTStartMonth;
			dstStart.Type = m_datetime[i].szDSTStartType;
			dstStart.WeekDay = m_datetime[i].iDSTStartWeekDay;
			dstStart.Time = m_datetime[i].iDSTStartTime;

			// 夏時間終了情報
			dstEnd.Month = m_datetime[i].iDSTEndMonth;
			dstEnd.Type = m_datetime[i].szDSTEndType;
			dstEnd.WeekDay = m_datetime[i].iDSTEndWeekDay;
			dstEnd.Time = m_datetime[i].iDSTEndTime;
		}

		// 世界時刻取得
		getWorldTime(&dt, &dtGMT, m_datetime[i].offset, m_datetime[i].hasDST, &dstStart, &dstEnd);

		wsprintf(m_datetime[i].szDate, _T("%02d/%02d"), dt.Month, dt.Day);
		wsprintf(m_datetime[i].szTime, _T("%02d:%02d"), dt.Hour, dt.Minute);
		m_datetime[i].isDST = dt.isDST;
	}
}

/* タイムゾーン名を分割して取得
	TCHAR* szNameIn タイムゾーン名 (形式: AEST/AEDT)
	TCHAR* szST 標準時タイムゾーン名 (出力)
	TCHAR* szDT 夏時間タイムゾーン名 (出力)
	戻り値: OK 成功時 ERR エラー時
*/
int CMainDlg::splitTZName(TCHAR* szNameIn, TCHAR* szST, TCHAR* szDT)
{
	TCHAR* pToken = NULL;
	size_t iLen = 0;

	// 最初のトークン
	if((pToken = _tcsstr(szNameIn, _T("/"))) == NULL){
		return ERR;
	}

	iLen = _tcslen(szNameIn) - _tcslen(pToken);
	_tcsncpy(szST, szNameIn, iLen);
	*(szST + iLen) = '\0';

	// 次のトークン
	_tcscpy(szDT, pToken+1);

	return OK;
}

/* テキストを描画
	戻り値: なし
*/
void CMainDlg::drawText()
{
	int iTZOffset;		// タイムゾーンオフセット調整
	TCHAR szST[10];		// 標準時タイムゾーン名格納用
	TCHAR szDT[10];		// 夏時間タイムゾーン名格納用

	// それぞれのタイムゾーンの日時を表示
	for(int i=0; i<m_datetime_number; i++){
		// タイムゾーン名を分割
		if(splitTZName(m_datetime[i].szName, szST, szDT) != OK){
			_tcscpy(szST, m_datetime[i].szName);
			_tcscpy(szDT, m_datetime[i].szName);
		}

		// 夏時間
		if(m_datetime[i].isDST){

			// タイムゾーン名の微妙な位置調整
			iTZOffset = (lstrlen(szDT) == 3)?8:4;

			// テキスト色は黄
			SetTextColor(m_WorkDC, RGB(255, 255, 0));

			// タイムゾーン名表示
			TextOut(m_WorkDC, m_datetime[i].x+iTZOffset, m_datetime[i].y, szDT, lstrlen(szDT));
		}else{

			// タイムゾーン名の微妙な位置調整
			iTZOffset = (lstrlen(szST) == 3)?8:4;

			// テキスト色は白
			SetTextColor(m_WorkDC, RGB(255, 255, 255));

			// タイムゾーン名表示
			TextOut(m_WorkDC, m_datetime[i].x+iTZOffset, m_datetime[i].y, szST, lstrlen(szST));
		}

		// 日付と時刻
		TextOut(m_WorkDC, m_datetime[i].x, m_datetime[i].y+10, m_datetime[i].szDate, lstrlen(m_datetime[i].szDate));
		TextOut(m_WorkDC, m_datetime[i].x, m_datetime[i].y+20, m_datetime[i].szTime, lstrlen(m_datetime[i].szTime));
	}
}

/* Alpha 100%
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnAlpha100(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 100;
	setAlpha(100);
	return 0;
}

/* Alpha 80%
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnAlpha80(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 80;
	setAlpha(80);
	return 0;
}

/* Alpha 60%
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnAlpha60(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 60;
	setAlpha(60);
	return 0;
}

/* Alpha 40%
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnAlpha40(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 40;
	setAlpha(40);
	return 0;
}

/* Alpha 20%
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnAlpha20(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 20;
	setAlpha(20);
	return 0;
}

/* 透過設定
	alpha 透明度
*/
void CMainDlg::setAlpha(int alpha){

	MENUITEMINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;

	// メニューをチェック
	for(int i=0; i<5; i++){
		m_PopupMenu.GetMenuItemInfo(ID_ALPHA_20+i, FALSE, &menuInfo);

		if(alpha == i*20+20){
			menuInfo.fState = MFS_CHECKED;
		}else{
			menuInfo.fState = MFS_UNCHECKED;
		}

		m_PopupMenu.SetMenuItemInfo(ID_ALPHA_20+i, FALSE, &menuInfo);
	}

	// 透過設定
	LONG style = GetWindowLong(GWL_EXSTYLE);
	SetWindowLong(GWL_EXSTYLE, style | WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_hWnd, NULL, (int)((double)alpha*2.55), LWA_ALPHA);
}

/* 閉じるボタン押下時
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CloseDialog(IDOK);
	return 0;
}

/* 終了メニュー選択時
	戻り値: LRESULT
*/
LRESULT CMainDlg::OnMenuExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(IDOK);
	return 0;
}

/* ダイアログを閉じる
	int ステータスコード
	戻り値: なし
*/
void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}
