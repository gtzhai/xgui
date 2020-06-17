#include "xgui.h"

CControlWrapper::CControlWrapper() : m_control(0) {}
CControlWrapper::CControlWrapper(wxWindow* control){m_control = control;};

void CControlWrapper::SetWindowText(const wxString& str)
{
	if(m_control)
	{
		if(m_control->IsKindOf(CLASSINFO(wxTextCtrl)))
			((wxTextCtrl*)m_control)->SetValue(str);
		else if(m_control->IsKindOf(CLASSINFO(wxComboBox)))
			((wxComboBox*)m_control)->SetValue(str);
		else
			m_control->SetLabel(str);
	}
};

wxString CControlWrapper::GetWindowText()
{
	wxString str;
	GetWindowText(str);

	return str;
}

void CControlWrapper::GetWindowText(wxString& str) const
{
	if(m_control)
	{
		if(m_control->IsKindOf(CLASSINFO(wxTextCtrl)))
			str = ((wxTextCtrl*)m_control)->GetValue();
		else
			str = m_control->GetLabel();
	}
	else
		str = wxEmptyString;
}

int CControlWrapper::GetWindowText(char* buffer, int len)
{
	wxString str  = wxEmptyString;
	GetWindowText(str);

	strncpy(buffer, str.c_str(), len);
	buffer[len-1] = 0;

	return strlen(buffer);
}

void CControlWrapper::EnableWindow(int enable)
{
	if(m_control){
		if(enable)
			m_control->Enable();
		else
			m_control->Disable();
	}
}

void CControlWrapper::ShowWindow(int bshow)
{
	if(m_control){
		m_control->Show(bshow == 0 ? false : true);
	}
}

void CControlWrapper::GetWindowRect(CRect* prect)
{
	if(m_control){
		*prect = m_control->GetRect();
	}
}

void CControlWrapper::GetClientRect(CRect* prect)
{
	if(m_control){
		int cx,cy;
		m_control->GetClientSize(&cx,&cy);
		prect->left = 0;
		prect->top = 0;
		prect->right = cx;
		prect->bottom = cy;
	}
}

void CControlWrapper::SetWindowPos(void*, int l, int t, int w, int h, int flags)
{
	if(m_control){
		wxPoint p = m_control->GetPosition();
		wxSize s = m_control->GetSize();
		if(!(flags & SWP_NOSIZE)){
			s.x = w;
			s.y = h;
		}
		if(!(flags & SWP_NOMOVE)){
			p.x = l;
			p.y = t;
		}
		m_control->SetSize(p.x, p.y, s.x, s.y, 0);

		if(flags & SWP_SHOWWINDOW){
			m_control->Show();
		}
		if(flags & SWP_HIDEWINDOW){
			m_control->Show(false);
		}
	}
}

int CControlWrapper::GetDlgCtrlID(){
	int id = -1;
	if(m_control){
		id = m_control->GetId();
	}
	return id;
}

void CControlWrapper::Invalidate(BOOL bErase)
{
	if(m_control){
		m_control->Refresh(bErase);
	}
}

wxWindow* CControlWrapper::SetFocus()
{
	wxWindow* prev = wxWindow::FindFocus();
	if(m_control){
		m_control->SetFocus();
	}
	return prev;
}

void CControlWrapper::SetToolTip(const wxString& tip)
{
	if(m_control){
		m_control->SetToolTip(tip);
	}
}

bool CControlWrapper::IsWindowEnabled(){
    if(m_control){
		return m_control->IsEnabled();
	}
}

void CControlWrapper::MoveWindow(int l, int t, int r, int b)
{
	if(m_control){
		m_control->SetSize(l, t, r-l, b-t);
	}
}

void CControlWrapper::MoveWindow(CRect* rect)
{
	MoveWindow(rect->left, rect->right, rect->right, rect->bottom);
}

/*
 *	CSpinButtonCtrl
 */
void CSpinButtonCtrl::SetBuddy(const CControlWrapper& buddy) {
	//since wxSpinCtrl already has a textctrl, we do bot need this.
}

void CSpinButtonCtrl::SetRange(int nLower, int nUpper) {
	((wxSpinCtrl*)m_control)->SetRange(nLower, nUpper);
}

void CSpinButtonCtrl::SetPos(int pos) {
	((wxSpinButton*)m_control)->SetValue(pos);
}

int CSpinButtonCtrl::GetPos() {
	return ((wxSpinButton*)m_control)->GetValue();
}

int CControlWithItems::GetCount()
{
	return ((wxControlWithItems*)m_control)->GetCount();
};

int CControlWithItems::GetCurSel()
{
	return ((wxControlWithItems*)m_control)->GetSelection();
};

int CControlWithItems::SetCurSel(int sel)
{
	((wxControlWithItems*)m_control)->SetSelection(sel); return GetCurSel();
};

void CControlWithItems::ResetContent()
{
	if(m_control)
	{
		SetWindowText("");
		((wxControlWithItems*)m_control)->SetSelection(wxNOT_FOUND);
		((wxControlWithItems*)m_control)->Clear();
	}
}

void CControlWithItems::SetItemData(int index, unsigned long data)
{
	((wxControlWithItems*)m_control)->SetClientData(index, (void*)data);
};

unsigned long CControlWithItems::GetItemData(int index)
{
	return (unsigned long long)((wxControlWithItems*)m_control)->GetClientData(index);
};

