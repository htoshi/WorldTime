/*
 * ���E�̎��Ԃ��擾
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
*/
#ifndef __DRAW_WORLDTIME_H__
#define __DRAW_WORLDTIME_H__

#define WORLDMAP_WIDTH 356
#define WORLDMAP_HEIGHT 184

#include <windows.h>
#include <commctrl.h>
#include "WorldTimeConfig.h"    // datetimeinfo_t readAreaInfo()
#include "WorldTimeCalc.h"

// #define __DEBUG__ 1

/* ���E�����`��N���X */
class DrawWorldTime {

    private:
        HFONT hFont;            // �t�H���g�I�u�W�F�N�g
        HWND hTool;             // �c�[���`�b�v�n���h��
        TOOLINFO toolInfo;      // �c�[���`�b�v���\����
        int iMaxDateTime;       // �������\���̂Ɋi�[���ꂽ��

        // �������\���̔z��
        datetimeinfo_t mDateTime[MAX_AREA];

        // ���E�����v�Z�N���X
        WorldTimeCalc* mWorldTimeCalc;

        // ���ݓ������Z�b�g
        void setCurrentTime(datetimeinfo_t*, datetime_t*);

        // �c�[���`�b�v���A�N�e�B�x�[�g
        void activateToolTip(TCHAR*);

        // �^�C���]�[�����𕪊����Ď擾
        int splitTZName(TCHAR*, TCHAR*, TCHAR*);

#ifdef __DEBUG__
        void debug(TCHAR*);
#endif

    public:

        DrawWorldTime(HWND, LPARAM); // �R���X�g���N�^
        ~DrawWorldTime();            // �f�X�g���N�^

        void drawBitmap(HDC, HBITMAP);
        void drawText(HDC);
        void setCurrentTime(void);

        // �c�[���`�b�v�\��
        void showToolTip(POINT*);

        // �c�[���`�b�v���f�B�A�N�e�B�x�[�g
        void deActivateToolTip();
};

#endif /* __DRAW_WORLDTIME_H__ */
