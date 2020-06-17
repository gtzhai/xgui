#include "xgui.h"

/*
 *	xFrameWnd
 */
IMPLEMENT_DYNAMIC_CLASS(CFrameWnd, wxDocParentFrame)
BEGIN_MESSAGE_MAP(CFrameWnd, CWndBase)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CFrameWnd::CFrameWnd()
{
    m_docManager = GetApp()->m_pDocManager;
	m_evtHandler->SetHost(this, this);
}

CFrameWnd::~CFrameWnd()
{

}

// Extend event processing to search the view's event table
bool CFrameWnd::ProcessEvent(wxEvent& event)
{
    // Try the document manager, then do default processing
    if (!m_docManager || !m_docManager->ProcessEvent(event))
        return wxEvtHandler::ProcessEvent(event);
    else
        return true;
}

BOOL CFrameWnd::OnCreateClient(CREATESTRUCT* cs, CCreateContext* pContext)
{
    return TRUE;
}

int CFrameWnd::OnCreate(CREATESTRUCT*)
{
	return 0;
}

bool CFrameWnd::Create(wxFrame *parent, wxWindowID id, const wxString &title,
                    const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{

	CREATESTRUCT cs;
	cs.lpCreateParams = 0;
	cs.hMenu = 0;
	cs.hwndParent = 0;
	cs.cx = size.x;
	cs.cy = size.y;
	cs.y = pos.x;
	cs.x = pos.y;
	cs.style = wxDEFAULT_FRAME_STYLE;
	cs.lpszName = 0;
	cs.dwExStyle = 0;

	if(PreCreateWindow(cs))
	{
		m_tempInfo = &cs;//store this for OnCreate

        if(wxDocParentFrame::Create(m_docManager,
			parent, id, title, pos, size, style,  name))
		{
			//since Create event happens after windows been shown,
			//we make it it happen early. xEvetHandler make sure
			//the later Create event will be discarded.
			wxWindowCreateEvent event(this);
			GetEventHandler()->ProcessEvent(event);

			CCreateContext context;
			context.m_pNewViewClass = 0;
			context.m_pCurrentDoc = 0;
            m_wndParent = this;
			if(OnCreateClient(&cs, &context)){
				return true;
			}
		}
	}

	Destroy();

	return false;
}

bool CFrameWnd::LoadFrame(const wxString& resource)
{
    return wxXmlResource::Get()->LoadObject(this, 0, resource, "CFrameWnd");
}

void CFrameWnd::SetActiveView(CView* pViewNew, BOOL bNotify)
{
    wxView *oldview = m_docManager->GetCurrentView();

    m_docManager->ActivateView (oldview, false);
    m_docManager->ActivateView (pViewNew, true);
    pViewNew->GetCanvas()->Layout();
	if(bNotify){
		oldview->OnActivateView(FALSE, pViewNew, oldview);
		pViewNew->OnActivateView(TRUE, pViewNew, oldview);
	}
}

CView* CFrameWnd::GetActiveView()
{
    wxView* pView = 0;
    pView = m_docManager->GetCurrentView();
    if(pView == 0) return NULL;
    return wxStaticCast(pView, CView);
}

CDocument* CFrameWnd::GetActiveDocument()
{
    wxDocument *doc = m_docManager->GetCurrentDocument();
    if(doc == NULL) return NULL;
    return wxStaticCast(m_docManager->GetCurrentDocument(), CDocument);
}


IMPLEMENT_DYNAMIC_CLASS(CFrameWndXmlHandler, wxFrameXmlHandler)
wxObject *CFrameWndXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(frame, CFrameWnd);


    if(frame->Create(NULL, wxID_ANY, GetText(wxT("title")),
		wxDefaultPosition, wxDefaultSize,
		GetStyle(wxT("style"), wxDEFAULT_FRAME_STYLE),
		GetName()))
	{

		if (HasParam(wxT("size")))
			frame->SetClientSize(GetSize(wxT("size"), frame));
		else
			frame->SetClientSize(500, 400);

		if (HasParam(wxT("pos")))
			frame->Move(GetPosition());
		if (HasParam(wxT("icon")))
			frame->SetIcon(GetIcon(wxT("icon"), wxART_FRAME_ICON));

		SetupWindow(frame);

		CreateChildren(frame);

		if (GetBool(wxT("centered"), true))
			frame->Centre();
	}
	else
		frame = NULL;

	return frame;
}

bool CFrameWndXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("CFrameWnd"));
}

/*
 *	xView
 */

CViewCanvas::CViewCanvas(wxView *view, wxWindow *parent,const wxPoint& pos, const wxSize& size,long style)
    : wxScrolledWindow(parent ? parent : view->GetFrame(), wxID_ANY, pos, size, style)
{
    m_view = view;
    m_wndParent = this;
    //SetCursor(wxCursor(wxCURSOR_PENCIL));

    // this is completely arbitrary and is done just for illustration purposes
    SetVirtualSize(480, 272);
    SetScrollRate(20, 20);
    SetBackgroundColour(*wxWHITE);
}

CViewCanvas::~CViewCanvas()
{

}

