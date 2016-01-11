#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "advapi32.lib")

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

#include "MediaEngineWap.h"

struct FILEASSOCIATION
{
    TCHAR   szExt[10];
    int	    nImage;
    TCHAR   szDescription[MAX_PATH];
};

TCHAR szAppName[] = TEXT("SuPlayer");
TCHAR szAppClassName[] = TEXT("Media Player Classic");

#define FILEASSOCIATION_COUNT    24
FILEASSOCIATION fa[FILEASSOCIATION_COUNT] = 
{
    TEXT(".3gp"), 7, TEXT("�ƶ���Ƶ"),
    TEXT(".mpg"), 8, TEXT("MPEG �ļ�"),
    TEXT(".mpeg"), 8, TEXT("MPEG �ļ�"),
    TEXT(".dat"), 1, TEXT("MPEG �ļ�"),
    TEXT(".avi"), 5, TEXT("��Ƶ�ļ�"),
    TEXT(".rmvb"), 10, TEXT("RealVideo VBR"),
    TEXT(".rm"), 10, TEXT("RealMedia"),
    TEXT(".ra"), 10, TEXT("RealMedia"),
    TEXT(".ram"), 10, TEXT("RealMedia"),
    TEXT(".mp3"), 6, TEXT("MP3 ��Ƶ�ļ�"),
    TEXT(".mid"), 4, TEXT("MIDI ��Ƶ�ļ�"),
    TEXT(".midi"), 4, TEXT("MIDI ��Ƶ�ļ�"),
    TEXT(".wav"), 5, TEXT("Wave ��Ƶ�ļ�"),
    TEXT(".mov"), 9, TEXT("QuickTime ӰƬ"),
    TEXT(".qt"), 9, TEXT("QuickTime ӰƬ"),
    TEXT(".wmv"), 3, TEXT("Windows Media ��Ƶ�ļ�"),
    TEXT(".wma"), 2, TEXT("Windows Media ��Ƶ�ļ�"),
    TEXT(".asf"), 3, TEXT("Windows Media ��Ƶ�ļ�"),
    TEXT(".vob"), 13, TEXT("DVD �ļ�"),
    TEXT(".mkv"), 14, TEXT("Matroska ��Ƶ�ļ�"),
    TEXT(".mp4"), 14, TEXT("Matroska ��Ƶ�ļ�"),
    TEXT(".mka"), 14, TEXT("Matroska ��Ƶ�ļ�"),
    TEXT(".ogg"), 11, TEXT("Vorbis/Ogg ��Ƶ�ļ�"),
    TEXT(".omg"), 11, TEXT("Vorbis/Omg ��Ƶ�ļ�"),
};

MediaInfo    g_MediaInfo;

MediaEngine* g_pMediaEng;
HINSTANCE    g_hInstance;
HWND         g_hWnd;
HWND         g_hWndPanel;
HWND         g_hWndProgress;
HWND         g_hWndVolume;
HWND         g_hWndToolBar;
HWND         g_hWndVideoScreen;
HMENU        g_hMenu;
HCURSOR      g_hCursor;
bool         g_bTrackMouseDown;
int          g_TopMode;
bool         g_bHide;
bool         g_bSilence;
bool         g_bIsSeeking;
bool         g_bVideoFullScreen;
HICON        g_hIcon;
TCHAR        g_AppPath[MAX_PATH];
TCHAR        g_IclPath[MAX_PATH];

unsigned int lastMouseMovTime;
unsigned int entyFullScreenTime;

unsigned int ctrlPanelWndWidth;
unsigned int ctrlPanelWndHeight;

unsigned int mainWndWidth;
unsigned int mainWndHeight;
unsigned int videoWndWidth;
unsigned int videoWndHeight;
unsigned int mediaContentDuration;

WNDPROC oldProgressProc;
WNDPROC oldVolumeProc;
WNDPROC oldToolbarProc;
WNDPROC oldVideoScreenProc;

