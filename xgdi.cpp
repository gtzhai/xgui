#include "xgui.h"

CPen xNullPen(wxTRANSPARENT, 1, 0);
CBrush xNullBrush(wxTRANSPARENT, 0);
CFont xNullFont;
CPalette xNullPalette;

/*
 *	Point
 */
CPoint::CPoint(){}
CPoint::CPoint(int x, int y) : wxPoint(x,y){}
CPoint::CPoint(const wxPoint& pnt)
{
	x = pnt.x;
	y = pnt.y;
}

/*
 *	Size
 */
CSize::CSize(){}
CSize::CSize(int x, int y) : cx(x), cy(y){}


/*
 *	Rect
 */
CRect::CRect(){}
CRect::CRect(const wxRect& rect){
	left = rect.x;
	top = rect.y;
	right = left + rect.width;
	bottom = top + rect.height;
}
CRect::CRect(CPoint& pnt1, CPoint& pnt2){
	left = pnt1.x; top = pnt1.y;
	right = pnt2.x; bottom = pnt2.y;
}
CRect::CRect(const CRect* rect) {
	*this = *rect;
}
CRect::CRect(int l, int t, int r, int b) {left = l; top = t; right = r; bottom = b;}
int CRect::Width() {return right - left;}
int CRect::Height() {return bottom - top;}
wxPoint CRect::GetPosition()
{
	return wxPoint(left,top);
}
wxSize CRect::GetSize()
{
	return wxSize(Width(), Height());
}
void CRect::NormalizeRect(){
	int nTemp;
	if (left > right)
	{
		nTemp = left;
		left = right;
		right = nTemp;
	}
	if (top > bottom)
	{
		nTemp = top;
		top = bottom;
		bottom = nTemp;
	}
}
bool CRect::IntersectRect(const CRect* rc1, const CRect* rc2){
	left = std::max(rc1->left, rc2->left);
	top = std::max(rc1->top, rc2->top);
	right = std::min(rc1->right, rc2->right);
	bottom = std::min(rc1->bottom, rc2->bottom);
	return (left < right) && (top < bottom);
}
bool CRect::PtInRect(CPoint& pnt){
	return (pnt.x >= left && pnt.y >= top && pnt.x < right && pnt.y < bottom);
}
void CRect::InflateRect(int dx, int dy){
	if (-2*dx>(right-left)){
		 left+=(right-left)/2;
		 right = left;
	 }else{
		 // The inflate is valid.
		 left-=dx;
		 right+=dx;
	 }

	 if (-2*dy>(bottom-top)){
		 top+=(bottom-top)/2;
		 bottom=top;
	 }else{
		 // The inflate is valid.
		 top-=dy;
		 bottom+=dy;
	 }
}
void CRect::OffsetRect(int dx, int dy){
	left += dx;
	right += dx;
	top += dy;
	bottom += dy;
}

/*
 *	Region
 */

CRegion::CRegion(){}
#if defined(_WIN32) || defined(_WIN64)
CRegion::CRegion(CPoint* pnt, int nr, int op) :
	wxRegion((size_t)nr, pnt, (wxPolygonFillMode)op){}
#endif
CRegion::CRegion(int l, int t, int r, int b) :
	wxRegion(l, t, r-l, b-t){}
CRegion::CRegion(const wxBitmap& bmp, const wxColour& trans) :
	wxRegion(bmp, trans) {}
CRegion::CRegion(const wxRegion& rgn) : wxRegion(rgn){}
CRegion* CRegion::CreateRectRgn(int l, int t, int r, int b){
	CRegion* rgn = new CRegion(l, t, r, b);
	return rgn;
}
#if defined(_WIN32) || defined(_WIN64)
CRegion* CRegion::CreatePolygonRgn(CPoint* pnt, int nr, int op){
	ASSERT(op == wxODDEVEN_RULE);//currently only wxODDEVEN_RULE hacked
	CRegion* rgn = new CRegion(pnt, nr, op);
	return rgn;
}
#endif
CRegion* CRegion::CreateRoundRectRgn(int l, int t, int r, int b, int rw, int rh){
	//create mono bitmap with black backgroud white colored shape
/*
	wxMemoryDC dc;
	int width = r-l;
	int height = b-t;
	wxBitmap bmp(width, height, 1);
	dc.SelectObject(bmp);
	dc.SetBackground(*wxBLACK_BRUSH);
	dc.Clear();
	dc.SetPen(*wxWHITE_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.DrawRoundedRectangle(0, 0, width, height, (rw+rh)/4.);
	dc.SelectObject(wxNullBitmap);

	xRegion* rgn = new xRegion(bmp, *wxBLACK);
	rgn->Offset(l, t);

	return rgn;
*/
	return CreateRectRgn(l,t,r,b);
}

