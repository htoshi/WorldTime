/*
 * �����ݒ�t�@�C���N���X
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#include "WorldTimeConfig.h"

// �R���X�g���N�^
WorldTimeConfig::WorldTimeConfig(){}

// �f�X�g���N�^
WorldTimeConfig::~WorldTimeConfig(){}

/* �ݒ�t�@�C�������t���p�X�ŕԂ�
    TCHAR* �ݒ�t�@�C����
    DWORD �ݒ�t�@�C�����o�b�t�@�̃T�C�Y
*/
int WorldTimeConfig::getIniFileName(TCHAR* szFullPathName, DWORD nSize){

    TCHAR szDrive[_MAX_DRIVE];
    TCHAR szDir[_MAX_DIR];

    // ���s�t�@�C���̃t���p�X�ƃt�@�C�������擾
    if(!GetModuleFileName(NULL, szFullPathName, nSize))
        return ERR;

    // �t���p�X�𕪉�
    _tsplitpath(szFullPathName, szDrive, szDir, NULL, NULL);

    // szFullPathName �� INI �t�@�C�������t���p�X�Ŋi�[
    _tcscpy(szFullPathName, szDrive);
    _tcscat(szFullPathName, szDir);
    _tcscat(szFullPathName, INIFILENAME);

    return OK;
}

/* �ݒ�t�@�C����Window�ʒu��ǂݍ���ŕԂ�
    LPRECT rect �E�B���h�E�̍��W�l
    �߂�l OK ����
*/
int WorldTimeConfig::readWindowRect(RECT* rect){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[100];

    // INI �t�@�C�������t���p�X�Ŏ擾
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // Top �ʒu�̎擾
    if(GetPrivateProfileString(
         TEXT("General"), TEXT("Top"), DEFAULT_WINDOW_TOP,
         szBuf, sizeof(szBuf), szIniFileName) == 0)
        return ERR;

    rect->top = (_tstoi(szBuf) == 0)?(int)DEFAULT_WINDOW_TOP:_tstoi(szBuf);

    // Left �ʒu�̎擾
    if(GetPrivateProfileString(
         TEXT("General"), TEXT("Left"), DEFAULT_WINDOW_LEFT,
         szBuf, sizeof(szBuf), szIniFileName) == 0)
        return ERR;

    rect->left = (_tstoi(szBuf) == 0)?(int)DEFAULT_WINDOW_LEFT:_tstoi(szBuf);

    return OK;
}

/* ���݂�Window�ʒu��ݒ�t�@�C���ɏ����o��
    LPRECT rect �E�B���h�E�̍��W�l
    �߂�l OK ����
*/
int WorldTimeConfig::writeWindowRect(RECT* rect){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[100];

    // INI �t�@�C�������t���p�X�Ŏ擾
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // Top �ʒu�̕ۑ�
    wsprintf(szBuf, "%d", rect->top);

    if(!WritePrivateProfileString(
         TEXT("General"), TEXT("Top"), szBuf, szIniFileName))
        return ERR;

    // Left �ʒu�̕ۑ�
    wsprintf(szBuf, "%d", rect->left);

    if(!WritePrivateProfileString(
         TEXT("General"), TEXT("Left"), szBuf, szIniFileName))
        return ERR;

    return OK;
}

/* ��ɍőO�ʕ\�����ǂݏo��
    �߂�l OK ����
*/
int WorldTimeConfig::readTopMostFlag(bool* isTop){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[5];

    // INI �t�@�C�������t���p�X�Ŏ擾
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // ��ɍőO�ʕ\�����ǂݏo�� (1: ��ɍőO��)
    if(GetPrivateProfileString(
         TEXT("General"), TEXT("AlwaysTop"), TEXT("0"),
         szBuf, sizeof(szBuf), szIniFileName) == 0)
        return ERR;

    *isTop = (_tstoi(szBuf) == 1)?true:false;

    return OK;
}

/* ��ɍőO�ʕ\����񏑂��o��
    �߂�l OK ����
*/
int WorldTimeConfig::writeTopMostFlag(bool isTop){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[10];

    // INI �t�@�C�������t���p�X�Ŏ擾
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // �őO�ʏ�Ԃ̕ۑ�
    wsprintf(szBuf, "%d", isTop?1:0);

    if(!WritePrivateProfileString(
         TEXT("General"), TEXT("AlwaysTop"), szBuf, szIniFileName))
        return ERR;

    return OK;
}

/* �c�[���`�b�v�\�����ǂݏo��
    bool* isShow
    �߂�l OK ����
*/
int WorldTimeConfig::readToolTipFlag(bool* isShow){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[5];

    // INI �t�@�C�������t���p�X�Ŏ擾
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // �c�[���`�b�v�\�����ǂݏo�� (1: �\��)
    if(GetPrivateProfileString(
         TEXT("General"), TEXT("ToolTip"), TEXT("0"),
         szBuf, sizeof(szBuf), szIniFileName) == 0)
        return ERR;

    *isShow = (_tstoi(szBuf) == 1)?true:false;

    return OK;
}

/* �c�[���`�b�v�\����񏑂��o��
    bool isShow
    �߂�l OK ����
*/
int WorldTimeConfig::writeToolTipFlag(bool isShow){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[10];

    // INI �t�@�C�������t���p�X�Ŏ擾
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // �c�[���`�b�v�\�����ۑ�
    wsprintf(szBuf, "%d", isShow?1:0);

    if(!WritePrivateProfileString(
         TEXT("General"), TEXT("ToolTip"), szBuf, szIniFileName))
        return ERR;

    return OK;
}

/* �f�o�b�O���O
   char* szBuf �f�o�b�O���b�Z�[�W
   �߂�l: �Ȃ�
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
        MessageBox(0, TEXT("debug() �q�[�v�m�ۃG���["),
                      TEXT("�G���["), MB_ICONSTOP);
        return;
    }

    GetLocalTime(&stTime);

    wsprintf(szLogMsg, "%04d/%02d/%02d %02d:%02d:%02d %s\r\n",
                stTime.wYear, stTime.wMonth, stTime.wDay,
                stTime.wHour, stTime.wMinute, stTime.wSecond, szBuf);

    // �t�@�C���쐬
    hFile = CreateFile("debug.log", GENERIC_WRITE,
                        FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE){
        MessageBox(0, TEXT("�t�@�C���I�[�v���G���["),
                      TEXT("�G���["), MB_ICONSTOP);
        goto lblHeapFree;
    }

    // �t�@�C���ŏI�[�Ƀ|�C���^���ړ�
    SetFilePointer(hFile , 0 , NULL , FILE_END);

    // ��������
    bRet = WriteFile(hFile, szLogMsg, (DWORD)strlen(szLogMsg), &dwWritten, NULL);

    CloseHandle(hFile);

lblHeapFree:
    HeapFree(GetProcessHeap(), 0, szLogMsg);
}

#endif /* __DEBUG__ */
