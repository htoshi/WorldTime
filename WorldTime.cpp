#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "DrawWorldTime.h"
#include "WorldTimeConfig.h"

/* ���j���[ID�ݒ� */
#define IDM_WORLDTIME_ABOUT 1
#define IDM_WORLDTIME_STAYTOP 2
#define IDM_WORLDTIME_TOOLTIP 3

/* �萔 */
#define IDC_STATIC_WINDOWCLASSNAME "WorldTime"
#define IDC_STATIC_APPNAME "���E���v"
#define IDC_STATIC_AUTHOR "Toshi"
#define IDC_STATIC_VERSION "Version 0.1 Alpha"
#define IDC_STATIC_COPYRIGHT \
		IDC_STATIC_APPNAME " " IDC_STATIC_VERSION " " IDC_STATIC_AUTHOR

// #define __DEBUG__ 1

/* �v���g�^�C�v�錾 */
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitInstance(HINSTANCE, int);

void addMenu(HWND, bool, bool);
void deleteMenu(HWND);
void toggleMenuStayTop(HWND);
void toggleShowToolTip(HWND);

void OnMouseMove(HWND, DrawWorldTime*);
void OnTimer(HWND, DrawWorldTime*);

#ifdef __DEBUG__
void debug(TCHAR* szBuf);
#endif

/* �A�v���P�[�V�����G���g���[�|�C���g */
int WINAPI WinMain(HINSTANCE    hInstance,
                   HINSTANCE    hPrevInstance,
                   LPSTR        CmdLine,
                   int          CmdShow){

    MSG msg;

    // ���d�N���h�~
    HWND hWnd = FindWindow(IDC_STATIC_WINDOWCLASSNAME, NULL);

    if(hWnd){
        // �A�C�R��������Ă���Ό��̃T�C�Y�ɖ߂�
        if (IsIconic(hWnd))
            ShowWindow(hWnd, SW_RESTORE);

        // ���������E�B���h�E���t�H�A�O���E���h�ɂ���
        SetForegroundWindow(::GetLastActivePopup(hWnd));

        return FALSE;
    }

    // �E�B���h�E����
    if(!InitInstance(hInstance, CmdShow)){
        MessageBox(NULL, TEXT("���������ɃG���[���������܂���"),
                         IDC_STATIC_APPNAME, MB_ICONINFORMATION | MB_OK);

        return FALSE;
    }

    // ���b�Z�[�W���[�v
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

/* �E�B���h�E����
    HINSTANCE hInstance
    int CmdShow
*/
BOOL InitInstance(HINSTANCE hInstance, int CmdShow){

    HWND             hwnd;
    WNDCLASS         wc;
    WorldTimeConfig* config;    // �ݒ�t�@�C���N���X
    RECT             Rect;
    bool             isTop, isToolTipShow;

    wc.style        = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc  = WindowProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 0;
    wc.hInstance    = hInstance;
    wc.hIcon        = LoadIcon(hInstance, TEXT("WORLDICO"));
    wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground= (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "WorldTime";

    if(RegisterClass(&wc)==0) // �E�C���h�E�N���X�o�^
        return FALSE;

    /* �ݒ�t�@�C������ݒ��ǂݍ��� */
    config = new WorldTimeConfig();

    if(config->readWindowRect(&Rect) != OK ||
       config->readTopMostFlag(&isTop) != OK ||
       config->readToolTipFlag(&isToolTipShow) != OK){

        delete(config);
        return FALSE;
    }

    delete(config);

    // �E�C���h�E�쐬
    hwnd = CreateWindow(IDC_STATIC_WINDOWCLASSNAME,
                       IDC_STATIC_APPNAME,
                       WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                       Rect.left,
                       Rect.top,
                       WORLDMAP_WIDTH,
                       WORLDMAP_HEIGHT + 32,
                       NULL,
                       (HMENU)NULL,
                       hInstance,
                       0);

    if(hwnd == NULL)
        return FALSE;

    ShowWindow(hwnd, CmdShow);   // �E�C���h�E�̕\��
    UpdateWindow(hwnd);          // �E�C���h�E�̍ŏ��̍X�V
    deleteMenu(hwnd);            // �V�X�e�����j���[�폜

    // ��ɍőO�ʂɕ\��
    if(isTop)
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));

    // ���j���[�ǉ�
    addMenu(hwnd, isTop, isToolTipShow);

    // �ŏ�����\ 100ms �C���^�[�o�� 500ms �Ń^�C�}�[�Z�b�g
    SetTimer(hwnd, 100, 500, NULL);

    // �������g�p�ʂ�}����
    HANDLE hProcess = GetCurrentProcess();
    SetProcessWorkingSetSize(hProcess, -1, -1);

    return TRUE;
}

