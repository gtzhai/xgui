#ifndef MFC_CLASS_H
#define MFC_CLASS_H

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif
#include <wx/event.h>
#include <wx/spinctrl.h>
#include <wx/animate.h>
#include <wx/datectrl.h>
#include <wx/calctrl.h>
#include <wx/listctrl.h>
#include <wx/gauge.h>
#include <wx/scrolbar.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/statbox.h>
#include <wx/statbmp.h>
#include <wx/statusbr.h>
#include <wx/toolbar.h>
#include <wx/tooltip.h>
#include <wx/treectrl.h>
#include <wx/propdlg.h>
#include <wx/bookctrl.h>
#include <wx/splitter.h>
#include <wx/docview.h>
#include <wx/xrc/xh_frame.h>
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/socket.h>
#include <wx/protocol/ftp.h>
#include <wx/protocol/http.h>
#include <wx/cmdline.h>
#include <wx/aui/auibar.h>
#include <wx/splash.h>
#include <wx/docmdi.h>
#include <wx/printdlg.h>

#include "win32_types.h"
#include "xmacro.h"

class xEvtHandler;
class xScrollHelper;
class CDC;
class CBrush;
class CWndBase;
class CControlWrapper;
class CDialogBase;
class CMDIChildFrame;
class CMDIParentFrame;
class CView;
class CDocument;
class CPoint;
struct CPlex;
struct CCreateContext;

/*Data Structure Classes*/

class CPtrList{
protected:
    struct CNode
    {
        CNode* pNext; // 后指针
        CNode* pPrev; // 前指针
        void *data; // 数据
    };
    CPtrList::CNode* NewNode(CPtrList::CNode* pPrev, CPtrList::CNode* pNext);
    void FreeNode(CPtrList::CNode* pNode);
public:
    CPtrList(int nBlockSize);
    ~CPtrList();
    POSITION AddHead(void* newElement);
    POSITION AddTail(void* newElement);
    void AddHead(CPtrList* pNewList);
    void AddTail(CPtrList* pNewList);
    void* RemoveHead();
    void* RemoveTail();
    POSITION InsertBefore(POSITION position, void* newElement);
    POSITION InsertAfter(POSITION position, void* newElement);
    void RemoveAt(POSITION position);
    POSITION FindIndex(int nIndex) const;
    POSITION Find(void* searchValue, POSITION startAfter) const;
    void RemoveAll();

    POSITION  GetTailPosition() { return  (POSITION) m_pNodeTail;}
    POSITION  GetHeadPosition() { return  (POSITION) m_pNodeHead;}
    void* GetPrev(POSITION pos){
        CNode* pNode = (CNode*) pos;
        return pNode->pPrev->data;
    }
    void* GetNext(POSITION pos){
        CNode* pNode = (CNode*) pos;
        return pNode->pNext->data;
    }
    CNode* m_pNodeHead; // 指向链表头的指针
    CNode* m_pNodeTail; // 指向链表尾的指针
    int m_nCount; // 元素个数
    CNode* m_pNodeFree; // 空闲节点链表
    struct CPlex* m_pBlocks; // 数据块载体链表
    int m_nBlockSize; // 每个数据块元素个数
};
/*class COleVariant: wxVariant{};
class CMap: wxHashMap{};
class CStringList: wxStringList{};
class CList: wxList{};
class CStringArray: wxArrayString{};
class CArray: wxArray{};*/
class CString: public wxString {
public:
    CString():wxString (){}
    CString(const char *psz):wxString(psz){}
    CString(const wchar_t *psz):wxString(psz){}
    CString(const wxString& s):wxString (s){}
    size_t GetLength(){ return Length();}
    TCHAR GetAt(size_t n){return GetChar(n);}
    CString Delete(size_t start, size_t nCount){wxString l = Left (start);wxString r = Right  (Length()-nCount); return l+r;}
};

#include "xmsg.h"
class xMessageHandler{
public:
    struct mh_timer{
        int nId;
        wxTimer *Timer;
    };
    xMessageHandler();
    ~xMessageHandler();
    virtual const msg_map* GetMessageMap() const;
    static const msg_map* GetThisMessageMap();

    void SetEvtHandler(xEvtHandler* handler);
    xEvtHandler *GetEvtHandler() {return m_evtHandler;}

    virtual bool OnCommand(WPARAM w,LPARAM l){ return true;}
    int KillTimer(int id);
    void RemoveTimer(int index);
    int SetTimer(int id, unsigned int milllisecs);

    CREATESTRUCT * m_tempInfo;
	xEvtHandler *m_evtHandler;
	bool m_bOnCreateCalled;
    mh_timer m_Timers[10];
};

class xEvtHandler: public wxEvtHandler{
public:
    xEvtHandler();
    ~xEvtHandler();
    virtual bool ProcessEvent(wxEvent& event);

    void SetHost(wxWindow* host, xMessageHandler* msg_handler);
    wxWindow* GetHost();
    void NotHostCurrentEvent(){m_bNotHostEvent=false;}
    bool HostEvent();
private:
    bool TryCommand(wxEvent& event);
    bool doEvents(wxEvent& event, const msg_map_entry* entry);
    void HandleMouseEvent(wxMouseEvent& evt, const msg_map_entry* entry);
    bool HandleScrollEvent(wxScrollEvent& event, const msg_map_entry* entry);
    bool HandleScrollWinEvent(wxScrollWinEvent& event, const msg_map_entry* entry);

public:
	wxWindow *m_pHost;
	wxEvtHandler *m_pHostEvtHandler;
	xMessageHandler *m_msg_handler;
	wxEvent *m_pRecentEvent;
	bool m_bNotHostEventfalse;
	bool m_bNotHostEvent;
};

class xScrollHelperEvtHandler: public wxEvtHandler{
public:
    xScrollHelperEvtHandler(xScrollHelper *s):m_scrollHelper(s){}
    bool ProcessEvent(wxEvent& event);
    bool m_hasDrawnWindow;
    xScrollHelper *m_scrollHelper;
};