// Define the repainting behaviour
void CViewCanvas::OnDraw(wxDC& dc)
{
    if ( m_view )
        m_view->OnDraw(& dc);
}


IMPLEMENT_DYNAMIC_CLASS(CView, wxView)

BEGIN_MESSAGE_MAP(CView, xMessageHandler)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CView::CView() : m_pView(0), m_Canvas(0)
{
	m_ScrollHelper = 0;
}

CView::~CView()
{
    if(m_evtHandler)
        m_evtHandler->SetHost(0,0);//must do this before delete scroll helper
	if(m_ScrollHelper) delete m_ScrollHelper;
}

void CView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// Default to one page printing if doc length not known
	if (pInfo != NULL)
		pInfo->m_bContinuePrinting =
		(pInfo->GetMaxPage() != 0xffff || (pInfo->m_nCurPage == 1));

	int x = GetCanvas()->GetScrollPos(wxHORIZONTAL);
	int y = GetCanvas()->GetScrollPos(wxVERTICAL);

	wxPoint pt = pDC->GetViewportOrg();
	pDC->SetViewportOrg(pt.x-x, pt.y-y);
}


void CView::OnPrint(CDC* pDC, CPrintInfo*)
{
	//OnDraw(pDC);
}

void CView::OnActiveView(BOOL bActived, CView* pActiveView, CView* pDeactiveView)
{
	if(bActived)
		m_Canvas->SetFocus();
}

void CView::OnUpdate(CView* sender)
{
    wxView::OnUpdate(sender);
	if (m_Canvas)
		m_Canvas->Refresh();
}

void CView::OnUpdate(wxView* sender, wxObject* hint)
{
    wxView::OnUpdate(sender, hint);
    if ( m_Canvas )
        m_Canvas->Refresh();
}

void CView::OnInitialUpdate(){

}

bool CView::OnCreate(wxDocument *doc, long flags)
{
    return true;
}

wxWindow* CView::CreateCanvas(wxWindow* parent, wxPoint& point, wxSize& size, long style)
{
    CViewCanvas* wnd = new CViewCanvas(this, parent, point, size, style);
	m_evtHandler->SetHost(wnd, this);

	//if(!wnd->Create(parent, wxID_ANY, point, size, style | wxSUNKEN_BORDER | wxFULL_REPAINT_ON_RESIZE))
	if(!wnd)
	{
		m_evtHandler->SetHost(0,0);
		delete wnd;
		wnd = 0;
	}

	return wnd;
}

bool CView::Create(wxWindow* parent, wxPoint& point, wxSize& size)
{
	CREATESTRUCT cs;
	cs.lpCreateParams = 0;
	cs.hMenu = 0;
	cs.hwndParent = 0;
	cs.cx = size.x;
	cs.cy = size.y;
	cs.y = point.x;
	cs.x = point.y;
	cs.style = wxTAB_TRAVERSAL;
	cs.lpszName = 0;
	cs.dwExStyle = 0;

	if(PreCreateWindow(cs))
	{
		wxWindow* wnd = CreateCanvas(parent, point, size, cs.style);
		if(wnd)
		{
			m_Canvas = wnd;

            m_evtHandler->SetHost(0,0);
            m_ScrollHelper = new xScrollHelper(m_Canvas);
			m_evtHandler->SetHost(m_Canvas,this);
			 m_wndParent = m_Canvas;
            m_Canvas->Layout();

			return true;
		}
	}

	return false;
}

CDocument* CView::GetDocument()
{
	return (CDocument*)m_pView->GetDocument();
}

wxFrame* CView::GetParentFrame()
{
	return (wxFrame*)m_pView->GetFrame();
}

int CView::GetScrollRange(int orientation)
{
	int w,h;
	m_Canvas->GetVirtualSize(&w,&h);

	if(orientation == wxVERTICAL)
		return h;
	return w;
}

void CView::SetScrollRange(int orientation, int minpos, int maxpos, BOOL bRedraw)
{
	int w,h;
	m_Canvas->GetVirtualSize(&w,&h);

	if(orientation == wxVERTICAL)
		m_ScrollHelper->SetScrollbars(1, 1, w, maxpos, 0, 0, true);
	else
		m_ScrollHelper->SetScrollbars(1, 1, maxpos, h, 0, 0, true);

	if(bRedraw)
		m_ScrollHelper->AdjustScrollbars();
}

void CView::ScrollPage(int orientation, int delta)
{
	if(delta < 0)
	{
		wxScrollWinEvent event(wxEVT_SCROLLWIN_PAGEUP, 0, orientation);
		m_evtHandler->ProcessEvent(event);
	}
	else
	{
		wxScrollWinEvent event(wxEVT_SCROLLWIN_PAGEDOWN, 0, orientation);
		m_evtHandler->ProcessEvent(event);
	}
}

void CView::ScrollLine(int orientation, int delta)
{
	if(delta < 0)
	{
		wxScrollWinEvent event(wxEVT_SCROLLWIN_LINEUP, 0, orientation);
		m_evtHandler->ProcessEvent(event);
	}
	else
	{
		wxScrollWinEvent event(wxEVT_SCROLLWIN_LINEDOWN, 0, orientation);
		m_evtHandler->ProcessEvent(event);
	}
}