/* �E�C���h�E�v���V�[�W�� */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message,
                            WPARAM wparam, LPARAM lparam){

    static HBITMAP hBitmap;               // �r�b�g�}�b�v�I�u�W�F�N�g
    static DrawWorldTime* drawWorldTime;  // ���E�����`��N���X

    switch(message) {

        case WM_DESTROY:

            // ���E�����`��N���X�폜
            delete(drawWorldTime);

            // �E�B���h�E�T�C�Y�̐ݒ��ۑ�
            RECT Rect;
            GetWindowRect(hwnd, (LPRECT)&Rect);

            // �ݒ�t�@�C���N���X
            WorldTimeConfig* config;

            config = new WorldTimeConfig();
            config->writeWindowRect(&Rect);

            delete(config);

            PostQuitMessage(0);
            return  0;

        case WM_CREATE:

            // �r�b�g�}�b�v�I�u�W�F�N�g�̓ǂݍ���
            hBitmap = LoadBitmap(((LPCREATESTRUCT)lparam)->hInstance,
                                 TEXT("WORLDMAP"));

            // ���E�����`��N���X����
            drawWorldTime = new DrawWorldTime(hwnd, lparam);

            // ���ݎ������Z�b�g
            drawWorldTime->setCurrentTime();

            return 0;

        // �^�C�}�[�C�x���g������
        case WM_TIMER:

            OnTimer(hwnd, drawWorldTime);
            return 0;

        // �`��C�x���g
        case WM_PAINT:

            PAINTSTRUCT ps;
            HDC hdc;

            hdc = BeginPaint(hwnd , &ps);

            // �r�b�g�}�b�v�`��
            drawWorldTime->drawBitmap(hdc, hBitmap);

            // �e�L�X�g�`��
            drawWorldTime->drawText(hdc);

            EndPaint(hwnd , &ps);

            return 0;

        // �}�E�X�ړ���
        case WM_MOUSEMOVE:

            OnMouseMove(hwnd, drawWorldTime);
            return 0;

        // ���j���[
        case WM_SYSCOMMAND:

            switch(wparam){

                // About
                case IDM_WORLDTIME_ABOUT:
                    MessageBox(hwnd, IDC_STATIC_COPYRIGHT,
                               IDC_STATIC_APPNAME,
                               MB_ICONINFORMATION | MB_OK);
                    break;

                // ��Ɏ�O�ɕ\��
                case IDM_WORLDTIME_STAYTOP:

                    // ���j���[���g�O��
                    toggleMenuStayTop(hwnd);
                    break;

                // �c�[���`�b�v�\��
                case IDM_WORLDTIME_TOOLTIP:

                    // ���j���[���g�O��
                    toggleShowToolTip(hwnd);
                    break;

                default:
                    return DefWindowProc(hwnd, message, wparam, lparam);
                    break;
            }
    }

    return  DefWindowProc(hwnd, message, wparam, lparam);
}