class xScrollHelper:wxScrolledWindow{
public:
    xScrollHelper(wxWindow *win);
    ~xScrollHelper();
    void SetScrollbars(int pixelsPerUnitX,
                                   int pixelsPerUnitY,
                                   int noUnitsX,
                                   int noUnitsY,
                                   int xPos,
                                   int yPos,
                                   bool noRefresh);
    void DeleteEvtHandler();
    void SetWindow(wxWindow *win);
    void SetTargetWindow(wxWindow *target);
    wxWindow *GetTargetWindow() const;
    void DoSetTargetWindow(wxWindow *target);
    void HandleOnScroll(wxScrollWinEvent& event);
    int CalcScrollInc(wxScrollWinEvent& event);
    void AdjustScrollbars();
    void DoPrepareDC(wxDC& dc);
    void SetScrollRate( int xstep, int ystep );
    int GetScrollPageSize(int orient) const;
    void GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const;
    void SetScrollPageSize(int orient, int pageSize);
    void Scroll( int x_pos, int y_pos );
    void EnableScrolling (bool x_scroll, bool y_scroll);
    void GetViewStart (int *x, int *y) const;
    void DoCalcScrolledPosition(int x, int y, int *xx, int *yy) const;
    void DoCalcUnscrolledPosition(int x, int y, int *xx, int *yy) const;
    void HandleOnSize(wxSizeEvent& WXUNUSED(event));
    void HandleOnPaint(wxPaintEvent& WXUNUSED(event));
    void HandleOnChar(wxKeyEvent& event);
    bool SendAutoScrollEvents(wxScrollWinEvent& event) const;
    void StopAutoScrolling();
    void HandleOnMouseEnter(wxMouseEvent& event);
    void HandleOnMouseLeave(wxMouseEvent& event);
#if wxUSE_MOUSEWHEEL
    void HandleOnMouseWheel(wxMouseEvent& event);
#endif

    bool m_hasDrawnWindow;
    int m_xScrollPixelsPerLine;
    int m_yScrollPixelsPerLine;
    int m_xScrollPosition;
    int m_yScrollPosition;
    int m_xScrollLines;
    int m_yScrollLines;
    int m_xScrollLinesPerPage;
    int m_yScrollLinesPerPage;
    bool m_xScrollingEnabled;
    bool m_yScrollingEnabled;
    float m_scaleX;
    float m_scaleY;
#if wxUSE_MOUSEWHEEL
    int m_wheelRotation = 0;
#endif
    wxWindow *m_win;
    wxWindow *m_targetWindow;
    wxTimer *m_timerAutoScroll;
    xScrollHelperEvtHandler *m_handler;
};

/*Graphics Classes*/
class CPoint: public wxPoint {
public:
    CPoint();
    CPoint(int x, int y);
    CPoint(const wxPoint& pnt);
};

class CSize: public wxSize {
public:
    CSize();
    CSize(int x, int y);
    int cx;
    int cy;
};

class CRect: public wxRect {
public:
    CRect();
    CRect(CPoint& pnt1, CPoint& pnt2);
    CRect(const CRect* rect);
    CRect(int l, int t, int r, int b);
    CRect(const wxRect& rect);
    wxPoint GetPosition();
    wxSize GetSize();
    int Width();
    int Height();
    void NormalizeRect();
    bool IntersectRect(const CRect* rc1, const CRect* rc2);
    bool PtInRect(CPoint& pnt);
    void InflateRect(int dx, int dy);
    void OffsetRect(int dx, int dy);
    int left;
	int top;
	int right;
	int bottom;
};

class CRegion: public wxRegion{
public:
    CRegion();
#if defined(_WIN32) || defined(_WIN64)
    CRegion(CPoint* pnt, int nr, int op);
#endif
    CRegion(int l, int t, int r, int b);
    CRegion(const wxBitmap& bmp, const wxColour& trans);
    CRegion(const wxRegion& rgn);
    static CRegion* CreateRectRgn(int l, int t, int r, int b);
#if defined(_WIN32) || defined(_WIN64)
    static CRegion* CreatePolygonRgn(CPoint* pnt, int nr, int op);
#endif
    static CRegion* CreateRoundRectRgn(int l, int t, int r, int b, int rw, int rh);
    static CRegion* CreateEllipticRgn(int l, int t, int r, int b);
};
class CRgn: public CRegion{};
int GetRgnBox(const CRegion* rgn, CRect& rect);
int CombineRgn(CRegion *dst, const CRegion* src1, const CRegion* src2, int op);
void FillRgn(CDC* pDC, const CRegion* rgn, const CBrush* brush);

class CFont: public wxFont{
public:
    CFont();
    void CreateFont( int nHeight,//
			   int nWidth,//
			   int /*nEscapement*/,
			   int /*nOrientation*/,
			   int nWeight,//
			   BYTE bItalic,//
			   BYTE bUnderline,//
			   BYTE cStrikeOut,//
			   BYTE /*nCharSet*/,
			   BYTE /*nOutPrecision*/,
			   BYTE /*nClipPrecision*/,
			   BYTE /*nQuality*/,
			   wxFontFamily family,
			   const char* lpszFacename);
    bool CreateFontIndirect(const LOGFONT* lf);
};

class CPalette{//: public wxPalette{
public:
    bool CreatePalette(const LOGPALETTE* lp);
    wxColour GetColor(int index);
};

class CBitmap: public wxBitmap{
public:
    CBitmap();
    CBitmap(int width, int height, int depth);
    CBitmap(int res);
    CBitmap(const char* szbitmap);
    bool LoadBitmap(const char* szbitmap);
    bool LoadBitmap(int res);
};

class CPen: public wxPen{
public:
    CPen();
    CPen(int style, int width, const COLORREF& clr);
    CPen(wxPen& pen);
};

class CBrush: public wxBrush{
public:
    CBrush();
    CBrush(const wxBitmap& bmp);
    CBrush(const wxBrush& brush);
    CBrush(const COLORREF& clr);
    CBrush(int style, const COLORREF& clr);
};

