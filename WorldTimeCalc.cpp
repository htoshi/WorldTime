/*
 * ���E�̎��Ԃ��擾
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
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
    �߂�l: ���̓���
*/
int WorldTimeCalc::getDayOfMonth(int iYear, int iMonth){

    static const int dayMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(iMonth == 2 && (iYear%4 == 0 && iYear%100 != 0
              || iYear%400 == 0)){
        return 29 ;
    }

    return dayMonth[iMonth-1] ;
}

/* ������Ώۓ�����ɂ��邩�ǂ����`�F�b�N����
   int iBaseMonth ���
   int iBaseDay ���
   int iMonth �Ώی�
   int iDay �Ώۓ�
   �߂�l: 1 ��ɂ���ꍇ 0 ���� -1 �O�ɂ���ꍇ
*/
int WorldTimeCalc::isDateAfterSpecifiedDate(int iBaseMonth, int iBaseDay, int iMonth, int iDay){

    if(iBaseMonth < iMonth){
        // ���������
        return 1;
    }else if(iBaseMonth > iMonth){
        // ��������O
        return -1;
    }

    if(iBaseDay < iDay){
        // ���������
        return 1;
    }else if(iBaseDay > iDay){
        // ��������O
        return -1;
    }

    return 0;
}

/* ���̍ŏ���N�j���̓��t�����߂�
   int iYear �N
   int iMonth ��
   int iWeekDay N�j�� (0 ���j���`6 �y�j��)
   �߂�l: ���̍ŏ���N�j���̓��t
*/
int WorldTimeCalc::getMonthFirstWeekDay(int iYear, int iMonth, int iWeekDay){

    // 1���̗j�������߂�
    int iWeek = zeller(iYear, iMonth, 1);

    // 1�����ŏ���N�j��
    if(iWeek == iWeekDay)
        return 1;

    // (N+1)�j������1���̗j�������������̂����߂���t
    int iRet = iWeekDay + 1 - iWeek;

    // 0�ȉ��̏ꍇ��7�𑫂�
    if(iRet <= 0)
        iRet = iRet + 7;

    return iRet;
}

/* ���̍Ō��N�j���̓��t�����߂�
   int iYear �N
   int iMonth ��
   int iWeekDay N�j�� (0 ���j���`6 �y�j��)
   �߂�l: ���̍Ō��N�j���̓��t
*/
int WorldTimeCalc::getMonthLastWeekDay(int iYear, int iMonth, int iWeekDay){

    // ���̍Ō�̓������߂�
    int iMonthLastDay = getDayOfMonth(iYear, iMonth);

    // ���̍Ō�̓��̗j�������߂�
    int iWeek = zeller(iYear, iMonth, iMonthLastDay);

    // N�j�����猎�̍Ō�̓��̗j�����������l�����߂�
    int iRet = iWeekDay - iWeek ;

    // 0���傫���ꍇ��7������
    if(iRet > 0)
        iRet = iRet - 7;

    return (iMonthLastDay + iRet);
}