CRegion* CRegion::CreateEllipticRgn(int l, int t, int r, int b){
	//create mono bitmap with black backgroud white colored shape
/*
	wxMemoryDC dc;
	int width = r-l;
	int height = b-t;
	wxBitmap bmp(width, height, 1);
	dc.SelectObject(bmp);
	dc.SetBackground(*wxBLACK_BRUSH);
	dc.Clear();
	dc.SetPen(*wxWHITE_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.DrawEllipse(0, 0, width, height);
	dc.SelectObject(wxNullBitmap);

	xRegion* rgn = new xRegion(bmp, *wxBLACK);
	rgn->Offset(l, t);

	return rgn;
*/
	return CreateRectRgn(l,t,r,b);
}

int GetRgnBox(const CRegion* rgn, CRect& rect){
	rect = rgn->GetBox();
	return 1;
}

int CombineRgn(CRegion *dst, const CRegion* src1, const CRegion* src2, int op){
	dst->Clear();
	*dst = *src1;
	bool ret = false;
	switch(op) {
	case RGN_AND:
		ret = dst->Intersect(*src2);
		break;
	case RGN_OR:
		ret = dst->Union(*src2);
		break;
	default:
		ASSERT(false);
		break;
	}

	return ret;
}

void FillRgn(CDC* pDC, const CRegion* rgn, const CBrush* brush){
	CPen* oldpen = pDC->SelectObject(&xNullPen);
	CBrush* oldbrush = pDC->SelectObject(brush);

	CRect r(rgn->GetBox());
	pDC->Rectangle(&r);

	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
}

/*
 *	Font
 */
#define FONT_CACHE_SIZE 128
struct font_cache_entry
{
	int nHeight;//
	int nWidth;//
	int nWeight;//
	BYTE bItalic;//
	BYTE bUnderline;//
	BYTE cStrikeOut;//
	wxFontFamily family;
	wxString strFace;
	wxFont font;
}_font_cache[FONT_CACHE_SIZE];

CFont::CFont(){}
void CFont::CreateFont( int nHeight,//
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
			   const char* lpszFacename)
{
//	int style = (bItalic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
	int flag = wxFONTFLAG_DEFAULT;
	if(bItalic)
		flag |= wxFONTFLAG_ITALIC;
	if(bUnderline)
		flag |= wxFONTFLAG_UNDERLINED;

	switch(nWeight) {
	case FW_LIGHT:
		flag |= wxFONTFLAG_LIGHT;
		break;
	case FW_BOLD:
		flag |= wxFONTFLAG_BOLD;
		break;
	default:
		break;
	}

	if(nHeight < 0) nHeight = -nHeight;

	//cache some fonts, 'cause wxFont::New with pixelsize is time eater on wxGTK2
	int n = 0;
	int empty_cache = -1;
	for(;n < FONT_CACHE_SIZE; n ++)
	{
		font_cache_entry& fe = _font_cache[n];
		if(fe.font.Ok())
		{
			if(fe.nHeight == nHeight &&
				fe.nWidth == nWidth &&
				fe.nWeight == nWeight &&
				fe.bItalic == bItalic &&
				fe.bUnderline == bUnderline &&
				fe.cStrikeOut == cStrikeOut &&
				fe.family == family &&
				fe.strFace == lpszFacename)
			{
				//hit cache;
				*((wxFont*)this) = fe.font;
				return;
			}
		}
		else
		{
			if(empty_cache == -1)
				empty_cache = n;
		}
	}

	if(empty_cache == -1)
	{//cache is full
		static int washout = -1;
		washout ++;
		if(washout >= FONT_CACHE_SIZE)
			washout = 0;

		empty_cache = washout;
	}

	wxFont* ft  = wxFont::New(wxSize(nWidth, nHeight),
		family,
		flag,
		lpszFacename);

	*((wxFont*)this) = *ft;
	delete ft;

	font_cache_entry& empty = _font_cache[empty_cache];
	empty.nHeight = nHeight;
	empty.nWidth = nWidth;
	empty.nWeight = nWeight;
	empty.bItalic = bItalic;
	empty.bUnderline = bUnderline;
	empty.cStrikeOut = cStrikeOut;
	empty.family = family;
	empty.strFace = lpszFacename;
	empty.font = *this;
}

