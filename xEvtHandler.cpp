#include "xgui.h"
wxDEFINE_EVENT(wxEVT_CTL_COLOR, CtlColorEvent);
/*
 *	xEvtHandler
 */
xEvtHandler::xEvtHandler()
	: wxEvtHandler()
{
	m_pHost = 0;
	m_pHostEvtHandler = 0;
	m_msg_handler = 0;
	m_pRecentEvent = 0;
	m_bNotHostEvent = false;
	//make destroy event
}

xEvtHandler::~xEvtHandler()
{
	SetHost(0,0);
}

wxWindow* xEvtHandler::GetHost()
{
	return m_pHost;
}

void xEvtHandler::SetHost(wxWindow* host, xMessageHandler* msg_handler)
{
	if(m_pHost){
		m_pHost->SetEventHandler(m_pHostEvtHandler);
		m_pHost = 0;
		m_pHostEvtHandler = 0;

		#if 1
		if(msg_handler)
			msg_handler->SetEvtHandler(0);
		msg_handler = 0;
		#endif
	}

	if(host && msg_handler){
		m_pHost = host;
		m_msg_handler = msg_handler;
		m_msg_handler->SetEvtHandler(this);
		m_pHostEvtHandler = m_pHost->GetEventHandler();
		m_pHost->SetEventHandler(this);
	}
}

bool xEvtHandler::HostEvent()
{
	if(m_pRecentEvent)
		return m_pHostEvtHandler->ProcessEvent(*m_pRecentEvent);

	return false;
}

union mmfunction
{
	msg_routine pfn;
	void (xMessageHandler::*pfn_DD)(void*);
	int (xMessageHandler::*pfn_iD)(void*);
	void (xMessageHandler::*pfn_DUi)(UINT);
	void (xMessageHandler::*pfn_DUiUiD)(UINT,UINT,void*);
	long (xMessageHandler::*pfn_lWL)(WPARAM, LPARAM);
	BOOL (xMessageHandler::*pfn_bWL)(WPARAM, LPARAM);
	void (xMessageHandler::*pfn_Dii)(int x, int y);
	void (xMessageHandler::*pfn_DUiii)(UINT, int x, int y);
	void (xMessageHandler::*pfn_DUiUiUi)(UINT, UINT x, UINT y);
	void (xMessageHandler::*pfn_DUiPnt)(UINT, CPoint);
	BOOL (xMessageHandler::*pfn_bUisPnt)(UINT, short, CPoint);
};

bool xEvtHandler::HandleScrollWinEvent(wxScrollWinEvent& event, const msg_map_entry* entry)
{
	if(entry->nID == event.GetOrientation())
	{
		wxEventType type = event.GetEventType();

		UINT code = 0;
		if(type == wxEVT_SCROLLWIN_TOP)
			code = SB_TOP;
		else if(type == wxEVT_SCROLLWIN_BOTTOM)
			code = SB_BOTTOM;
		else if(type == wxEVT_SCROLLWIN_LINEUP)
			code = SB_LINEUP;
		else if(type == wxEVT_SCROLLWIN_LINEDOWN)
			code = SB_LINEDOWN;
		else if(type == wxEVT_SCROLLWIN_PAGEUP)
			code = SB_PAGEUP;
		else if(type == wxEVT_SCROLLWIN_PAGEDOWN)
			code = SB_PAGEDOWN;
		else if(type == wxEVT_SCROLLWIN_THUMBTRACK)
			code = SB_THUMBTRACK;
		else if(type == wxEVT_SCROLLWIN_THUMBRELEASE)
			code = SB_THUMBPOSITION;

		mmfunction mmf;
		mmf.pfn = entry->pfn;
		(m_msg_handler->*(mmf.pfn_DUiUiD))(code, (UINT)event.GetPosition(), 0);
		return true;
	}

	return false;
}

bool xEvtHandler::HandleScrollEvent(wxScrollEvent& event, const msg_map_entry* entry)
{
	if(event.GetOrientation() == 0)
	{//spin button
		(m_msg_handler->*(entry->pfn))();
	}
	else if(entry->nID == event.GetOrientation())
	{
		wxEventType type = event.GetEventType();

		UINT code = 0;
		if(type == wxEVT_SCROLL_TOP)
			code = SB_TOP;
		else if(type == wxEVT_SCROLL_BOTTOM)
			code = SB_BOTTOM;
		else if(type == wxEVT_SCROLL_LINEUP)
			code = SB_LINEUP;
		else if(type == wxEVT_SCROLL_LINEDOWN)
			code = SB_LINEDOWN;
		else if(type == wxEVT_SCROLL_PAGEUP)
			code = SB_PAGEUP;
		else if(type == wxEVT_SCROLL_PAGEDOWN)
			code = SB_PAGEDOWN;
		else if(type == wxEVT_SCROLL_THUMBTRACK)
			code = SB_THUMBTRACK;
		else if(type == wxEVT_SCROLL_THUMBRELEASE)
			code = SB_THUMBPOSITION;

		CScrollBar* psc = 0;
		CScrollBar scb;
		wxWindow* win = (wxWindow*)event.GetEventObject();
		if(win && win->IsKindOf(CLASSINFO(wxScrollBar)))
		{
			scb.m_control = win;
			psc = &scb;
		}

		mmfunction mmf;
		mmf.pfn = entry->pfn;
		(m_msg_handler->*(mmf.pfn_DUiUiD))(code, (UINT)event.GetPosition(), psc);
		return true;
	}

	return false;
}