WINDOWPLACEMENT ctrlPanelState;

TCHAR g_StatusBarText1[10];
TCHAR g_StatusBarText2[200];
TCHAR g_StatusBarText3[128];

TCHAR g_PlaybakTimeText[128];
TCHAR g_MediaDurationText[64];
TCHAR g_PlaybakPositionText[64];

RECT rt_StatusBarText1 = {23, 49, 70, 69};
RECT rt_StatusBarText2 = {75, 49, 200, 69};
RECT rt_StatusBarText3 = {23, 49, 70, 69};

#define     TB_PLAY         0
#define     TB_PAUSE        1
#define     TB_STOP         2
#define     TB_PREV         3
#define     TB_DECREASERATE 4
#define     TB_INCREASERATE 5
#define     TB_NEXT         6
#define     TB_NEXTFRAME    7
#define     TB_SOUND        8
#define     TB_SEPARATE     0
#define     TB_NONE         100

#define     MAX_PROGRESS    10000

#define     TIMER_GETPRGRESS    1
#define     TIMER_MOUSEMOV      2

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK PanelWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ProgressWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK VolumeWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ToolbarWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK VideoScreenWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

typedef BOOL (WINAPI * SHGIL_PROC)	(HIMAGELIST *phLarge, HIMAGELIST *phSmall);

void CreatePanel(HWND hWndParent);
void SetProgrss(HWND hWnd, int nValue);
int GetProgress(HWND hWnd);
void UpdatePlayProgressBar(unsigned int pos);
BOOL SetToolBarState(int iButton, int iState);
void EnableMenu(int nID, bool flag = true);
void CheckMenu(int nID, bool flag = true);
void EnableWindow(HWND hWnd, bool flag = true);
void CreateVideoScreenWindow(HWND hWndParent);
void UpdateVideoScreen(unsigned int nWidth, unsigned int nHeight);
void UpdateMainWindow(unsigned int nVideoW, unsigned int nVideoH, float scale);
int MediaoNotifyHandler(void* pUserData, int ID, void* pParam1, void* pParam2);

void Play();
void Pause();
void Stop();
void Close();
void PlayOrPause();
void Seek(unsigned int seekPos);
void UpdatePlayPostion();
void SetMediaDuration(unsigned int duration);
void DecreaseRate();
void IncreaseRate();
void ResetRate();
void StepFrame();
void SetTopMode(int nMode);
void PlayPrev();
void PlayNext(bool random = false);
void CenterWindow(HWND hwnd);
bool PlayFile(const TCHAR* pszFileName);
void SplitFileName(const TCHAR* pszFileName, int nFlag, TCHAR* pReturn);

void VideoEntryFullScreen();
void VideoExitFullScreen();
void ControlPanelEntryFullScreen();
void ControlPanelExitFullScreen();
bool IsPointInCtrlPanelRect(POINT point);
void ShowFullScreenCtrlPanel();
void HideFullScreenCtrlPanel();

/* Volume */
UINT g_nNumMixers;      //�����������
HMIXER g_hMixer;        //��ǰ������ľ��
MIXERCAPS g_mxcaps;     //��ǰ����������ܲ���
DWORD g_dwMinimum, g_dwMaximum; //�����С������ֵ
DWORD g_dwVolumeControlID;      //������ؼ���ID

BOOL amdInitialize();
BOOL amdUninitialize();
BOOL amdIsMasterVolumeMute();
BOOL amdGetMasterVolumeControl();
BOOL amdGetMasterVolumeValue(DWORD &dwVal);
BOOL amdSetMasterVolumeValue(DWORD dwVal);
BOOL amdSetMasterVolumeMute(BOOL mute);
void InitSystemVolumeControl();
void UninitSystemVolumeControl();
void SetSystemVolumeMute(BOOL mute);
void SetSystemVolumeValue(DWORD value);
/* Volume */

/* DubPrintf*/
void DebugPrintf(char* lpszFormat, ...);