class CDC{
public:
    CDC();
    CDC(CWndBase* win);
    CDC(CControlWrapper* win);
    ~CDC();
    void SetDC(wxDC* pDC, CWndBase* wnd);
    wxWindow* GetWindow();
    void GetRect(CRect* rect);
    void Init();
    bool IsPrinting() ;
    void RealizePalette ();
    bool RectVisible(CRect* rect);
    void SetTextColor(const COLORREF& clr);
    const COLORREF& SetBkColor(const COLORREF& clr);
    CPen* SelectObject(CPen* pen);
    CBrush* SelectObject(const CBrush* brush);
    CFont* SelectObject(const CFont* font);
    CPalette* SelectPalette(const CPalette* palette, int);
    void SetViewportOrg(int x, int y);
    CPoint GetViewportOrg();
    void SetClippingRegion(int l, int t, int r, int b);
    int SetBkMode(int mode);
    int GetDeviceCaps(int option);
    int SetTextAlign(int align);
    void MoveTo(const CPoint& pt);
    void MoveTo(int x, int y);
    void LineTo(const CPoint& pt);
    void LineTo(int x, int y);
    void TextOut(int x, int y, const char* str, int len);
    void TextOut(int x, int y, const wxString& str);
    void Rectangle(CRect* rect);
    void Rectangle(const CRect& rect);
    void DrawRoundRect(wxDC* pDC, int l, int t, int r, int b, int rw, int rh);
    void RoundRect(int l, int t, int r, int b, int rw, int rh);
    void Rectangle(int l, int t, int r,int b);
    void Ellipse(CRect* rect);
    void Ellipse(const CRect& rect);
    void Ellipse(int l, int t, int r, int b);
    void Draw3dRect(int l, int r, int t, int b,
					 const COLORREF& clr_leftop,
					 const COLORREF& clr_rightbottom);
    void Draw3dRect(const CRect* rect, const COLORREF& clr_leftop,
					 const COLORREF& clr_rightbottom);
    void Polyline(CPoint* pt, int nr);
    void Polygon(CPoint* pt, int nr);
    void Arc(const CRect& rect, const CPoint& start, const CPoint& end);
    void Arc( int nLeftRect,   // x-coord of upper-left corner of rectangle
		  int nTopRect,    // y-coord of upper-left corner of rectangle
		  int nRightRect,  // x-coord of lower-right corner of rectangle
		  int nBottomRect, // y-coord of lower-right corner of rectangle
		  int nXRadial1,   // x-coord of first radial's endpoint
		  int nYRadial1,   // y-coord of first radial's endpoint
		  int nXRadial2,   // x-coord of second radial's endpoint
		  int nYRadial2    // y-coord of second radial's endpoint
		  );
    void Pie(const CRect& rect, const CPoint& start, const CPoint& end);
    void Pie( int nLeftRect,   // x-coord of upper-left corner of rectangle
		  int nTopRect,    // y-coord of upper-left corner of rectangle
		  int nRightRect,  // x-coord of lower-right corner of rectangle
		  int nBottomRect, // y-coord of lower-right corner of rectangle
		  int nXRadial1,   // x-coord of first radial's endpoint
		  int nYRadial1,   // y-coord of first radial's endpoint
		  int nXRadial2,   // x-coord of second radial's endpoint
		  int nYRadial2    // y-coord of second radial's endpoint
		  );
    void Chord(const CRect& rect, const CPoint& start, const CPoint& end);
    void DrawPushButton(const CRect& rect);
    void SetPixel(int x, int y, const COLORREF& clr);
    void FillRect(const CRect& rect, CBrush* brush);
    void FillSolidRect(CRect* rect, const COLORREF& clr);
    CSize GetTextExtent(const wxString& str) ;
    CSize GetTextExtent(const char* sz, int len) ;
    bool BitBlt(int x, int y, int width, int height, CDC* pSrcDC, int xsrc, int ysrc, int op);
    int SetROP2(int op);
    void LPtoDP(CRect& rect);
    void DPtoLP(CRect& rect);
    void LPtoDP(CSize& s);
    void DPtoLP(CSize& s);
    void DrawFocusRect(const CRect& rect);
    void SetMapMode(int nMode);

	bool m_bPrinting;
	bool m_bSelfAlloc;
	CPoint m_StartPos;
	DWORD m_TextAlign;
	CBrush *m_prev_brush;
	CPen *m_prev_pen;
	CFont *m_prev_font;
	CPalette *m_prev_palette;
	wxDC *m_pDC;
	wxWindow *m_pWnd;
};

class CDrawingManager : public CDC{
public:
    void FillGradient(
        CRect rect,
        COLORREF colorStart,
        COLORREF colorFinish,
        BOOL bHorz = TRUE,
        int nStartFlatPercentage = 0,
        int nEndFlatPercentage = 0){
            wxDirection nDirection=wxRIGHT;
            if(!bHorz)  nDirection=wxUP;
            m_pDC->GradientFillLinear (rect, wxColour(colorStart), wxColour(colorFinish), nDirection);
        }
};

#if  defined(_WIN64) || defined(_WIN32)
#else
unsigned long  GetSysColor(int nIndex);
#endif

class CImageList: public wxImageList{
public:
    BOOL Create(int cx, int cy, UINT, int initial, int);
    BOOL Create(const wxString& strBmp, int cx, int, const COLORREF& crMask);
    BOOL Create(int nBitmapID, int cx, int, const COLORREF& crMask);
    BOOL Add(const wxBitmap& Bitmap, int cx, int cy, const COLORREF& maskclr);
    BOOL Add(wxBitmap* Bitmap, int cx, int cy, const COLORREF& maskclr);
    void DeleteImageList();
};

/*class CClientDC: wxClientDC{};
class CMetaFileDC: wxMetaFileDC{};
class CPaintDC: CPaintDC{};
class CWindowDC: wxWindowDC{};*/