void CControlWithItems::GetText(int index, wxString& str)
{
	str = ((wxControlWithItems*)m_control)->GetString(index);
};

void CControlWithItems::GetText(int index, wxChar* pstr)
{
	wcscpy(pstr, ((wxControlWithItems*)m_control)->GetString(index));
};

int CControlWithItems::GetTextLen(int index)
{
	return ((wxControlWithItems*)m_control)->GetString(index).Len();
}

#ifdef __WXGTK20__
int CComboBox::GetCurSel()
{
	return ((wxComboBox*)m_control)->GetCurrentSelection();
}//GetCurSel of wxComboBox don't work in event of SEL_CHANGE
#endif

int CComboBox::AddString(const wxString& str)
{
	return ((wxComboBox*)m_control)->Append(str);
};

void CComboBox::GetLBText(int index, wxString& str)
{
	str = ((wxComboBox*)m_control)->GetString(index);
};

void CComboBox::GetLBText(int index, char* str)
{
	strcpy(str, ((wxComboBox*)m_control)->GetString(index));
};

int CComboBox::FindString(int, const wxString& text)
{
	return ((wxComboBox*)m_control)->FindString(text);
}

///*****************************************************
///CListBox
///*****************************************************
BOOL CListBox::Create(DWORD dwStyle, CRect& cRect, wxWindow* pParent, UINT nID)
{
	wxListBox* wxlistbox = new wxListBox(pParent, nID, cRect.GetPosition(), cRect.GetSize(), 0, 0, dwStyle);
	m_control = wxlistbox;
	return true;
}

int CListBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
	wxString str(lpszItem);
	((wxListBox*)m_control)->InsertItems(1, &str, nIndex);

	return nIndex;
}

int CListBox::AddString(const char* lpszItem)
{
	return ((wxListBox*)m_control)->Append(lpszItem, (void*)0);
}

int CListBox::DeleteString(int index)
{
	((wxListBox*)m_control)->Delete(index);
	return index;
}

int CListBox::SetSel(int index, BOOL bSelect)
{
	if(bSelect)
		((wxListBox*)m_control)->Select(index);
	else
		((wxListBox*)m_control)->Deselect(index);
	return index;
}

int CListBox::GetSel(int index)
{
	return ((wxListBox*)m_control)->IsSelected(index) ? 1 : 0;
}

void CListBox::SetItemDataPtr(int nIndex, void* pData)
{
	((wxListBox*)m_control)->SetClientData(nIndex, pData);
}

void* CListBox::GetItemDataPtr(int nIndex)
{
	return ((wxListBox*)m_control)->GetClientData(nIndex);
}

void CListBox::SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem)
{
	((wxListBox*)m_control)->SetFirstItem(nFirstItem);
	if(bSelect)
	{
		((wxListBox*)m_control)->SetSelection(nLastItem - nFirstItem);
	}
	else
	{
		((wxListBox*)m_control)->Deselect(nLastItem - nFirstItem);
	}
	((wxListBox*)m_control)->SetFirstItem(0);


}

int CListBox::GetSelCount()
{
	return ((wxListBox*)m_control)->GetSelection();
}

int CListBox::GetSelItems(int nMaxItem, int * pItem)
{
	wxArrayInt arrayint;
	arrayint.Alloc(128);
	((wxListBox*)m_control)->GetSelections(arrayint);
	size_t j = arrayint.GetCount();
	for(size_t i = 0; i < j; i++)
	{
		*(pItem+i) = arrayint.Item(i);
	}
	return j;
}

/*
 *	xWndBase
 */
BEGIN_MESSAGE_MAP(CWndBase, xMessageHandler)

END_MESSAGE_MAP()
CWndBase::CWndBase()
{
	Init();
}

CWndBase::CWndBase(wxWindow *parent)
{
	Init();
	m_wndParent = parent;
}

static xEvtHandler eh;
void CWndBase::Init()
{
	m_wndParent = NULL;
	m_evtHandler = &eh;
}

void CWndBase::Invalidate(BOOL bErase)
{
	CControlWrapper(GetWindow()).Invalidate(bErase);
}

CControlWrapper CWndBase::GetDlgItem(int id)
{
	return CControlWrapper(GetWindow()->FindWindow(id));
}

CControlWrapper CWndBase::GetParent()
{
	return CControlWrapper(GetWindow()->GetParent());
}

int CWndBase::MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
	return wxMessageBox(lpszText, lpszCaption, nType, (wxDialog*)GetWindow());
}

void CWndBase::GetWindowRect(CRect* prect)
{
	*prect = GetWindow()->GetRect();
}

void CWndBase::GetClientRect(CRect* prect)
{
	prect->left = prect->top = 0;
	wxSize sz = GetWindow()->GetClientSize();
	prect->right = sz.x;
	prect->bottom = sz.y;
}

void CWndBase::SetWindowPos(void*, int l, int t, int w, int h, int flags)
{
	CControlWrapper(GetWindow()).SetWindowPos(0, l, t, w, h, flags);
}

void CWndBase::SetWindowText(const wxString& str)
{
	CControlWrapper(GetWindow()).SetWindowText(str);
}

void CWndBase::ShowWindow(int bshow)
{
    m_wndParent;
	CControlWrapper(GetWindow()).ShowWindow(bshow);
}
