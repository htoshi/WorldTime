/*
 * ���E�̎��Ԃ��擾
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
*/
#ifndef __WORLDTIMECALC_H__
#define __WORLDTIMECALC_H__

#include <windows.h>

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

// �Ď��ԏ��\����
typedef struct {
    short Month;    // ��
    short Day;      // ��
    char Type;      // �j���^�C�v (F: ���T E: �ŏI�T)
    short WeekDay;  // �j�� (0:���j���`6:�y�j��)
    short Time;     // ����(��)
} dstinfo_t;

/* ���E�����v�Z�N���X */
class WorldTimeCalc {
    private:
        int zeller(int, int, int);
        int getDayOfMonth(int, int);
        bool isDataInSpecifiedRange(int, int, int, int, int, int, bool);
        int getDateTimeByOffset(datetime_t*, int);
        int getTotalMinutesInDay(int, int);
        int setDSTDay(dstinfo_t*, int);

    public:

        WorldTimeCalc();      // �R���X�g���N�^
        ~WorldTimeCalc();     // �f�X�g���N�^

        int getDayOfYear(int, int, int);

        // ���̍ŏ���N�j���̓��t�����߂�
        int getMonthFirstWeekDay(int, int, int);

        // ���̍Ō��N�j���̓��t�����߂�
        int getMonthLastWeekDay(int, int, int);

        // ������Ώۓ�����ɂ��邩�ǂ����`�F�b�N����
        int isDateAfterSpecifiedDate(int, int, int, int);

        // ���݂̃O���j�b�W�W����
        int getCurrentGMT(datetime_t*);

        // ���E�̔C�ӎ����E�w�肳�ꂽ�T�}�[�^�C���Ŏ������擾
        int getWorldTime(datetime_t*, datetime_t*, double,
                bool, dstinfo_t*, dstinfo_t*);

};

#endif /* __WORLDTIMECALC_H__ */
