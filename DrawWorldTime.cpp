/*
 * ���E�̎�����`��
 *  $Id$
 *
 * Copyright (C) 2005, Toshi All rights reserved.
*/
#include "resource.h"
#include "DrawWorldTime.h"

/* �R���X�g���N�^ */
DrawWorldTime::DrawWorldTime(HWND hwnd, LPARAM lparam){

    WorldTimeConfig* config;    // �ݒ�t�@�C���N���X

    // �R�����R���g���[��������
    InitCommonControls();

    // �c�[���`�b�v�쐬
    hTool = CreateWindowEx(0, TOOLTIPS_CLASS,
                NULL, TTS_ALWAYSTIP,
                CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, CW_USEDEFAULT,
                hwnd, NULL, ((LPCREATESTRUCT)(lparam))->hInstance,
                NULL);

    // �N���C�A���g�̈��ݒ�
    GetClientRect(hwnd, (LPRECT)&toolInfo.rect);

    toolInfo.cbSize = sizeof(TOOLINFO);
    toolInfo.uFlags = TTF_SUBCLASS;
    toolInfo.hwnd = hwnd;
    toolInfo.lpszText = "";

    SendMessage(hTool, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

    // ���E�����v�Z�N���X
    mWorldTimeCalc = new WorldTimeCalc();

    // �t�H���g�̍쐬
    this->hFont = CreateFont(
            12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            VARIABLE_PITCH | FF_ROMAN , "MS Gothic");

    // �ݒ�t�@�C������ݒ��ǂݍ���
    config = new WorldTimeConfig();

    int iRet = config->readAreaInfo(mDateTime);

    // INI �t�@�C���ُ�
    if(iRet == ERR){
        MessageBox(NULL, TEXT("INI�t�@�C���ُ�"),
                         IDC_STATIC_APPNAME, MB_ICONSTOP | MB_OK);

        exit(ERR);
    }

    iMaxDateTime = iRet;    // �������\���̂Ɋi�[���ꂽ����ݒ�

    delete(config);
}

/* �f�X�g���N�^ */
DrawWorldTime::~DrawWorldTime() {

    // ���E�����v�Z�N���X�폜
    if(mWorldTimeCalc != NULL)
        delete(mWorldTimeCalc);

    // �t�H���g�폜
    DeleteObject(hFont);
}

/* �r�b�g�}�b�v��`��
   HDC hdc �f�o�C�X�R���e�L�X�g�n���h��
   HBITMAP hBitmap �r�b�g�}�b�v�n���h��
   �߂�l: �Ȃ�
*/
void DrawWorldTime::drawBitmap(HDC hdc, HBITMAP hBitmap){
    HDC hBuffer;

    // DC �擾
    hBuffer = CreateCompatibleDC(hdc);
    SelectObject(hBuffer, hBitmap);

    // BitBlt�ŕ`��
    BitBlt(hdc, 0, 0, WORLDMAP_WIDTH, WORLDMAP_HEIGHT, hBuffer, 0, 0, SRCCOPY);
    DeleteDC(hBuffer);
}

/* �e�L�X�g��`��
   HDC hdc �f�o�C�X�R���e�L�X�g�n���h��
   �߂�l: �Ȃ�
*/
void DrawWorldTime::drawText(HDC hdc){

    int iTZOffset;

    SetBkMode(hdc , TRANSPARENT);           // �w�i�F�𓧖��ɂ���
    SelectObject(hdc , this->hFont);        // �t�H���g�̑I��

    // ���ꂼ��̃^�C���]�[���̓�����\��
    for(int i=0; i<iMaxDateTime; i++){

        // �^�C���]�[�����̔����Ȉʒu����
        iTZOffset = (lstrlen(mDateTime[i].szName) == 3)?8:4;

        // �Ď���
        if(mDateTime[i].isDST){
            SetTextColor(hdc, RGB(255, 255, 0));    // �e�L�X�g�F�͉�
        }else{
            SetTextColor(hdc, RGB(255, 255, 255));  // �e�L�X�g�F�͔�
        }

        // �^�C���]�[����
        TextOut(hdc, mDateTime[i].x+iTZOffset, mDateTime[i].y,
                mDateTime[i].szName, lstrlen(mDateTime[i].szName));

        // ���t�Ǝ���
        TextOut(hdc, mDateTime[i].x, mDateTime[i].y+10,
                mDateTime[i].szDate, lstrlen(mDateTime[i].szDate));
        TextOut(hdc, mDateTime[i].x, mDateTime[i].y+20,
                mDateTime[i].szTime, lstrlen(mDateTime[i].szTime));
    }
}

/* ���ݓ������Z�b�g����
   datetimeinfo_t* dtinfo �������i�[�\����
   datetime_t* dt �������i�[�\����
   �߂�l: �Ȃ�
*/
void DrawWorldTime::setCurrentTime(datetimeinfo_t* dtinfo, datetime_t* dt){

    wsprintf(dtinfo->szDate, "%02d/%02d", dt->Month, dt->Day);
    wsprintf(dtinfo->szTime, "%02d:%02d", dt->Hour, dt->Minute);
    dtinfo->isDST = dt->isDST;

}

/* ���ݓ������Z�b�g����
   �߂�l: �Ȃ�
*/
void DrawWorldTime::setCurrentTime(){

    datetime_t dtGMT, dt;
    dstinfo_t dstStart, dstEnd;

    // GMT ���擾
    mWorldTimeCalc->getCurrentGMT(&dtGMT);

    // �e�G���A�̎������擾���ăZ�b�g
    for(int i=0; i<iMaxDateTime; i++){

        // �Ď��ԏ����Z�b�g
        if(mDateTime[i].hasDST){

            // �Ď��ԊJ�n���
            dstStart.Month = mDateTime[i].iDSTStartMonth;
            dstStart.Type = mDateTime[i].szDSTStartType;
            dstStart.WeekDay = mDateTime[i].iDSTStartWeekDay;
            dstStart.Time = mDateTime[i].iDSTStartTime;

            // �Ď��ԏI�����
            dstEnd.Month = mDateTime[i].iDSTEndMonth;
            dstEnd.Type = mDateTime[i].szDSTEndType;
            dstEnd.WeekDay = mDateTime[i].iDSTEndWeekDay;
            dstEnd.Time = mDateTime[i].iDSTEndTime;
        }

        // ���E�����擾
        mWorldTimeCalc->getWorldTime(&dt, &dtGMT, mDateTime[i].offset,
            mDateTime[i].hasDST, &dstStart, &dstEnd);

        setCurrentTime(&mDateTime[i], &dt);
    }
}

/* �c�[���`�b�v���A�N�e�B�x�[�g
   TCHAR* szBuf �e�L�X�g
*/
void DrawWorldTime::activateToolTip(TCHAR* szBuf){

    // �e�L�X�g���Z�b�g
    this->toolInfo.lpszText = szBuf;

    // �A�N�e�B�x�[�g
    SendMessage(hTool, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
    SendMessage(hTool, TTM_ACTIVATE, TRUE, 0);
}

/* �c�[���`�b�v���f�B�A�N�e�B�x�[�g */
void DrawWorldTime::deActivateToolTip(){

    SendMessage(this->hTool, TTM_ACTIVATE, FALSE, 0);
}

/* �c�[���`�b�v�\��
   POINT* po �}�E�X�ʒu
*/
void DrawWorldTime::showToolTip(POINT* po){

    // ���ꂼ��̃^�C���]�[���̃c�[���`�b�v��\��
    for(int i=0; i<sizeof(mDateTime)/sizeof(mDateTime[0]); i++){

        if((po->x >= mDateTime[i].x) && (po->x <= mDateTime[i].x + 34) &&
           (po->y >= mDateTime[i].y) && (po->y <= mDateTime[i].y + 32)){

            // �c�[���`�b�v��������
            activateToolTip(mDateTime[i].szInfo);

            return;
        }
    }

    // �c�[���`�b�v��񊈐���
    deActivateToolTip();
}

/* �f�o�b�O���O
   char* szBuf �f�o�b�O���b�Z�[�W
   �߂�l: �Ȃ�
*/
#ifdef __DEBUG__
void DrawWorldTime::debug(TCHAR* szBuf){
    HANDLE hFile;
    BOOL bRet;
    DWORD dwWritten;
    SYSTEMTIME stTime;

    TCHAR szLogMsg[1024];

    GetLocalTime(&stTime);

    wsprintf(szLogMsg, "%04d/%02d/%02d %02d:%02d:%02d %s\r\n",
                stTime.wYear, stTime.wMonth, stTime.wDay,
                stTime.wHour, stTime.wMinute, stTime.wSecond, szBuf);

    // �t�@�C���쐬
    hFile = CreateFile("debug.log", GENERIC_WRITE,
                        FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE){
        MessageBox(0, TEXT("�t�@�C���I�[�v���G���["),
                      TEXT("�G���["), MB_ICONSTOP);
        return;
    }

    // �t�@�C���ŏI�[�Ƀ|�C���^���ړ�
    SetFilePointer(hFile , 0 , NULL , FILE_END);

    // ��������
    bRet = WriteFile(hFile, szLogMsg, (DWORD)strlen(szLogMsg), &dwWritten, NULL);

    CloseHandle(hFile);
}
#endif /* __DEBUG__ */