/*Control Classes*/
class CDataExchange: public wxValidator{
public:
    CDataExchange(CDialogBase* pDlgWnd, BOOL bSaveAndValidate);
    CDialogBase *pDlgWnd;
    BOOL Ok();

    bool m_bSaveAndValidate;
    CDialogBase* m_pDlgWnd;
    bool m_bExchangeOk;
};
void DDX_Radio(CDataExchange* pDX, int nIDC, int value);
void DDX_Check(CDataExchange* pDX, int nIDC, BOOL& value);
void DDX_Text(CDataExchange* pDX, int nIDC, wxDateTime& value);
void DDX_Text(CDataExchange* pDX, int nIDC, double& value);
void DDX_Text(CDataExchange* pDX, int nIDC, float& value);
void DDX_Text(CDataExchange* pDX, int nIDC, wxChar* value, int nMaxLen);
void DDX_Text(CDataExchange* pDX, int nIDC, wxString& value);
void DDX_Text(CDataExchange* pDX, int nIDC, unsigned long& value);
void DDX_Text(CDataExchange* pDX, int nIDC, long& value);
void DDX_Text(CDataExchange* pDX, int nIDC, unsigned int& value);
void DDX_Text(CDataExchange* pDX, int nIDC, int& value);
void DDX_Text(CDataExchange* pDX, int nIDC, short& value);
void DDX_Text(CDataExchange* pDX, int nIDC, wxByte& value);
void DDV_MaxChars(CDataExchange* pDX, wxString const& value, int nChars);
void DDV_MinMaxInt(CDataExchange* pDX, int value, int mixValue, int MaxValue);
void DDX_Control(CDataExchange* pDX, int nIDC, CControlWrapper& control);

class CControlWrapper{
public:
    CControlWrapper();
    CControlWrapper(wxWindow* control);
    void SetWindowText(const wxString& str);
    wxString GetWindowText();
    void GetWindowText(wxString& str) const;
    int GetWindowText(char* buffer, int len);
    void EnableWindow(int enable) ;
    void ShowWindow(int bshow);
    void GetWindowRect(CRect* prect);
    void GetClientRect(CRect* prect);
    void SetWindowPos(void*, int l, int t, int w, int h, int flags);
    int GetDlgCtrlID();
    wxWindow* SetFocus();
    void SetToolTip(const wxString& tip);
    void MoveWindow(int l, int t, int r, int b);
    void MoveWindow(CRect* rect);
    void Invalidate(BOOL bErase = TRUE);
    bool IsWindowEnabled();

    wxWindow *m_control;
};

class CSpinButtonCtrl: public wxSpinCtrl, public CControlWrapper{
public:
    CSpinButtonCtrl(): CControlWrapper(this){};
    void SetBuddy(const CControlWrapper& buddy);
    void SetRange(int nLower, int nUpper);
    void SetPos(int pos);
    int GetPos();
};

class CControlWithItems: public wxControlWithItems, public CControlWrapper{
public:
    CControlWithItems(): CControlWrapper(this){};
    int GetCount();
    int GetCurSel();
    int SetCurSel(int sel);
    void ResetContent();
    void SetItemData(int index, unsigned long data);
    void Invalidate(BOOL bErase = TRUE);
    unsigned long GetItemData(int index) ;
    void GetText(int index, wxString& str);
    void GetText(int index, wxChar* pstr);
    int GetTextLen(int index) ;
};

class CComboBox: public wxComboBox, public CControlWrapper{
public:
    CComboBox(): CControlWrapper(this){};
#ifdef __WXGTK20__
    int GetCurSel();
#endif
    int AddString(const wxString& str);
    void GetLBText(int index, wxString& str);
    void GetLBText(int index, char* str);
    int FindString(int, const wxString& text);
};

class CListBox : public wxListBox, public CControlWrapper{
public:
    CListBox (): CControlWrapper(this){};
    int DeleteString(int index);
    BOOL Create(DWORD dwStyle, CRect& cRect, wxWindow* pParent, UINT nID);
    int InsertString(int nIndex, LPCTSTR lpszItem);
    int AddString(const char* lpszItem);
    int SetSel(int index, BOOL bSelect);
    int GetSel(int index);
    void SetItemDataPtr(int nIndex, void* pData);
    void* GetItemDataPtr(int nIndex);
    void SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem);
    int GetSelCount();
    int GetSelItems(int nMaxItem, int * pItem);
    void SetCurSel(int i){SetSel(i, TRUE);}
    void ResetContent(){DoClear();}
};

class CWndBase: public xMessageHandler{//, public xEvtHandler
public:
    CWndBase();
    CWndBase(wxWindow *parent);

public:
    void Init();
    void Invalidate(BOOL bErase = TRUE);
    int MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType);
    CControlWrapper GetDlgItem(int id);
    CControlWrapper GetParent();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs){ return TRUE;};
    virtual BOOL PreTranslateMessage(MSG* pMsg) {return TRUE;}

    void GetWindowRect(CRect* prect);
    void GetClientRect(CRect* prect);
    void SetWindowPos(void*, int l, int t, int w, int h, int flags);
    void SetWindowText(const wxString& str);
    void ShowWindow(int bshow);
    wxWindow *GetWindow(){ return m_wndParent;}
    bool DestroyWindow(wxWindow *w){return w->Destroy();}
    bool DestroyWindow(){return m_wndParent->Destroy();}
protected:
    virtual void DoDataExchange(CDataExchange *e){
        if(e->m_bSaveAndValidate)
            e->TransferFromWindow();
        else
            e->TransferToWindow();
    }
    DECLARE_MESSAGE_MAP()
    wxWindow *m_wndParent ;
};

class CDialogBase: public CWndBase{
public:
    CDialogBase();
    CDialogBase(const char* resource, wxWindow *parent);
    BOOL UpdateData(BOOL bSaveData);
    bool LoadXmlRes();
private:
    const char *GetIDDS(){return m_szResource;}
public:
    BOOL OnInitDialog();
protected:
    virtual void OnOK();
    virtual void OnCancel();
private:
    void OnButtonOk();
    void OnButtonCancel();

public:
	const char *m_szResource;
	bool m_bInitDialogCalled;
    DECLARE_MESSAGE_MAP()
};