#ifndef OS_WINDOWS
#define FF_ROMAN            (1<<4)  /* Variable stroke width, serifed. */
                                    /* Times Roman, Century Schoolbook, etc. */
#define FF_SWISS            (2<<4)  /* Variable stroke width, sans-serifed. */
                                    /* Helvetica, Swiss, etc. */
#define FF_MODERN           (3<<4)  /* Constant stroke width, serifed or sans-serifed. */
                                    /* Pica, Elite, Courier, etc. */
#define FF_SCRIPT           (4<<4)  /* Cursive, etc. */
#define FF_DECORATIVE       (5<<4)  /* Old English, etc. */
#endif

bool CFont::CreateFontIndirect(const LOGFONT* lf)
{
	wxFontFamily family = wxFONTFAMILY_SWISS;

	switch(lf->lfPitchAndFamily & 0xF0)
	{
        case FF_ROMAN:
            family = wxFONTFAMILY_ROMAN;
            break;
        case FF_SWISS:
            family = wxFONTFAMILY_SWISS;
            break;

        case FF_SCRIPT:
            family = wxFONTFAMILY_SCRIPT;
            break;

        case FF_MODERN:
            family = wxFONTFAMILY_MODERN;
            break;
        case FF_DECORATIVE:
            family = wxFONTFAMILY_DECORATIVE;
            break;
	}

	CreateFont((int)lf->lfHeight, (int)lf->lfWidth, 0, 0, (int)lf->lfWeight, lf->lfItalic, lf->lfUnderline,
		lf->lfStrikeOut, 0, 0, 0, 0, family, (const char* )lf->lfFaceName);

	return true;
}

/*
 *	Palette
 */
bool CPalette::CreatePalette(const LOGPALETTE* lp){
#if 0
	BYTE* r = new BYTE[lp->palNumEntries];
	BYTE* g = new BYTE[lp->palNumEntries];
	BYTE* b = new BYTE[lp->palNumEntries];
	int n = 0;
	for(;n < lp->palNumEntries; n ++){
		r[n]= lp->palPalEntry[n].peRed;
		g[n]= lp->palPalEntry[n].peGreen;
		b[n]= lp->palPalEntry[n].peBlue;
	}
	bool ret = Create(lp->palNumEntries, r, g, b);

	delete r;
	delete g;
	delete b;

	return ret;
#else
    return false;
#endif
}
wxColour CPalette::GetColor(int index)
{
	BYTE r = 0,g = 0,b = 0;
#if 0
	GetRGB(index,&r,&g,&b);
#endif
	return wxColour(r,g,b);
}

/*
 *	Bitmap
 */
CBitmap::CBitmap() {}
CBitmap::CBitmap(int width, int height, int depth):wxBitmap(width, height, depth){}
CBitmap::CBitmap(int res)
{
	LoadFile(GetApp()->GetResName(res));
}

CBitmap::CBitmap(const char* szbitmap)
{
	LoadFile(szbitmap);
}

bool CBitmap::LoadBitmap(const char* szbitmap){
	return LoadFile(szbitmap, wxBITMAP_TYPE_ANY);
}
bool CBitmap::LoadBitmap(int res){
	return LoadFile(GetApp()->GetResName(res), wxBITMAP_TYPE_ANY);
}

/*
 *	Pen
 */
CPen::CPen(){}
CPen::CPen(wxPen& pen) : wxPen(pen)  {}
CPen::CPen(int style, int width, const COLORREF& clr)
	: wxPen(clr,
		width, style)
{
#if defined(_WIN32) || defined(_WIN64)
	SetJoin(wxJOIN_MITER);
#endif
}

/*
 *	Brush
 */
