/*
 * ���E�̎��Ԃ��擾
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#include "WorldTimeCalc.h"

// �R���X�g���N�^
WorldTimeCalc::WorldTimeCalc() {}

// �f�X�g���N�^
WorldTimeCalc::~WorldTimeCalc() {}

/* zeller �̌����ɂ��j�������߂�
   int iYear �N
   int iMonth ��
   int iDay ��
   �߂�l: 0 ���j���`6 �y�j��
*/
int WorldTimeCalc::zeller(int iYear, int iMonth, int iDay){

    // 1����2���͑O�N��13���E14���Ƃ��Čv�Z
    if(iMonth <= 2){
        iYear = iYear - 1;
        iMonth = iMonth + 12;
    }

    return ((5*iYear/4 - iYear/100 + iYear/400
             + (26*iMonth + 16)/10 + iDay) % 7);
}

/* ���̓��������߂�
   int iYear �N
   int iMonth ��
*/
int WorldTimeCalc::getDayOfMonth(int iYear, int iMonth){

    static const int dayMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(iMonth == 2 && (iYear%4 == 0 && iYear%100 != 0
              || iYear%400 == 0)){
        return 29 ;
    }

    return dayMonth[iMonth-1] ;
}

/* ���̍ŏ��̓��j�������߂�
   int iYear �N
   int iMonth ��
   �߂�l: ���̍ŏ��̓��j���̓��t
*/
int WorldTimeCalc::getMonthFirstSunday(int iYear, int iMonth){

    // 1���̗j�������߂�
    int iDayWeek = zeller(iYear, iMonth, 1);

    // 1�����ŏ��̓��j��
    if(iDayWeek == 0)
        return 1;

    return (8-iDayWeek);
}

/* ���̍Ō�̓��j�������߂�
   int iYear �N
   int iMonth ��
   �߂�l: ���̍Ō�̓��j���̓��t
*/
int WorldTimeCalc::getMonthLastSunday(int iYear, int iMonth){

    int iDayWeek;

    if(iMonth == 12){
        // 12���̏ꍇ�͗��N1��1���̗j�������߂�
        iDayWeek = zeller(iYear+1, 1, 1);
    }else{
        // ����1���̗j�������߂�
        iDayWeek = zeller(iYear, iMonth+1, 1);
    }

    // ����1�������j���Ȃ̂Ō����� 6 ���O�����j��
    if(iDayWeek == 0)
        return getDayOfMonth(iYear, iMonth) - 6;

    return (getDayOfMonth(iYear, iMonth) + 1 - iDayWeek);
}

/* ������t���w�肳�ꂽ���t�͈͓̔��ɂ��邩
   int iMonth �w�茎
   int iDay �w���
   int iFromMonth �J�n��
   int iFromDay �J�n��
   int iToMonth �I����
   int iToDay �I����
   bool isInclude �w������܂ޏꍇ true
   �߂�l: true �͈͓� false �͈͊O
*/
bool WorldTimeCalc::isDataInSpecifiedRange(int iMonth, int iDay,
        int iFromMonth, int iFromDay, int iToMonth, int iToDay,
        bool isInclude){

    // �w�肵�����͈̔͊O
    if(iMonth < iFromMonth || iMonth > iToMonth)
        return false;

    // �w������܂ޏꍇ
    if(isInclude){
        // �w�肵�������J�n���œ����͈͊O�̏ꍇ
        if(iMonth == iFromMonth && iDay < iFromDay)
            return false;

        // �w�肵�������I�����œ����͈͊O�̏ꍇ
        if(iMonth == iToMonth && iDay > iToDay)
            return false;

    // �w������܂܂Ȃ��ꍇ
    }else{
        // �w�肵�������J�n���œ����͈͊O�̏ꍇ
        if(iMonth == iFromMonth && iDay <= iFromDay)
            return false;

        // �w�肵�������I�����œ����͈͊O�̏ꍇ
        if(iMonth == iToMonth && iDay >= iToDay)
            return false;
    }

    return true;
}