class CDialog: public wxDialog, public CDialogBase{
public:
    CDialog();
    CDialog(wxWindow *parent, const char* resource);
    CDialog(int idd, wxWindow *parent);
    ~CDialog();
public:
    BOOL OnInitDialog() {return CDialogBase::OnInitDialog();}
    int DoModal();
    BOOL Create(int, wxWindow* parent);
    BOOL OnEraseBkgnd(CDC* pDC){return TRUE;}
    void OnClose(){}
    void OnPaint(){}

    wxDECLARE_DYNAMIC_CLASS(CDialog);
};

class CButton: public wxButton, public CControlWrapper{

};

class CEdit: public wxTextCtrl, public CControlWrapper{

};

//class CAnimateCtrl: public wxAnimationCtrl{};
class CBitmapButton: public wxBitmapButton, public CControlWrapper{};
class CDateTimeCtrl: public wxDatePickerCtrl, public CControlWrapper{};
class CCheckListBox: public wxCheckListBox, public CControlWrapper{};
class CMonthCalCtrl: public wxCalendarCtrl, public CControlWrapper{};
class CProgressCtrl: public wxGauge, public CControlWrapper {};
class CRichEditCtrl: public wxTextCtrl, public CControlWrapper {};
class CScrollBar: public wxScrollBar, public CControlWrapper{};
class CSliderCtrl: public wxSlider{};
class CStatic: public wxStaticText, public wxStaticLine, public wxStaticBox, public wxStaticBitmap, public CControlWrapper {};
class CStatusBarCtrl: public wxStatusBar, public CControlWrapper{};
//class CTabCtrl: public wxTabCtrl, public CControlWrapper{};
class CToolBarCtrl: public wxToolBar, public CControlWrapper {};
//class CToolTipCtrl:  public wxToolTip, public CControlWrapper{};
class CListCtrl: public wxListCtrl, public CControlWrapper{
    CListCtrl():wxListCtrl(){}
};
class CTreeCtrl:  public wxTreeCtrl, public CControlWrapper{
public:
        CTreeCtrl():wxTreeCtrl(){}
};

class CPropertyPage: public wxPanel, public CDialogBase{
public:
    CPropertyPage();
    CPropertyPage(UINT idd);
    CPropertyPage(const char *res);

    ~CPropertyPage();
     wxDECLARE_DYNAMIC_CLASS(CPropertyPage);
};

class CPropertySheet: public wxPropertySheetDialog, public CDialogBase{
public:
    CPropertySheet();
    CPropertySheet(const wxString& title, wxWindow *parent, UINT sel) ;
    ~CPropertySheet();
    void AddPage(CPropertyPage* page);
    int GetActiveIndex();
    int DoModal();
    virtual BOOL OnInitDialog();

    UINT m_nSelectedPage;
    wxDECLARE_DYNAMIC_CLASS(CPropertySheet);
};

/*Window Classes*/
class CMFCToolBarImages: public wxImage{};

class CMFCToolBar: public wxAuiToolBar {

};

class CToolBar: public wxToolBar, public CWndBase{
    BOOL SetButtons(const UINT* ids, int count);
};

class CStatusBar: public wxStatusBar, public CWndBase{
public:
    CStatusBar();
    ~CStatusBar();
    void FreeIDArray();
    BOOL SetPaneText(int index, const wxString& text);
    BOOL SetIndicators(const UINT* width, int nIDCount);

	UINT *m_pIDArray;
	int m_nIDCount;
};
class CMenu: public wxMenu, public CWndBase{
public:
    BOOL LoadMenu(const char* szName);
    CMenu* GetSubMenu(int index);
    BOOL ModifyMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem,LPCTSTR lpszNewItem);
    BOOL AppendMenu(UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem);
    BOOL TrackPopupMenu(int x, int y, wxWindow* pWnd);
};
void CopyMenu(wxMenu* src, wxMenu* dst);
class CSplashWnd: public wxSplashScreen {};

/*Dialog Classes*/


/*class CColorDialog: public wxColourDialog{};
class CFileDialog: public wxFileDialog{};
class CFindReplaceDialog:  public wxFindReplaceDialog{};
class CFontDialog:  public	wxFontDialog{};
class CPageSetupDialog: public wxPageSetupDialog{};
class CPrintDialog: public wxPrintDialog{};*/

/*Document/View Classes*/
class CSplitterWnd: public wxSplitterWindow, public CWndBase{
public:
    CSplitterWnd();
    BOOL CreateStatic(CMDIChildFrame* Parent, int rows, int cols);
    BOOL CreateView(int row, int col, wxClassInfo* pViewClass, const CSize& sizeInit, CCreateContext* pContext);
    CView* GetPane(int row, int col);

    bool m_bSplitVerticaly;
    CView *m_Win1;
    CView *m_Win2;
    CSize m_Size;
    CMDIChildFrame *m_Parent;
};

class CFrameWnd: public wxDocParentFrame, public CWndBase{
public:
    CFrameWnd();
    ~CFrameWnd();
    bool ProcessEvent(wxEvent& event);
    bool Create(wxFrame *parent, wxWindowID id, const wxString &title,
                    const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr);
    bool LoadFrame(const wxString& resource);

    void SetActiveView(CView* pViewNew, BOOL bNotify=TRUE);
    CView* GetActiveView();
    CDocument* GetActiveDocument();
    void RecalcLayout(){Layout();Refresh ();PostSizeEvent ();}

protected:
    int OnCreate(CREATESTRUCT*);
    BOOL OnCreateClient(CREATESTRUCT* cs, CCreateContext* pContext);

    wxDECLARE_DYNAMIC_CLASS(CFrameWnd);
    DECLARE_MESSAGE_MAP()
};

