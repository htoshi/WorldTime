/*
 * �����ݒ�t�@�C���N���X
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
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

// �G���A���ő�l
#define MAX_AREA 10

#define OK 0
#define ERR -1

//#define __DEBUG__ 1

/* �������i�[�\���� */
typedef struct {
    TCHAR szDate[6];        // ���t (01/01)
    TCHAR szTime[6];        // ���� (12:00)
    TCHAR szName[5];        // �]�[����
    bool isDST;             // �Ď��ԃt���O (true: �Ď��� false: �W������)
    int x;                  // �\���ʒu X ���W
    int y;                  // �\���ʒu Y ���W
    double offset;          // GMT ����̃I�t�Z�b�g (����)
    TCHAR szInfo[128];      // �c�[���`�b�v�ɕ\��������
    bool hasDST;            // �Ď��ԕێ��t���O (true: �Ď��Ԃ��� false: �Ď��ԂȂ�)
    int iDSTStartMonth;     // �Ď��ԊJ�n��
    TCHAR szDSTStartType;   // �Ď��ԊJ�n�j���^�C�v(F:���T E:�ŏI�T)
    int iDSTStartWeekDay;   // �Ď��ԊJ�n�j�� (0:���j���`6:�y�j��)
    int iDSTStartTime;      // �Ď��ԊJ�n���� (��)
    int iDSTEndMonth;       // �Ď��ԏI����
    TCHAR szDSTEndType;     // �Ď��ԏI���j���^�C�v(F:���T E:�ŏI�T)
    int iDSTEndWeekDay;     // �Ď��ԏI���j�� (0:���j���`6:�y�j��)
    int iDSTEndTime;        // �Ď��ԏI������ (��)
} datetimeinfo_t;

/* ���E�����ݒ�t�@�C���N���X */
class WorldTimeConfig {

    private:
        int getIniFileName(TCHAR*, DWORD);

#ifdef __DEBUG__
        void debug(TCHAR* szBuf);
#endif

        int parseDSTInfo(TCHAR*, int*, TCHAR*, int*, int*);

    public:

        WorldTimeConfig();      // �R���X�g���N�^
        ~WorldTimeConfig();     // �f�X�g���N�^

        int readWindowRect(RECT*);   // Window�ʒu��ǂݍ���ŕԂ�
        int writeWindowRect(RECT*);  // Window�ʒu�������o��

        int readTopMostFlag(bool*);  // ��ɍőO�ʕ\�����ǂݏo��
        int writeTopMostFlag(bool);  // ��ɍőO�ʕ\����񏑂��o��

        int readToolTipFlag(bool*);  // �c�[���`�b�v�\�����ǂݏo��
        int writeToolTipFlag(bool);  // �c�[���`�b�v�\����񏑂��o��

        int readAreaInfo(datetimeinfo_t datetimeinfo[]);  // �G���A���擾
};

#endif /* __WORLDTIMECONFIG_H__ */
