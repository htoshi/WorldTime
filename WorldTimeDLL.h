/*
 * ���E�̎��Ԃ��擾
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#ifndef __WORLDTIMEDLL_H__
#define __WORLDTIMEDLL_H__

#include <windows.h>

// DLL �p�錾(�R���p�C���I�v�V������ /D "_EXPORTING" ���w��)
#ifdef _EXPORTING
#define _EXPORT extern "C" __declspec(dllexport)
#else
#define _EXPORT extern "C" __declspec(dllimport)
#endif

#define OK 0
#define ERR -1

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

/* DLL �����֐��̃v���g�^�C�v�錾 */
#ifdef _EXPORTING
int zeller(int iYear, int iMonth, int iDay);
int getDayOfMonth(int iYear, int iMonth);
int getMonthFirstSunday(int iYear, int iMonth);
int getMonthLastSunday(int iYear, int iMonth);
bool isDataInSpecifiedRange(int iMonth, int iDay,
        int iFromMonth, int iFromDay, int iToMonth,
        int iToDay, bool isInclude);
int getDateTimeByOffset(datetime_t* t, int iOffsetMin);
int getTotalMinutesInDay(int iHour, int iMin);
#endif /* _EXPORTING */

/* Export �֐� */
// ���݂̃O���j�b�W�W����
_EXPORT int getCurrentGMT(datetime_t* dt);
// ���{�W����
_EXPORT int getJST(datetime_t* dt, datetime_t* dt_in);
// ���B�����W����
_EXPORT int getAEST(datetime_t* dt, datetime_t* dt_in);
// ���B�����W����
_EXPORT int getAWST(datetime_t* dt, datetime_t* dt_in);
// �����m�W����
_EXPORT int getPST(datetime_t* dt, datetime_t* dt_in);
// �����W����
_EXPORT int getEST(datetime_t* dt, datetime_t* dt_in);
// �������[���b�p�W����(�h�C�c)
_EXPORT int getCET(datetime_t* dt, datetime_t* dt_in);
// ���X�N���W����
_EXPORT int getMSK(datetime_t* dt, datetime_t* dt_in);

#endif /* __WORLDTIMEDLL_H__ */
