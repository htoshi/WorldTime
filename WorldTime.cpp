#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "DrawWorldTime.h"
#include "WorldTimeConfig.h"

/* メニューID設定 */
#define IDM_WORLDTIME_ABOUT 1
#define IDM_WORLDTIME_STAYTOP 2
#define IDM_WORLDTIME_TOOLTIP 3

/* 定数 */
#define IDC_STATIC_WINDOWCLASSNAME "WorldTime"
#define IDC_STATIC_APPNAME "世界時計"
#define IDC_STATIC_AUTHOR "Toshi"
#define IDC_STATIC_VERSION "Version 0.1 Alpha"
#define IDC_STATIC_COPYRIGHT \
		IDC_STATIC_APPNAME " " IDC_STATIC_VERSION " " IDC_STATIC_AUTHOR

// #define __DEBUG__ 1

/* プロトタイプ宣言 */
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

/* アプリケーションエントリーポイント */
int WINAPI WinMain(HINSTANCE    hInstance,
                   HINSTANCE    hPrevInstance,
                   LPSTR        CmdLine,
                   int          CmdShow){

    MSG msg;

    // 多重起動防止
    HWND hWnd = FindWindow(IDC_STATIC_WINDOWCLASSNAME, NULL);

    if(hWnd){
        // アイコン化されていれば元のサイズに戻す
        if (IsIconic(hWnd))
            ShowWindow(hWnd, SW_RESTORE);

        // 見つかったウィンドウをフォアグラウンドにする
        SetForegroundWindow(::GetLastActivePopup(hWnd));

        return FALSE;
    }

    // ウィンドウ生成
    if(!InitInstance(hInstance, CmdShow)){
        MessageBox(NULL, TEXT("初期化時にエラーが発生しました"),
                         IDC_STATIC_APPNAME, MB_ICONINFORMATION | MB_OK);

        return FALSE;
    }

    // メッセージループ
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

/* ウィンドウ生成
    HINSTANCE hInstance
    int CmdShow
*/
BOOL InitInstance(HINSTANCE hInstance, int CmdShow){

    HWND             hwnd;
    WNDCLASS         wc;
    WorldTimeConfig* config;    // 設定ファイルクラス
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

    if(RegisterClass(&wc)==0) // ウインドウクラス登録
        return FALSE;

    /* 設定ファイルから設定を読み込み */
    config = new WorldTimeConfig();

    if(config->readWindowRect(&Rect) != OK ||
       config->readTopMostFlag(&isTop) != OK ||
       config->readToolTipFlag(&isToolTipShow) != OK){

        delete(config);
        return FALSE;
    }

    delete(config);

    // ウインドウ作成
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

    ShowWindow(hwnd, CmdShow);   // ウインドウの表示
    UpdateWindow(hwnd);          // ウインドウの最初の更新
    deleteMenu(hwnd);            // システムメニュー削除

    // 常に最前面に表示
    if(isTop)
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));

    // メニュー追加
    addMenu(hwnd, isTop, isToolTipShow);

    // 最小分解能 100ms インターバル 500ms でタイマーセット
    SetTimer(hwnd, 100, 500, NULL);

    // メモリ使用量を抑える
    HANDLE hProcess = GetCurrentProcess();
    SetProcessWorkingSetSize(hProcess, -1, -1);

    return TRUE;
}

/* ウインドウプロシージャ */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message,
                            WPARAM wparam, LPARAM lparam){

    static HBITMAP hBitmap;               // ビットマップオブジェクト
    static DrawWorldTime* drawWorldTime;  // 世界時刻描画クラス

    switch(message) {

        case WM_DESTROY:

            // 世界時刻描画クラス削除
            delete(drawWorldTime);

            // ウィンドウサイズの設定を保存
            RECT Rect;
            GetWindowRect(hwnd, (LPRECT)&Rect);

            // 設定ファイルクラス
            WorldTimeConfig* config;

            config = new WorldTimeConfig();
            config->writeWindowRect(&Rect);

            delete(config);

            PostQuitMessage(0);
            return  0;

        case WM_CREATE:

            // ビットマップオブジェクトの読み込み
            hBitmap = LoadBitmap(((LPCREATESTRUCT)lparam)->hInstance,
                                 TEXT("WORLDMAP"));

            // 世界時刻描画クラス生成
            drawWorldTime = new DrawWorldTime(hwnd, lparam);

            // 現在時刻をセット
            drawWorldTime->setCurrentTime();

            return 0;

        // タイマーイベント発生時
        case WM_TIMER:

            OnTimer(hwnd, drawWorldTime);
            return 0;

        // 描画イベント
        case WM_PAINT:

            PAINTSTRUCT ps;
            HDC hdc;

            hdc = BeginPaint(hwnd , &ps);

            // ビットマップ描画
            drawWorldTime->drawBitmap(hdc, hBitmap);

            // テキスト描画
            drawWorldTime->drawText(hdc);

            EndPaint(hwnd , &ps);

            return 0;

        // マウス移動時
        case WM_MOUSEMOVE:

            OnMouseMove(hwnd, drawWorldTime);
            return 0;

        // メニュー
        case WM_SYSCOMMAND:

            switch(wparam){

                // About
                case IDM_WORLDTIME_ABOUT:
                    MessageBox(hwnd, IDC_STATIC_COPYRIGHT,
                               IDC_STATIC_APPNAME,
                               MB_ICONINFORMATION | MB_OK);
                    break;

                // 常に手前に表示
                case IDM_WORLDTIME_STAYTOP:

                    // メニューをトグル
                    toggleMenuStayTop(hwnd);
                    break;

                // ツールチップ表示
                case IDM_WORLDTIME_TOOLTIP:

                    // メニューをトグル
                    toggleShowToolTip(hwnd);
                    break;

                default:
                    return DefWindowProc(hwnd, message, wparam, lparam);
                    break;
            }
    }

    return  DefWindowProc(hwnd, message, wparam, lparam);
}