class CFrameWndXmlHandler: public wxFrameXmlHandler{
public:
    wxObject *DoCreateResource();
    bool CanHandle(wxXmlNode *node);
    wxDECLARE_DYNAMIC_CLASS(CFrameWndXmlHandler);
};


class CMDIFrameWnd: public wxDocMDIParentFrame, public CWndBase{

};

class CMDIChildWnd: public wxDocMDIChildFrame, public CWndBase{

};

class CPrintInfo{
public:
    CPrintInfo();
    ~CPrintInfo();
    void SetMinPage(unsigned int nMinPage);
    void SetMaxPage(unsigned int nMaxPage);
    unsigned int GetMinPage() const;
    unsigned int GetMaxPage() const;

	wxPrintDialogData *m_pPD;
	bool m_bDirect;
	UINT m_nCurPage;
	bool m_bPreview;         // initialize to not preview
	bool m_bContinuePrinting; // Assume it is OK to print
	CRect m_rectDraw;
};

class CPrintout: public wxPrintout{
public:
    CPrintout(CView* pView, const wxChar *title);
    ~CPrintout();

    bool HasPage(int pageNum);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);
    bool OnPrintPage(int page);
    void OnPreparePrinting();
    void OnBeginPrinting();
    void OnEndPrinting();

    //wxChar *wxPrintout;
    CView *m_pView;
	CPrintInfo *m_printInfo;
};

class _innerView : public wxView, public CWndBase{
public:
    _innerView();
    ~_innerView();

    bool ProcessEvent(wxEvent& evt);
    void OnDraw(wxDC* dc);
    void OnUpdate(wxView* sender, wxObject* hint);
    bool CreateCanvas(wxWindow* parent, wxDocument* doc, wxClassInfo* userViewClassInfo);

    CView *m_xview;
    wxDECLARE_DYNAMIC_CLASS(_innerView);
};

class CViewCanvas : public wxScrolledWindow, public CWndBase
{
public:
    // view may be NULL if we're not associated with one yet, but parent must
    // be a valid pointer

    CViewCanvas(wxView *view, wxWindow *parent = NULL,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,long style = wxScrolledWindowStyle);
    virtual ~CViewCanvas();

    virtual void OnDraw(wxDC& dc);

    void SetView(wxView *view)
    {
        wxASSERT_MSG( !m_view, "shouldn't be already associated with a view" );
        m_view = view;
    }

    void ResetView()
    {
        wxASSERT_MSG( m_view, "should be associated with a view" );
        m_view = NULL;
    }

private:
    wxView *m_view;
};

class CView: public wxView, public CWndBase{
public:
    CView();
    ~CView();
    void OnPaint();

    virtual bool OnCreate(wxDocument *doc, long flags);
    virtual void OnDraw(wxDC *dc);
    virtual void OnUpdate(CView* sender);
    virtual void OnUpdate(wxView* sender, wxObject* hint);
    virtual void OnActiveView(BOOL bActived, CView* pActiveView, CView* pDeactiveView);
    virtual bool OnClose(bool deleteWindow = true);

    virtual bool Create(wxWindow* parent, wxPoint& point, wxSize& size);

    wxFrame* GetParentFrame();

    void SetInnerView(_innerView *view) {;}
    virtual bool LoadView(wxFrame*){return false;}
    int GetScrollRange(int orientation);
    void SetScrollRange(int orientation, int minpos, int maxpos, BOOL bRedraw);
    void SetScrollRange(int orientation, int minpos, int maxpos){SetScrollRange(orientation, minpos, maxpos, false);}
    void ScrollPage(int orientation, int delta);
    void ScrollLine(int orientation, int delta);
    void ScrollTo(int orientation, int pos);
    void Scroll(int orientation, int delta);
    void OnPrint(CDC* pDC, CPrintInfo*);
    void ResizeParentToFit(){}
    wxWindow* GetCanvas(){return m_Canvas;}
    wxWindow* CreateCanvas(wxWindow* parent, wxPoint& point, wxSize& size, long style);
public:
    virtual void OnInitialUpdate();
    void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
    CDocument* GetDocument();

    void OnBeginPrinting(CDC* pDC, CPrintInfo*);
    void OnEndPrinting(CDC* pDC, CPrintInfo*);
    BOOL OnPreparePrinting(CPrintInfo*);
protected:

    void OnFilePrint();
    void OnFilePrintPreview();


private:
    BOOL DoPreparePrinting(CPrintInfo*);


    //void SetInnerView
public:
    wxView *m_pView;
	xScrollHelper *m_ScrollHelper;
    wxWindow* m_Canvas;

    wxDECLARE_DYNAMIC_CLASS(CView);
    DECLARE_MESSAGE_MAP();
};

class CGLView: public CView{
public:
    wxWindow* CreateCanvas(wxWindow* parent, wxPoint& point, wxSize& size, long style);
    wxDECLARE_DYNAMIC_CLASS(CGLView);
};

class CTreeView: public CView{
public:
    virtual bool Create(wxWindow* parent, wxPoint& point, wxSize& size);
    virtual void OnPaint();
    CTreeCtrl *GetTreeCtrl(){return ((CTreeCtrl *)m_Canvas); }
    wxDECLARE_DYNAMIC_CLASS(CTreeView);
    DECLARE_MESSAGE_MAP();
};

class CListView: public CView{
public:
    virtual bool Create(wxWindow* parent, wxPoint& point, wxSize& size);
    virtual void OnPaint();
    wxListCtrl *GetListCtrl(){return ((wxListCtrl *)m_Canvas); }
    wxDECLARE_DYNAMIC_CLASS(CListView);
    DECLARE_MESSAGE_MAP()
};

class CScrollView: public CView{
public:
    wxDECLARE_DYNAMIC_CLASS(CScrollView);
};

class CFormView: public CView{
public:
    CFormView(){}
    CFormView(const char* lpszTemplateName);
    CFormView(UINT nIDTemplate);
    bool LoadView(wxFrame*);