CBrush::CBrush(){}
CBrush::CBrush(const wxBitmap& bmp) : wxBrush(bmp){}
CBrush::CBrush(const wxBrush& brush) : wxBrush(brush){}
CBrush::CBrush(const COLORREF& clr) : wxBrush(clr){}
CBrush::CBrush(int style, const COLORREF& clr) : wxBrush(clr, style){}

/*
 *	DC
 */
CDC::CDC(){
	Init();
}

CDC::CDC(CWndBase* win)
{
	Init();
	m_pDC = new wxClientDC(win->GetWindow());
	m_bSelfAlloc = true;

	m_pWnd = win->GetWindow();
}

CDC::CDC(CControlWrapper* win)
{
	Init();
	m_pDC = new wxClientDC(win->m_control);
	m_bSelfAlloc = true;

	m_pWnd = win->m_control;
}

void CDC::SetDC(wxDC* pDC, CWndBase* wnd)
{
	if(m_bSelfAlloc)
	{
		delete m_pDC;
		m_bSelfAlloc = false;
	}
	m_pDC = pDC;
	m_pWnd = wnd->GetWindow();
}

CDC::~CDC()
{
	if(m_bSelfAlloc)
	{
		delete m_pDC;
		m_bSelfAlloc = false;
	}

	m_pDC = 0;
}

wxWindow* CDC::GetWindow()
{
	return m_pWnd;
}

void CDC::GetRect(CRect* rect){
	wxSize s;
	s = m_pDC->GetSize();
	rect->left = 0;
	rect->top = 0;
	rect->right = s.x;
	rect->bottom = s.y;
}

void CDC::Init()
{
	m_bPrinting = false;
	m_bSelfAlloc = false;
	m_TextAlign = TA_LEFT|TA_TOP;
	m_prev_brush = &xNullBrush;
	m_prev_pen = &xNullPen;
	m_prev_font = &xNullFont;
	m_prev_palette = &xNullPalette;
}

bool CDC::IsPrinting()
{
	return m_bPrinting;
}

void CDC::RealizePalette (){
}

bool CDC::RectVisible(CRect* rect)
{
	CRect xrect;
	m_pDC->GetClippingBox(&xrect.left, &xrect.top, &xrect.right, &xrect.bottom);
	if(xrect.right == 0 || xrect.bottom == 0)
	{
		return true;//no clipping
	}

	xrect.right += xrect.left;
	xrect.bottom += xrect.top;
	CRect r;
	return r.IntersectRect(rect, &xrect);
}

void CDC::SetTextColor(const COLORREF& clr){
	m_pDC->SetTextForeground(clr);
}

const COLORREF& CDC::SetBkColor(const COLORREF& clr){
	const wxColour& oldclr = m_pDC->GetTextBackground();
	m_pDC->SetTextBackground(clr);
#if 0
	return oldclr.GetPixel();
#else
	return oldclr.GetRGB();
#endif
}

CPen* CDC::SelectObject(CPen* pen){
	CPen* prev = m_prev_pen;
	m_prev_pen = (CPen*)pen;
	m_pDC->SetPen((const wxPen&)(*pen));
	return prev;
}

CBrush* CDC::SelectObject(const CBrush* brush){
	CBrush* prev = m_prev_brush;
	m_prev_brush = (CBrush*)brush;
	m_pDC->SetBrush((const wxBrush&)(*brush));
	return prev;
}

CFont* CDC::SelectObject(const CFont* font){
	CFont* prev = m_prev_font;
	m_prev_font = (CFont*)font;
	m_pDC->SetFont((const wxFont&)(*font));
	return prev;
}

CPalette* CDC::SelectPalette(const CPalette* palette, int){
	CPalette* prev = m_prev_palette;
	m_prev_palette = (CPalette*)palette;
	//no acutually using palette for wxGTK use palette not well.
//	m_pDC->SetPalette((const wxPalette&)(*palette));
	return prev;
}

void CDC::SetViewportOrg(int x, int y)
{
	m_pDC->SetDeviceOrigin(x, y);
}

CPoint CDC::GetViewportOrg()
{
	CPoint pt;
	m_pDC->GetDeviceOrigin(&pt.x, &pt.y);
	return pt;
}

void CDC::SetClippingRegion(int l, int t, int r, int b)
{
	m_pDC->SetClippingRegion(l,t,r-l,b-t);
}