/* ���̑�M N�j���̓��t�����߂�
   int iYear �N
   int iMonth ��
   int iNum �� M �j���� M ���w��
   int iWeekDay N�j�� (0 ���j���`6 �y�j��)
   �߂�l: ���̍ŏ���N�j���̓��t (���݂��Ȃ��ꍇ�� -1)
*/
int WorldTimeCalc::getMonthMWeekDay(int iYear, int iMonth, int iNum, int iWeekDay){

    // ���̍ŏ���N�j���̓��t�����߂�
    int iDay = getMonthFirstWeekDay(iYear, iMonth, iWeekDay);

    // M�Ԗڂ�N�j���̓��t�����߂�
    iDay = iDay + (iNum - 1)*7;

    // ���݂��Ȃ�
    if(iDay > getDayOfMonth(iYear, iMonth))
        return -1;

    return iDay;
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
    �߂�l: OK
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
    �߂�l: 1���̒��ł̗ݐϕ�
*/
int WorldTimeCalc::getTotalMinutesInDay(int iHour, int iMin){

    return iHour*60 + iMin;
}

/* ���݂�GMT�������擾 (�O���j�b�W�W����)
    datetime_t* dt  ���t�����\����
    �߂�l: OK
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

/* DST ���t��ݒ�
    dstinfo_t* dstInfo �Ď��ԏ��\����
    int iYear �N
    �߂�l: OK
*/
int WorldTimeCalc::setDSTDay(dstinfo_t* dstInfo, int iYear){

    // ���̑��T�̗j���̓��t��ݒ�
    if(dstInfo->Type == 'F'){

        dstInfo->Day = getMonthFirstWeekDay(iYear,
            dstInfo->Month, dstInfo->WeekDay);

    // ���̍ŏI�T�̗j���̓��t��ݒ�
    }else if(dstInfo->Type == 'E'){

        dstInfo->Day = getMonthLastWeekDay(iYear,
            dstInfo->Month, dstInfo->WeekDay);

	// ���̑�M N�j���̓��t��ݒ�
	}else{
		int iNum = _tstoi(&(dstInfo->Type));
		if(iNum >= 1 && iNum <= 5){
			dstInfo->Day = getMonthMWeekDay(iYear,
            dstInfo->Month, iNum, dstInfo->WeekDay);
		}
	}

    return OK;
}

/* ���E�̔C�ӎ����E�w�肳�ꂽ�T�}�[�^�C���Ŏ������擾
    datetime_t* dt  ���t�����\���́E�ϊ�����
    datetime_t* dt_in  ���t�����\���́E���͒l(GMT)
    double offset �I�t�Z�b�g (����)
    bool hasDST �Ď��ԕێ��t���O (true: �Ď��Ԃ��� false: �Ď��ԂȂ�)
    dstinfo_t* dstStart �Ď��ԊJ�n���
    dstinfo_t* dstEnd �Ď��ԏI�����
    bool* isDST �Ď��ԃt���O (true: �Ď��� false: �W������)
    �߂�l: OK
*/
int WorldTimeCalc::getWorldTime(datetime_t* dt, datetime_t* dt_in, double offset,
        bool hasDST, dstinfo_t* dstStart, dstinfo_t* dstEnd){

    // ���͒l���Z�b�g
    dt->Year = dt_in->Year;
    dt->Month = dt_in->Month;
    dt->Day = dt_in->Day;
    dt->Hour = dt_in->Hour;
    dt->Minute = dt_in->Minute;
    dt->Second = dt_in->Second;
    dt->isDST = false;

    // �I�t�Z�b�g�␳
    getDateTimeByOffset(dt, (int)(offset*60));

    // �Ď��ԃt���O
    dt->isDST = false;

    // �Ď��ԏ��Ȃ�
    if(!hasDST)
        return OK;

    // DST ���t��ݒ�
    setDSTDay(dstStart, dt->Year);
    setDSTDay(dstEnd, dt->Year);

    // 1�N���ŃT�}�[�^�C���J�n�����I���������O�ɂ���(�k����)
    if(isDateAfterSpecifiedDate(dstStart->Month, dstStart->Day, dstEnd->Month, dstEnd->Day) > 0){

        // �T�}�[�^�C���J�n���̗����`�T�}�[�^�C���I�����̑O��
        if(isDataInSpecifiedRange(dt->Month, dt->Day,
            dstStart->Month, dstStart->Day, dstEnd->Month, dstEnd->Day, false)){

                // �T�}�[�^�C���␳
                goto lblSummerTime;
        }

        // �T�}�[�^�C���J�n���̏ꍇ (dstStart->Time �ɃT�}�[�^�C���J�n)
        if(dt->Month == dstStart->Month && dt->Day == dstStart->Day){

            // ���n�W������ dstStart->Time ����̓T�}�[�^�C��
            if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= dstStart->Time){

                // �T�}�[�^�C���␳
                goto lblSummerTime;
            }
        }

        // �T�}�[�^�C���I�����̏ꍇ (dstEnd->Time �ɃT�}�[�^�C���I��)
        if(dt->Month == dstEnd->Month && dt->Day == dstEnd->Day){

            // ���n�W������ dstEnd->Time ���O�̓T�}�[�^�C��
            if(getTotalMinutesInDay(dt->Hour, dt->Minute) < dstEnd->Time){

                // �T�}�[�^�C���␳
                goto lblSummerTime;
            }
        }

        // �T�}�[�^�C���ł͂Ȃ�
        return OK;

    // 1�N���ŃT�}�[�^�C���J�n�����I����������ɂ���(�씼��)
    }else{

        // �T�}�[�^�C���I�����̗����`�T�}�[�^�C���J�n���̑O���̏ꍇ
        if(isDataInSpecifiedRange(dt->Month, dt->Day,
            dstEnd->Month, dstEnd->Day, dstStart->Month, dstStart->Day, false)){

            return OK;    // �T�}�[�^�C���ł͂Ȃ�
        }

        // �T�}�[�^�C���I�����̏ꍇ (dstEnd->Time �ɃT�}�[�^�C���I��)
        if(dt->Month == dstEnd->Month && dt->Day == dstEnd->Day){

            // ���n�W������ dstEnd->Time ����̓T�}�[�^�C���ł͂Ȃ�
            if(getTotalMinutesInDay(dt->Hour, dt->Minute) >= dstEnd->Time)
                return OK;
        }

        // �T�}�[�^�C���J�n���̏ꍇ (dstStart->Time �ɃT�}�[�^�C���J�n)
        if(dt->Month == dstStart->Month && dt->Day == dstStart->Day){

            // ���n�W������ dstStart->Time ���O�̓T�}�[�^�C���ł͂Ȃ�
            if(getTotalMinutesInDay(dt->Hour, dt->Minute) < dstStart->Time)
                return OK;
        }

        // ����ȊO�̓T�}�[�^�C��
    }

lblSummerTime:
    // �T�}�[�^�C���␳ (+60min)
    getDateTimeByOffset(dt, 60);
    dt->isDST = true;

    return OK;
}