/* datetime_t ��^���ĔC�ӂ̎�����^�������̓��t�Ǝ����𓾂�
    datetime_t* dt  ���t�����\����
    int iOffsetMin  �I�t�Z�b�g(��)
*/
int WorldTimeCalc::getDateTimeByOffset(datetime_t* dt, int iOffsetMin){

    int iHour, iMin;

    iHour = int(iOffsetMin/60);
    iMin = iOffsetMin - iHour*60;

    dt->Hour += iHour;
    dt->Minute += iMin;

    // ���̌J�グ�E�J�艺������
    if(dt->Minute >= 60){      // ����60�ȏ�̏ꍇ
        dt->Hour += 1;
        dt->Minute -= 60;
    }else if(dt->Minute < 0){  // ����0�����̏ꍇ
        dt->Hour -= 1;
        dt->Minute += 60;
    }

    // ���̌J�グ�E�J�艺������
    if(dt->Hour >= 24){     // ����24�ȏ�̏ꍇ
        dt->Day += 1;
        dt->Hour -= 24;
    }else if(dt->Hour < 0){ // ����0�����̏ꍇ
        dt->Day -= 1;
        dt->Hour += 24;
    }

    // ���̌J�グ�E�J�艺������
    // �������������ȏ�̏ꍇ
    if(dt->Day > getDayOfMonth(dt->Year, dt->Month)){

        dt->Month += 1;
        dt->Day = 1;

        // 12 ���̏ꍇ�͗��N��1/1���Z�b�g���ďI��
        if(dt->Month > 12){
            dt->Month = 1;
            dt->Year += 1;
            return OK;
        }

    }else if(dt->Day <= 0){ // ����0�ȉ��̏ꍇ

        dt->Month -= 1;

        // 1���̏ꍇ��N��12/31���Z�b�g���ďI��
        if(dt->Month <= 0){
            dt->Month = 12;
            dt->Day = 31;
            dt->Year -= 1;
            return OK;
        }

        // �挎�������Z�b�g
        dt->Day = getDayOfMonth(dt->Year, dt->Month);
    }

    return OK;
}

/* 1���̒��ŗݐϕ������߂�
    int iHour ��
    int iMin ��
*/
int WorldTimeCalc::getTotalMinutesInDay(int iHour, int iMin){

    return iHour*60 + iMin;
}

/* ���݂�GMT�������擾 (�O���j�b�W�W����)
    datetime_t* dt  ���t�����\����
*/
int WorldTimeCalc::getCurrentGMT(datetime_t* dt){

    SYSTEMTIME stTime;

    // �V�X�e�����������̂܂܎g�p
    GetSystemTime(&stTime);

    dt->Year = stTime.wYear;
    dt->Month = stTime.wMonth;
    dt->Day = stTime.wDay;
    dt->Hour = stTime.wHour;
    dt->Minute = stTime.wMinute;
    dt->Second = stTime.wSecond;
    dt->isDST = false;

    return OK;
}

/* JST�������擾 (���{�W����)
    datetime_t* dt  ���t�����\���́E�ϊ�����
    datetime_t* dt_in  ���t�����\���́E���͒l(GMT)
*/
int WorldTimeCalc::getJST(datetime_t* dt, datetime_t* dt_in){

    // ���͒l���Z�b�g
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // JST = GMT + 9 (+540min)
    getDateTimeByOffset(dt, 540);

    return OK;
}