void xEvtHandler::HandleMouseEvent(wxMouseEvent& evt, const msg_map_entry* entry)
{
	mmfunction mmf;
	mmf.pfn = entry->pfn;

	UINT flag = 0;
	flag |= evt.LeftIsDown() ? MK_LBUTTON : 0;
	flag |= evt.RightIsDown() ? MK_RBUTTON : 0;
	flag |= evt.ShiftDown() ? MK_SHIFT : 0;
	flag |= evt.ControlDown() ? MK_CONTROL : 0;
	flag |= evt.MiddleIsDown()? MK_MBUTTON : 0;

	CPoint pt = evt.GetPosition();
	(m_msg_handler->*(mmf.pfn_DUiPnt))(flag, pt);
}

bool xEvtHandler::doEvents(wxEvent& event, const msg_map_entry* entry)
{
	wxEventType type = event.GetEventType();
	bool bret = true;

	mmfunction mmf;
	mmf.pfn = entry->pfn;

	if(type == wxEVT_TIMER)
	{
		(m_msg_handler->*(mmf.pfn_DUi))(((wxTimerEvent&)event).GetId());
	}
	else if(type == wxEVT_CREATE)
	{
		//discard child window creation events
		if(event.GetEventObject() == (wxObject*)m_pHost)
		{
			if(!m_msg_handler->m_bOnCreateCalled)
			{
				m_msg_handler->m_bOnCreateCalled = true;
				if((m_msg_handler->*(mmf.pfn_iD))(m_msg_handler->m_tempInfo) != 0){
					//OnCreate fialed.
					wxCloseEvent evt(wxEVT_CLOSE_WINDOW);
					evt.SetEventObject(m_pHost);
					m_pHost->ProcessWindowEvent(evt);
				}
			}
		}
		else
			bret = false;
	}
	else if(type == wxEVT_ERASE_BACKGROUND)
	{
		CDC dc;
		dc.SetDC(((wxEraseEvent&)event).GetDC(), (CWndBase*)m_msg_handler);
		//if OnEraseBkgnd reutrn FALSE, the background are not erased
		bret = ((m_msg_handler->*(mmf.pfn_iD))(&dc) == TRUE);
	}
	else if(type >= wxEVT_SCROLLWIN_TOP &&
		type <= wxEVT_SCROLLWIN_THUMBRELEASE)
	{
		bret = HandleScrollWinEvent((wxScrollWinEvent&)event, entry);
	}
	else if(type >= wxEVT_SCROLL_TOP &&
		type <= wxEVT_SCROLL_THUMBRELEASE)
	{
		bret = HandleScrollEvent((wxScrollEvent&)event, entry);
	}
	else if(type == wxEVT_MOVE)
	{
		wxPoint pt = ((wxMoveEvent&)event).GetPosition();
		(m_msg_handler->*(mmf.pfn_Dii))(pt.x, pt.y);
	}
	else if(type >= wxEVT_LEFT_DOWN &&
		type <= wxEVT_RIGHT_DCLICK)
	{
		HandleMouseEvent((wxMouseEvent&)event, entry);
	}
	else if(type == wxEVT_MOUSEWHEEL)
	{
		wxMouseEvent& evt = (wxMouseEvent&)event;
		UINT flag = 0;
		flag |= evt.LeftDown() ? MK_LBUTTON : 0;
		flag |= evt.RightDown() ? MK_RBUTTON : 0;
		flag |= evt.ShiftDown() ? MK_SHIFT : 0;
		flag |= evt.ControlDown() ? MK_CONTROL : 0;
		flag |= evt.MiddleDown()? MK_MBUTTON : 0;
		CPoint pt = evt.GetPosition();

		(m_msg_handler->*(mmf.pfn_bUisPnt))(flag, evt.m_wheelDelta, pt);
	}
	else if(type == wxEVT_UPDATE_UI)
	{
		(m_msg_handler->*(mmf.pfn_DD))(&(wxUpdateUIEvent&)event);
	}
	else if(type == wxEVT_SIZE)
	{
		wxSize sz = ((wxSizeEvent&)event).GetSize();
		(m_msg_handler->*(mmf.pfn_DUiii))(0, sz.x, sz.y);
	}
	else if(type == wxEVT_SET_FOCUS)
	{
		(m_msg_handler->*(mmf.pfn_DD))(0);
	}
	else if(type == wxEVT_KEY_DOWN ||
		type == wxEVT_KEY_UP)
	{
		wxKeyEvent& evt = (wxKeyEvent&)event;
		(m_msg_handler->*(mmf.pfn_DUiUiUi))(evt.GetKeyCode(), 1, 0);
	}
	/*else if(type >= wxEVT_USER_FIRST)
	{
		WPARAM wp = ((wxHREvent&)event).wp;
		LPARAM lp = ((wxHREvent&)event).lp;
		(m_msg_handler->*(mmf.pfn_lWL))(wp, lp);
	}*/
	else
	{
		if(entry->nID != entry->nLastID && entry->nLastID != 0)
			(m_msg_handler->*(mmf.pfn_DUi))(event.GetId());//range
		else
			(m_msg_handler->*(mmf.pfn))();
	}

	return bret;
}

