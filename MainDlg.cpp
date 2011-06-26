/*
 * ���E���v�_�C�A���O�N���X
 * $Id$
 *
 * Copyright (C) 2011, Toshi All rights reserved.
*/
#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "Config.h"

/* �E�B���h�E���b�Z�[�W�̃t�B���^����
	�߂�l: BOOL
*/
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

/* �A�C�h����
	�߂�l BOOL
*/
BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

/* �_�C�A���O������
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT Rect;
	int iRet;

	// INI�t�@�C������ݒ��ǂݍ���
	m_config = new Config();
	m_datetime_number = m_config->readAreaInfo(m_datetime);

	iRet = m_config->readWindowRect(&Rect);					// �ʒu�E�T�C�Y
	iRet += m_config->readTopMostFlag(&m_topmost_flag);		// �őO�ʂɕ\��
	iRet += m_config->readToolTipFlag(&m_tooltip_flag);		// �c�[���`�b�v
	iRet += m_config->readAlpha(&m_alpha);					// �����x
	iRet += m_config->readSimpleMode(&m_simplemode);		// �V���v�����[�h

	if(iRet != OK){
		MessageBox(_T("INI�t�@�C�����ُ�ł�"), _T("Error"), MB_ICONSTOP | MB_OK);
		delete(m_config);
		return -1;
	}

	// �ʒu��ݒ�
	SetWindowPos(NULL, Rect.left, Rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// �|�b�v�A�b�v���j���[�ǂݍ���
	m_PopupMenu.LoadMenu(IDR_RMENU);

	// �c�[���`�b�v�쐬
	m_ToolTip.Create(m_hWnd);

	setToolTipState(false);		// �c�[���`�b�v�E���j���[�`�F�b�N��Ԑݒ�
	setAlwaysTopState(false);	// ��ɍőO�ʂɕ\���E���j���[�`�F�b�N��Ԑݒ�
	setAlpha(m_alpha);			// ���ߐݒ�
	setSimpleMode();			// �V���v�����[�h(�^�C�g���o�[�E�g�E�^�X�N�o�[�A�C�R����\��)

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

	// �r�b�g�}�b�v�I�u�W�F�N�g�̓ǂݍ���
	m_bmp = LoadBitmap(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_WORLDMAP));
	{
		// ���ƍ������擾
		BITMAP bmp;
		GetObject(m_bmp, sizeof(BITMAP), &bmp);
		m_bitmap_width = bmp.bmWidth;
		m_bitmap_height = bmp.bmHeight;
	}

	// ���[�L���O�R���e�L�X�g�쐬
	m_WorkDC = CreateCompatibleDC(dc);

	// �w�i�𓧉߂ɐݒ�	
	SetBkMode(m_WorkDC , TRANSPARENT);

	SelectObject(m_WorkDC, m_bmp);

	// ����ʗp�R���e�L�X�g�쐬�E�������r�b�g�}�b�v�쐬
	m_BackDC = CreateCompatibleDC(dc);
	m_BackBmp = CreateCompatibleBitmap(dc, m_bitmap_width, m_bitmap_height);

	SelectObject(m_BackDC, m_BackBmp);

	// �t�H���g�ݒ�
	m_font = CreateFont(12, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
				SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				FIXED_PITCH | FF_MODERN, _T("MS Gothic"));
	SelectObject(m_WorkDC , m_font);

	ReleaseDC(dc);

	UIAddChildWindowContainer(m_hWnd);

	// �`��
	draw();

	// �C���^�[�o�� 500ms �Ń^�C�}�[�Z�b�g
	SetTimer(IDC_TIMER, 500, NULL);

	// �ʏ�\��
//	ShowWindow(SW_SHOWNORMAL);
	SetWindowPos(HWND_TOP, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME | SWP_SHOWWINDOW);

	return TRUE;
}

/* �I��������
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// ��n��
	DeleteDC(m_WorkDC);
	DeleteDC(m_BackDC);

	DeleteObject(m_bmp);
	DeleteObject(m_BackBmp);
	DeleteObject(m_font);

	RECT Rect;

	// ���݈ʒu�ƃT�C�Y���擾
	GetWindowRect((LPRECT)&Rect);

	// �ݒ��ۑ�
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

/* �^�C�}�[�C�x���g������
	wParam: �^�C�}�[ID
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(wParam == IDC_TIMER){
		SYSTEMTIME stTime;					// ���ݎ���
		static bool isTimeUpdated = false;	// �`�揈�������t���O

		// ���ݎ������擾
		GetSystemTime(&stTime);

		// 1 ���� 1 ����s
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

/* �`��C�x���g������
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	// ��ʂ̃f�U�C����ύX�����ꍇ�̃T�C�Y����
	int frame_width = 2*GetSystemMetrics(SM_CXFIXEDFRAME);

	int width;
	int height;

	// �V���v�����[�h��
	if(m_simplemode){
		width = m_bitmap_width;
		height = m_bitmap_height;
	}else{
		width = m_bitmap_width + frame_width;
		height = m_bitmap_height + GetSystemMetrics(SM_CYCAPTION) + frame_width;
	}

	// �T�C�Y��ݒ�
	SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);

	// �r�b�g�}�b�v�]��
	BitBlt(dc, 0, 0, m_bitmap_width, m_bitmap_height, m_WorkDC, 0, 0, SRCCOPY);

	return 0;
}

/* ���E���v�ɂ��āE���j���[�I����
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();

	return 0;
}

/* �c�[���`�b�v�E���j���[��Ԃ�ݒ�
	toggle: ��Ԃ��g�O��
*/
void CMainDlg::setToolTipState(bool toggle)
{
	MENUITEMINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;

	// ���j���[���擾
	if(m_PopupMenu.GetMenuItemInfo(IDM_TOOLTIP, FALSE, &menuInfo) == FALSE){
		return;
	}

	// �g�O��
	if(toggle){
		// �c�[���`�b�v�\����
		if(m_tooltip_flag){
			m_tooltip_flag = false;
			menuInfo.fState = MFS_UNCHECKED;	// �A���`�F�b�N
			m_ToolTip.DelTool(m_hWnd, 0);		// �c�[���`�b�v��\��
		}else{
			m_tooltip_flag = true;
			menuInfo.fState = MFS_CHECKED;		// �`�F�b�N
			CToolInfo ti(TTF_SUBCLASS, m_hWnd, 0, NULL, _T(" "));	// �c�[���`�b�v�\��
			m_ToolTip.AddTool(ti);
		}

	// ��Ԑݒ�̂�
	}else{
		if(m_tooltip_flag){
			menuInfo.fState = MFS_CHECKED;		// �`�F�b�N
			CToolInfo ti(TTF_SUBCLASS, m_hWnd, 0, NULL, _T(" "));	// �c�[���`�b�v�\��
			m_ToolTip.AddTool(ti);
		}else{
			menuInfo.fState = MFS_UNCHECKED;	// �A���`�F�b�N
			m_ToolTip.DelTool(m_hWnd, 0);		// �c�[���`�b�v��\��
		}
	}

	// ���j���[���ݒ�
	m_PopupMenu.SetMenuItemInfo(IDM_TOOLTIP, FALSE, &menuInfo);
}