/* ��ɍőO�ʃ��j���[���g�O��
   HWND hwnd �E�B���h�E�n���h��
*/
void toggleMenuStayTop(HWND hwnd){

    HMENU hMenu;
    MENUITEMINFO menuInfo;
    WorldTimeConfig* config;    // �ݒ�t�@�C���N���X

    config = new WorldTimeConfig();

    // �V�X�e�����j���[�n���h�����擾
    hMenu = GetSystemMenu(hwnd, FALSE);

    // ��ɍőO�ʂ̃��j���[�����擾
    menuInfo.cbSize = sizeof (MENUITEMINFO);
    menuInfo.fMask = MIIM_STATE;
    GetMenuItemInfo(hMenu, IDM_WORLDTIME_STAYTOP, FALSE, &menuInfo);

    // �`�F�b�N����Ă���ꍇ
    if(menuInfo.fState == MFS_CHECKED){

        // �`�F�b�N���O���őO�ʉ����E�ݒ�t�@�C���ɕۑ�
        menuInfo.fState = MFS_UNCHECKED;
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
        config->writeTopMostFlag(false);

    // �`�F�b�N����Ă��Ȃ��ꍇ
    }else{

        // �`�F�b�N���čőO�ʁE�ݒ�t�@�C���ɕۑ�
        menuInfo.fState = MFS_CHECKED;
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
        config->writeTopMostFlag(true);
    }

    delete(config);

    // �g�O������ݒ�
    SetMenuItemInfo(hMenu, IDM_WORLDTIME_STAYTOP, FALSE, &menuInfo);
}

/* �c�[���`�b�v�\�������g�O��
   HWND hwnd �E�B���h�E�n���h��
*/
void toggleShowToolTip(HWND hwnd){

    HMENU hMenu;
    MENUITEMINFO menuInfo;
    WorldTimeConfig* config;    // �ݒ�t�@�C���N���X

    config = new WorldTimeConfig();

    // �V�X�e�����j���[�n���h�����擾
    hMenu = GetSystemMenu(hwnd, FALSE);

    // �c�[���`�b�v�\���̃��j���[�����擾
    menuInfo.cbSize = sizeof (MENUITEMINFO);
    menuInfo.fMask = MIIM_STATE;
    GetMenuItemInfo(hMenu, IDM_WORLDTIME_TOOLTIP, FALSE, &menuInfo);

    // �`�F�b�N����Ă���ꍇ
    if(menuInfo.fState == MFS_CHECKED){

        // �`�F�b�N���O���Đݒ�t�@�C���ɕۑ�
        menuInfo.fState = MFS_UNCHECKED;
        config->writeToolTipFlag(false);

    // �`�F�b�N����Ă��Ȃ��ꍇ
    }else{

        // �`�F�b�N���Đݒ�t�@�C���ɕۑ�
        menuInfo.fState = MFS_CHECKED;
        config->writeToolTipFlag(true);
    }

    delete(config);

    // �g�O������ݒ�
    SetMenuItemInfo(hMenu, IDM_WORLDTIME_TOOLTIP, FALSE, &menuInfo);
}

/* �V�X�e�����j���[���폜
   HWND hwnd �E�B���h�E�n���h��
*/
void deleteMenu(HWND hwnd){

    HMENU hMenu;

    // �V�X�e�����j���[�n���h�����擾
    hMenu = GetSystemMenu(hwnd, FALSE);

    // �T�C�Y�ύX�E�ő剻�E���ɖ߂����j���[���폜
    DeleteMenu(hMenu, SC_SIZE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_RESTORE, MF_BYCOMMAND);
}

 /* ���j���[��ǉ�
   HWND hwnd �E�B���h�E�n���h��
   bool isTop �őO�ʕ\��
   bool isToolTipShow �c�[���`�b�v�\��
*/
void addMenu(HWND hwnd, bool isTop, bool isToolTipShow){

    HMENU hMenu;
    MENUITEMINFO menuInfo;

    hMenu = GetSystemMenu(hwnd, FALSE);

    // About ���j���[��ǉ�
    menuInfo.cbSize = sizeof(MENUITEMINFO);
    menuInfo.fMask = MIIM_TYPE | MIIM_ID;
    menuInfo.wID = IDM_WORLDTIME_ABOUT;
    menuInfo.fType = MFT_STRING;
    menuInfo.dwTypeData = TEXT("���E���v�ɂ���");

    InsertMenuItem(hMenu, 0, TRUE, &menuInfo);

    // ��Ɏ�O�ɕ\�����j���[��ǉ�
    menuInfo.cbSize = sizeof(MENUITEMINFO);
    menuInfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;

    // �����ݒ�l�ɂ���ă`�F�b�N��Ԃ�ݒ�
    if(isTop)
        menuInfo.fState = MFS_CHECKED;
    else
        menuInfo.fState = MFS_UNCHECKED;

    menuInfo.wID = IDM_WORLDTIME_STAYTOP;
    menuInfo.fType = MFT_STRING;
    menuInfo.dwTypeData = TEXT("��Ɏ�O�ɕ\��");

    InsertMenuItem(hMenu, 0, TRUE, &menuInfo);

    // �c�[���`�b�v�\�����j���[��ǉ�
    menuInfo.cbSize = sizeof(MENUITEMINFO);
    menuInfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;

    // �����ݒ�l�ɂ���ă`�F�b�N��Ԃ�ݒ�
    if(isToolTipShow)
        menuInfo.fState = MFS_CHECKED;
    else
        menuInfo.fState = MFS_UNCHECKED;

    menuInfo.wID = IDM_WORLDTIME_TOOLTIP;
    menuInfo.fType = MFT_STRING;
    menuInfo.dwTypeData = TEXT("�c�[���`�b�v��\��");

    InsertMenuItem(hMenu, 0, TRUE, &menuInfo);
}