    virtual bool OnCreate(wxDocument *doc, long flags);
    virtual HBRUSH  OnCtlColor(CDC *pDC, void*, int nCtlColor) {return 0;}
    virtual bool Create(wxWindow* parent, wxPoint& point, wxSize& size);
    virtual void OnPaint();
    wxDECLARE_DYNAMIC_CLASS(CFormView);

    const char* m_lpszTemplateName;
    UINT m_nIDTemplate;
    DECLARE_MESSAGE_MAP()
};

class CFormViewXmlHandler: public wxXmlResourceHandler{
public:
    CFormViewXmlHandler();
    wxObject *DoCreateResource();
    bool CanHandle(wxXmlNode *node);
    wxDECLARE_DYNAMIC_CLASS(CFormViewXmlHandler);
};


class CDocument: public wxDocument, public CWndBase{
public:
    bool OnSaveDocument(const wxString& filename);
    BOOL OnOpenDocument(LPCTSTR filename);
    bool OnOpenDocument(const wxString& filename);
    virtual bool OnNewDocument();
    void UpdateAllViews(CView *sender);
    bool DeleteAllViews();

private:
    void SendInitialUpdate();
    wxDECLARE_DYNAMIC_CLASS(CDocument);
	DECLARE_MESSAGE_MAP()
};

class CSingleDocTemplate : public wxDocTemplate{
public:
    CSingleDocTemplate(const wxString& resource, wxClassInfo* docinfo,
            wxClassInfo* childfrm_info, wxClassInfo* viewinfo);
    enum DocStringIndex{
        windowTitle,
        docName ,
        fileNewName,
        filterName,
        filterExt,
        regFileTypeId,
        regFileTypeName
    };
    virtual BOOL GetDocString(
    wxString& rString,
    enum DocStringIndex index) const{
        if(index == filterExt)
        {
            rString = _T(".test");
        }
        else if( index == fileNewName || index == filterName )
        {
            rString = _T("test(*.test)");
        }
        else
        {
            rString = _T(".test");
        }
        return TRUE;
    }

    wxString m_resource;
    wxClassInfo *m_frmClassInfo;
};
class CDocChildFrame: public wxFrame, public CWndBase{//wxDocChildFrame
public:
    CDocChildFrame();
    bool Create(wxDocument *doc,
                wxFrame *frame,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long type,
                const wxString& name);
        wxDocument * GetDocument () const {return m_childDocument;}
    wxView * GetView () const {return m_childView;}
    void 	SetDocument (wxDocument *doc) {m_childDocument=doc;}
    void 	SetView (wxView *view) {m_childView=view;}

    BOOL OnCreateClient(CREATESTRUCT* cs, CCreateContext* pContext);
    virtual bool ProcessEvent(wxEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnCloseWindow(wxCloseEvent& event);


    wxDocument *m_childDocument;
    wxView *m_childView;
    DECLARE_CLASS(CDocChildFrame);
    DECLARE_EVENT_TABLE()
    DECLARE_MESSAGE_MAP()
};

class CDocParentFrame: public wxFrame, public CWndBase{//wxDocParentFrame
public:
    CDocParentFrame();
    bool Create(wxDocManager *manager, wxFrame *parent, wxWindowID id,
        const wxString& title, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name);
        wxDocManager * 	GetDocumentManager () const { return m_docManager; }

    BOOL OnCreateClient(CREATESTRUCT* cs, CCreateContext* pContext){return TRUE;}
    void OnMRUFile(wxCommandEvent& event);
    virtual bool ProcessEvent(wxEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);

    wxDocManager *m_docManager;
    DECLARE_CLASS(CDocParentFrame);
    DECLARE_EVENT_TABLE()
    DECLARE_MESSAGE_MAP()
};
class CMDIParentFrameXmlHandler: public wxFrameXmlHandler{
public:
    wxObject *DoCreateResource();
    bool CanHandle(wxXmlNode *node);
    wxDECLARE_DYNAMIC_CLASS(CMDIParentFrameXmlHandler);
};

class CMDIChildFrameXmlHandler: public wxFrameXmlHandler{
public:
    wxObject *DoCreateResource();
    bool CanHandle(wxXmlNode *node);
    wxDECLARE_DYNAMIC_CLASS(CMDIChildFrameXmlHandler);

};

class CMDIParentFrame: public CDocParentFrame{//public wxMDIParentFrame
public:
    CMDIParentFrame();
    ~CMDIParentFrame();
    void OnCreate(wxWindowCreateEvent&);
    int OnCreate(CREATESTRUCT*);
    BOOL OnCreateClient(CREATESTRUCT* cs, CCreateContext* pContext){return TRUE;}
    bool Create(const wxString& title, const wxPoint& pos,
                                                         const wxSize& size, long style, const wxString& name);
    void MDIActivate(CMDIChildFrame* child);
    void MDINext();
    CMDIChildFrame* MDIGetActive();
    bool LoadFrame(const wxString& resource);

    wxDECLARE_DYNAMIC_CLASS(CMDIParentFrame);
    DECLARE_MESSAGE_MAP()
};

class CMDIChildFrame: public CDocChildFrame{//public wxMDIChildFrame,
public:
    CMDIChildFrame();
    ~CMDIChildFrame();

    virtual BOOL OnCreateClient(CREATESTRUCT* cs, CCreateContext* pContext);
    BOOL LoadFrame(const wxString& resource, wxWindow* parent);
    void SetActiveView(CView* pViewNew, BOOL bNotify);
    CView* GetActiveView();
    CDocument* GetActiveDocument();
    BOOL IsIconic();
    void Activate();
    void OnCreate(wxWindowCreateEvent& e);
    int OnCreate(CREATESTRUCT*);
    bool Create(CMDIParentFrame* parent);
    BOOL PreCreateWindow(CREATESTRUCT& );
    void OnClose();

    wxDECLARE_DYNAMIC_CLASS(CMDIChildFrame);
    DECLARE_MESSAGE_MAP()
};

class CMultiDocTemplate : public wxDocTemplate{
public:

    CMultiDocTemplate(const wxString& resource, wxClassInfo* docinfo,
            wxClassInfo* childfrm_info, wxClassInfo* viewinfo);

