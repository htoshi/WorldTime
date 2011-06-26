/*
 * ���E���v���C��
 * $Id$
 *
 * Copyright (C) 2011, Toshi All rights reserved.
*/
#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "Config.h"

#define WINDOW_CLASSNAME _T("C9132D07-D564-4506-99AB-AE118B6FBB77")

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("���C���_�C�A���O�̍쐬�Ɏ��s���܂���!\n"));
		return 0;
	}

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	// CDialogImpl �� ClassName ������
	// http://support.microsoft.com/kb/251059/ja
	// ���\�[�X�t�@�C���� CLASS "xxxxx" ��ǉ����Ĉȉ��̃R�[�h��ǉ�
	{
		WNDCLASS wc;

		// Get the info for this class.
		// #32770 is the default class name for dialogs boxes.
		::GetClassInfo(_Module.m_hInst, _T("#32770"), &wc);

		// Change the name of the class.
		wc.lpszClassName = WINDOW_CLASSNAME;

		// Register this class
		::RegisterClass(&wc);
	}

	// ���d�N���h�~
	HWND hWnd = FindWindow(WINDOW_CLASSNAME, NULL);

	if(hWnd != NULL){
		// �A�C�R��������Ă���Ό��̃T�C�Y�ɖ߂�
		if(IsIconic(hWnd)){
			ShowWindow(hWnd, SW_RESTORE);
		}

		// ���������E�B���h�E���t�H�A�O���E���h�ɂ���
		SetForegroundWindow(::GetLastActivePopup(hWnd));

		return -1;
	}

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