/* �}�E�X�ړ���
    HWND hwnd �E�B���h�E�n���h��
    DrawWorldTime* drawWorldTime ���E�����`��N���X
*/
void OnMouseMove(HWND hwnd, DrawWorldTime* drawWorldTime){

    POINT po;                // �}�E�X�ʒu
    static DWORD nTickCount; // �N�����Ă���̎���
    static bool isBeforeChecked;
    HMENU hMenu;
    MENUITEMINFO menuInfo;

    // �Ō�̏�ԕω����� 500ms �����Ȃ牽�����Ȃ�
    if(GetTickCount() - nTickCount < 500)
        return;

    // �c�[���`�b�v�\���̃��j���[�����擾
    hMenu = GetSystemMenu(hwnd, FALSE);
    menuInfo.cbSize = sizeof (MENUITEMINFO);
    menuInfo.fMask = MIIM_STATE;
    GetMenuItemInfo(hMenu, IDM_WORLDTIME_TOOLTIP, FALSE, &menuInfo);

    // �c�[���`�b�v�\�����`�F�b�N����Ă���ꍇ
    if(menuInfo.fState == MFS_CHECKED){

        // ���݂̃}�E�X�ʒu���擾���ăN���C�A���g���W�ɕϊ�
        GetCursorPos(&po);
        ScreenToClient(hwnd, &po);

        // �c�[���`�b�v�\��
        drawWorldTime->showToolTip(&po);

        // ���݂̎��Ԃ�ۑ�
        nTickCount = GetTickCount();
isBeforeChecked = true;

    }else if(isBeforeChecked){
        // �c�[���`�b�v�񊈐���
        drawWorldTime->deActivateToolTip();

        // ���݂̎��Ԃ�ۑ�
        nTickCount = GetTickCount();
isBeforeChecked = false;
    }
}

/* �^�C�}�[�C�x���g������
    HWND hwnd �E�B���h�E�n���h��
    DrawWorldTime* drawWorldTime ���E�����`��N���X
*/
void OnTimer(HWND hwnd, DrawWorldTime* drawWorldTime){

    SYSTEMTIME stTime;                  // ���ݎ���
    static bool isTimeUpdated = false;  // �`�揈�������t���O

    // ���ݎ������擾
    GetSystemTime(&stTime);

    // 1 ���� 1 ����s
    if((stTime.wSecond == 0) && !isTimeUpdated){

        // ���ݎ������Z�b�g
        drawWorldTime->setCurrentTime();

        // �X�V�̈�ǉ� (WM_PAINT ���Ă΂��)
        InvalidateRect(hwnd, NULL, FALSE);

        // �������g�p�ʂ�}����
        HANDLE hProcess = GetCurrentProcess();
        SetProcessWorkingSetSize(hProcess, -1, -1);

        isTimeUpdated = true;

    }else if((stTime.wSecond != 0) && isTimeUpdated){
        isTimeUpdated = false;
    }
}

/* �f�o�b�O���O
   char* szBuf �f�o�b�O���b�Z�[�W
   �߂�l: �Ȃ�
*/
#ifdef __DEBUG__
void debug(TCHAR* szBuf){
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
