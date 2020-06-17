#include "xgui.h"

/*
 *	xDataExchange
 */
CDataExchange::CDataExchange(CDialogBase* pDlgWnd, BOOL bSaveAndValidate)
	: m_bSaveAndValidate(bSaveAndValidate),
	m_pDlgWnd(pDlgWnd), m_bExchangeOk(TRUE) {}

BOOL CDataExchange::Ok()
{
	return m_bExchangeOk;
}

/*
 *	xDialogBase
 */
BEGIN_MESSAGE_MAP(CDialogBase, CWndBase)
	ON_BN_CLICKED(wxID_OK, OnButtonOk)
	ON_BN_CLICKED(wxID_CANCEL, OnButtonCancel)
END_MESSAGE_MAP()

CDialogBase::CDialogBase() : CWndBase()
{
	m_szResource = 0;
	m_bInitDialogCalled = false;
}

CDialogBase::CDialogBase(const char* resource, wxWindow *parent) :
		CWndBase(parent)
{
	m_szResource = resource;
	m_bInitDialogCalled = false;
}

BOOL CDialogBase::UpdateData(BOOL bSaveData)
{
	CDataExchange dx(this, bSaveData);
	DoDataExchange(&dx);

	return dx.Ok();
}

BOOL CDialogBase::OnInitDialog()
{
    CDataExchange dx(this, FALSE);
	DoDataExchange(&dx);

	return TRUE;
}

void CDialogBase::OnOK()
{
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	evt.SetId(wxID_OK);
	GetEvtHandler()->GetHost()->ProcessWindowEvent(evt);
}

void CDialogBase::OnCancel()
{
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	evt.SetId(wxID_CANCEL);
	GetEvtHandler()->GetHost()->ProcessWindowEvent(evt);
}

void CDialogBase::OnButtonOk()
{
	OnOK();
}

void CDialogBase::OnButtonCancel()
{
	OnCancel();
}

bool CDialogBase::LoadXmlRes()
{
	return wxXmlResource::Get()->LoadDialog((wxDialog*)GetWindow(), m_wndParent, GetIDDS());
}

/*
 *	xDialog
 */
IMPLEMENT_DYNAMIC_CLASS(CDialog, wxDialog)

CDialog::CDialog()
{
	m_evtHandler->SetHost(this, this);
}

CDialog::CDialog(wxWindow *parent, const char* resource) :
	CDialogBase(resource, parent)
{
	m_evtHandler->SetHost(this, this);
}

CDialog::CDialog(int idd, wxWindow *parent) :
	CDialogBase(0, parent)
{
	m_evtHandler->SetHost(this, this);
}

//CDialog::CDialog(wxWindow *parent, wxWindowID id,
//             const wxString& title,
//             const wxPoint& pos,
//             const wxSize& size,
//             long style,
//             const wxString& name) :
//	CDialogBase(0, parent),
//	wxDialog(parent, id, title, pos, size, style, name)
//{
//}

CDialog::~CDialog()
{
}

int CDialog::DoModal()
{
	int ret = -1;
	if(LoadXmlRes()){
		Center();
		InitDialog();
		ret = ShowModal();
		DestroyWindow();
	}

	return ret;
}

BOOL CDialog::Create(int, wxWindow* parent)
{
	if(wxWindow::GetParent())
		wxWindow::GetParent()->RemoveChild(this);

	m_wndParent = parent;

	if(LoadXmlRes()){
		InitDialog();
		return TRUE;
	}

	return FALSE;
}

//
//BOOL CDialog::MakeWindow()
//{
//	return Create()
//}

/*
 *	xPropertyPage
 */
IMPLEMENT_DYNAMIC_CLASS(CPropertyPage, wxPanel)

CPropertyPage::CPropertyPage()
{
}

CPropertyPage::CPropertyPage(UINT idd) :
	CDialogBase(GetApp()->GetResName(idd), 0)
{
}


CPropertyPage::CPropertyPage(const char *res) :
	CDialogBase(res, 0)
{
}

/*CPropertyPage::CPropertyPage(wxWindow *parent, const char* resource) :
	CDialogBase(resource, parent)
{
}*/

CPropertyPage::~CPropertyPage()
{
}

/*
 *	xPropertySheet
 */
IMPLEMENT_DYNAMIC_CLASS(CPropertySheet, wxPropertySheetDialog)

CPropertySheet::CPropertySheet()
{
}

CPropertySheet::CPropertySheet(const wxString& title, wxWindow *parent, UINT sel) :
	CDialogBase(0, parent),
	wxPropertySheetDialog(parent, wxID_ANY, title),
	m_nSelectedPage(sel)
{
}

CPropertySheet::~CPropertySheet()
{
}

void CPropertySheet::AddPage(CPropertyPage* page)
{
	if(page->LoadXmlRes())
		GetBookCtrl()->AddPage(page, page->GetLabel());
}

int CPropertySheet::GetActiveIndex()
{
	return GetBookCtrl()->GetSelection();
}

BOOL CPropertySheet::OnInitDialog()
{
	GetBookCtrl()->SetSelection(m_nSelectedPage);

	return TRUE;
}

int CPropertySheet::DoModal()
{
	int ret = ShowModal();
	DestroyWindow();
	return ret;
}

/*
 *	DDX
 */