void CView::ScrollTo(int orientation, int pos)
{
	wxScrollWinEvent event(wxEVT_SCROLLWIN_THUMBRELEASE, pos, orientation);
	m_evtHandler->ProcessEvent(event);
}

void CView::Scroll(int orientation, int delta)
{
	int w,h;
	m_Canvas->GetVirtualSize(&w,&h);
	int poslimit = (orientation == wxVERTICAL ? h : w);
	int pos = GetCanvas()->GetScrollPos(orientation);
	pos += delta;
	pos = std::max(0, pos);
	pos = std::min(pos, poslimit);
	ScrollTo(orientation, pos);
}

BOOL CView::DoPreparePrinting(CPrintInfo*)
{
        return true;
}

BOOL CView::OnPreparePrinting(CPrintInfo*)
{
        return true;
}

void CView::OnBeginPrinting(CDC* pDC, CPrintInfo*)
{
}

void CView::OnEndPrinting(CDC* pDC, CPrintInfo*)
{
}

void CView::OnFilePrint()
{
    CPrintout printout(this,wxT(""));
	wxPrinter printer(printout.m_printInfo->m_pPD);
	if (printer.Print((wxWindow *)GetParentFrame(), &printout, true /*prompt*/))
	{
		(*(GetApp()->m_pPrintData)) = printer.GetPrintDialogData().GetPrintData();
  	}
}

