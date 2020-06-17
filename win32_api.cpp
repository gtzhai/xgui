#include "xgui.h"

#if !defined(_WIN32) && !defined(_WIN64)
BOOL GetAsyncKeyState(int key){
    return wxGetKeyState((wxKeyCode)key);
}

void Sleep(unsigned int ms)
{
	usleep(ms*1000);
}

#endif

CBitmap* LoadBitmap(void*, LPTSTR res)
{
	wxImage img;
	img.LoadFile(wxString(res));

	wxBitmap* bmp = 0;
	if(img.Ok())
		bmp = new wxBitmap(img);

	return (CBitmap*)bmp;
}

DWORD GetEnvironmentVariable(const char* env, char* buffer,int size)
{
	wxString var;
	DWORD ret = 0;
	if(wxGetEnv(env, &var))
	{
		strncpy(buffer, var.c_str(), size - 1);
		buffer[size - 1] = 0;
		ret = strlen(buffer);
	}

	return ret;
}

/*
 * Mimic Clipboard
 */
wxCriticalSection _clipboard_sync;

#define MAX_FMT 8
struct CLIPBORAD_FMT
{
	wxString* str;
	void* data;
};

CLIPBORAD_FMT _clipboard_fmt[MAX_FMT] = {
	{0,0,},
	{0,0,},
	{0,0,},
	{0,0,},
	{0,0,},
	{0,0,},
	{0,0,},
	{0,0,},
};

UINT xRegisterClipboardFormat(const char* str)
{
	UINT ret = 0;
	int n;

	_clipboard_sync.Enter();

	for(n = 0;n < MAX_FMT; n ++)
	{
		if(_clipboard_fmt[n].str == 0){
			_clipboard_fmt[n].str = new wxString(str);
			ret = n + 1;
			break;
		}
	}

	_clipboard_sync.Leave();
	return ret;
}

BOOL xOpenClipboard()
{
	_clipboard_sync.Enter();
	return TRUE;
}

BOOL xEmptyClipboard()
{
	return TRUE;
}

void xSetClipboardData(UINT fmt, void* mem)
{
	fmt --;
	if(fmt < MAX_FMT  && (_clipboard_fmt[fmt].str != 0)){
		if(_clipboard_fmt[fmt].data)
			free(_clipboard_fmt[fmt].data);
		_clipboard_fmt[fmt].data = mem;
	}
}

void* xGetClipboardData(UINT fmt)
{
	fmt --;
	if(fmt < MAX_FMT  && (_clipboard_fmt[fmt].str != 0)){
		return _clipboard_fmt[fmt].data;
	}
	return 0;
}

BOOL xIsClipboardFormatAvailable(UINT fmt)
{
	fmt --;
	return (fmt < MAX_FMT  && (_clipboard_fmt[fmt].str != 0));
}

BOOL xCloseClipboard()
{
	_clipboard_sync.Leave();
	return TRUE;
}
