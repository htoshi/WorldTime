/*
 * 世界時計ダイアログクラス
 * $Id$
 *
 * Copyright (C) 2011-2014, Toshi All rights reserved.
*/
#pragma once

#include "Config.h"

#define PI 3.14159265358979323846
#define D2R(x) PI*x/180.0
#define R2D(x) x*180.0/PI

#define WM_SYSTEMMENU 0x313

// 定数
static const double a[5]={0.000075, 0.001868, 0.032077, 0.014615, 0.040849};
static const double b[7]={0.006918, 0.399912, 0.070257, 0.006758, 0.000907, 0.002697, 0.001480};
static const int daymonth[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNCHITest)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SYSTEMMENU, OnSystemMenu)
		MSG_WM_CONTEXTMENU(OnContextMenu)
		COMMAND_ID_HANDLER(IDM_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDM_TOOLTIP, OnToolTip)
		COMMAND_ID_HANDLER(IDM_MOVABLE, OnMovable)
		COMMAND_ID_HANDLER(IDM_ALWAYSTOP, OnAlwaysTop)
		COMMAND_ID_HANDLER(IDM_SIMPLEMODE, OnSimpleMode)
		COMMAND_ID_HANDLER(IDM_OPEN_INIFILE, OnOpenInifile)
		COMMAND_ID_HANDLER(IDM_EXIT, OnMenuExit)
		COMMAND_ID_HANDLER(ID_ALPHA_100, OnAlpha100)
		COMMAND_ID_HANDLER(ID_ALPHA_80, OnAlpha80)
		COMMAND_ID_HANDLER(ID_ALPHA_60, OnAlpha60)
		COMMAND_ID_HANDLER(ID_ALPHA_40, OnAlpha40)
		COMMAND_ID_HANDLER(ID_ALPHA_20, OnAlpha20)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnToolTip(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMovable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAlwaysTop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSimpleMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpenInifile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMenuExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNCHITest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSystemMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnAlpha100(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAlpha80(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAlpha60(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAlpha40(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAlpha20(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnContextMenu(HWND, CPoint);

private:
	Config* m_config;
	datetimeinfo_t m_datetime[MAX_AREA];	// 日時情報構造体配列
	int m_datetime_number;					// 日時情報構造体に格納された数
	CMenu m_PopupMenu;						// ポップアップメニュー
	CToolTipCtrl m_ToolTip;					// ツールチップ
	bool m_topmost_flag;					// 常に最前面に表示
	bool m_tooltip_flag;					// ツールチップ
	int m_alpha;							// 透過率
	bool m_simplemode;						// シンプルモード
	int m_bitmap_width;						// ビットマップ幅
	int m_bitmap_height;					// ビットマップ高さ
	bool m_movable;							// 移動可フラグ

	// デバイスコンテキスト
	HDC m_WorkDC;
	HDC m_BackDC;

	HBITMAP m_bmp;
	HBITMAP m_BackBmp;
	HFONT m_font;

	void setWindowPosition(RECT);
	void setCurrentTime(datetime_t);
	int splitTZName(TCHAR*, TCHAR*, size_t, TCHAR*, size_t);
	void draw(void);
	void drawText(void);
	void CloseDialog(int);
	void setToolTipState(bool);
	void setAlwaysTopState(bool);
	void setAlpha(int);
	void setSimpleMode(void);

	// WorldTimeCalc.cpp
	int getMonthFirstWeekDay(int, int, int);
	int getMonthLastWeekDay(int, int, int);
	int getMonthMWeekDay(int, int, int, int);
	int isDateAfterSpecifiedDate(int, int, int, int);
	int getCurrentGMT(datetime_t*);
	int getWorldTime(datetime_t*, datetime_t*, double, bool, dstinfo_t*, dstinfo_t*);
	int zeller(int, int, int);
	int getDayOfMonth(int, int);
	bool isDataInSpecifiedRange(int, int, int, int, int, int, bool);
	int getDateTimeByOffset(datetime_t*, int);
	int getTotalMinutesInDay(int, int);
	int setDSTDay(dstinfo_t*, int);

};