/* �c�[���`�b�v��\���E���j���[�I����
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnToolTip(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	setToolTipState(true);
	return 0;
}

/* ��Ɏ�O�ɕ\���E���j���[��Ԃ�ݒ�
	toggle: ��Ԃ��g�O��
*/
void CMainDlg::setAlwaysTopState(bool toggle)
{
	MENUITEMINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;

	// ���j���[���擾
	if(m_PopupMenu.GetMenuItemInfo(IDM_ALWAYSTOP, FALSE, &menuInfo) == FALSE){
		return;
	}

	// �g�O��
	if(toggle){
		if(m_topmost_flag){
			m_topmost_flag = false;
			menuInfo.fState = MFS_UNCHECKED;	// �A���`�F�b�N
			SetWindowPos(HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
		}else{
			m_topmost_flag = true;
			menuInfo.fState = MFS_CHECKED;		// �`�F�b�N
			SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
		}
	// ��Ԑݒ�̂�
	}else{
		if(m_topmost_flag){
			menuInfo.fState = MFS_CHECKED;		// �`�F�b�N
			SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
		}else{
			menuInfo.fState = MFS_UNCHECKED;	// �A���`�F�b�N
			SetWindowPos(HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
		}
	}

	// ���j���[���ݒ�
	m_PopupMenu.SetMenuItemInfo(IDM_ALWAYSTOP, FALSE, &menuInfo);
}

/* ��Ɏ�O�ɕ\���E���j���[�I����
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnAlwaysTop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	setAlwaysTopState(true);
	return 0;
}

/* �ݒ�t�@�C�����J���E���j���[�I����
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnOpenInifile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR szIniFileName[MAX_PATHLEN];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(m_config->getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// INI �t�@�C�����J��
	ShellExecute(m_hWnd, _T("open"), szIniFileName, _T(""), _T(""), SW_SHOW);

	return 0;
}

/* �V���v�����[�h�ݒ�
	�߂�l: �Ȃ�
*/
void CMainDlg::setSimpleMode()
{
	if(m_simplemode){
		MENUITEMINFO menuInfo;
		menuInfo.cbSize = sizeof(MENUITEMINFO);
		menuInfo.fMask = MIIM_STRING;

		TCHAR szBuf[] = TEXT("�ʏ탂�[�h");
		menuInfo.dwTypeData = szBuf;

		// �R���e�L�X�g���j���[�ݒ�
		m_PopupMenu.SetMenuItemInfo(IDM_SIMPLEMODE, FALSE, &menuInfo);

		// �E�B���h�E�X�^�C���ݒ�
		LONG style = GetWindowLong(GWL_EXSTYLE);
		SetWindowLong(GWL_EXSTYLE, style | WS_EX_TOOLWINDOW);
		SetWindowLong(GWL_STYLE, WS_POPUP);
	}
}

/* �V���v�����[�h�E���j���[�I����
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnSimpleMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MENUITEMINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE | MIIM_STRING;
	menuInfo.fState = MFS_DISABLED;					// �I��s��

	if(m_simplemode){
		TCHAR szBuf[] = TEXT("�V���v�����[�h");
		menuInfo.dwTypeData = szBuf;
		m_config ->writeSimpleMode(false);
	}else{
		TCHAR szBuf[] = TEXT("�ʏ탂�[�h");
		menuInfo.dwTypeData = szBuf;
		m_config ->writeSimpleMode(true);
	}

	// ���j���[���ݒ�
	m_PopupMenu.SetMenuItemInfo(IDM_SIMPLEMODE, FALSE, &menuInfo);

	// ���b�Z�[�W�\��
	TCHAR szMsg[] = TEXT("�A�v���P�[�V�������ċN�����Ă�������");

	SetTextColor(m_WorkDC, RGB(255, 0, 0));
	TextOut(m_WorkDC, 70, 155, szMsg, _tcslen(szMsg));
	InvalidateRect(NULL, TRUE);

	return 0;
}

/* �N���C�A���g�̈���N���b�N���Ĉړ�
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnNCHITest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lr = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

	bHandled = false;

	// ���{�^���������ꂽ��
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

/* �}�E�X�ړ��C�x���g������
	LPARAM �}�E�X�ʒu
	�߂�l: LRESULT
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

	// ���݂� tick count
	ULONGLONG Count = GetTickCount();

	// �J��オ��΍�
	if(Count < LastCount){
		Count+=ULONG_MAX;
		Count++;
	}

	// ���ꂼ��̃^�C���]�[���̃c�[���`�b�v��\��
	for(int i=0; i<sizeof(m_datetime)/sizeof(m_datetime[0]); i++){
		if((x >= m_datetime[i].x) && (x <= m_datetime[i].x + 34) &&
			(y >= m_datetime[i].y) && (y <= m_datetime[i].y + 32)){

			// 0.5 �b�ȓ�
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

/* �L�[������(�f�o�b�O�p)
	WPARAM �L�[�R�[�h
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(wParam != VK_SPACE) return 0;

//ATLTRACE(_T("OnKeyDown\n"));

	return 0;
}

/* �E�N���b�N���R���e�L�X�g���j���[�\��
	HWND �E�B���h�E�n���h��
	CPoint �N���b�N�ʒu
	�߂�l: �Ȃ�
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

/* �`�揈��
	�߂�l: �Ȃ�
*/
void CMainDlg::draw()
{
	HDC dc = GetDC();
	datetime_t dtGMT;

	// ���[�L���O�R���e�L�X�g�č쐬
	if(m_WorkDC != NULL){
		DeleteDC(m_WorkDC);
		m_WorkDC = NULL;
		m_WorkDC = CreateCompatibleDC(dc);

		// �w�i�𓧉߂ɐݒ�
		SetBkMode(m_WorkDC , TRANSPARENT);
		SelectObject(m_WorkDC , m_font);
	}

	// �r�b�g�}�b�v�ǂݒ���
	if(m_bmp != NULL){
		DeleteObject(m_bmp);
		m_bmp = NULL;
		m_bmp = LoadBitmap(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_WORLDMAP));
	}

	// �r�b�g�}�b�v�đI��
	SelectObject(m_WorkDC, m_bmp);
	// ����ʏ�����
	PatBlt(m_BackDC, 0, 0, m_bitmap_width, m_bitmap_height, BLACKNESS);

	// GMT ���擾
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
			// �ܓx�����̍��W�ϊ�
			if(x>=210){
				lon = x-390.0;
			}else{
				lon = x-30.0;
			}
			// �o�x�����̍��W�ϊ�
			lat = -(10.0/11.0)*(double)y+90.0;

			double phi=D2R(lat);
			double MST=hour+lon/15.0;

			double TST = MST + ET;
			double t = D2R(15.0)*(TST-12.0);

			double thetaV=acos(sin(delta)*sin(phi)+cos(delta)*cos(phi)*cos(t));
			double ele = 90.0-180.0*thetaV/PI;

			// ���ԕ����̂�
			if(ele>0){
				SetPixel(m_BackDC, x, y, RGB(64, 64, 64));
			}
		} // for
	} // for