/* AEST�������擾 (���B�����W����)
    datetime_t* dt  ���t�����\���́E�ϊ�����
    datetime_t* dt_in  ���t�����\���́E���͒l(GMT)
*/
int WorldTimeCalc::getAEST(datetime_t* dt, datetime_t* dt_in){

    // ���͒l���Z�b�g
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // AEST = GMT + 10 (+600min)
    getDateTimeByOffset(dt, 600);

    // 3���̍ŏI���j�����擾
    int iMarchLastSundayDay = getMonthLastSunday(dt->Year, 3);

    // 10���̍ŏI���j�����擾
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 3���̍ŏI���j���̗����`10���̍ŏI���j���̑O���̏ꍇ
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        3, iMarchLastSundayDay, 10, iOctoberLastSundayDay, false)){

        return OK;    // �T�}�[�^�C���ł͂Ȃ�
    }

    // 3���̍ŏI���j���̏ꍇ (03:00 �ɃT�}�[�^�C���I��)
    if(dt->Month == 3 && dt->Day == iMarchLastSundayDay){

        // ���n�W������ 02:00 ����̓T�}�[�^�C���ł͂Ȃ�
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120)
            return OK;
    }

    // 10���̍ŏI���j���̏ꍇ (02:00 �ɃT�}�[�^�C���J�n)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // ���n�W������ 02:00 ���O�̓T�}�[�^�C���ł͂Ȃ�
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 120)
            return OK;
    }

    // �T�}�[�^�C���␳ (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}

/* AWST�������擾 (���B�����W����)
    datetime_t* dt  ���t�����\���́E�ϊ�����
    datetime_t* dt_in  ���t�����\���́E���͒l(GMT)
*/
int WorldTimeCalc::getAWST(datetime_t* dt, datetime_t* dt_in){

    // ���͒l���Z�b�g
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // AWST = GMT + 8 (+480min)
    getDateTimeByOffset(dt, 480);

    return OK;
}

/* PST�������擾 (�����m�W����)
    datetime_t* dt  ���t�����\���́E�ϊ�����
    datetime_t* dt_in  ���t�����\���́E���͒l(GMT)
*/
int WorldTimeCalc::getPST(datetime_t* dt, datetime_t* dt_in){

    // ���͒l���Z�b�g
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // PST = GMT - 8 (-480min)
    getDateTimeByOffset(dt, -480);

    // 4���̍ŏ��̓��j�����擾
    int iAprilFirstSundayDay = getMonthFirstSunday(dt->Year, 4);

    // 10���̍ŏI���j�����擾
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 4���̍ŏ��̓��j���̗����`10���̍ŏI���j���̑O��
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        4, iAprilFirstSundayDay, 10, iOctoberLastSundayDay, false)){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
    }

    // 4���̍ŏI���j���̏ꍇ (02:00 �ɃT�}�[�^�C���J�n)
    if(dt->Month == 4 && dt->Day == iAprilFirstSundayDay){

        // ���n�W������ 02:00 ����̓T�}�[�^�C��
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
        }
    }

    // 10���̍ŏI���j���̏ꍇ (01:00 �ɃT�}�[�^�C���I��)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // ���n�W������ 01:00 ���O�̓T�}�[�^�C��
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 60){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
        }
    }

    // �T�}�[�^�C���ł͂Ȃ�
    return OK;

lblSummerTime:
    // �T�}�[�^�C���␳ (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}

