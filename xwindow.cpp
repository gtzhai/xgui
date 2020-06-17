#include "xgui.h"

/*
 * xSplitterWnd
 */
CSplitterWnd::CSplitterWnd() : m_bSplitVerticaly(TRUE),
	m_Parent(NULL), m_Win1(0), m_Win2(0)
{
}

BOOL CSplitterWnd::CreateStatic(CMDIChildFrame* Parent, int rows, int cols){
	if(wxSplitterWindow::Create(Parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxSP_3D)){
		SetMinimumPaneSize(1);
		if(rows == 1 && cols == 2){
			m_bSplitVerticaly = TRUE;
		}else if(rows == 2 && cols == 1){
			m_bSplitVerticaly = FALSE;
		}else{
			ASSERT(FALSE);
		}
		m_Parent= Parent;
		return TRUE;
	}
	return FALSE;
}

BOOL CSplitterWnd::CreateView(int row, int col, wxClassInfo* pViewClass,
	const CSize& sizeInit, CCreateContext* pContext)
{
	ASSERT(!(m_Win1 && m_Win2));

	_innerView* iview = new _innerView();
	iview->SetFrame(m_Parent);

	if(!iview->CreateCanvas(this, pContext->m_pCurrentDoc, pViewClass))
	{
		delete iview;
		return FALSE;
	}
	else
	{
		pContext->m_pCurrentDoc->AddView(iview);
		iview->SetDocument(pContext->m_pCurrentDoc);
		m_Parent->SetView(iview);
	}

	BOOL bAllViewCreated = FALSE;
	if(m_bSplitVerticaly)
	{
		ASSERT(row == 0 && col < 2);
		if(col == 0){
			m_Win1 = iview->m_xview;
			m_Size = sizeInit;
			if(m_Win2 != 0){
				bAllViewCreated = TRUE;
			}
		}else{
			m_Win2 = iview->m_xview;
			if(m_Win1 != 0){
				bAllViewCreated = TRUE;
			}
		}

		if(bAllViewCreated)
			SplitVertically(m_Win1->GetCanvas(), m_Win2->GetCanvas(), m_Size.cx);
	}
	else
	{
		ASSERT(col == 0 && row < 2);
		if(row == 0){
			m_Win1 = iview->m_xview;
			m_Size = sizeInit;
			if(m_Win2 != 0){
				bAllViewCreated = TRUE;
			}
		}else{
			m_Win2 = iview->m_xview;
			if(m_Win1 != 0){
				bAllViewCreated = TRUE;
			}
		}
		if(bAllViewCreated)
			SplitHorizontally(m_Win1->GetCanvas(), m_Win2->GetCanvas(), m_Size.cy);
	}

	return TRUE;
}

CView* CSplitterWnd::GetPane(int row, int col)
{
	CView* ret = m_Win1;
	if(m_bSplitVerticaly)
	{
		ASSERT(row == 0 && col < 2);
		if(col)
			ret = m_Win2;
	}
	else
	{
		ASSERT(col == 0 && row < 2);
		if(row)
			ret = m_Win2;
	}
	return ret;
}

/*
 *	Menu
 */
void CopyMenu(wxMenu* src, wxMenu* dst)
{
	int count = src->GetMenuItemCount();
	int n;
	for(n = 0; n < count; n ++)
	{
		wxMenuItem* item = src->FindItemByPosition(n);

		wxMenuItem* res = new wxMenuItem(dst, item->GetId(), item->GetText(),
			item->GetHelp(), item->GetKind(), 0);

		if(item->IsCheckable())
		{
			res->SetCheckable(true);
		}

		if(!item->IsEnabled())
			res->Enable(false);

		if(item->IsSubMenu())
		{
			wxMenu* submenu = item->GetSubMenu();
			wxMenu* menu = new wxMenu(submenu->GetTitle(), submenu->GetStyle());
			CopyMenu(submenu, menu);
			res->SetSubMenu(menu);
		}

		dst->Append(res);
	}
}

BOOL CMenu::LoadMenu(const char* szName)
{
	wxMenu* menu = (wxMenu*)wxXmlResource::Get()->LoadObject(0, szName, "wxMenu");
	if(menu)
	{
		CopyMenu(menu, this);
		delete menu;
	}

	return menu != NULL;
}

CMenu* CMenu::GetSubMenu(int index)
{
	int subcnt = 0;
	wxMenu* submenu = 0;

	int n;
	int count = GetMenuItemCount();

	for(n = 0; n < count; n ++)
	{
		wxMenuItem* item = FindItemByPosition(n);
		wxMenu* temp = item->GetSubMenu();
		if(temp){
			if(subcnt == index){
				submenu = temp;
				break;
			}
		}
		subcnt ++;
	}

	return (CMenu*)submenu;
}

BOOL CMenu::ModifyMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem,LPCTSTR lpszNewItem)
{
	wxMenuItem* item = 0;
	if(nFlags == MF_BYPOSITION){
		item = FindItemByPosition(nPosition);
	}else{
		item = FindItem(nPosition);
	}

	if(item){
		Insert(nPosition, nIDNewItem, lpszNewItem,
			item->GetHelp(), item->IsCheckable());
		Remove(item);
		return TRUE;
	}

	return FALSE;
}

BOOL CMenu::AppendMenu(UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
	BOOL ret = TRUE;
	if(nFlags == MF_STRING){
		Append(nIDNewItem, lpszNewItem);
	}else if(nFlags == MF_SEPARATOR){
		AppendSeparator();
	}else
		ret = FALSE;

	return ret;
}

BOOL CMenu::TrackPopupMenu(int x, int y, wxWindow* pWnd)
{
	return pWnd->PopupMenu(this)  ? TRUE : FALSE;
}

/*
 * xStatusBar
 */
CStatusBar::CStatusBar()
{
	m_pIDArray = 0;
	m_nIDCount = 0;
	m_evtHandler->SetHost(this, this);
}

CStatusBar::~CStatusBar()
{
	FreeIDArray();
}

void CStatusBar::FreeIDArray()
{
	if(m_pIDArray)
	{
		delete m_pIDArray;
	}
	m_pIDArray = 0;
	m_nIDCount = 0;
}

BOOL CStatusBar::SetPaneText(int index, const wxString& text)
{
	SetStatusText(text, index);
	return TRUE;
}

BOOL CStatusBar::SetIndicators(const UINT* width, int nIDCount)
{
	if(nIDCount > 0)
	{
		FreeIDArray();
		m_pIDArray = new UINT[nIDCount];
		memcpy(m_pIDArray, width, nIDCount*sizeof(UINT));

		SetFieldsCount(nIDCount, (int*)m_pIDArray);
	}

	return TRUE;
}

/*
 * xToolBar
 */
BOOL CToolBar::SetButtons(const UINT* ids, int count)
{
	//TODO
//	int n = 0;
//	for(;n < count; n ++)
//	{
//		AddTool(ids[n], "", )
//	}
	return TRUE;
}