/*
	// �o�ܓx��(�`�F�b�N�p)
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

	// ���R���e�L�X�g���烏�[�L���O�R���e�L�X�g�ɓ]��
	BitBlt(m_WorkDC, 0, 0, m_bitmap_width, m_bitmap_height, m_BackDC, 0, 0, SRCINVERT);
	ReleaseDC(dc);

	// ���ݎ�����ݒ�
	setCurrentTime(dtGMT);

	// ������`��
	drawText();

	InvalidateRect(NULL, FALSE);
}

/* ���ݓ������Z�b�g����
	datetime_t GMT����
	�߂�l: �Ȃ�
*/
void CMainDlg::setCurrentTime(datetime_t dtGMT)
{
	datetime_t dt;
	dstinfo_t dstStart, dstEnd;

	// �e�G���A�̎������擾���ăZ�b�g
	for(int i=0; i<m_datetime_number; i++){

		// �Ď��ԏ����Z�b�g
		if(m_datetime[i].hasDST){

			// �Ď��ԊJ�n���
			dstStart.Month = m_datetime[i].iDSTStartMonth;
			dstStart.Type = m_datetime[i].szDSTStartType;
			dstStart.WeekDay = m_datetime[i].iDSTStartWeekDay;
			dstStart.Time = m_datetime[i].iDSTStartTime;

			// �Ď��ԏI�����
			dstEnd.Month = m_datetime[i].iDSTEndMonth;
			dstEnd.Type = m_datetime[i].szDSTEndType;
			dstEnd.WeekDay = m_datetime[i].iDSTEndWeekDay;
			dstEnd.Time = m_datetime[i].iDSTEndTime;
		}

		// ���E�����擾
		getWorldTime(&dt, &dtGMT, m_datetime[i].offset, m_datetime[i].hasDST, &dstStart, &dstEnd);

		wsprintf(m_datetime[i].szDate, _T("%02d/%02d"), dt.Month, dt.Day);
		wsprintf(m_datetime[i].szTime, _T("%02d:%02d"), dt.Hour, dt.Minute);
		m_datetime[i].isDST = dt.isDST;
	}
}