/* 常に最前面メニューをトグル
   HWND hwnd ウィンドウハンドル
*/
void toggleMenuStayTop(HWND hwnd){

    HMENU hMenu;
    MENUITEMINFO menuInfo;
    WorldTimeConfig* config;    // 設定ファイルクラス

    config = new WorldTimeConfig();

    // システムメニューハンドルを取得
    hMenu = GetSystemMenu(hwnd, FALSE);

    // 常に最前面のメニュー情報を取得
    menuInfo.cbSize = sizeof (MENUITEMINFO);
    menuInfo.fMask = MIIM_STATE;
    GetMenuItemInfo(hMenu, IDM_WORLDTIME_STAYTOP, FALSE, &menuInfo);

    // チェックされている場合
    if(menuInfo.fState == MFS_CHECKED){

        // チェックを外し最前面解除・設定ファイルに保存
        menuInfo.fState = MFS_UNCHECKED;
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
        config->writeTopMostFlag(false);

    // チェックされていない場合
    }else{

        // チェックして最前面・設定ファイルに保存
        menuInfo.fState = MFS_CHECKED;
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
        config->writeTopMostFlag(true);
    }

    delete(config);

    // トグル情報を設定
    SetMenuItemInfo(hMenu, IDM_WORLDTIME_STAYTOP, FALSE, &menuInfo);
}

/* ツールチップ表示情報をトグル
   HWND hwnd ウィンドウハンドル
*/
void toggleShowToolTip(HWND hwnd){

    HMENU hMenu;
    MENUITEMINFO menuInfo;
    WorldTimeConfig* config;    // 設定ファイルクラス

    config = new WorldTimeConfig();

    // システムメニューハンドルを取得
    hMenu = GetSystemMenu(hwnd, FALSE);

    // ツールチップ表示のメニュー情報を取得
    menuInfo.cbSize = sizeof (MENUITEMINFO);
    menuInfo.fMask = MIIM_STATE;
    GetMenuItemInfo(hMenu, IDM_WORLDTIME_TOOLTIP, FALSE, &menuInfo);

    // チェックされている場合
    if(menuInfo.fState == MFS_CHECKED){

        // チェックを外して設定ファイルに保存
        menuInfo.fState = MFS_UNCHECKED;
        config->writeToolTipFlag(false);

    // チェックされていない場合
    }else{

        // チェックして設定ファイルに保存
        menuInfo.fState = MFS_CHECKED;
        config->writeToolTipFlag(true);
    }

    delete(config);

    // トグル情報を設定
    SetMenuItemInfo(hMenu, IDM_WORLDTIME_TOOLTIP, FALSE, &menuInfo);
}

