#ifndef XMSG_H_INCLUDED
#define XMSG_H_INCLUDED


class xMessageHandler;
class CtlColorEvent;
wxDECLARE_EVENT(wxEVT_CTL_COLOR, CtlColorEvent);
typedef void (wxEvtHandler::*CtlColorEventFunction)(CtlColorEvent&);
#define CtlColorEventHandler(func) wxEVENT_HANDLER_CAST(CtlColorEventFunction, func)
#define EVT_CTL_COLOR(id, func) wx__DECLARE_EVT1(wxEVT_CTL_COLOR, id, CtlColorEventHandler(func))
class CtlColorEvent : public wxEvent
{
public:
    CtlColorEvent(wxEventType type = wxEVT_CTL_COLOR, int id = wxID_ANY):wxEvent(id, type){}
    CtlColorEvent(const CtlColorEvent& event): wxEvent(event){}
    virtual wxEvent *Clone() const { return new CtlColorEvent(*this); }
private:
};


typedef void (xMessageHandler:: *msg_routine)(void);
struct msg_map_entry
{
    int event_type;
	int nID;
    int nLastID;
	msg_routine pfn;
};

struct msg_map{
    const msg_map *pBaseMap;
    const msg_map_entry *lpEntries;
};

#undef WM_COMMAND
#undef WM_KEYDOWN
#undef WM_KEYUP
#undef WM_TIMER
#undef WM_CREATE
#undef WM_ERASEBKGND
#undef WM_MOVE
#undef WM_LBUTTONDOWN
#undef WM_LBUTTONDOWN
#undef WM_LBUTTONUP
#undef WM_MBUTTONDOWN
#undef WM_MBUTTONUP
#undef WM_RBUTTONDOWN
#undef WM_RBUTTONUP
#undef WM_LBUTTONDBLCLK
#undef WM_MBUTTONDBLCLK
#undef WM_RBUTTONDBLCLK
#undef WM_MOUSEWHEEL
#undef WM_MOUSEMOVE
#undef WM_UPDATEUISTATE
#undef WM_SETFOCUS
#undef WM_KILLFOCUS
#undef CBN_SELCHANGE
#undef BN_CLICKED
#undef EN_SETFOCUS
#undef EN_KILLFOCUS
#undef EN_CHANGE
#undef WM_CLOSE
#undef WM_PAINT


#define WM_COMMAND wxEVT_COMMAND_LEFT_CLICK
#define WM_KEYDOWN wxEVT_KEY_DOWN
#define WM_KEYUP   wxEVT_KEY_UP
#define WM_TIMER   wxEVT_TIMER
#define WM_CREATE  wxEVT_CREATE
#define WM_ERASEBKGND wxEVT_ERASE_BACKGROUND
#define WM_MOVE wxEVT_MOVE
#define WM_LBUTTONDOWN wxEVT_LEFT_DOWN
#define WM_LBUTTONUP wxEVT_LEFT_UP
#define WM_MBUTTONDOWN wxEVT_MIDDLE_DOWN
#define WM_MBUTTONUP wxEVT_MIDDLE_UP
#define WM_RBUTTONDOWN wxEVT_RIGHT_DOWN
#define WM_RBUTTONUP wxEVT_RIGHT_UP
#define WM_LBUTTONDBLCLK wxEVT_LEFT_DCLICK
#define WM_MBUTTONDBLCLK wxEVT_MIDDLE_DCLICK
#define WM_RBUTTONDBLCLK wxEVT_RIGHT_DCLICK
#define WM_MOUSEWHEEL wxEVT_MOUSEWHEEL
#define WM_MOUSEMOVE wxEVT_MOTION
#define WM_UPDATEUISTATE wxEVT_UPDATE_UI
#define WM_SETFOCUS wxEVT_SET_FOCUS
#define WM_KILLFOCUS wxEVT_KILL_FOCUS
#define CBN_SELCHANGE wxEVT_COMBOBOX
#define BN_CLICKED wxEVT_BUTTON
#define EN_SETFOCUS wxEVT_SET_FOCUS
#define EN_KILLFOCUS wxEVT_KILL_FOCUS
#define EN_CHANGE wxEVT_TEXT
#define WM_CLOSE wxEVT_CLOSE_WINDOW
#define WM_PAINT wxEVT_PAINT
#define WM_CTLCOLOR wxEVT_CTL_COLOR

