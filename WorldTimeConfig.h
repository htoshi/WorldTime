/*
 * �����ݒ�t�@�C���N���X
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#ifndef __WORLDTIMECONFIG_H__
#define __WORLDTIMECONFIG_H__

#include <windows.h>
#include <tchar.h>

// �����ݒ�t�@�C����
#define INIFILENAME "worldtime.ini"

// �E�B���h�E�̃f�t�H���g�ʒu
#define DEFAULT_WINDOW_TOP "10"
#define DEFAULT_WINDOW_LEFT "10"

#define OK 0
#define ERR -1

// #define __DEBUG__ 1

/* ���E�����ݒ�t�@�C���N���X */
class WorldTimeConfig {

    private:
        int getIniFileName(TCHAR*, DWORD);

#ifdef __DEBUG__
        void debug(TCHAR* szBuf);
#endif

    public:

        WorldTimeConfig();      // �R���X�g���N�^
        ~WorldTimeConfig();     // �f�X�g���N�^

        int readWindowRect(RECT*);   // Window�ʒu��ǂݍ���ŕԂ�
        int writeWindowRect(RECT*);  // Window�ʒu�������o��

        int readTopMostFlag(bool*);  // ��ɍőO�ʕ\�����ǂݏo��
        int writeTopMostFlag(bool);  // ��ɍőO�ʕ\����񏑂��o��

        int readToolTipFlag(bool*);  // �c�[���`�b�v�\�����ǂݏo��
        int writeToolTipFlag(bool);  // �c�[���`�b�v�\����񏑂��o��
};

#endif /* __WORLDTIMECONFIG_H__ */
