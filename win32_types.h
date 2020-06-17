
#if  defined(_WIN64) || defined(_WIN32)
#include <windows.h>
struct __POSITION { int unused; };
typedef __POSITION* POSITION;
#else
#define INVALID_HANDLE_VALUE  -1
#define _MAX_PATH 260

#define HANDLE      int
#define HWND              int
#define HINSTANCE              int
#define HMENU              int
#define HBRUSH       int

#define MAX_PATH   260
//#define TRUE true
//#define FALSE false
#define __stdcall
#define __declspec(x)
#define __cdecl
//#define max(a,b) (((a) > (b)) ? (a) : (b))
//#define min(a,b) (((a) < (b)) ? (a) : (b))

typedef bool BOOL;
typedef unsigned char BYTE;
typedef float FLOAT;
typedef FLOAT   *PFLOAT;
typedef char  CHAR;
typedef wchar_t  TCHAR;
typedef unsigned char UCHAR;
typedef unsigned char *PUCHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef unsigned short *PUSHORT;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long long    LONGLONG;
typedef unsigned long long ULONGLONG;
typedef ULONGLONG *PULONGLONG;
typedef unsigned long ULONG;
typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;
typedef void VOID;

typedef char *LPSTR;
typedef char *LPTSTR;
typedef const char *LPCSTR;
typedef wchar_t WCHAR;
typedef WCHAR *LPWSTR;
typedef const WCHAR *LPCWSTR;
typedef DWORD *LPDWORD;
typedef unsigned long UINT_PTR;
typedef UINT_PTR SIZE_T;
typedef LONGLONG USN;
typedef BYTE BOOLEAN;
typedef void  *PVOID;
typedef void  *LPVOID;
typedef long long       __int64;
typedef LONG LRESULT;

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
  } FILETIME,*PFILETIME,*LPFILETIME;

typedef union _ULARGE_INTEGER {
 struct {
 DWORD LowPart;
 DWORD HighPart;
 };
 struct {
 DWORD LowPart;
 DWORD HighPart;
 } u;
 ULONGLONG QuadPart;
} ULARGE_INTEGER,
*PULARGE_INTEGER;


typedef const wxChar *LPCTSTR;

#define LF_FACESIZE 256
typedef struct tagLOGFONT
{
    LONG lfHeight;
	LONG lfWidth;
	LONG lfEscapement;
	LONG lfOrientation;
	LONG lfWeight;
	BYTE lfItalic;
	BYTE lfUnderline;
	BYTE lfStrikeOut;
	BYTE lfCharSet;
	BYTE lfOutPrecision;
	BYTE lfClipPrecision;
	BYTE lfQuality;
	BYTE lfPitchAndFamily;
	TCHAR lfFaceName[LF_FACESIZE];
} LOGFONT;

typedef struct tagPALETTEENTRY {
  BYTE peRed;
  BYTE peGreen;
  BYTE peBlue;
  BYTE peFlags;
} PALETTEENTRY;

typedef struct tagLOGPALETTE {
 WORD         palVersion;
 WORD         palNumEntries;
 PALETTEENTRY palPalEntry[1];
} LOGPALETTE;

typedef DWORD COLORREF;
typedef DWORD *LPCOLORREF;

typedef struct tagCREATESTRUCT
{
    LPVOID lpCreateParams;
    HINSTANCE hInstance;   //应用程序的实例句柄
    HMENU hMenu;           //窗口菜单句柄
    HWND hwndParent;       //父窗口句柄
    int cy;                //指定新窗口的高度，以像素为单位
    int cx;                //指定新窗口的宽度，以像素为单位
    int y;                 //指定新窗口的左上角x坐标
    int x;                 //指定新窗口的左上角y坐标
    LONG style;            //指定新窗口的类型(风格)
    LPCTSTR lpszName;      //指定新窗口的名称
    LPCTSTR lpszClass;     //指定新窗口类的名称
    DWORD dwExStyle;       //指定新窗口扩展风格。
}CREATESTRUCT,*LPCREATESTRUCT;

typedef UINT WPARAM;
typedef LONG LPARAM;
struct __POSITION { int unused; };
typedef __POSITION* POSITION;

typedef struct tagPOINT {
  LONG x;
  LONG y;
} POINT, *PPOINT;

typedef struct tagMSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
} MSG;

#define RGN_AND 1
#define RGN_OR 2
#define RGN_XOR 3
#define RGN_DIFF 4
#define RGN_COPY 5
#define RGN_MIN RGN_AND
#define RGN_MAX RGN_COPY

#define TA_NOUPDATECP 0
#define TA_UPDATECP 1

#define TA_LEFT 0
#define TA_RIGHT 2
#define TA_CENTER 6

#define TA_TOP 0
#define TA_BOTTOM 8
#define TA_BASELINE 24
#define TA_RTLREADING 256
#define TA_MASK (TA_BASELINE+TA_CENTER+TA_UPDATECP+TA_RTLREADING)

#define VTA_BASELINE TA_BASELINE
#define VTA_LEFT TA_BOTTOM
#define VTA_RIGHT TA_TOP
#define VTA_CENTER TA_CENTER
#define VTA_BOTTOM TA_RIGHT
#define VTA_TOP TA_LEFT

#define FW_DONTCARE 0
#define FW_THIN 100
#define FW_EXTRALIGHT 200
#define FW_LIGHT 300
#define FW_NORMAL 400
#define FW_MEDIUM 500
#define FW_SEMIBOLD 600
#define FW_BOLD 700
#define FW_EXTRABOLD 800
#define FW_HEAVY 900