void CView::OnFilePrintPreview()
{
    wxPrintPreview *preview = new wxPrintPreview(new CPrintout(this, wxT("")),
		new CPrintout(this, wxT("")),
		GetApp()->m_pPrintData);
    if (!preview->Ok())
    {
        delete preview;
        wxMessageBox(_T("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _T("Previewing"), wxOK);
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, GetParentFrame(), _T("��ӡԤ��"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}

void CView::OnPaint()
{
	wxPaintDC pdc(m_Canvas);

	CDC dc;
	dc.SetDC(&pdc, this);

	OnPrepareDC(&dc, NULL);
	//OnDraw(&dc);
}


void CView::OnDraw(wxDC *WXUNUSED(dc))
{
    // nothing to do here, wxTextCtrl draws itself
}

bool CView::OnClose(bool deleteWindow)
{
    return wxView::OnClose(deleteWindow);
}

/*
 * xGLView
 */
IMPLEMENT_DYNAMIC_CLASS(CGLView, CView)

wxWindow* CGLView::CreateCanvas(wxWindow* parent, wxPoint& point, wxSize& size, long style)
{
	//WJF: no opengl support on unix;
	return CView::CreateCanvas(parent, point, size, style);

//	int attr[] = {WX_GL_RGBA,0};
//	wxGLCanvas* wnd = new wxGLCanvas(parent, wxID_ANY, point, size, style|wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE, wxGLCanvasName, attr);
//	m_evtHandler->SetHost(wnd, this);
//	//wxEVT_CREATE happened before m_evtHandler->SetHost, we make a OnCreate();
//	m_tempInfo = 0;
//	wxWindowCreateEvent evt(wnd);
//	m_evtHandler->ProcessEvent(evt);
//
//	return wnd;
}

/*
 *	xTreeView
 */
IMPLEMENT_DYNAMIC_CLASS(CTreeView, CView)
BEGIN_MESSAGE_MAP(CTreeView, CView)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CTreeView::OnPaint()
{
	GetEvtHandler()->m_pRecentEvent->Skip();
}

bool CTreeView::Create(wxWindow* parent, wxPoint& point, wxSize& size)
{
	CREATESTRUCT cs;
	cs.lpCreateParams = 0;
	cs.hMenu = 0;
	cs.hwndParent = 0;
	cs.cx = size.x;
	cs.cy = size.y;
	cs.y = point.x;
	cs.x = point.y;
	cs.style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxBORDER_NONE;
	cs.lpszName = 0;
	cs.dwExStyle = 0;

	if(PreCreateWindow(cs))
	{
		m_Canvas = new CTreeCtrl();
		m_evtHandler->SetHost(m_Canvas, this);
		((CTreeCtrl*)m_Canvas)->Create(parent, wxID_ANY, point, size, cs.style|wxTR_HIDE_ROOT);

		return true;
	}

	return false;
}

/*
 *	xListView
 */
IMPLEMENT_DYNAMIC_CLASS(CListView, CView)
BEGIN_MESSAGE_MAP(CListView, CView)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CListView::OnPaint()
{
	GetEvtHandler()->m_pRecentEvent->Skip();
}


bool CListView::Create(wxWindow* parent, wxPoint& point, wxSize& size)
{
	CREATESTRUCT cs;
	cs.lpCreateParams = 0;
	cs.hMenu = 0;
	cs.hwndParent = 0;
	cs.cx = size.x;
	cs.cy = size.y;
	cs.y = point.x;
	cs.x = point.y;
	cs.style = wxLC_ICON;
	cs.lpszName = 0;
	cs.dwExStyle = 0;

	if(PreCreateWindow(cs))
	{
		m_Canvas = new wxListCtrl();
		m_evtHandler->SetHost(m_Canvas, this);
		((wxListCtrl*)m_Canvas)->Create(parent, wxID_ANY, point, size, cs.style);

		return true;
	}

	return false;
}

IMPLEMENT_DYNAMIC_CLASS(CFormView, CView)
BEGIN_MESSAGE_MAP(CFormView, CView)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CFormView::CFormView(const char* lpszTemplateName):m_lpszTemplateName(lpszTemplateName), m_nIDTemplate(0){

}

CFormView::CFormView(UINT nIDTemplate):m_lpszTemplateName(0), m_nIDTemplate(nIDTemplate){

}

void CFormView::OnPaint()
{
	GetEvtHandler()->m_pRecentEvent->Skip();
}

bool CFormView::OnCreate(wxDocument *doc, long flags)
{
    bool ret;
    if(m_lpszTemplateName)
        ret = wxXmlResource::Get()->LoadObject (this, wxStaticCast(GetApp()->GetTopWindow(), wxDocParentFrame), m_lpszTemplateName, "CFormView");
    else {
        ret = wxXmlResource::Get()->LoadObject (this, wxStaticCast(GetApp()->GetTopWindow(), wxDocParentFrame), wxXmlResource::FindXRCIDById(m_nIDTemplate), "CFormView");
    }

    return ret;
}

bool CFormView::LoadView(wxFrame*parent)
{
    bool ret;
    if(m_lpszTemplateName)
        ret = wxXmlResource::Get()->LoadObject (this, parent, m_lpszTemplateName, "CFormView");
    else {
        ret = wxXmlResource::Get()->LoadObject (this, parent, wxXmlResource::FindXRCIDById(m_nIDTemplate), "CFormView");
    }

    return ret;
}

bool CFormView::Create(wxWindow* parent, wxPoint& point, wxSize& size)
{
	CREATESTRUCT cs;
	cs.lpCreateParams = 0;
	cs.hMenu = 0;
	cs.hwndParent = 0;
	cs.cx = size.x;
	cs.cy = size.y;
	cs.y = point.x;
	cs.x = point.y;
	cs.style = wxLC_ICON;
	cs.lpszName = 0;
	cs.dwExStyle = 0;

	if(PreCreateWindow(cs))
	{
            m_Canvas = CView::CreateCanvas(parent, point, size, cs.style);
            if(m_evtHandler)
                m_evtHandler->SetHost(m_Canvas, this);
            m_wndParent = m_Canvas;
            // m_Canvas->SetSize(size);
            //m_Canvas->Centre();
            //m_Canvas->Show();
            //Sleep(5);
            return true;
	}

	return false;
}

IMPLEMENT_DYNAMIC_CLASS(CFormViewXmlHandler, wxXmlResourceHandler)

CFormViewXmlHandler::CFormViewXmlHandler() : wxXmlResourceHandler()
{

}

wxObject *CFormViewXmlHandler::DoCreateResource()
{
    CFormView * view = wxStaticCast(GetInstance(), CFormView);
    wxWindow* win;
    wxPoint point = GetPosition();
    wxSize size = GetSize();
   /* view->Create(GetParentAsWindow(),
                  GetID(),
                  GetPosition(), GetSize(),
                  GetStyle(wxT("style"), wxTAB_TRAVERSAL),
                  GetName());*/
      view->Create(GetParentAsWindow(), point, size);
    win = view->GetCanvas();

    SetupWindow(win);
    CreateChildren(win);

    return win;
}

bool CFormViewXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("CFormView"));
}

/*
 *	xDocument
 */
IMPLEMENT_DYNAMIC_CLASS(CDocument, wxDocument)
BEGIN_MESSAGE_MAP(CDocument, CWndBase)
END_MESSAGE_MAP()

bool CDocument::OnSaveDocument(const wxString& filename)
{
    wxString msgTitle;
    if (!wxTheApp->GetAppName().empty())
        msgTitle = wxTheApp->GetAppName();
    else
        msgTitle = wxString(_("�ļ�����"));

	CFile file;
	if(!file.Open(filename, CFile::modeReadWrite))
    {
		wxString str;
		str.Printf(_("���ļ� %s ʧ��"), filename.c_str());
        (void)wxMessageBox(str, msgTitle, wxOK | wxICON_EXCLAMATION,
                           GetDocumentWindow());
        // Saving error
        return false;
    }

	/*CArchive ar(&file, CArchive::save);
    Serialize(ar);*/

	return true;
}

BOOL CDocument::OnOpenDocument(LPCTSTR filename)
{
   wxString msgTitle;
    if (!wxTheApp->GetAppName().empty())
        msgTitle = wxTheApp->GetAppName();
    else
        msgTitle = wxString(_("�ļ�����"));

	CFile file;
	if(!file.Open(filename, CFile::modeRead))
    {
		wxString str;
		str.Printf(_("���ļ� %s ʧ��"), filename);
         (void)wxMessageBox(str, msgTitle, wxOK | wxICON_EXCLAMATION,
                           GetDocumentWindow());
        // Saving error
        return false;
    }

	/*CArchive ar(&file, CArchive::load);
	Serialize(ar);*/

	return TRUE;
}

bool CDocument::OnOpenDocument(const wxString& filename)
{
 	BOOL ret = OnOpenDocument(filename.c_str().AsChar());

	SendInitialUpdate();

	return ret != 0;
}

bool CDocument::OnNewDocument()
{
	bool ret = wxDocument::OnNewDocument();
	if(ret){
		SendInitialUpdate();
	}

	return ret;
}

void CDocument::SendInitialUpdate()
{
	wxList& list = GetViews();
	wxNode* node = list.GetFirst();
	#if 0
	while(node){
		_innerView* view = (_innerView*)node->GetData();
		view->m_xview->OnInitialUpdate();
		node = node->GetNext();
	}
	#else
    while(node){
		CView* view = (CView*)node->GetData();
		view->OnInitialUpdate();
		node = node->GetNext();
	}
	#endif
}

void CDocument::UpdateAllViews(CView *sender){
	wxDocument::UpdateAllViews(sender ? (wxView*)sender->m_pView : NULL, 0);
}

bool CDocument::DeleteAllViews()
{
	CMDIChildFrame* frame = 0;
	wxView* view = GetFirstView();
	if(view)
		frame = (CMDIChildFrame*)view->GetFrame();
	bool ret = wxDocument::DeleteAllViews();
	if(ret && frame){
		frame->SetView(0);
	}
	return ret;
}

/*
 *xSingleDocTemplate
 */
CSingleDocTemplate::CSingleDocTemplate(const wxString& resource, wxClassInfo* docinfo,
        wxClassInfo* frm_info, wxClassInfo* viewinfo):wxDocTemplate(GetApp()->m_pDocManager, "", ""
                                                                         ,"", "", "", "", docinfo, viewinfo), m_resource(resource), m_frmClassInfo(frm_info)
{
    wxDocManager *m = GetDocumentManager();
    m->SetMaxDocsOpen(1);
}

/*
 *	xMultiDocTemplate
 */
CMultiDocTemplate::CMultiDocTemplate(const wxString& resource, wxClassInfo* docinfo,
	wxClassInfo* childfrm_info, wxClassInfo* viewinfo) :
		wxDocTemplate(GetApp()->m_pDocManager, "", ""
		,"", "", "", "", docinfo, CLASSINFO(_innerView)),
		m_resource(resource),
		m_childfrmClassInfo(childfrm_info),
		m_userViewClassInfo(viewinfo)
{
	m_docTypeName = resource;
	m_viewTypeName = resource;
}

wxClassInfo* CMultiDocTemplate::GetUserViewClassInfo()
{
	return m_userViewClassInfo;
}

wxDocument* CMultiDocTemplate::OpenDocumentFile(const char* path, long flags)
{
	if(flags & wxDOC_NEW)
	{
		wxDocument* newDoc = CreateDocument(path, flags);
		if(newDoc)
		{
			newDoc->SetDocumentName(GetDocumentName());
			newDoc->SetDocumentTemplate(this);
			if (!newDoc->OnNewDocument() )
			{
				 // Document is implicitly deleted by DeleteAllViews
				 newDoc->DeleteAllViews();
				 return NULL;
			}
		}
		return newDoc;
	}
	else
	{
        wxDocument *newDoc = CreateDocument(path, flags);
        if (newDoc)
        {
            newDoc->SetDocumentName(GetDocumentName());
            newDoc->SetDocumentTemplate(this);
            if (!newDoc->OnOpenDocument(path))
            {
                newDoc->DeleteAllViews();
                // delete newDoc; // Implicitly deleted by DeleteAllViews
                return (wxDocument *) NULL;
            }
            // A file that doesn't use the default extension of its document
            // template cannot be opened via the FileHistory, so we do not
            // add it.
            if (FileMatchesTemplate(path))
                GetDocumentManager()->AddFileToHistory(path);
        }
        return newDoc;
	}
}

wxDocument* CMultiDocTemplate::CreateDocument(const wxString& path, long flags)
{
	CDocument *doc = (CDocument*)DoCreateDocument();

	if(doc){
		doc->SetFilename(path);
		doc->SetDocumentTemplate(this);
		GetDocumentManager()->AddDocument(doc);
		doc->SetCommandProcessor(doc->OnCreateCommandProcessor());

		//create child frame now;
		CMDIChildFrame* childframe = (CMDIChildFrame*)m_childfrmClassInfo->CreateObject();
		childframe->SetParent(GetApp()->GetTopWindow());
		childframe->SetDocument(doc);
		if(!childframe->LoadFrame(m_resource,GetApp()->GetTopWindow() ))
		{
			doc->DeleteAllViews();
			doc = 0;
		}
	}

	return doc;
}
IMPLEMENT_CLASS(CDocChildFrame, wxFrame)
IMPLEMENT_CLASS(CDocParentFrame, wxFrame)
BEGIN_MESSAGE_MAP(CDocChildFrame, CWndBase)

END_MESSAGE_MAP()
BEGIN_MESSAGE_MAP(CDocParentFrame, CWndBase)

END_MESSAGE_MAP()
BEGIN_EVENT_TABLE(CDocChildFrame, wxFrame)
    EVT_ACTIVATE(CDocChildFrame::OnActivate)
    EVT_CLOSE(CDocChildFrame::OnCloseWindow)
END_EVENT_TABLE()

CDocChildFrame::CDocChildFrame()
{
    m_childDocument = 0;
    m_childView = 0;
}

bool CDocChildFrame::Create(wxDocument *doc,
                wxFrame *frame,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long type,
                const wxString& name)
{
        m_childDocument = doc;
        return wxFrame::Create(frame,id, title, pos, size, type, name);
}

// Extend event processing to search the view's event table
bool CDocChildFrame::ProcessEvent(wxEvent& event)
{
    if (m_childView)
        m_childView->Activate(true);

    if ( !m_childView || ! m_childView->ProcessEvent(event) )
    {
        #if 0
        // Only hand up to the parent if it's a menu command
        if (!event.IsKindOf(CLASSINFO(wxCommandEvent))
            || !wxWindow::GetParent()
            || !wxWindow::GetParent()->ProcessEvent(event))
            return wxEvtHandler::ProcessEvent(event);
        else
            return true;
        #else
            if (!event.IsKindOf(CLASSINFO(wxCommandEvent)))
            return wxEvtHandler::ProcessEvent(event);
        else
            return true;
        #endif
    }
    else
        return true;
}

void CDocChildFrame::OnActivate(wxActivateEvent& event)
{
    wxFrame::OnActivate(event);

    if (m_childView)
        m_childView->Activate(event.GetActive());
}

void CDocChildFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (m_childView)
    {
        bool ans = event.CanVeto()
                    ? m_childView->Close(false) // false means don't delete associated window
                    : true; // Must delete.

        if (ans)
        {
            m_childView->Activate(false);
            delete m_childView;
            m_childView = (wxView *) NULL;
            m_childDocument = (wxDocument *) NULL;

            this->Destroy();
        }
        else
            event.Veto();
    }
    else
        event.Veto();
}

BEGIN_EVENT_TABLE(CDocParentFrame, wxFrame)
    EVT_MENU(wxID_EXIT, CDocParentFrame::OnExit)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, CDocParentFrame::OnMRUFile)
    EVT_CLOSE(CDocParentFrame::OnCloseWindow)