/* EST�������擾 (�����W����)
    datetime_t* dt  ���t�����\���́E�ϊ�����
    datetime_t* dt_in  ���t�����\���́E���͒l(GMT)
*/
int WorldTimeCalc::getEST(datetime_t* dt, datetime_t* dt_in){

    // ���͒l���Z�b�g
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    dt->isDST = false;

    // PST = GMT - 5 (-300min)
    getDateTimeByOffset(dt, -300);

    // 4���̍ŏ��̓��j�����擾
    int iAprilFirstSundayDay = getMonthFirstSunday(dt->Year, 4);

    // 10���̍ŏI���j�����擾
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 4���̍ŏ��̓��j���̗����`10���̍ŏI���j���̑O��
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        4, iAprilFirstSundayDay, 10, iOctoberLastSundayDay, false)){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
    }

    // 4���̍ŏ��̓��j���̏ꍇ (02:00 �ɃT�}�[�^�C���J�n)
    if(dt->Month == 4 && dt->Day == iAprilFirstSundayDay){

        // ���n�W������ 02:00 ����̓T�}�[�^�C��
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
        }
    }

    // 10���̍ŏI���j���̏ꍇ (01:00 �ɃT�}�[�^�C���I��)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // ���n�W������ 01:00 ���O�̓T�}�[�^�C��
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 60){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
        }
    }

    // �T�}�[�^�C���ł͂Ȃ�
    return OK;

lblSummerTime:
    // �T�}�[�^�C���␳ (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}

/* CET�������擾 (�������[���b�p�W�����E�h�C�c)
    datetime_t* dt  ���t�����\���́E�ϊ�����
    datetime_t* dt_in  ���t�����\���́E���͒l(GMT)
*/
int WorldTimeCalc::getCET(datetime_t* dt, datetime_t* dt_in){

    // ���͒l���Z�b�g
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // CET = GMT + 1 (+60min)
    getDateTimeByOffset(dt, 60);

    // 3���̍ŏI���j�����擾
    int iMarchLastSundayDay = getMonthLastSunday(dt->Year, 3);

    // 10���̍ŏI���j�����擾
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 3���̍ŏI���j���̗����`10���̍ŏI���j���̑O��
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        3, iMarchLastSundayDay, 10, iOctoberLastSundayDay, false)){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
    }

    // 3���̍ŏI���j���̏ꍇ (02:00 �ɃT�}�[�^�C���J�n)
    if(dt->Month == 3 && dt->Day == iMarchLastSundayDay){

        // ���n�W������ 02:00 ����̓T�}�[�^�C��
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
        }
    }

    // 10���̍ŏI���j���̏ꍇ (01:00 �ɃT�}�[�^�C���I��)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // ���n�W������ 01:00 ���O�̓T�}�[�^�C��
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 60){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
        }
    }

    // �T�}�[�^�C���ł͂Ȃ�
    return OK;

lblSummerTime:
    // �T�}�[�^�C���␳ (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}

/* MSK�������擾 (���X�N���W����)
    datetime_t* dt  ���t�����\���́E�ϊ�����
    datetime_t* dt_in  ���t�����\���́E���͒l(GMT)
*/
int WorldTimeCalc::getMSK(datetime_t* dt, datetime_t* dt_in){

    // ���͒l���Z�b�g
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // MSK = GMT + 3 (+180min)
    getDateTimeByOffset(dt, 180);

    // 3���̍ŏI���j�����擾
    int iMarchLastSundayDay = getMonthLastSunday(dt->Year, 3);

    // 10���̍ŏI���j�����擾
    int iOctoberLastSundayDay = getMonthLastSunday(dt->Year, 10);

    // 3���̍ŏI���j���̗����`10���̍ŏI���j���̑O��
    if(isDataInSpecifiedRange(dt->Month, dt->Day,
        3, iMarchLastSundayDay, 10, iOctoberLastSundayDay, false)){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
    }

    // 3���̍ŏI���j���̏ꍇ (02:00 �ɃT�}�[�^�C���J�n)
    if(dt->Month == 3 && dt->Day == iMarchLastSundayDay){

        // ���n�W������ 02:00 ����̓T�}�[�^�C��
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= 120){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
        }
    }

    // 10���̍ŏI���j���̏ꍇ (02:00 �ɃT�}�[�^�C���I��)
    if(dt->Month == 10 && dt->Day == iOctoberLastSundayDay){

        // ���n�W������ 02:00 ���O�̓T�}�[�^�C��
        if(getTotalMinutesInDay(dt->Hour, dt->Minute) < 120){

            // �T�}�[�^�C���␳
            goto lblSummerTime;
        }
    }

    // �T�}�[�^�C���ł͂Ȃ�
    return OK;

lblSummerTime:
    // �T�}�[�^�C���␳ (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}
