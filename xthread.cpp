#include "xgui.h"

void xBeginThread(THREAD_ENTRY entry, void* param, wxThread** pThread)
{
	CWinThread *thread;
	if(pThread)
	{
		thread = new CWinThread(entry, param, wxTHREAD_JOINABLE);
		*pThread = thread;
	}
	else
		thread = new CWinThread(entry, param, wxTHREAD_DETACHED);

	thread->Create();
	thread->Run();
}

CWinThread *AfxBeginThread(THREAD_ENTRY entry, void *param)
{
	CWinThread *thread;
	thread = new CWinThread(entry, param, wxTHREAD_JOINABLE);

	thread->Create();
	thread->Run();
	return thread;
}

