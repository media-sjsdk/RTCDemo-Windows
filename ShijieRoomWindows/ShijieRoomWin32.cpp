// ShijeRoomWin32.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ShijieRoomWin32.h"
#include "RoomCallback.h"
#define MAX_LOADSTRING 100
#define ARRAY_LEN 256
#define VIDEO_X_PIXEL 640
#define VIDEO_Y_PIXEL 360
#define COUNT_ROOM 12
using namespace shijie;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//
DeviceManagerCallback deviceCallback;
RoomCallback roomCallback;
WindowsRenderManager* renderManager = NULL;
DeviceManager *deviceManager = NULL;
bool start_flag = false;
HWND hVideoWinLocal = 0;
shijie::Room *room = NULL;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPWSTR    lpCmdLine,
        _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    char setting_url[ARRAY_LEN];
    freopen("conferenceDemo.log", "w", stdout);
    FILE *file_setting = fopen("setting.ini", "r+");
    if (file_setting) {
        fgets(setting_url, ARRAY_LEN, file_setting);
        fclose(file_setting);
        file_setting = NULL;
    }
    // Initialize global strings

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SHIJEROOMWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SHIJEROOMWIN32));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDB_PNG1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDB_PNG1));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_DLGFRAME | WS_SYSMENU,
            0, 0, VIDEO_X_PIXEL + 20,
            VIDEO_Y_PIXEL + 180, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }
    if (hVideoWinLocal == 0)
    {
        hVideoWinLocal = CreateWindow(szWindowClass,
                szTitle,
                WS_CHILD | WS_VISIBLE,
                0,
                0,
                VIDEO_X_PIXEL,
                VIDEO_Y_PIXEL,
                hWnd,//parent window handle.
                NULL,
                hInst,
                NULL);

        if (hVideoWinLocal == 0) {
            int error = GetLastError();
            MessageBox(NULL, TEXT("hVideoWinLocal error"), 0, 0);
        }
        else {
            deviceCallback.setlocalWindow(hVideoWinLocal);
            roomCallback.setWindowsparentHWNDforRender(hWnd, hInst, szTitle, szWindowClass);
            roomCallback.setlocalWindow(hVideoWinLocal);
        }

    }

    int sizeFrameX = GetSystemMetrics(SM_CXFRAME);
    int sizeFrameY = GetSystemMetrics(SM_CYFRAME);
    int paddingBorderX = GetSystemMetrics(SM_CXPADDEDBORDER);
    int paddingBorderY = GetSystemMetrics(SM_CXPADDEDBORDER);
    int captionHeight = GetSystemMetrics(SM_CYCAPTION);
    std::vector<HWND> windowsforrender;
    for (int i = 0; i < COUNT_ROOM; i++) {
        HWND hwnd = CreateWindow(szWindowClass,
                TEXT("userid"),
                WS_TILED | SW_HIDE,
                VIDEO_X_PIXEL,
                0,
                640 + paddingBorderX * 2 + sizeFrameX * 2,
                360 + captionHeight + paddingBorderY * 2 + sizeFrameY * 2,
                hWnd,//parent window handle.
                NULL,
                hInst,
                NULL);
        windowsforrender.push_back(hwnd);
        ShowWindow(hwnd, SW_HIDE);
    }
    roomCallback.setWindowsForRender(windowsforrender);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hStatic_LocalName;
    HWND hStaticRoomId;
	HWND hStaticServerId;
    HWND hButton_Call;
    HWND hButton_Cancel;
    HWND hEdit_LocalName;
    HWND hEdit_RemoteName;
	HWND hEdit_ServerDomain;
    HWND hEdit_IsCaller;
    HWND hButton_Exit;

    HWND hComBox_Resolute = 0;
    HWND hComBox_Role = 0;
    HWND hStatic_Resolute;
    HWND hStatic_Role;
    int Button_Width = 65;
    int Button_Height = 25;
    int Button_XPos = 10;
    int Button_YPos = 380;
    int labelWidth = 65;
	int controlWidth = 130;
    int windows_width = VIDEO_X_PIXEL;
    int windows_height = VIDEO_Y_PIXEL;

    TCHAR LocalName_TCHAR[ARRAY_LEN] = { 0 };
    wchar_t LocalName_WCHAR_T[ARRAY_LEN] = { 0 };
    char tmp_LocalName_CHAR[ARRAY_LEN] = { 0 };
    TCHAR RemoteName_TCHAR[ARRAY_LEN] = { 0 };
    wchar_t RoomId_WCHAR_T[ARRAY_LEN] = { 0 };
    char tmp_RoomId_CHAR[ARRAY_LEN] = { 0 };
    TCHAR IsCaller_TCHAR[ARRAY_LEN] = { 0 };
    wchar_t IsCaller_WCHAR_T[ARRAY_LEN] = { 0 };
    char tmp_IsCaller_CHAR[ARRAY_LEN] = { 0 };

    TCHAR role[3][50] = {TEXT("会议-参与者"),TEXT("直播-主播"),TEXT("直播-观众")};
    Win32ClientRole user_select_role[3] = { CLIENT_ROLE_ATTENDEE , CLIENT_ROLE_COHOST, CLIENT_ROLE_VIEWER};
    TCHAR resolution[8][50] ={ TEXT("320x180"),TEXT("320x180[自适应]"),TEXT("640x360"),TEXT("640x360[自适应]"),TEXT("960x540"),TEXT("960x540[自适应]"),TEXT("1280x720"),TEXT("1280x720[自适应]")};
    struct video_resulations user_resulations[8] ={{320,180,false},{320,180,true},{640,360,false},{640,360,true},{960,540,false},{960,540,true},{1280,720,false},{1280,720,true}};

    switch (message)
    {
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, RGB(255, 255, 255));
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
		}

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                RECT rect;
                GetClientRect(hWnd, &rect);
                DrawText(hdc, TEXT("There is no video."), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                EndPaint(hWnd, &ps);
                if (hVideoWinLocal) {
                    UpdateWindow(hVideoWinLocal);
                }
            }
            break;
        case WM_CREATE:
            {
                if (!deviceManager)
                    deviceManager = new DeviceManager(&deviceCallback);

                if (!renderManager) {
                    renderManager = new WindowsRenderManager();
                    deviceCallback.setRenderManger(renderManager);
                    roomCallback.setRenderManger(renderManager);
                }

                if (!room) {
                    room = new Room((long)deviceManager->getDeviceManagerInstance(), "ShijieWinDemo", 4003, "com.shijie.voipclient",(IRoomEventHandler*) &roomCallback);
                }

                hStatic_LocalName = (HWND)CreateWindow(TEXT("Static"),
                        TEXT("用户名："),
                        WS_VISIBLE | WS_CHILD,
                        Button_XPos, Button_YPos, labelWidth, Button_Height,
                        hWnd,
                        (HMENU)LOCALNAME,
                        hInst,
                        NULL);
                hEdit_LocalName = (HWND)CreateWindow(TEXT("EDIT"),
                        TEXT("1001"),
                        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                        Button_XPos + labelWidth, Button_YPos, controlWidth, Button_Height,
                        hWnd,
                        (HMENU)ELOCALNAME,
                        hInst,
                        NULL);

                hStaticRoomId = (HWND)CreateWindow(TEXT("Static"),
                        TEXT("房间名："),
                        WS_VISIBLE | WS_CHILD,
                        Button_XPos + VIDEO_X_PIXEL / 3, Button_YPos, labelWidth, Button_Height,
                        hWnd,
                        (HMENU)REMOTENAME,
                        hInst,
                        NULL);

                hEdit_RemoteName = (HWND)CreateWindow(TEXT("EDIT"),
                        TEXT("8787"),
                        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                        Button_XPos + VIDEO_X_PIXEL/3 + labelWidth, Button_YPos, controlWidth, Button_Height,
                        hWnd,
                        (HMENU)EREMOTENAME,
                        hInst,
                        NULL);

				hStaticServerId = (HWND)CreateWindow(TEXT("Static"),
                        TEXT("服务器："),
                        WS_VISIBLE | WS_CHILD,
                        Button_XPos + VIDEO_X_PIXEL * 2 / 3, Button_YPos, labelWidth, Button_Height,
                        hWnd,
                        (HMENU)SERVERNAME,
                        hInst,
                        NULL);

                hEdit_ServerDomain = (HWND)CreateWindow(TEXT("EDIT"),
                        TEXT("mcu.sjsdk.com"),
                        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                        Button_XPos + VIDEO_X_PIXEL * 2 / 3 + labelWidth, Button_YPos, controlWidth, Button_Height,
                        hWnd,
                        (HMENU)ESERVERNAME,
						hInst,
                        NULL);


                hButton_Call = (HWND)CreateWindow(TEXT("Button"),
                        TEXT("开始"),
                        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        Button_XPos + (VIDEO_X_PIXEL / 2 - controlWidth) / 2, Button_YPos + 80, controlWidth, Button_Height,
                        hWnd,
                        (HMENU)BCALL,
                        hInst,
                        NULL);

                hButton_Cancel = (HWND)CreateWindow(TEXT("Button"),
                        TEXT("离开"),
                        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        Button_XPos + (VIDEO_X_PIXEL / 2 - controlWidth) / 2 + VIDEO_X_PIXEL / 2, Button_YPos + 80, controlWidth, Button_Height,
                        hWnd,
                        (HMENU)BCANCEL,
                        hInst,
                        NULL);

                hStatic_Resolute = (HWND)CreateWindow(TEXT("Static"),
                        TEXT("分辨率："),
                        WS_VISIBLE | WS_CHILD,
                        Button_XPos, Button_YPos + 40 , labelWidth, Button_Height,
                        hWnd,
                        (HMENU)REMOTENAME,
                        hInst,
                        NULL);

                hComBox_Resolute = (HWND)CreateWindow(TEXT("COMBOBOX"),
                        TEXT("DROPWINDOWLIST"),
                        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                        Button_XPos + labelWidth, Button_YPos + 40, controlWidth, 6*Button_Height,
                        hWnd,
                        (HMENU)COMBOXRESOLUTE,
                        hInst,
                        NULL);

                hStatic_Role = (HWND)CreateWindow(TEXT("Static"),
                        TEXT("角色:"),
                        WS_VISIBLE | WS_CHILD,
                        Button_XPos + VIDEO_X_PIXEL /3 , Button_YPos + 40, labelWidth, Button_Height,
                        hWnd,
                        (HMENU)REMOTENAME,
                        hInst,
                        NULL);

                hComBox_Role = (HWND)CreateWindow(TEXT("COMBOBOX"),
                        TEXT("DROPWINDOWLIST"),
                        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST ,
                        Button_XPos + VIDEO_X_PIXEL / 3 + labelWidth, Button_YPos + 40, 2* Button_Width, 3 * Button_Height,
                        hWnd,
                        (HMENU)COMBOXROLE,
                        hInst,
                        NULL);
                for (int i = 0; i< 3; i++){
                    SendMessage(hComBox_Role, CB_ADDSTRING, 0, (LPARAM)(role[i]));
                }
                for ( int j =0; j< 8; j ++) {
                    SendMessage(hComBox_Resolute, CB_ADDSTRING, 0, (LPARAM)resolution[j]);
                }
                UpdateWindow(hWnd);
                SendMessage(hComBox_Resolute, CB_SETCURSEL, 0, 0);
                SendMessage(hComBox_Role, CB_SETCURSEL, 0, 0);
            }
            break;
        case WM_COMMAND:
            {

                if (LOWORD(wParam) == BCALL && HIWORD(wParam) == BN_CLICKED)
                {

                    if (start_flag) {
                        MessageBox(NULL, TEXT("你已经进入房间！"), TEXT("提示"), MB_OK);
                        break;
                    }
                    start_flag = true;
                    std::string localName = "";
                    std::string roomId = "";
                    bool isCaller = false;

                    memset(tmp_LocalName_CHAR, 0, ARRAY_LEN);
                    GetWindowText(GetDlgItem(hWnd, ELOCALNAME), LocalName_WCHAR_T, ARRAY_LEN);
                    TcharToChar(LocalName_WCHAR_T, tmp_LocalName_CHAR);
                    localName = std::string(tmp_LocalName_CHAR);
                    memset(tmp_RoomId_CHAR, 0, ARRAY_LEN);
                    
					GetWindowText(GetDlgItem(hWnd, EREMOTENAME), RoomId_WCHAR_T, ARRAY_LEN);
                    TcharToChar(RoomId_WCHAR_T, tmp_RoomId_CHAR);
                    roomId = std::string(tmp_RoomId_CHAR);

					wchar_t wcharServerAddress[ARRAY_LEN] = { 0 };
					char serverAddress[ARRAY_LEN] = { 0 };
					GetWindowText(GetDlgItem(hWnd, ESERVERNAME), wcharServerAddress, ARRAY_LEN);
					TcharToChar(wcharServerAddress, serverAddress);

                    int index_role = SendMessage(GetDlgItem(hWnd, COMBOXROLE), CB_GETCURSEL, 0, 0);
                    int index_resolute = SendMessage(GetDlgItem(hWnd, COMBOXRESOLUTE), CB_GETCURSEL, 0, 0);

                    //MessageBox(NULL, role[index_role], resolution[index_resolute], 0);
                    Win32RoomProfile profile;
                    profile.joinWithoutVideo = false;
                    profile.role = user_select_role[index_role];
                    roomCallback.setcurrentRole(user_select_role[index_role]);
                    profile.startMixer = false;
                    profile.videoWidth = user_resulations[index_resolute].width;
                    profile.videoHeight = user_resulations[index_resolute].height;
					profile.enableAdaptiveResolution = user_resulations[index_resolute].enableAdaptiveResolution;
					profile.serverAddress = serverAddress;
                    deviceManager->setTargetVideo(profile.videoWidth, profile.videoHeight,15);
                    room->joinRoom(roomId, localName, profile, "NTFmNTExYWFmMGFhMjFlY2RjYTdjMTdlZDdiYmI2NGFiNzlhMmI3ZS0xNTU2MTcwMTQ4");
                    if (profile.role == CLIENT_ROLE_VIEWER) {
                        deviceManager->startCamera(false);
                        deviceManager->startMicrophone(false);
                    }
                    else {
                        deviceManager->startCamera(true);

                    }
                    deviceManager->startSpeaker(true);
                    deviceManager->startMicrophone(true);
                }
                else if (LOWORD(wParam) == BCANCEL && HIWORD(wParam) == BN_CLICKED)
                {
                    roomCallback.ClearWindowsAndRenders();
                    deviceManager->startCamera(false);
                    deviceManager->startMicrophone(false);
                    deviceManager->startSpeaker(false);
                    room->leaveRoom();
                    start_flag = false;
                }
                else if (LOWORD(wParam) == BEXIT && HIWORD(wParam) == BN_CLICKED)
                {
                    //MessageBox(hWnd, TEXT("Exit Demo!"), TEXT("Button"), MB_OK);
                    roomCallback.ClearWindowsAndRenders();
                    room->leaveRoom();
                    delete room;
                    room = NULL;
                    deviceManager->startCamera(false);
                    deviceManager->startMicrophone(false);
                    deviceManager->startSpeaker(false);
                    delete deviceManager;
                    deviceManager = NULL;
                    delete renderManager;
                    renderManager = NULL;
                }
                // Parse the menu selections:
                //int wmId = LOWORD(wParam);
                //switch (wmId)
                //{
                //case IDM_ABOUT:
                //   DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                //   break;
                //case IDM_EXIT:
                //    DestroyWindow(hWnd);
                //    break;
                //default:
                // return DefWindowProc(hWnd, message, wParam, lParam);
                // }
            }
            break;

        case WM_TIMER:
            break;
        case WM_DESTROY:
            if (room)
            {
                room->leaveRoom();
                roomCallback.ClearWindowsAndRenders();
                delete room;
                room = NULL;
            }

            if (deviceManager)
            {
                deviceManager->startCamera(false);
                deviceManager->startMicrophone(false);
                deviceManager->startSpeaker(false);
                delete deviceManager;
                deviceManager = NULL;
            }

            if (renderManager)
            {
                delete renderManager;
                renderManager = NULL;
            }
            PostQuitMessage(0);
            break;
        case WM_CLOSE: 
			DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}
