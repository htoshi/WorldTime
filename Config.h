/*
 * �����ݒ�t�@�C���N���X
 * $Id$
 *
 * Copyright (C) 2011, Toshi All rights reserved.
*/
#pragma once

#include <windows.h>
#include <tchar.h>

// �����ݒ�t�@�C����
#define INIFILENAME _T("worldtime.ini")

// �E�B���h�E�̃f�t�H���g�ʒu
#define DEFAULT_WINDOW_TOP _T("10")
#define DEFAULT_WINDOW_LEFT _T("10")

// �G���A���ő�l
#define MAX_AREA 15

// �p�X�ő�l
#define MAX_PATHLEN 32768

#define OK 0
#define ERR -1

/* �������i�[�\���� */
typedef struct {
	TCHAR szDate[6];		// ���t (01/01)
	TCHAR szTime[6];		// ���� (12:00)
	TCHAR szName[10];		// �]�[����
	bool isDST;				// �Ď��ԃt���O (true: �Ď��� false: �W������)
	int x;					// �\���ʒu X ���W
	int y;					// �\���ʒu Y ���W
	double offset;			// GMT ����̃I�t�Z�b�g (����)
	TCHAR szInfo[128];		// �c�[���`�b�v�ɕ\��������
	bool hasDST;			// �Ď��ԕێ��t���O (true: �Ď��Ԃ��� false: �Ď��ԂȂ�)
	int iDSTStartMonth;		// �Ď��ԊJ�n��
	TCHAR szDSTStartType;	// �Ď��ԊJ�n�j���^�C�v(F:���T E:�ŏI�T)
	int iDSTStartWeekDay;	// �Ď��ԊJ�n�j�� (0:���j���`6:�y�j��)
	int iDSTStartTime;		// �Ď��ԊJ�n���� (��)
	int iDSTEndMonth;		// �Ď��ԏI����
	TCHAR szDSTEndType;		// �Ď��ԏI���j���^�C�v(F:���T E:�ŏI�T)
	int iDSTEndWeekDay;		// �Ď��ԏI���j�� (0:���j���`6:�y�j��)
	int iDSTEndTime;		// �Ď��ԏI������ (��)
} datetimeinfo_t;

// �������i�[�\����
typedef struct {
	short Year;
	short Month;
	short DayOfWeek;
	short Day;
	short Hour;
	short Minute;
	short Second;
	bool  isDST;
} datetime_t;

// �Ď��ԏ��\����
typedef struct {
	short Month;	// ��
	short Day;		// ��
	TCHAR Type;		// �j���^�C�v (F: ���T E: �ŏI�T 1�`5: ��N�T)
	short WeekDay;	// �j�� (0:���j���`6:�y�j��)
	short Time;		// ����(��)
} dstinfo_t;

/* �ݒ�t�@�C���N���X */
class Config {

	private:
		int parseDSTInfo(TCHAR*, int*, TCHAR*, int*, int*);

	public:
		Config();	// �R���X�g���N�^
		~Config();	// �f�X�g���N�^

		int getIniFileName(TCHAR*, DWORD);

		int readWindowRect(RECT*);	// Window�ʒu��ǂݍ���ŕԂ�
		int writeWindowRect(RECT*);	// Window�ʒu�������o��

		int readTopMostFlag(bool*);	// ��ɍőO�ʕ\�����ǂݏo��
		int writeTopMostFlag(bool);	// ��ɍőO�ʕ\����񏑂��o��

		int readToolTipFlag(bool*);	// �c�[���`�b�v�\�����ǂݏo��
		int writeToolTipFlag(bool);	// �c�[���`�b�v�\����񏑂��o��

		int readAlpha(int*);		// �����x�ǂݏo��
		int writeAlpha(int);		// �����x�����o��

		int readSimpleMode(bool*);	// �V���v�����[�h�ǂݏo��
		int writeSimpleMode(bool);	// �V���v�����[�h�����o��

		int readAreaInfo(datetimeinfo_t datetimeinfo[]);	// �G���A���擾
};