END_EVENT_TABLE()

CDocParentFrame::CDocParentFrame()
{
    m_docManager = 0;
}

bool CDocParentFrame::Create(wxDocManager *manager, wxFrame *parent, wxWindowID id,
        const wxString& title, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name)
{
        m_docManager = manager;
        return wxFrame::Create(parent, id, title, pos, size, style, name);
}

void CDocParentFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void CDocParentFrame::OnMRUFile(wxCommandEvent& event)
{
    int n = event.GetId() - wxID_FILE1;  // the index in MRU list
    wxString filename(m_docManager->GetHistoryFile(n));

    if ( !filename.empty() )
    {
        // verify that the file exists before doing anything else
        if ( wxFile::Exists(filename) )
        {
            // try to open it
            if (!m_docManager->CreateDocument(filename, wxDOC_SILENT))
            {
                // remove the file from the MRU list. The user should already be notified.
                m_docManager->RemoveFileFromHistory(n);

                wxLogError(_("The file '%s' couldn't be opened.\nIt has been removed from the most recently used files list."),
                       filename.c_str());
            }
        }
        else
        {
            // remove the bogus filename from the MRU list and notify the user
            // about it
            m_docManager->RemoveFileFromHistory(n);

            wxLogError(_("The file '%s' doesn't exist and couldn't be opened.\nIt has been removed from the most recently used files list."),
                       filename.c_str());
        }
    }
}