int CDC::SetBkMode(int mode){
	int prev_mode = m_pDC->GetBackgroundMode();
	m_pDC->SetBackgroundMode(mode);
	return prev_mode;
};

int CDC::GetDeviceCaps(int option){
	switch(option) {
	case LOGPIXELSX:
		return m_pDC->GetPPI().x;
		break;
	case LOGPIXELSY:
		return m_pDC->GetPPI().y;
		break;
	case HORZRES:
		return m_pDC->GetSize().x;
		break;
	case VERTRES:
		return m_pDC->GetSize().y;
		break;
	case NUMCOLORS:
		return 16;
		break;
	default:
		ASSERT(false);
		return -1;
		break;
	}
}

int CDC::SetTextAlign(int align){
	int oalign = m_TextAlign;
	m_TextAlign=align;
	return oalign;
}

void CDC::MoveTo(const CPoint& pt){
	m_StartPos.x = pt.x;
	m_StartPos.y = pt.y;
}

void CDC::MoveTo(int x, int y){
	m_StartPos.x = x;
	m_StartPos.y = y;
}

void CDC::LineTo(const CPoint& pt){
	m_pDC->DrawLine(m_StartPos.x, m_StartPos.y, pt.x, pt.y);
	m_StartPos.x = pt.x;
	m_StartPos.y = pt.y;
}

void CDC::LineTo(int x, int y){
	m_pDC->DrawLine(m_StartPos.x, m_StartPos.y, x, y);
	m_StartPos.x = x;
	m_StartPos.y = y;
}

void CDC::TextOut(int x, int y, const char* str, int len){
	TextOut(x, y, wxString(str,len));
}

void CDC::TextOut(int x, int y, const wxString& str){
	int w=-1,h=-1;
	if(m_TextAlign & TA_RIGHT){
		if(w==-1)
			m_pDC->GetTextExtent(str, &w, &h);
		x -= w;
	}else if(m_TextAlign & TA_CENTER){
		if(w==-1)
			m_pDC->GetTextExtent(str, &w, &h);
		x -= w/2;
	}

	if(m_TextAlign & TA_BOTTOM){
		if(h!=-1)
			m_pDC->GetTextExtent(str, &w, &h);
		y -= h;
	}else if(m_TextAlign & VTA_CENTER){
		if(h!=-1)
			m_pDC->GetTextExtent(str, &w, &h);
		y -= h/2;
	}

	m_pDC->DrawText(str, x, y);
}

void CDC::Rectangle(CRect* rect){
	m_pDC->DrawRectangle(rect->left, rect->top, rect->Width(), rect->Height());
}

void CDC::Rectangle(const CRect& rect){
	Rectangle((CRect*)&rect);
}

void CDC::DrawRoundRect(wxDC* pDC, int l, int t, int r, int b, int rw, int rh){
	pDC->DrawRoundedRectangle(l, t, r-l, b-t, (rw+rh)/4.);
}

void CDC::RoundRect(int l, int t, int r, int b, int rw, int rh){
	DrawRoundRect(m_pDC, l, t, r, b, rw, rh);
}

void CDC::Rectangle(int l, int t, int r,int b){
	m_pDC->DrawRectangle(l, t, r-l, b-t);
}

void CDC::Ellipse(CRect* rect){
	m_pDC->DrawEllipse(rect->left, rect->top, rect->Width(), rect->Height());
}

void CDC::Ellipse(const CRect& rect){
	Ellipse((CRect*)&rect);
}

void CDC::Ellipse(int l, int t, int r, int b){
	m_pDC->DrawEllipse(l, t, r-l, b-t);
}

void CDC::Draw3dRect(int l, int r, int t, int b,
					 const COLORREF& clr_leftop,
					 const COLORREF& clr_rightbottom)
{
	wxBrush br(clr_leftop, wxSOLID);
	m_pDC->SetBrush(br);
	m_pDC->DrawRectangle(l, t, r, t + 1);
	m_pDC->DrawRectangle(l, t, r+1, b + 1);

	wxBrush br1(clr_rightbottom, wxSOLID);
	m_pDC->SetBrush(br1);
	m_pDC->DrawRectangle(l+1, t+1, r+1, b+1);

	m_pDC->SetBrush(xNullBrush);
}