void DDX_Control(CDataExchange* pDX, int nIDC, CControlWrapper& control)
{
	wxWindow* win =  pDX->m_pDlgWnd->GetWindow()->FindWindow(nIDC);
	ASSERT(win);

	control.m_control = win;
}

void DDV_MinMaxInt(CDataExchange* pDX, int value, int mixValue, int MaxValue)
{
	if(value < mixValue || value > MaxValue){
		wxString str;
		str.Printf("������һ����%d��%d֮�����ֵ", mixValue, MaxValue);
		wxString msgTitle;
		if (!wxTheApp->GetAppName().empty())
			msgTitle = wxTheApp->GetAppName();
		else
			msgTitle = wxString(_("���ݼ������"));

		wxMessageDialog(pDX->m_pDlgWnd->GetWindow(), str, msgTitle, wxOK | wxICON_EXCLAMATION);
	}
}

void DDV_MaxChars(CDataExchange* pDX, wxString const& value, int nChars)
{
	if(value.Len() > (size_t)nChars){
		wxString str;
		str.Printf("������һ�����Ȳ�����%d���ַ���", nChars);
		wxString msgTitle;
		if (!wxTheApp->GetAppName().empty())
			msgTitle = wxTheApp->GetAppName();
		else
			msgTitle = wxString(_("���ݼ������"));

		wxMessageDialog(pDX->m_pDlgWnd->GetWindow(), str, msgTitle, wxOK | wxICON_EXCLAMATION);
	}
}

void _CDDX_Text(CDataExchange* pDX, int nIDC, wxString& value)
{
	CControlWrapper control = pDX->m_pDlgWnd->GetDlgItem(nIDC);
	if(pDX->Ok())
	{
		if(pDX->m_bSaveAndValidate){
			control.GetWindowText(value);
		}else{
			control.SetWindowText(value);
		}
	}
}

bool _CDDX_Numeric(CDataExchange* pDX, int nIDC, long& value)
{
	wxString str;
	str.Printf("%d", value);

	_CDDX_Text(pDX, nIDC, str);

	if(pDX->m_bSaveAndValidate)
	{
		if(!str.ToLong(&value)){
			value = 0;

			wxString msgTitle;
			if (!wxTheApp->GetAppName().empty())
				msgTitle = wxTheApp->GetAppName();
			else
				msgTitle = wxString(_("�ļ�����"));

			wxMessageDialog(pDX->m_pDlgWnd->GetWindow(), "������һ����ֵ", msgTitle, wxOK | wxICON_EXCLAMATION);

			return false;
		}
	}

	return true;
}

bool _CDDX_Double(CDataExchange* pDX, int nIDC, double& value)
{
	wxString str;
	_CDDX_Text(pDX, nIDC, str);

	if(!str.ToDouble(&value)){
		value = 0.f;
		return false;
	}

	return true;
}

void DDX_Text(CDataExchange* pDX, int nIDC, wxByte& value)
{
	long val;
	_CDDX_Numeric(pDX, nIDC, val);
	if(pDX->m_bSaveAndValidate)
		value = (wxByte)val;
}

void DDX_Text(CDataExchange* pDX, int nIDC, short& value)
{
	long val;
	_CDDX_Numeric(pDX, nIDC, val);
	if(pDX->m_bSaveAndValidate)
		value = (short)val;
}

void DDX_Text(CDataExchange* pDX, int nIDC, int& value)
{
	long val;
	_CDDX_Numeric(pDX, nIDC, val);
	if(pDX->m_bSaveAndValidate)
		value = (int)val;
}

void DDX_Text(CDataExchange* pDX, int nIDC, unsigned int& value)
{
	long val;
	_CDDX_Numeric(pDX, nIDC, val);
	if(pDX->m_bSaveAndValidate)
		value = (unsigned int)val;
}

void DDX_Text(CDataExchange* pDX, int nIDC, long& value)
{
	long val;
	_CDDX_Numeric(pDX, nIDC, val);
	if(pDX->m_bSaveAndValidate)
		value = val;
}

void DDX_Text(CDataExchange* pDX, int nIDC, unsigned long& value)
{
	long val;
	_CDDX_Numeric(pDX, nIDC, val);
	if(pDX->m_bSaveAndValidate)
		value = (unsigned long)val;
}

void DDX_Text(CDataExchange* pDX, int nIDC, wxString& value)
{
	_CDDX_Text(pDX, nIDC, value);
}

void DDX_Text(CDataExchange* pDX, int nIDC, wxChar* value, int nMaxLen)
{
	wxString str;
	_CDDX_Text(pDX, nIDC, str);
	if(pDX->m_bSaveAndValidate)
	{
		wxStrncpy(value, str.c_str(), nMaxLen);
		value[nMaxLen-1] = 0;
	}
}

void DDX_Text(CDataExchange* pDX, int nIDC, float& value)
{
	double val;
	_CDDX_Double(pDX, nIDC, val);
	value = (float)val;
}

void DDX_Text(CDataExchange* pDX, int nIDC, double& value)
{
	double val;
	_CDDX_Double(pDX, nIDC, val);
	value = val;
}

void DDX_Text(CDataExchange* pDX, int nIDC, wxDateTime& value)
{
}

void DDX_Check(CDataExchange* pDX, int nIDC, BOOL& value)
{

}

void DDX_Radio(CDataExchange* pDX, int nIDC, int value)
{

}