    wxDocument* CreateDocument(const wxString& path, long flags);



    wxClassInfo* GetUserViewClassInfo();
    wxDocument* OpenDocumentFile(const char* path, long flags);
    wxString GetResource(){ return m_resource;}

        wxString m_resource;
        wxClassInfo *m_childfrmClassInfo;
        wxClassInfo	*m_userViewClassInfo;
        wxString m_docTypeName;
        wxString m_viewTypeName;
};

/*Miscellaneous Classes*/
class CApp: public wxApp{
public:
    CApp();
    ~CApp();

    bool OnInit();
    int OnExit();

public:
    virtual BOOL InitInstance() ;
    virtual int ExitInstance();
    void AddDocTemplate(wxDocTemplate* pTemplate);
    wxString GetResName(int id){return wxXmlResource::FindXRCIDById(id);}

    long GetProfileInt(const wxString& section, const wxString& entry, long default_val);
    wxString GetProfileString(const wxString& section, const wxString& entry, const wxString& default_val);
    void WriteProfileInt(const wxString& section, const wxString& entry, int value);
    void WriteProfileString(const wxString& section, const wxString& entry, const wxString& value);

    wxCursor* LoadCursor(const char* str);
    void BeginWaitCursor();
    void EndWaitCursor();

    wxDocManager *m_pDocManager;
    wxPrintData *m_pPrintData;
    wxLocale m_Locale;
};
CApp* GetApp();
class CWinApp: public CApp{};
class CWinAppEx: public CApp{};

struct CCreateContext
{
    wxClassInfo* m_pNewViewClass; //新建视图的动态创建类
    wxDocument* m_pCurrentDoc;//当前的文档类，这文档类将和m_pNewViewClass动态创建类创建的视图类关联
    wxDocTemplate* m_pNewDocTemplate;//和框架窗口关联的文档模版
    CView* m_pLastView;//原先的视图类，通常在切分窗口的视图类中使用（splitterWnd）
    CFrameWnd* m_pCurrentFrame;//当前框架窗口
    CCreateContext():m_pNewViewClass(0),m_pCurrentDoc(0),m_pNewDocTemplate(0),m_pLastView(0),m_pCurrentFrame(0){}
};


class CObject: public wxObject{};
class CCmdTarget: public wxEvtHandler{};
class CCommandLineInfo: 	public wxCmdLineParser{};
class CWaitCursor: public wxBusyCursor{};


/*Internet Classes*/
class CSocket: public wxSocketBase{};
class CFtpConnection: public wxFTP{};
class CHttpConnection: public wxHTTP{};

/*Drag and Drop Classes*/
/*class COleDataSource: wxDataObject{};
class COleDropSource: wxDropSource{};
class COleDropTarget: wxDropTarget{};*/

/*File Classes*/
class CTime:  public wxDateTime{
public:
    CTime();
    CTime(int year, int month, int day, int hour, int minute, int second);
    int GetDayOfWeek();
    static CTime GetCurrentTime();
};

class COleDateTime: public wxDateTime{
public:
    COleDateTime();
    COleDateTime(int year, int month, int day, int hour, int minute, int second);
    int GetDayOfWeek();
    static COleDateTime GetCurrentTime();
};

struct CFileStatus{
    CTime m_ctime;
    CTime m_mtime;
    CTime m_atime;
    ULONGLONG m_size;
    BYTE m_attribute;
    char m_szFullName[_MAX_PATH];
};

class CFile: public wxFile{
public:
    CFile();
    CFile(const wxString& path, int mode);
    ~CFile();
    enum CFileMode{
        modeCreate,
        modeRead,
        modeReadWrite,
        modeWrite
    };
    BOOL Open(const wxString& path, int mode=modeRead);
    long SeekToEnd();
    long Seek(long off, unsigned int from);
    DWORD GetPosition() const;
    unsigned int GetLength();
    wxString GetFilePath();
    wxString GetFileName();
    void Remove(const wxString& fname);
    void Rename(const wxString& fname, const wxString& newname);
    static BOOL SetStatus(const wxString& fname, const CFileStatus& rStatus);
    BOOL GetStatus(const wxString& fname, CFileStatus& rStatus);

    wxString m_strFileName;
};

class CFileFind: public CFile{
public:
    BOOL FindFile(const wxString& fname, DWORD);
    BOOL FindNextFile();
    BOOL GetLastWriteTime(CTime& dtm);
    wxString GetFileTitle();
    wxString GetFilePath();
    BOOL GetFirst(wxString *f, wxString path){*f=wxFindFirstFile(path);return !f->empty();}
    BOOL GetNext(wxString *f){*f = wxFindNextFile(); return !f->empty();}

    wxString m_strFile;
    wxString m_prev_file;
    wxString m_strPath;
};

/*class CTimeSpan: wxTimeSpan{};
class CMemFile: wxMemoryInputStream,wxMemoryOutputStream{};
class CSocketFile: wxSocketInputStream,wxSocketOutputStream{};
class CRecentFileList: wxFileHistory{};*/

/*Multithreading Classes*/
typedef UINT (*THREAD_ENTRY)(void *);
class CWinThread: public wxThread
{
public:
	CWinThread(THREAD_ENTRY entry, void* param, wxThreadKind kind = wxTHREAD_DETACHED) :
    wxThread(kind), m_entry(entry), m_param(param) {}

	virtual ExitCode Entry()
	{
		return (ExitCode)m_entry(m_param);
	}

    virtual void OnExit()
	{
	}

protected:
	THREAD_ENTRY m_entry;
	void* m_param;
};
void xBeginThread(THREAD_ENTRY entry, void* param, wxThread** pThread);
CWinThread *AfxBeginThread(THREAD_ENTRY entry, void *param);
#define AfxMessageBox wxMessageBox

class CCriticalSection: public wxCriticalSection{};
class CMutex: public wxMutex{};
class CSemaphore: public wxSemaphore{};

class CArchive{};
#endif

