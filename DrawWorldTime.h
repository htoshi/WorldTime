/*
 * ���E�̎��Ԃ��擾
 *  $Id$
 *
 * Copyright (C) 2004, Toshi All rights reserved.
*/
#ifndef __DRAW_WORLDTIME_H__
#define __DRAW_WORLDTIME_H__

#define WORLDMAP_WIDTH 356
#define WORLDMAP_HEIGHT 184

#include <windows.h>
#include <commctrl.h>
#include "WorldTimeDLL.h"

// #define __DEBUG__ 1

/* �������i�[�\���� */
typedef struct {
    TCHAR szDate[6];   // ���t (01/01)
    TCHAR szTime[6];   // ���� (12:00)
    TCHAR szName[5];   // �]�[����
    bool isDST;        // �Ď��ԃt���O (true: �Ď��� false: �W������)
    int x;             // �\���ʒu X ���W
    int y;             // �\���ʒu Y ���W
    TCHAR szInfo[128]; // �c�[���`�b�v�ɕ\��������
} datetimeinfo_t;

/* ���E�����`��N���X */
class DrawWorldTime {

    private:
        HFONT hFont;            // �t�H���g�I�u�W�F�N�g
        HWND hTool;             // �c�[���`�b�v�n���h��
        TOOLINFO toolInfo;      // �c�[���`�b�v���\����

        // �������\���̔z��
        datetimeinfo_t mDateTime[8];

        // ���ݓ������Z�b�g
        void setCurrentTime(datetimeinfo_t*, datetime_t*);

        // �c�[���`�b�v���A�N�e�B�x�[�g
        void activateToolTip(TCHAR*);

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