/* システムメニューを削除
   HWND hwnd ウィンドウハンドル
*/
void deleteMenu(HWND hwnd){

    HMENU hMenu;

    // システムメニューハンドルを取得
    hMenu = GetSystemMenu(hwnd, FALSE);

    // サイズ変更・最大化・元に戻すメニューを削除
    DeleteMenu(hMenu, SC_SIZE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_RESTORE, MF_BYCOMMAND);
}

 /* メニューを追加
   HWND hwnd ウィンドウハンドル
   bool isTop 最前面表示
   bool isToolTipShow ツールチップ表示
*/
void addMenu(HWND hwnd, bool isTop, bool isToolTipShow){

    HMENU hMenu;
    MENUITEMINFO menuInfo;

    hMenu = GetSystemMenu(hwnd, FALSE);

    // About メニューを追加
    menuInfo.cbSize = sizeof(MENUITEMINFO);
    menuInfo.fMask = MIIM_TYPE | MIIM_ID;
    menuInfo.wID = IDM_WORLDTIME_ABOUT;
    menuInfo.fType = MFT_STRING;
    menuInfo.dwTypeData = TEXT("世界時計について");

    InsertMenuItem(hMenu, 0, TRUE, &menuInfo);

    // 常に手前に表示メニューを追加
    menuInfo.cbSize = sizeof(MENUITEMINFO);
    menuInfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;

    // 初期設定値によってチェック状態を設定
    if(isTop)
        menuInfo.fState = MFS_CHECKED;
    else
        menuInfo.fState = MFS_UNCHECKED;

    menuInfo.wID = IDM_WORLDTIME_STAYTOP;
    menuInfo.fType = MFT_STRING;
    menuInfo.dwTypeData = TEXT("常に手前に表示");

    InsertMenuItem(hMenu, 0, TRUE, &menuInfo);

    // ツールチップ表示メニューを追加
    menuInfo.cbSize = sizeof(MENUITEMINFO);
    menuInfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;

    // 初期設定値によってチェック状態を設定
    if(isToolTipShow)
        menuInfo.fState = MFS_CHECKED;
    else
        menuInfo.fState = MFS_UNCHECKED;

    menuInfo.wID = IDM_WORLDTIME_TOOLTIP;
    menuInfo.fType = MFT_STRING;
    menuInfo.dwTypeData = TEXT("ツールチップを表示");

    InsertMenuItem(hMenu, 0, TRUE, &menuInfo);
}

/* マウス移動時
    HWND hwnd ウィンドウハンドル
    DrawWorldTime* drawWorldTime 世界時刻描画クラス
*/
void OnMouseMove(HWND hwnd, DrawWorldTime* drawWorldTime){

    POINT po;                // マウス位置
    static DWORD nTickCount; // 起動してからの時間
    static bool isBeforeChecked;
    HMENU hMenu;
    MENUITEMINFO menuInfo;

    // 最後の状態変化から 500ms 未満なら何もしない
    if(GetTickCount() - nTickCount < 500)
        return;

    // ツールチップ表示のメニュー情報を取得
    hMenu = GetSystemMenu(hwnd, FALSE);
    menuInfo.cbSize = sizeof (MENUITEMINFO);
    menuInfo.fMask = MIIM_STATE;
    GetMenuItemInfo(hMenu, IDM_WORLDTIME_TOOLTIP, FALSE, &menuInfo);

    // ツールチップ表示がチェックされている場合
    if(menuInfo.fState == MFS_CHECKED){

        // 現在のマウス位置を取得してクライアント座標に変換
        GetCursorPos(&po);
        ScreenToClient(hwnd, &po);

        // ツールチップ表示
        drawWorldTime->showToolTip(&po);

        // 現在の時間を保存
        nTickCount = GetTickCount();
isBeforeChecked = true;

    }else if(isBeforeChecked){
        // ツールチップ非活性化
        drawWorldTime->deActivateToolTip();

        // 現在の時間を保存
        nTickCount = GetTickCount();
isBeforeChecked = false;
    }
}

/* タイマーイベント発生時
    HWND hwnd ウィンドウハンドル
    DrawWorldTime* drawWorldTime 世界時刻描画クラス
*/
void OnTimer(HWND hwnd, DrawWorldTime* drawWorldTime){

    SYSTEMTIME stTime;                  // 現在時刻
    static bool isTimeUpdated = false;  // 描画処理完了フラグ

    // 現在時刻を取得
    GetSystemTime(&stTime);

    // 1 分に 1 回実行
    if((stTime.wSecond == 0) && !isTimeUpdated){

        // 現在時刻をセット
        drawWorldTime->setCurrentTime();

        // 更新領域追加 (WM_PAINT が呼ばれる)
        InvalidateRect(hwnd, NULL, FALSE);

        // メモリ使用量を抑える
        HANDLE hProcess = GetCurrentProcess();
        SetProcessWorkingSetSize(hProcess, -1, -1);

        isTimeUpdated = true;

    }else if((stTime.wSecond != 0) && isTimeUpdated){
        isTimeUpdated = false;
    }
}

/* デバッグログ
   char* szBuf デバッグメッセージ
   戻り値: なし
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

    // ファイル作成
    hFile = CreateFile("debug.log", GENERIC_WRITE,
                        FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE){
        MessageBox(0, TEXT("ファイルオープンエラー"),
                      TEXT("エラー"), MB_ICONSTOP);
        return;
    }

    // ファイル最終端にポインタを移動
    SetFilePointer(hFile , 0 , NULL , FILE_END);

    // 書き込み
    bRet = WriteFile(hFile, szLogMsg, (DWORD)strlen(szLogMsg), &dwWritten, NULL);

    CloseHandle(hFile);
}

#endif /* __DEBUG__ */
