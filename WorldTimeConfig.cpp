/*
 * �����ݒ�t�@�C���N���X
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
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

/* �G���A���ǂݏo��
    datetimeinfo_t dtInfo[] �������i�[�\����
    �߂�l �ǂݍ��񂾃G���A���
*/
int WorldTimeConfig::readAreaInfo(datetimeinfo_t dtInfo[]){

    TCHAR szIniFileName[_MAX_PATH];
    TCHAR szBuf[128];

    TCHAR szSectionName[6];     // �Z�N�V������
    int iCount = 0;             // �J�E���^�[

    // INI �t�@�C�������t���p�X�Ŏ擾
    if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
        return ERR;

    // �G���A���ǂݏo��
    for(int i=0; i<MAX_AREA; i++){

        // �Z�N�V�������쐬
        wsprintf(szSectionName, "Area%d", i);

        // �G���A���擾
        if(GetPrivateProfileString(
             szSectionName, TEXT("Name"), "",
             szBuf, sizeof(dtInfo[0].szName), szIniFileName) == 0)
            continue;

        lstrcpy(dtInfo[iCount].szName, szBuf);

        // X ���W�ʒu�擾
        if(GetPrivateProfileString(
             szSectionName, TEXT("X"), "0", szBuf, 5, szIniFileName) == 0)
            continue;

        dtInfo[iCount].x = _tstoi(szBuf);

        // Y ���W�ʒu�擾
        if(GetPrivateProfileString(
             szSectionName, TEXT("Y"), "0", szBuf, 5, szIniFileName) == 0)
            continue;

        dtInfo[iCount].y = _tstoi(szBuf);

        // �I�t�Z�b�g�擾 (�K�{)
        if(GetPrivateProfileString(
             szSectionName, TEXT("Offset"), "0", szBuf, 6, szIniFileName) == 0)
            return ERR;

        dtInfo[iCount].offset = _tstof(szBuf);

        // �I�t�Z�b�g�l�`�F�b�N
        if(dtInfo[iCount].offset < -12 || dtInfo[iCount].offset > 12)
            return ERR;

        // �c�[���`�b�v���擾
        if(GetPrivateProfileString(
             szSectionName, TEXT("Info"), "",
             szBuf, sizeof(dtInfo[0].szInfo), szIniFileName) == 0)
            continue;

        lstrcpy(dtInfo[iCount].szInfo, szBuf);

        // �Ď��ԊJ�n���擾
        if(GetPrivateProfileString(
            szSectionName, TEXT("DSTStart"), "", szBuf, 12, szIniFileName) != 0){

            // �Ď��ԏ����G���A���Ɋi�[
            if(parseDSTInfo(szBuf, &dtInfo[iCount].iDSTStartMonth, &dtInfo[iCount].szDSTStartType,
                            &dtInfo[iCount].iDSTStartWeekDay, &dtInfo[iCount].iDSTStartTime) != OK)
                return ERR;

            // �Ď��ԏI�����擾
            if(GetPrivateProfileString(
                szSectionName, TEXT("DSTEnd"), "", szBuf, 12, szIniFileName) != 0){

                // �Ď��ԏ����G���A���Ɋi�[
                if(parseDSTInfo(szBuf, &dtInfo[iCount].iDSTEndMonth, &dtInfo[iCount].szDSTEndType,
                            &dtInfo[iCount].iDSTEndWeekDay, &dtInfo[iCount].iDSTEndTime) != OK)
                    return ERR;

                dtInfo[iCount].hasDST = true;
            }

        }else{
            // �Ď��ԏ��Ȃ�
            dtInfo[iCount].hasDST = false;
        }

        iCount++;
    }

    return iCount;
}

/* �Ď��ԏ��
   TCHAR* szBuf ���� (�`��: 3/E/0/02:10 ��/�j���^�C�v/�j��/����)
   int* iMonth ��
   TCHAR* szType �j���^�C�v
   int* iWeek �j��
   int* iTime ����
   �߂�l: OK ������ ERR �G���[��
*/
int WorldTimeConfig::parseDSTInfo(TCHAR* szBuf, int* iMonth, TCHAR* szType, int* iWeek, int* iTime){

    TCHAR* pToken = NULL;

    int iTmp = 0;

    // ���̉��
    if((pToken = _tcstok(szBuf, "/")) == NULL)
        return ERR;

    iTmp = _tstoi(pToken);

    // �͈̓`�F�b�N
    if(iTmp <= 0 || iTmp > 12)
        return ERR;

    *iMonth = iTmp;

    // �^�C�v�̉��
    if((pToken = _tcstok(NULL, "/")) == NULL)
        return ERR;

    // �l�`�F�b�N (E �ł� F �ł��Ȃ� 1�`5 �ȊO�͈̔͂��w��)
    if((_tcscmp(pToken, "E") != 0) && (_tcscmp(pToken, "F") != 0) &&
	   (_tstoi(pToken) < 1 || _tstoi(pToken) > 5))
        return ERR;

    _tcscpy(szType, pToken);

    // �j���̉��
    if((pToken = _tcstok(NULL, "/")) == NULL)
        return ERR;

    iTmp = _tstoi(pToken);

    // �͈̓`�F�b�N
    if(iTmp < 0 || iTmp > 6)
        return ERR;

    *iWeek = iTmp;

    // �����̉��
    if((pToken = _tcstok(NULL, "/")) == NULL)
        return ERR;

    // ���̉��
    if((pToken = _tcstok(pToken, ":")) == NULL)
        return ERR;

    iTmp = _tstoi(pToken);

    // ���͈̔̓`�F�b�N
    if(iTmp < 0 || iTmp > 23)
        return ERR;

    *iTime = iTmp*60;

    // ���̉��
    if((pToken = _tcstok(NULL, ":")) == NULL)
        return ERR;

    iTmp = _tstoi(pToken);

    // ���͈̔̓`�F�b�N
    if(iTmp < 0 || iTmp > 59)
        return ERR;

    *iTime = *iTime + iTmp;

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

    TCHAR szLogMsg[1024];

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
    }

    // �t�@�C���ŏI�[�Ƀ|�C���^���ړ�
    SetFilePointer(hFile , 0 , NULL , FILE_END);

    // ��������
    bRet = WriteFile(hFile, szLogMsg, (DWORD)strlen(szLogMsg), &dwWritten, NULL);

    CloseHandle(hFile);
}

#endif /* __DEBUG__ */