// Extend event processing to search the view's event table
bool CDocParentFrame::ProcessEvent(wxEvent& event)
{
    // Try the document manager, then do default processing
    if (!m_docManager || !m_docManager->ProcessEvent(event))
        return wxEvtHandler::ProcessEvent(event);
    else
        return true;
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
void CDocParentFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (m_docManager->Clear(!event.CanVeto()))
    {
        this->Destroy();
    }
    else
        event.Veto();
}

/*
 *	xMDIParentFrame
 */
IMPLEMENT_DYNAMIC_CLASS(CMDIParentFrame, CDocParentFrame)
BEGIN_MESSAGE_MAP(CMDIParentFrame, CWndBase)
        ON_WM_CREATE()
END_MESSAGE_MAP()

CMDIParentFrame::CMDIParentFrame()
{
        m_docManager = GetApp()->m_pDocManager;
        m_evtHandler->SetHost(this, this);
}

CMDIParentFrame::~CMDIParentFrame()
{

}

void CMDIParentFrame::OnCreate(wxWindowCreateEvent& e){

}

int CMDIParentFrame::OnCreate(CREATESTRUCT*)
{
        return 0;
}

bool CMDIParentFrame::Create(const wxString& title, const wxPoint& pos,
                                                         const wxSize& size, long style, const wxString& name)
{
        CREATESTRUCT cs;
        cs.lpCreateParams = 0;
        cs.hMenu = 0;
        cs.hwndParent = 0;
        cs.cx = size.x;
        cs.cy = size.y;
        cs.y = pos.x;
        cs.x = pos.y;
        cs.style = wxDEFAULT_FRAME_STYLE;
        cs.lpszName = 0;
        cs.dwExStyle = 0;

        if(PreCreateWindow(cs))
        {
                m_tempInfo = &cs;//store this for OnCreate

                if(CDocParentFrame::Create(GetApp()->m_pDocManager,
                        NULL,wxID_ANY, title, pos, size, style,  name))
                {
                        //since Create event happens after windows been shown,
                        //we make it it happen early. xEvetHandler make sure
                        //the later Create event will be discarded.
                        wxWindowCreateEvent event(this);
                        GetEventHandler()->ProcessEvent(event);

                        CCreateContext context;
                        context.m_pNewViewClass = 0;
                        context.m_pCurrentDoc = 0;

                        if(OnCreateClient(&cs, &context)){
                                return true;
                        }
                }
        }

        Destroy();

        return false;
}