#define WM_KEYFIRST  wxEVT_KEY_DOWN
#define WM_KEYLAST  wxEVT_KEY_UP
#define VK_SPACE   WXK_SPACE
#define VK_LCONTROL WXK_CONTROL

#define AFX_MSGMAP msg_map
#define AFX_MSGMAP_ENTRY msg_map_entry
#define AFX_PMSG msg_routine
#define CWnd xMessageHandler
#define AFX_PMSGW

#define DECLARE_MESSAGE_MAP() \
protected: \
	static const AFX_MSGMAP* GetThisMessageMap(); \
	virtual const AFX_MSGMAP* GetMessageMap() const; \

#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
	const AFX_MSGMAP* theClass::GetMessageMap() const \
		{ return GetThisMessageMap(); } \
	const AFX_MSGMAP* theClass::GetThisMessageMap() \
	{ \
		typedef theClass ThisClass;						   \
		typedef baseClass TheBaseClass;					   \
		static const AFX_MSGMAP_ENTRY _messageEntries[] =  \
		{

#define END_MESSAGE_MAP() \
            {0, 0, 0, static_cast<AFX_PMSG>(0) } \
        }; \
		static const AFX_MSGMAP messageMap = \
		{ TheBaseClass::GetThisMessageMap(), &_messageEntries[0] }; \
		return &messageMap; \
	}								  \

//static_cast<AFX_PMSG> (memberFxn) },
#define ON_WM_CTLCOLOR() \
	{ WM_CTLCOLOR, 0, 0, \
		(AFX_PMSG) \
		(static_cast< HBRUSH (CWnd::*)(CDC*, CWnd*, UINT)>  ( &ThisClass :: OnCtlColor)) },

#define ON_COMMAND(id, memberFxn) \
	{ WM_COMMAND, (WORD)id, (WORD)id, \
	(AFX_PMSG) \
		static_cast<void (CWnd::*)(void)> (&ThisClass :: memberFxn)},

#define ON_WM_CREATE() \
	{ WM_CREATE, 0, 0, \
	(AFX_PMSG) \
		(static_cast< int (CWnd::*)(LPCREATESTRUCT) > ( &ThisClass :: OnCreate)) },

#define ON_WM_DESTROY() \
	{ WM_DESTROY, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(void) > ( &ThisClass :: OnDestroy)) },

#define ON_WM_MOVE() \
	{ WM_MOVE, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(int, int) > ( &ThisClass :: OnMove)) },

#define ON_WM_SIZE() \
	{ WM_SIZE, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, int, int) > ( &ThisClass :: OnSize)) },

#define ON_WM_CLOSE() \
	{ WM_CLOSE, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(void) > ( &ThisClass :: OnClose)) },

#define ON_WM_SETFOCUS() \
	{ WM_SETFOCUS, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(CWnd*) > ( &ThisClass :: OnSetFocus)) },

#define ON_WM_KILLFOCUS() \
	{ WM_KILLFOCUS, 0, 0,  \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(CWnd*) > ( &ThisClass :: OnKillFocus)) },

#define ON_WM_PAINT() \
	{ WM_PAINT, 0, 0, \
		(static_cast< void (CWnd::*)(void) > ( &ThisClass :: OnPaint)) },


#define ON_WM_TIMER() \
	{ WM_TIMER, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT_PTR) > ( &ThisClass :: OnTimer)) },

#define ON_WM_ERASEBKGND() \
	{ WM_ERASEBKGND, 0, 0, \
	(AFX_PMSG) \
		(static_cast< BOOL (CWnd::*)(CDC*) > ( &ThisClass :: OnEraseBkgnd)) },