/* �^�C���]�[�����𕪊����Ď擾
	TCHAR* szNameIn �^�C���]�[���� (�`��: AEST/AEDT)
	TCHAR* szST �W�����^�C���]�[���� (�o��)
	TCHAR* szDT �Ď��ԃ^�C���]�[���� (�o��)
	�߂�l: OK ������ ERR �G���[��
*/
int CMainDlg::splitTZName(TCHAR* szNameIn, TCHAR* szST, TCHAR* szDT)
{
	TCHAR* pToken = NULL;
	size_t iLen = 0;

	// �ŏ��̃g�[�N��
	if((pToken = _tcsstr(szNameIn, _T("/"))) == NULL){
		return ERR;
	}

	iLen = _tcslen(szNameIn) - _tcslen(pToken);
	_tcsncpy(szST, szNameIn, iLen);
	*(szST + iLen) = '\0';

	// ���̃g�[�N��
	_tcscpy(szDT, pToken+1);

	return OK;
}

/* �e�L�X�g��`��
	�߂�l: �Ȃ�
*/
void CMainDlg::drawText()
{
	int iTZOffset;		// �^�C���]�[���I�t�Z�b�g����
	TCHAR szST[10];		// �W�����^�C���]�[�����i�[�p
	TCHAR szDT[10];		// �Ď��ԃ^�C���]�[�����i�[�p

	// ���ꂼ��̃^�C���]�[���̓�����\��
	for(int i=0; i<m_datetime_number; i++){
		// �^�C���]�[�����𕪊�
		if(splitTZName(m_datetime[i].szName, szST, szDT) != OK){
			_tcscpy(szST, m_datetime[i].szName);
			_tcscpy(szDT, m_datetime[i].szName);
		}

		// �Ď���
		if(m_datetime[i].isDST){

			// �^�C���]�[�����̔����Ȉʒu����
			iTZOffset = (lstrlen(szDT) == 3)?8:4;

			// �e�L�X�g�F�͉�
			SetTextColor(m_WorkDC, RGB(255, 255, 0));

			// �^�C���]�[�����\��
			TextOut(m_WorkDC, m_datetime[i].x+iTZOffset, m_datetime[i].y, szDT, lstrlen(szDT));
		}else{

			// �^�C���]�[�����̔����Ȉʒu����
			iTZOffset = (lstrlen(szST) == 3)?8:4;

			// �e�L�X�g�F�͔�
			SetTextColor(m_WorkDC, RGB(255, 255, 255));

			// �^�C���]�[�����\��
			TextOut(m_WorkDC, m_datetime[i].x+iTZOffset, m_datetime[i].y, szST, lstrlen(szST));
		}

		// ���t�Ǝ���
		TextOut(m_WorkDC, m_datetime[i].x, m_datetime[i].y+10, m_datetime[i].szDate, lstrlen(m_datetime[i].szDate));
		TextOut(m_WorkDC, m_datetime[i].x, m_datetime[i].y+20, m_datetime[i].szTime, lstrlen(m_datetime[i].szTime));
	}
}