void CDC::Draw3dRect(const CRect* rect, const COLORREF& clr_leftop,
					 const COLORREF& clr_rightbottom)
{
	Draw3dRect(rect->left, rect->top, rect->right, rect->bottom, clr_leftop, clr_rightbottom);
}

void CDC::Polyline(CPoint* pt, int nr){
	m_pDC->DrawLines(nr, pt);
}

void CDC::Polygon(CPoint* pt, int nr){
	m_pDC->DrawPolygon(nr, pt);
}

void CDC::Arc(const CRect& rect, const CPoint& start, const CPoint& end){
	Arc(rect.left, rect.top, rect.right, rect.bottom, start.x, start.y, end.x, end.y);
}

void CDC::Arc( int nLeftRect,   // x-coord of upper-left corner of rectangle
		  int nTopRect,    // y-coord of upper-left corner of rectangle
		  int nRightRect,  // x-coord of lower-right corner of rectangle
		  int nBottomRect, // y-coord of lower-right corner of rectangle
		  int nXRadial1,   // x-coord of first radial's endpoint
		  int nYRadial1,   // y-coord of first radial's endpoint
		  int nXRadial2,   // x-coord of second radial's endpoint
		  int nYRadial2    // y-coord of second radial's endpoint
		  ){
	//essentially a howllow pie;
	CBrush* br = SelectObject(&xNullBrush);
	Pie(nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
	SelectObject(br);
}

void CDC::Pie(const CRect& rect, const CPoint& start, const CPoint& end){
	Pie(rect.left, rect.top, rect.right, rect.bottom, start.x, start.y, end.x, end.y);
}

void CDC::Pie( int nLeftRect,   // x-coord of upper-left corner of rectangle
		  int nTopRect,    // y-coord of upper-left corner of rectangle
		  int nRightRect,  // x-coord of lower-right corner of rectangle
		  int nBottomRect, // y-coord of lower-right corner of rectangle
		  int nXRadial1,   // x-coord of first radial's endpoint
		  int nYRadial1,   // y-coord of first radial's endpoint
		  int nXRadial2,   // x-coord of second radial's endpoint
		  int nYRadial2    // y-coord of second radial's endpoint
		  ){
	int temp;
	if(nLeftRect > nRightRect) {
		temp = nLeftRect;
		nLeftRect = nRightRect;
		nRightRect = temp;
	}
	if(nTopRect > nBottomRect) {
		temp = nTopRect;
		nTopRect = nBottomRect;
		nBottomRect = temp;
	}

	double centerx = (nRightRect+nLeftRect) / 2.;
	double centery = (nBottomRect+nTopRect) / 2.;
	double sa = atan2((double)(centery - nYRadial1)/(nXRadial1-centerx), (double)(nBottomRect-nTopRect)/(nRightRect-nLeftRect));
	double sb = atan2((double)(centery - nYRadial2)/(nXRadial2-centerx), (double)(nBottomRect-nTopRect)/(nRightRect-nLeftRect));
	sa = sa / M_PI * 180;
	sb = sb / M_PI * 180;
	if(nXRadial1 < centerx)
			sa+=180.;
	if(nXRadial2 < centerx)
			sb+=180.;

	m_pDC->DrawEllipticArc(nLeftRect, nTopRect, nRightRect-nLeftRect, nBottomRect-nTopRect,
		sa, sb);
}

void CDC::Chord(const CRect& rect, const CPoint& start, const CPoint& end){
	ASSERT(false);
}

void CDC::DrawPushButton(const CRect& rect){
	ASSERT(false);
}

void CDC::SetPixel(int x, int y, const COLORREF& clr){
	CPen pen(wxSOLID, 1, clr);
	CPen* oldpen = SelectObject(&pen);
	m_pDC->DrawPoint(x, y);
	SelectObject(oldpen);
}

void CDC::FillRect(const CRect& rect, CBrush* brush){
	CBrush* br = SelectObject(brush);
	CPen* pen = SelectObject(&xNullPen);
	Rectangle((CRect*)&rect);
	SelectObject(pen);
	SelectObject(br);
}

void CDC::FillSolidRect(CRect* rect, const COLORREF& clr){
	CBrush br(clr);
	FillRect(*rect, &br);
}

CSize CDC::GetTextExtent(const wxString& str) {
	wxCoord w, h;
	m_pDC->GetTextExtent(str, &w, &h);
	return CSize(w,h);
}
CSize CDC::GetTextExtent(const char* sz, int len) {
	return GetTextExtent(wxString(sz, len));
}

bool CDC::BitBlt(int x, int y, int width, int height, CDC* pSrcDC, int xsrc, int ysrc, int op){
	return m_pDC->Blit(x, y, width, height, pSrcDC->m_pDC, xsrc, ysrc, (wxRasterOperationMode)op);
}

int CDC::SetROP2(int op){
	int oldop = m_pDC->GetLogicalFunction();
	m_pDC->SetLogicalFunction((wxRasterOperationMode)op);
	return oldop;
}

void CDC::LPtoDP(CRect& rect)
{
	rect.left = m_pDC->LogicalToDeviceX(rect.left);
	rect.right = m_pDC->LogicalToDeviceX(rect.right);
	rect.top = m_pDC->LogicalToDeviceY(rect.top);
	rect.bottom = m_pDC->LogicalToDeviceY(rect.bottom);
}

void CDC::DPtoLP(CRect& rect)
{
	rect.left = m_pDC->DeviceToLogicalX(rect.left);
	rect.right = m_pDC->DeviceToLogicalX(rect.right);
	rect.top = m_pDC->DeviceToLogicalY(rect.top);
	rect.bottom = m_pDC->DeviceToLogicalY(rect.bottom);
}

void CDC::LPtoDP(CSize& s)
{
	s.cx = m_pDC->LogicalToDeviceX(s.cx);
	s.cy = m_pDC->LogicalToDeviceY(s.cy);
}

void CDC::DPtoLP(CSize& s)
{
	s.cx = m_pDC->DeviceToLogicalX(s.cx);
	s.cy = m_pDC->DeviceToLogicalY(s.cy);
}

void CDC::DrawFocusRect(const CRect& rect)
{
	Rectangle(rect);
}

void CDC::SetMapMode(int nMode)
{
	m_pDC->SetMapMode((wxMappingMode)nMode);
}

unsigned long  GetSysColor(int nIndex)
{
#if 0
	return (unsigned long)((wxSystemSettings::GetColour((wxSystemColour)nIndex)).GetPixel());
#else
	return (unsigned long)((wxSystemSettings::GetColour((wxSystemColour)nIndex)).GetRGB());
#endif
}
//]
//
//xMemoryDC::xMemoryDC(): wxMemoryDC()
//{
//	xdc.SetDC(this);
//}

/*
 * xImageList
 */
BOOL CImageList::Create(int cx, int cy, UINT, int initial, int)
{
	return wxImageList::Create(cx, cy, true, initial) ? TRUE : FALSE;
}

BOOL CImageList::Create(const wxString& strBmp, int cx, int, const COLORREF& crMask)
{
	BOOL ret=FALSE;
	wxImage img;
	img.LoadFile(strBmp);
	if(img.Ok()){
		int height = img.GetHeight();
		int width = img.GetWidth();
		wxImageList::Create(cx, height, true, width/cx);
		wxBitmap bmp(img);
		ret = Add(&bmp, cx, height, crMask);
	}
	return ret;
}

BOOL CImageList::Create(int nBitmapID, int cx, int, const COLORREF& crMask)
{
	return Create(GetApp()->GetResName(nBitmapID), cx, 0, crMask);
}

BOOL CImageList::Add(const wxBitmap& Bitmap, int cx, int cy, const COLORREF& maskclr)
{
	return Add((wxBitmap*)&Bitmap, cx, cy, maskclr);
}

BOOL CImageList::Add(wxBitmap* Bitmap, int cx, int cy, const COLORREF& maskclr)
{
	{
		if(Bitmap->Ok())
		{
			int width  = Bitmap->GetWidth();
			int n;
			for( n = 0; n < width / cx; n ++){
				wxRect rect(n*cx, 0, cx, cy);
				wxBitmap bmp = Bitmap->GetSubBitmap(rect);
				wxImageList::Add(bmp,maskclr);
			}
		}
	}
	return false;
}

void CImageList::DeleteImageList()
{
	RemoveAll();
}
