#include "xgui.h"
#include <wx/config.h>
/*
 *	xApp
 */
CApp* GetApp()
{
	return (CApp*)wxTheApp;
}

CControlWrapper GetMainWnd()
{
	return CControlWrapper(GetApp()->GetTopWindow());
}

CApp::CApp()
{

}

CApp::~CApp()
{
	delete m_pDocManager;
}

BOOL CApp::InitInstance(){
	return true;
}

int CApp::ExitInstance()
{
	return 0;
}

void CApp::AddDocTemplate(wxDocTemplate* pTemplatein)
{
	wxString strDesc, strFilter, strExt;

    CSingleDocTemplate* pTemplate = wxStaticCast(pTemplatein, CSingleDocTemplate);
	if(pTemplate->GetDocString(strDesc,CSingleDocTemplate::filterExt ))
	{
		pTemplate->SetDescription(strDesc);
	}
	if(pTemplate->GetDocString( strFilter, CSingleDocTemplate::filterName))
	{
		pTemplate->SetFileFilter(strFilter);
	}
	if(pTemplate->GetDocString(strExt, CSingleDocTemplate::filterExt ))
	{
		pTemplate->SetDefaultExtension(strExt);
	}
}

bool CApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    SetVendorName("wxWidgets");
    SetAppName("wx_docview_sample");
    SetAppDisplayName("wxWidgets DocView Sample");

    m_pDocManager = new wxDocManager;
    //m_pPrintData = new wxPrintData;

    wxXmlResource::Get()->SetFlags(0);

    //wxXmlResource::Get()->AddHandler(new CMDIParentFrameXmlHandler);
    //wxXmlResource::Get()->AddHandler(new CMDIChildFrameXmlHandler);
    wxXmlResource::Get()->AddHandler(new CFrameWndXmlHandler);
     wxXmlResource::Get()->AddHandler(new CFormViewXmlHandler);
    wxXmlResource::Get()->InitAllHandlers();

    /*wxImage::AddHandler( new wxJPEGHandler );
    wxImage::AddHandler( new wxGIFHandler );
    wxImage::AddHandler( new wxICOHandler );
    wxImage::AddHandler( new wxCURHandler );
    wxImage::AddHandler( new wxPNGHandler );
    wxImage::AddHandler( new wxXPMHandler );
    wxImage::AddHandler( new wxBMPHandler );*/

    m_Locale.Init(wxLANGUAGE_CHINESE_SIMPLIFIED);

    return InitInstance() ? true : false;
}

int CApp::OnExit()
{
	return ExitInstance();
}

long CApp::GetProfileInt(const wxString& section, const wxString& entry, long default_val)
{
	wxConfigBase* conf = wxConfigBase::Get();
	long ret = default_val;
	conf->Read(section + '/' + entry, &ret, default_val);
	return ret;
}

wxString CApp::GetProfileString(const wxString& section, const wxString& entry, const wxString& default_val)
{
	wxConfigBase* conf = wxConfigBase::Get();
	return conf->Read(section + '/' + entry, default_val);
}

void CApp::WriteProfileInt(const wxString& section, const wxString& entry, int value)
{
	wxConfigBase* conf = wxConfigBase::Get();
	conf->Write(section + '/' + entry, value);
	conf->Flush();
}

void CApp::WriteProfileString(const wxString& section, const wxString& entry, const wxString& value)
{
	wxConfigBase* conf = wxConfigBase::Get();
	conf->Write(section + '/' + entry, value);
	conf->Flush();
}

wxCursor* CApp::LoadCursor(const char* str)
{
#if defined(_WIN32) || defined(_WIN64)
	return new wxCursor(wxImage(str, wxBITMAP_TYPE_CUR));
#else
	return new wxCursor(str, wxBITMAP_TYPE_CUR);
#endif
}

void CApp::BeginWaitCursor()
{
	wxBeginBusyCursor();
}

void CApp::EndWaitCursor()
{
	wxEndBusyCursor();
}