/* Alpha 100%
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnAlpha100(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 100;
	setAlpha(100);
	return 0;
}

/* Alpha 80%
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnAlpha80(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 80;
	setAlpha(80);
	return 0;
}

/* Alpha 60%
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnAlpha60(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 60;
	setAlpha(60);
	return 0;
}

/* Alpha 40%
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnAlpha40(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 40;
	setAlpha(40);
	return 0;
}

/* Alpha 20%
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnAlpha20(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_alpha = 20;
	setAlpha(20);
	return 0;
}

/* ���ߐݒ�
	alpha �����x
*/
void CMainDlg::setAlpha(int alpha){

	MENUITEMINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;

	// ���j���[���`�F�b�N
	for(int i=0; i<5; i++){
		m_PopupMenu.GetMenuItemInfo(ID_ALPHA_20+i, FALSE, &menuInfo);

		if(alpha == i*20+20){
			menuInfo.fState = MFS_CHECKED;
		}else{
			menuInfo.fState = MFS_UNCHECKED;
		}

		m_PopupMenu.SetMenuItemInfo(ID_ALPHA_20+i, FALSE, &menuInfo);
	}

	// ���ߐݒ�
	LONG style = GetWindowLong(GWL_EXSTYLE);
	SetWindowLong(GWL_EXSTYLE, style | WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_hWnd, NULL, (int)((double)alpha*2.55), LWA_ALPHA);
}

/* ����{�^��������
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CloseDialog(IDOK);
	return 0;
}

/* �I�����j���[�I����
	�߂�l: LRESULT
*/
LRESULT CMainDlg::OnMenuExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(IDOK);
	return 0;
}

/* �_�C�A���O�����
	int �X�e�[�^�X�R�[�h
	�߂�l: �Ȃ�
*/
void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}