void CMDIParentFrame::MDIActivate(CMDIChildFrame* child)
{
        //child->Activate();
}

void CMDIParentFrame::MDINext()
{
        //ActivateNext();
}

CMDIChildFrame* CMDIParentFrame::MDIGetActive()
{
        return 0;
        //return (xMDIChildFrame*)GetActiveChild();
}

bool CMDIParentFrame::LoadFrame(const wxString& resource)
{
        return wxXmlResource::Get()->LoadObject(this, 0, resource, "CMDIParentFrame");
}
//
//void CMDIParentFrame::UpdateUI(wxUpdateUIEvent& event)
//{
//	if(!event.GetEventObject()->IsKindOf(CLASSINFO(wxWindow)) &&
//		event.CanUpdate(this))
//		event.Enable(false);
//}

//Xml resource handler
IMPLEMENT_DYNAMIC_CLASS(CMDIParentFrameXmlHandler, wxFrameXmlHandler)

wxObject *CMDIParentFrameXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(frame, CMDIParentFrame);

        if(frame->Create(GetText(wxT("title")),
                wxDefaultPosition, wxDefaultSize,
                GetStyle(wxT("style"), wxDEFAULT_FRAME_STYLE),
                GetName()))
        {

                if (HasParam(wxT("size")))
                        frame->SetClientSize(GetSize(wxT("size"), frame));
                else
                        frame->SetClientSize(500, 400);

                if (HasParam(wxT("pos")))
                        frame->Move(GetPosition());
                if (HasParam(wxT("icon")))
                        frame->SetIcon(GetIcon(wxT("icon"), wxART_FRAME_ICON));

                SetupWindow(frame);

                CreateChildren(frame);

                if (GetBool(wxT("centered"), true))
                        frame->Centre();
        }
        else
                frame = NULL;

        return frame;
}

bool CMDIParentFrameXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("CMDIParentFrame"));
}


//Xml resource handler
IMPLEMENT_DYNAMIC_CLASS(CMDIChildFrameXmlHandler, wxFrameXmlHandler)

wxObject *CMDIChildFrameXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(frame, CMDIChildFrame);

        if(frame->Create((CMDIParentFrame*)m_parentAsWindow))
        {
                if (HasParam(wxT("size")))
                        frame->SetClientSize(GetSize(wxT("size"), frame));
                if (HasParam(wxT("pos")))
                        frame->Move(GetPosition());
                if (HasParam(wxT("icon")))
                        frame->SetIcon(GetIcon(wxT("icon"), wxART_FRAME_ICON));

                SetupWindow(frame);

                CreateChildren(frame);

                if (GetBool(wxT("centered"), false))
                        frame->Centre();
        }
        else
                frame = 0;

        return frame;
}

bool CMDIChildFrameXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("CMDIChildFrame"));
}


/*
 *	xMDIChildFrame
 */
IMPLEMENT_DYNAMIC_CLASS(CMDIChildFrame, CDocChildFrame)
BEGIN_MESSAGE_MAP(CMDIChildFrame, CWndBase)
        ON_WM_CREATE()
        ON_WM_CLOSE()
END_MESSAGE_MAP()

CMDIChildFrame::CMDIChildFrame()
{
        m_evtHandler->SetHost(this, this);
}

CMDIChildFrame::~CMDIChildFrame()
{
}


BOOL CMDIChildFrame::LoadFrame(const wxString& resource, wxWindow* parent)
{
        return wxXmlResource::Get()->LoadObject(this,
                        parent,
                        resource,
                        "CMDIChildFrame") ? TRUE : FALSE;
}

void CMDIChildFrame::SetActiveView(CView* pViewNew, BOOL bNotify)
{
        CView* oldview = ((_innerView*)GetView())->m_xview;

        SetView(pViewNew->m_pView);
        if(bNotify){
                oldview->OnActiveView(FALSE, pViewNew, oldview);
                pViewNew->OnActiveView(TRUE, pViewNew, oldview);
        }
}

CView* CMDIChildFrame::GetActiveView()
{
        CView* pView = 0;
        if(GetView())
                pView = ((_innerView*)GetView())->m_xview;
        return pView;
}

CDocument* CMDIChildFrame::GetActiveDocument()
{
        return (CDocument*)GetDocument();
}