bool xEvtHandler::TryCommand(wxEvent& event)
{
	wxEventType type = event.GetEventType();
	if(type == wxEVT_COMMAND_MENU_SELECTED)//currently only menu command would be tried.
	{
		WPARAM w = (USHORT)(event.GetId());
		LPARAM l = 0;
		return m_msg_handler->OnCommand(w,l) == TRUE;
	}

	return false;
}

bool xEvtHandler::ProcessEvent(wxEvent& event)
{
	//filter events;
	bool bProcessed = false;

	if(m_pHost)
	{
		wxEvent* prevEvent = m_pRecentEvent;
		m_pRecentEvent = &event;

		wxEventType type = event.GetEventType();
		int       nWinID = event.GetId();
		wxObject* obj = event.GetEventObject();
		#if 0
        MSG msg;

        msg.message = type;
        msg.time = event.GetTimestamp();
        //msg.hwnd = (void *)obj;
	    if(type == wxEVT_KEY_DOWN ||
		    type == wxEVT_KEY_UP)
	    {
		    wxKeyEvent& evt = (wxKeyEvent&)event;
            msg.wParam = evt.GetKeyCode();
	    }

	    bProcessed = ((CWndBase*)obj)->PreTranslateMessage(&msg);
	    #endif

		if(type == wxEVT_DESTROY)
		{
			return false;
		}

		if (type == wxEVT_INIT_DIALOG)
		{
			if(obj->IsKindOf(CLASSINFO(CDialog)))
			{
				if(!((CDialog*)obj)->m_bInitDialogCalled)
				{
					((CDialog*)obj)->CDialogBase::OnInitDialog();
					((CDialog*)obj)->m_bInitDialogCalled = true;
				}
				bProcessed = true;
			}
			else if(obj->IsKindOf(CLASSINFO(CPropertyPage)))
			{
				if(!((CPropertyPage*)obj)->m_bInitDialogCalled)
				{
					((CPropertyPage*)obj)->CDialogBase::OnInitDialog();
					((CPropertyPage*)obj)->m_bInitDialogCalled = true;
				}
				bProcessed = true;
			}
			else if(obj->IsKindOf(CLASSINFO(CPropertySheet)))
			{
				if(!((CPropertySheet*)obj)->m_bInitDialogCalled)
				{
					((CPropertySheet*)obj)->CDialogBase::OnInitDialog();
					((CPropertySheet*)obj)->m_bInitDialogCalled = true;
				}
				bProcessed = true;
			}
		}
		else
		{
			const msg_map* mp = m_msg_handler->GetMessageMap();

			while(mp && !bProcessed)
			{
				const msg_map_entry* entry = &mp->lpEntries[0];
				while(entry->event_type && entry->pfn)
				{
					if(entry->event_type == type)
					{
						if(entry->nID == wxID_ANY || entry->nID == 0 || nWinID == 0 || entry->nID == nWinID ||
								(entry->nID < nWinID && nWinID <= entry->nLastID))
						{
							bProcessed = doEvents(event, entry);
							break;
						}
					}
					else if(entry->event_type == wxEVT_SCROLLWIN_TOP)//special case for scroll
					{
						if(type >= wxEVT_SCROLLWIN_TOP && type <= wxEVT_SCROLLWIN_THUMBRELEASE &&
							entry->nID == ((wxScrollWinEvent&)event).GetOrientation())
						{
							bProcessed = doEvents(event, entry);
							break;
						}
					}
					else if(entry->event_type == wxEVT_SCROLL_TOP)//special case for scroll
					{
						if(type >= wxEVT_SCROLL_TOP && type <= wxEVT_SCROLL_THUMBRELEASE &&
							entry->nID == ((wxScrollEvent&)event).GetOrientation())
						{
							bProcessed = doEvents(event, entry);
							break;
						}
					}

					entry ++;
				}

				//try OnCommand interface
				if(!bProcessed)
					bProcessed = TryCommand(event);

				if(event.GetSkipped())
				{
					bProcessed = false;
					break;
				}

				mp = mp->pBaseMap;
			}
		}

		if(!bProcessed)
		{
			if(m_bNotHostEvent)
				m_bNotHostEvent = false;//cound be wxView host event to xView;
			else
			{
				bProcessed = HostEvent();
			}
		}

		m_pRecentEvent = prevEvent;
	}

	return bProcessed;
}