#define DRIVERVERSION 0
#define TECHNOLOGY 2
#define HORZSIZE 4
#define VERTSIZE 6
#define HORZRES 8
#define VERTRES 10
#define BITSPIXEL 12
#define PLANES 14
#define NUMBRUSHES 16
#define NUMPENS 18
#define NUMMARKERS 20
#define NUMFONTS 22
#define NUMCOLORS 24
#define PDEVICESIZE 26
#define CURVECAPS 28
#define LINECAPS 30
#define POLYGONALCAPS 32
#define TEXTCAPS 34
#define CLIPCAPS 36
#define RASTERCAPS 38
#define ASPECTX 40
#define ASPECTY 42
#define ASPECTXY 44


#define FW_ULTRALIGHT FW_EXTRALIGHT
#define FW_REGULAR FW_NORMAL
#define FW_DEMIBOLD FW_SEMIBOLD
#define FW_ULTRABOLD FW_EXTRABOLD
#define FW_BLACK FW_HEAVY

#define LOGPIXELSX 88
#define LOGPIXELSY 90

#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004
#define SWP_NOREDRAW 0x0008
#define SWP_NOACTIVATE 0x0010
#define SWP_FRAMECHANGED 0x0020
#define SWP_SHOWWINDOW 0x0040
#define SWP_HIDEWINDOW 0x0080
#define SWP_NOCOPYBITS 0x0100
#define SWP_NOOWNERZORDER 0x0200
#define SWP_NOSENDCHANGING 0x0400

#define SWP_DRAWFRAME SWP_FRAMECHANGED
#define SWP_NOREPOSITION SWP_NOOWNERZORDER
#define SWP_DEFERERASE 0x2000
#define SWP_ASYNCWINDOWPOS 0x4000

#define HWND_TOP ((HWND)0)
#define HWND_BOTTOM ((HWND)1)
#define HWND_TOPMOST ((HWND)-1)
#define HWND_NOTOPMOST ((HWND)-2)
#define MF_INSERT 0x00000000L
#define MF_CHANGE 0x00000080L
#define MF_APPEND 0x00000100L
#define MF_DELETE 0x00000200L
#define MF_REMOVE 0x00001000L

#define MF_BYCOMMAND 0x00000000L
#define MF_BYPOSITION 0x00000400L

#define MF_SEPARATOR 0x00000800L

#define MF_ENABLED 0x00000000L
#define MF_GRAYED 0x00000001L
#define MF_DISABLED 0x00000002L

#define MF_UNCHECKED 0x00000000L
#define MF_CHECKED 0x00000008L
#define MF_USECHECKBITMAPS 0x00000200L

#define MF_STRING 0x00000000L
#define MF_BITMAP 0x00000004L
#define MF_OWNERDRAW 0x00000100L

#define MF_POPUP 0x00000010L
#define MF_MENUBARBREAK 0x00000020L
#define MF_MENUBREAK 0x00000040L

#define MF_UNHILITE 0x00000000L
#define MF_HILITE 0x00000080L

#define MF_DEFAULT 0x00001000L
#define MF_SYSMENU 0x00002000L
#define MF_HELP 0x00004000L
#define MF_RIGHTJUSTIFY 0x00004000L
#define MF_MOUSESELECT 0x00008000L
#define MF_END 0x00000080L
#define MFT_STRING MF_STRING
#define MFT_BITMAP MF_BITMAP
#define MFT_MENUBARBREAK MF_MENUBARBREAK
#define MFT_MENUBREAK MF_MENUBREAK
#define MFT_OWNERDRAW MF_OWNERDRAW
#define MFT_RADIOCHECK 0x00000200L
#define MFT_SEPARATOR MF_SEPARATOR
#define MFT_RIGHTORDER 0x00002000L
#define MFT_RIGHTJUSTIFY MF_RIGHTJUSTIFY

#define MK_LBUTTON 0x0001
#define MK_RBUTTON 0x0002
#define MK_SHIFT 0x0004
#define MK_CONTROL 0x0008
#define MK_MBUTTON 0x0010
#define MK_XBUTTON1 0x0020
#define MK_XBUTTON2 0x0040

#define SB_HORZ 0
#define SB_VERT 1
#define SB_CTL 2
#define SB_BOTH 3

#define SB_LINEUP 0
#define SB_LINELEFT 0
#define SB_LINEDOWN 1
#define SB_LINERIGHT 1
#define SB_PAGEUP 2
#define SB_PAGELEFT 2
#define SB_PAGEDOWN 3
#define SB_PAGERIGHT 3
#define SB_THUMBPOSITION 4
#define SB_THUMBTRACK 5
#define SB_TOP 6
#define SB_LEFT 6
#define SB_BOTTOM 7
#define SB_RIGHT 7
#define SB_ENDSCROLL 8

#define SW_HIDE 0
#define SW_SHOW 1
#define CTLCOLOR_STATIC 6
void Sleep(unsigned int );
#endif

#ifndef afx_msg
#define afx_msg // intentional placeholder
#endif

#ifndef APIENTRY
#define APIENTRY // intentional placeholder
#endif

#ifndef OUTPUTAPI
#define OUTPUTAPI // intentional placeholder
#endif
#ifndef WINAPIV
#define WINAPIV
#endif

#ifndef USES_CONVERSION
#define USES_CONVERSION
#endif

#define A2W(x) (x)