#define ON_WM_KEYDOWN() \
	{ WM_KEYDOWN, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, UINT, UINT) > ( &ThisClass :: OnKeyDown)) },

#define ON_WM_KEYUP() \
	{ WM_KEYUP, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, UINT, UINT) > ( &ThisClass :: OnKeyUp)) },


#define ON_WM_MOUSEMOVE() \
	{ WM_MOUSEMOVE, 0, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnMouseMove)) },

#define ON_WM_MOUSEHOVER() \
	{ WM_MOUSEHOVER, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnMouseHover)) },

#define ON_WM_MOUSELEAVE() \
	{ WM_MOUSELEAVE, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(void) > ( &ThisClass :: OnMouseLeave)) },

#define ON_WM_MOUSEWHEEL() \
	{ WM_MOUSEWHEEL, 0, 0, \
	(AFX_PMSG) \
		(static_cast< BOOL (CWnd::*)(UINT, short, CPoint) > ( &ThisClass :: OnMouseWheel)) },

#define ON_WM_MOUSEHWHEEL() \
	{ WM_MOUSEHWHEEL, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, short, CPoint) > ( &ThisClass :: OnMouseHWheel)) },

#define ON_WM_LBUTTONDOWN() \
	{ WM_LBUTTONDOWN, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnLButtonDown)) },

#define ON_WM_LBUTTONUP() \
	{ WM_LBUTTONUP, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnLButtonUp)) },

#define ON_WM_LBUTTONDBLCLK() \
	{ WM_LBUTTONDBLCLK, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnLButtonDblClk)) },

#define ON_WM_RBUTTONDOWN() \
	{ WM_RBUTTONDOWN, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnRButtonDown)) },

#define ON_WM_RBUTTONUP() \
	{ WM_RBUTTONUP, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnRButtonUp)) },

#define ON_WM_RBUTTONDBLCLK() \
	{ WM_RBUTTONDBLCLK, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnRButtonDblClk)) },

#define ON_WM_MBUTTONDOWN() \
	{ WM_MBUTTONDOWN, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnMButtonDown)) },

#define ON_WM_MBUTTONUP() \
	{ WM_MBUTTONUP, 0, 0, \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnMButtonUp)) },

#define ON_WM_MBUTTONDBLCLK() \
	{ WM_MBUTTONDBLCLK, 0, 0,\
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, CPoint) > ( &ThisClass :: OnMButtonDblClk)) },

#define ON_WM_UPDATEUISTATE() \
	{ WM_UPDATEUISTATE, 0, 0, \
		(static_cast< void (CWnd::*)(UINT, UINT) > ( &ThisClass :: OnUpdateUIState)) },

#define ON_WM_MENUSELECT() \
	{ WM_MENUSELECT, 0, 0,   \
	(AFX_PMSG) \
		(static_cast< void (CWnd::*)(UINT, UINT, HMENU) > ( &ThisClass :: OnMenuSelect)) },

#define ON_CONTROL(wNotifyCode, id, memberFxn) \
	{ wNotifyCode, (WORD)id, (WORD)id, \
	(AFX_PMSG) \
		static_cast<void (CWnd::*)(void)> (&ThisClass :: memberFxn)},

#define ON_BN_CLICKED(id, memberFxn) \
	ON_CONTROL(BN_CLICKED, id, memberFxn)

// Edit Control Notification Codes
#define ON_EN_SETFOCUS(id, memberFxn) \
	ON_CONTROL(EN_SETFOCUS, id, memberFxn)
#define ON_EN_KILLFOCUS(id, memberFxn) \
	ON_CONTROL(EN_KILLFOCUS, id, memberFxn)
#define ON_EN_CHANGE(id, memberFxn) \
	ON_CONTROL(EN_CHANGE, id, memberFxn)

// Combo Box Notification Codes
#define ON_CBN_SELCHANGE(id, memberFxn) \
	ON_CONTROL(CBN_SELCHANGE, id, memberFxn)


#endif // XMSG_H_INCLUDED