BOOL CMDIChildFrame::IsIconic()
{
        return IsIconized() ? TRUE : FALSE;
}

void CMDIChildFrame::Activate()
{
        Raise();
}

void CMDIChildFrame::OnCreate(wxWindowCreateEvent& e){

}


int CMDIChildFrame::OnCreate(CREATESTRUCT*)
{
        //xDefault();
        return 0;
}

BOOL CMDIChildFrame::PreCreateWindow(CREATESTRUCT& )
{
    return TRUE;
}

BOOL CMDIChildFrame::OnCreateClient(CREATESTRUCT* cs, CCreateContext* pContext)
{
        if(pContext->m_pNewViewClass)
        {
                _innerView* view = new _innerView();
                view->SetFrame(this);
                view->SetDocument(GetDocument());

                if(!view->CreateCanvas(this, GetDocument(), pContext->m_pNewViewClass))
                {
                        delete view;
                }
                else
                {
                        GetDocument()->AddView(view);
                        SetView(view);

                        return TRUE;
                }
        }

        return FALSE;
}


bool CMDIChildFrame::Create(CMDIParentFrame* parent)
{
        CREATESTRUCT cs;
        cs.lpCreateParams = 0;
        cs.hMenu = 0;
        cs.hwndParent = 0;
        cs.cx = wxDefaultSize.x;
        cs.cy = wxDefaultSize.y;
        cs.y = wxDefaultPosition.x;
        cs.x = wxDefaultPosition.y;
        cs.style = wxDEFAULT_FRAME_STYLE;
        cs.lpszName = 0;
        cs.dwExStyle = 0;

        if(PreCreateWindow(cs))
        {
                m_tempInfo = &cs;//store this for OnCreate

                if(CDocChildFrame::Create(GetDocument(), parent,
                        wxID_ANY, cs.lpszName, wxDefaultPosition, wxDefaultSize, cs.style, cs.lpszName))
                {
                        CCreateContext context;
                        context.m_pCurrentDoc = GetDocument();
                        context.m_pNewViewClass = ((CMultiDocTemplate*)(GetDocument()->GetDocumentTemplate()))->GetUserViewClassInfo();
                        if(OnCreateClient(&cs, &context))
                        {
                                wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
                                wxWindowList& children = GetChildren();
                                wxWindowListNode* node = children.GetFirst();
                                while(node){
                                        wxWindow* win = (wxWindow*)node->GetData();
                                        if(!win->IsKindOf(CLASSINFO(wxStatusBar)))
                                        {
                                                if(win->IsKindOf(CLASSINFO(wxToolBar)))
                                                {
                                                        if(win != (wxWindow*)GetToolBar())
                                                                sizer->Add(win, 0, wxEXPAND, 0);
                                                }
                                                else if(win->IsKindOf(CLASSINFO(wxStaticLine)))
                                                        sizer->Add(win, 0, wxEXPAND, 0);
                                                else
                                                        sizer->Add(win, 1, wxEXPAND, 0);
                                        }
                                        node = node->GetNext();
                                }
                                SetAutoLayout(true);
                                SetSizer(sizer);
                                sizer->Fit(this);
                                Layout();

                                Show();
                                return true;
                        }
                }
        }

        Destroy();

        return false;
}

void CMDIChildFrame::OnClose()
{
        wxDocument* doc = GetDocument();
        if(doc){
                wxDocManager* docman = doc->GetDocumentManager();
                if(docman->CloseDocument(doc))
                {
                        Destroy();
                }
        }
}


/*
 *	inner view. dont't expose to user
 */
IMPLEMENT_DYNAMIC_CLASS(_innerView, wxView)

_innerView::_innerView():m_xview(0){};

_innerView::~_innerView()
{
        delete m_xview;
}

void _innerView::OnUpdate(wxView* sender, wxObject* hint)
{
        wxView::OnUpdate(sender, hint);
        if(sender)
                m_xview->OnUpdate(((_innerView*)sender)->m_xview);
}

bool _innerView::ProcessEvent(wxEvent& evt)
{
        bool bProcessed=false;
        if(evt.IsKindOf(CLASSINFO(wxCommandEvent)))
        {
                if(m_xview)
                {
                        xEvtHandler* evthandler = m_xview->GetEvtHandler();
                        if(evthandler != NULL){
                evthandler->NotHostCurrentEvent();
                bProcessed =evthandler->ProcessEvent(evt);
                        }
                }
        }

        if(!bProcessed)
                bProcessed = wxView::ProcessEvent(evt);

        return bProcessed;
}

void _innerView::OnDraw(wxDC* dc)
{
}

bool _innerView::CreateCanvas(wxWindow* parent, wxDocument* doc, wxClassInfo* userViewClassInfo)
{
        //create user view as canvas
        m_xview = (CView*)userViewClassInfo->CreateObject();
        m_xview->SetInnerView(this);

        wxSize size = parent->GetClientSize();
        wxPoint pnt(0,0);
        if(!m_xview->Create(parent, pnt, size))	{
                delete m_xview;
                m_xview = NULL;
                return false;
        }

        return true;
}
