/*
 * ���E�̎��Ԃ��擾
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#ifndef __WORLDTIMECALC_H__
#define __WORLDTIMECALC_H__

#include <windows.h>

// DLL �p�錾
#define DllExport __declspec(dllexport)

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

/* ���E�����v�Z�N���X */
class WorldTimeCalc {
    private:
        int zeller(int iYear, int iMonth, int iDay);
        int getDayOfMonth(int iYear, int iMonth);
        int getMonthFirstSunday(int iYear, int iMonth);
        int getMonthLastSunday(int iYear, int iMonth);
        bool isDataInSpecifiedRange(int iMonth, int iDay,
                int iFromMonth, int iFromDay, int iToMonth,
                int iToDay, bool isInclude);
        int getDateTimeByOffset(datetime_t* t, int iOffsetMin);
        int getTotalMinutesInDay(int iHour, int iMin);

    public:

        WorldTimeCalc();      // �R���X�g���N�^
        ~WorldTimeCalc();     // �f�X�g���N�^

        // ���݂̃O���j�b�W�W����
        int getCurrentGMT(datetime_t* dt);
        // ���{�W����
        int getJST(datetime_t* dt, datetime_t* dt_in);
        // ���B�����W����
        int getAEST(datetime_t* dt, datetime_t* dt_in);
        // ���B�����W����
        int getAWST(datetime_t* dt, datetime_t* dt_in);
        // �����m�W����
        int getPST(datetime_t* dt, datetime_t* dt_in);
        // �����W����
        int getEST(datetime_t* dt, datetime_t* dt_in);
        // �������[���b�p�W����(�h�C�c)
        int getCET(datetime_t* dt, datetime_t* dt_in);
        // ���X�N���W����
        int getMSK(datetime_t* dt, datetime_t* dt_in);

};

#endif /* __WORLDTIMECALC_H__ */
