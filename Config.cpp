/*
 * �����ݒ�t�@�C���N���X
 * $Id$
 *
 * Copyright (C) 2011, Toshi All rights reserved.
*/
#include "stdafx.h"		// for C1010 error
#include "Config.h"

// �R���X�g���N�^
Config::Config(){}

// �f�X�g���N�^
Config::~Config(){}

/* �ݒ�t�@�C�������t���p�X�ŕԂ�
	TCHAR* �ݒ�t�@�C����
	DWORD �ݒ�t�@�C�����o�b�t�@�̃T�C�Y
*/
int Config::getIniFileName(TCHAR* szFullPathName, DWORD nSize){

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[MAX_PATHLEN];

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

/* �ݒ�t�@�C����Window�ʒu��ǂݍ���ŕԂ�(Right��Bottom�͖���)
	LPRECT rect �E�B���h�E�̍��W�l
	�߂�l OK ����
*/
int Config::readWindowRect(RECT* rect){

	TCHAR szIniFileName[MAX_PATHLEN];
	TCHAR szBuf[100];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// Top �ʒu�̎擾
	if(GetPrivateProfileString(
		_T("General"), _T("Top"), DEFAULT_WINDOW_TOP,
		szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	rect->top = (_tstol(szBuf) == 0)?_tstol(DEFAULT_WINDOW_TOP):_tstol(szBuf);

	// Left �ʒu�̎擾
	if(GetPrivateProfileString(
	_T("General"), _T("Left"), DEFAULT_WINDOW_LEFT,
	szBuf, sizeof(szBuf), szIniFileName) == 0)
	return ERR;

	rect->left = (_tstol(szBuf) == 0)?_tstol(DEFAULT_WINDOW_LEFT):_tstol(szBuf);

	return OK;
}

/* ���݂�Window�ʒu��ݒ�t�@�C���ɏ����o��(Right��Bottom�͖���)
	LPRECT rect �E�B���h�E�̍��W�l
	�߂�l OK ����
*/
int Config::writeWindowRect(RECT* rect){

	TCHAR szIniFileName[MAX_PATHLEN];
	TCHAR szBuf[100];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// Top �ʒu�̕ۑ�
	wsprintf(szBuf, _T("%d"), rect->top);

	if(!WritePrivateProfileString(
		_T("General"), _T("Top"), szBuf, szIniFileName))
		return ERR;

	// Left �ʒu�̕ۑ�
	wsprintf(szBuf, _T("%d"), rect->left);

	if(!WritePrivateProfileString(
		_T("General"), _T("Left"), szBuf, szIniFileName))
		return ERR;

	return OK;
}

/* ��ɍőO�ʕ\�����ǂݏo��
    �߂�l OK ����
*/
int Config::readTopMostFlag(bool* isTop){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[5];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// ��ɍőO�ʕ\�����ǂݏo�� (1: ��ɍőO��)
	if(GetPrivateProfileString(
		_T("General"), _T("AlwaysTop"), _T("0"),
			szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	*isTop = (_tstoi(szBuf) == 1)?true:false;

	return OK;
}

/* ��ɍőO�ʕ\����񏑂��o��
    �߂�l OK ����
*/
int Config::writeTopMostFlag(bool isTop){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[10];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
	return ERR;

	// �őO�ʏ�Ԃ̕ۑ�
	wsprintf(szBuf, _T("%d"), isTop?1:0);

	if(!WritePrivateProfileString(
		_T("General"), _T("AlwaysTop"), szBuf, szIniFileName))
		return ERR;

	return OK;
}

/* �c�[���`�b�v�\�����ǂݏo��
    bool* isShow
    �߂�l OK ����
*/
int Config::readToolTipFlag(bool* isShow){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[5];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// �c�[���`�b�v�\�����ǂݏo�� (1: �\��)
	if(GetPrivateProfileString(
		_T("General"), _T("ToolTip"), _T("0"),
		szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	*isShow = (_tstoi(szBuf) == 1)?true:false;

	return OK;
}

/* �c�[���`�b�v�\����񏑂��o��
    bool isShow
    �߂�l OK ����
*/
int Config::writeToolTipFlag(bool isShow){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[10];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// �c�[���`�b�v�\�����ۑ�
	wsprintf(szBuf, _T("%d"), isShow?1:0);

	if(!WritePrivateProfileString(
		_T("General"), _T("ToolTip"), szBuf, szIniFileName))
		return ERR;

	return OK;
}

/* �����x�ǂݏo��
    int* alpha
    �߂�l OK ����
*/
int Config::readAlpha(int* alpha){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[5];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// �����x�ǂݏo��
	if(GetPrivateProfileString(
		_T("General"), _T("Alpha"), _T("0"),
		szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	*alpha = _tstoi(szBuf);

	// �ŏ��l 20 �ő�l 100
	if(*alpha < 20 || *alpha > 100){
		*alpha = 100;
	}

	return OK;
}

/* �����x�����o��
    int alpha
    �߂�l OK ����
*/
int Config::writeAlpha(int alpha){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[10];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// �����x�ۑ�
	wsprintf(szBuf, _T("%d"), alpha);

	if(!WritePrivateProfileString(
		_T("General"), _T("Alpha"), szBuf, szIniFileName))
		return ERR;

	return OK;
}


/* �V���v�����[�h�ǂݏo��
    �߂�l OK ����
*/
int Config::readSimpleMode(bool* isTop){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[5];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// �V���v�����[�h�ǂݏo�� (1: �V���v�����[�h)
	if(GetPrivateProfileString(
		_T("General"), _T("SimpleMode"), _T("0"),
			szBuf, sizeof(szBuf), szIniFileName) == 0)
		return ERR;

	*isTop = (_tstoi(szBuf) == 1)?true:false;

	return OK;
}

/* �V���v�����[�h�����o��
    �߂�l OK ����
*/
int Config::writeSimpleMode(bool isTop){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[10];

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
	return ERR;

	// �V���v�����[�h�̕ۑ�
	wsprintf(szBuf, _T("%d"), isTop?1:0);

	if(!WritePrivateProfileString(
		_T("General"), _T("SimpleMode"), szBuf, szIniFileName))
		return ERR;

	return OK;
}

/* �G���A���ǂݏo��
    datetimeinfo_t dtInfo[] �������i�[�\����
    �߂�l �ǂݍ��񂾃G���A���
*/
int Config::readAreaInfo(datetimeinfo_t dtInfo[]){

	TCHAR szIniFileName[_MAX_PATH];
	TCHAR szBuf[128];

	TCHAR szSectionName[16];	// �Z�N�V������
	int iCount = 0;				// �J�E���^�[

	// INI �t�@�C�������t���p�X�Ŏ擾
	if(getIniFileName(szIniFileName, sizeof(szIniFileName)) != OK)
		return ERR;

	// �G���A���ǂݏo��
	for(int i=0; i<MAX_AREA; i++){

		// �Z�N�V�������쐬
		wsprintf(szSectionName, _T("Area%d"), i);

		// �G���A���擾
		if(GetPrivateProfileString(
			szSectionName, _T("Name"), _T(""),
			szBuf, sizeof(dtInfo[0].szName), szIniFileName) == 0)
			continue;

		lstrcpy(dtInfo[iCount].szName, szBuf);

		// X ���W�ʒu�擾
		if(GetPrivateProfileString(
			szSectionName, _T("X"), _T("0"), szBuf, 5, szIniFileName) == 0)
			continue;

		dtInfo[iCount].x = _tstoi(szBuf);

		// Y ���W�ʒu�擾
		if(GetPrivateProfileString(
			szSectionName, _T("Y"), _T("0"), szBuf, 5, szIniFileName) == 0)
			continue;

		dtInfo[iCount].y = _tstoi(szBuf);

		// �I�t�Z�b�g�擾 (�K�{)
		if(GetPrivateProfileString(
			szSectionName, _T("Offset"), _T("0"), szBuf, 6, szIniFileName) == 0)
			return ERR;

		dtInfo[iCount].offset = _tstof(szBuf);

		// �I�t�Z�b�g�l�`�F�b�N
		if(dtInfo[iCount].offset < -12 || dtInfo[iCount].offset > 12)
			return ERR;

		// �c�[���`�b�v���擾
		if(GetPrivateProfileString(
			szSectionName, _T("Info"), _T(""),
			szBuf, sizeof(dtInfo[0].szInfo), szIniFileName) == 0)
			continue;

		lstrcpy(dtInfo[iCount].szInfo, szBuf);

		// �Ď��ԊJ�n���擾
		if(GetPrivateProfileString(
			szSectionName, _T("DSTStart"), _T(""), szBuf, 12, szIniFileName) != 0){

				// �Ď��ԏ����G���A���Ɋi�[
				if(parseDSTInfo(szBuf, &dtInfo[iCount].iDSTStartMonth, &dtInfo[iCount].szDSTStartType,
					&dtInfo[iCount].iDSTStartWeekDay, &dtInfo[iCount].iDSTStartTime) != OK)
					return ERR;

				// �Ď��ԏI�����擾
				if(GetPrivateProfileString(
					szSectionName, _T("DSTEnd"), _T(""), szBuf, 12, szIniFileName) != 0){

					// �Ď��ԏ����G���A���Ɋi�[
					if(parseDSTInfo(szBuf, &dtInfo[iCount].iDSTEndMonth, &dtInfo[iCount].szDSTEndType,
						&dtInfo[iCount].iDSTEndWeekDay, &dtInfo[iCount].iDSTEndTime) != OK)
						return ERR;

					dtInfo[iCount].hasDST = true;
				} // if

		}else{
			// �Ď��ԏ��Ȃ�
			dtInfo[iCount].hasDST = false;
		} // if

		iCount++;
	} // for

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
int Config::parseDSTInfo(TCHAR* szBuf, int* iMonth, TCHAR* szType, int* iWeek, int* iTime){

	TCHAR* pToken = NULL;

	int iTmp = 0;

	// ���̉��
	if((pToken = _tcstok(szBuf, _T("/"))) == NULL)
		return ERR;

	iTmp = _tstoi(pToken);

	// �͈̓`�F�b�N
	if(iTmp <= 0 || iTmp > 12)
		return ERR;

	*iMonth = iTmp;

	// �^�C�v�̉��
	if((pToken = _tcstok(NULL, _T("/"))) == NULL)
		return ERR;

	// �l�`�F�b�N (E �ł� F �ł��Ȃ� 1�`5 �ȊO�͈̔͂��w��)
	if((_tcscmp(pToken, _T("E")) != 0) && (_tcscmp(pToken, _T("F")) != 0) &&
		(_tstoi(pToken) < 1 || _tstoi(pToken) > 5))
	return ERR;

	_tcscpy(szType, pToken);

	// �j���̉��
	if((pToken = _tcstok(NULL, _T("/"))) == NULL)
		return ERR;

	iTmp = _tstoi(pToken);

	// �͈̓`�F�b�N
		if(iTmp < 0 || iTmp > 6)
			return ERR;

	*iWeek = iTmp;

	// �����̉��
	if((pToken = _tcstok(NULL, _T("/"))) == NULL)
		return ERR;

	// ���̉��
	if((pToken = _tcstok(pToken, _T(":"))) == NULL)
		return ERR;

	iTmp = _tstoi(pToken);

	// ���͈̔̓`�F�b�N
	if(iTmp < 0 || iTmp > 23)
		return ERR;

	*iTime = iTmp*60;

	// ���̉��
	if((pToken = _tcstok(NULL, _T(":"))) == NULL)
		return ERR;

	iTmp = _tstoi(pToken);

	// ���͈̔̓`�F�b�N
	if(iTmp < 0 || iTmp > 59)
		return ERR;

	*iTime = *iTime + iTmp;

	return OK;
}
