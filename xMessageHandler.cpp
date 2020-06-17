#include "xgui.h"

/*
 *	xMessageHandler
 */


const msg_map* xMessageHandler::GetThisMessageMap()
{
	static const msg_map_entry _messageEntries[] =
	{
		{ 0, 0, 0, 0}     // nothing here
	};

	static const msg_map messageMap =
	{
		0,
		&_messageEntries[0],
	};
	return &messageMap;
}

const msg_map* xMessageHandler::GetMessageMap() const
{
	return GetThisMessageMap();
}

xMessageHandler::xMessageHandler()
{
	m_tempInfo= 0;
	m_evtHandler = 0;
	m_bOnCreateCalled = false;


	memset(m_Timers, 0, sizeof(m_Timers));
}

void xMessageHandler::SetEvtHandler(xEvtHandler* handler)
{
	m_evtHandler = handler;
}

xMessageHandler::~xMessageHandler()
{
	int n;
	for(n = 0; n < (int)(sizeof(m_Timers)/sizeof(m_Timers[0])); n ++)
	{
		if(m_Timers[n].Timer)
		{
			RemoveTimer(n);
		}
	}
}

void xMessageHandler::RemoveTimer(int index)
{
	m_Timers[index].Timer->Stop();
	delete m_Timers[index].Timer;
	m_Timers[index].Timer = 0;
	m_Timers[index].nId = 0;
}

int xMessageHandler::KillTimer(int id)
{
	int n;
	for(n = 0; n < (int)(sizeof(m_Timers)/sizeof(m_Timers[0])); n ++)
	{
		if(m_Timers[n].Timer && (id == m_Timers[n].nId))
		{
			RemoveTimer(n);
			return n;
		}
	}

	return 0;
}

int xMessageHandler::SetTimer(int id, unsigned int milllisecs)
{
	int n;
	int timer_id = 0;

	//find exist timer for 'id';
	for(n = 0; n < (int)(sizeof(m_Timers)/sizeof(m_Timers[0])); n ++)
	{
		if(m_Timers[n].Timer && (id == m_Timers[n].nId))
		{
			wxTimer* timer = m_Timers[n].Timer;
			timer->Start((int)milllisecs);
			timer_id = id;
		}
	}

	if(!timer_id)
	{
		//make a new timer
		for(n = 0; n < (int)(sizeof(m_Timers)/sizeof(m_Timers[0])); n ++)
		{
			if(!m_Timers[n].Timer)
			{
				wxTimer* timer = new wxTimer(m_evtHandler, id);
				m_Timers[n].nId = id;
				m_Timers[n].Timer = timer;
				timer->Start((int)milllisecs);
				timer_id = id;
				break;
			}
		}
	}

	return timer_id;
}


