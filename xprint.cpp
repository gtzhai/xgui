#include "xgui.h"

unsigned int CPrintInfo::GetMinPage() const
{
	return m_pPD->GetMinPage();
}

void CPrintInfo::SetMinPage(unsigned int nMinPage)
{
	m_pPD->SetMinPage((int)nMinPage);
}

unsigned int CPrintInfo::GetMaxPage() const
{
	return m_pPD->GetMaxPage();
}

void CPrintInfo::SetMaxPage(unsigned int nMaxPage)
{
	m_pPD->SetMaxPage((int)nMaxPage);
}

CPrintInfo::CPrintInfo()
{
	m_pPD = new wxPrintDialogData(*(GetApp()->m_pPrintData));
	SetMinPage(1);
	SetMaxPage(1);

	m_bDirect = false;
	m_nCurPage = 1;

	m_bPreview = false;         // initialize to not preview
	m_bContinuePrinting = true; // Assume it is OK to print
}

CPrintInfo::~CPrintInfo()
{
	delete m_pPD;
}

/*
 * Print & Preview
 */
CPrintout::CPrintout(CView* pView, const wxChar *title) :
  wxPrintout(title) ,
  m_pView(pView)
{
	m_printInfo = new CPrintInfo;
}

CPrintout::~CPrintout()
{
	delete  m_printInfo;
}

bool CPrintout::OnPrintPage(int page)
{
	m_printInfo->m_nCurPage = page;
	wxSize sz = GetDC()->GetSize();
	m_printInfo->m_rectDraw.left = 0;
	m_printInfo->m_rectDraw.top = 0;
	m_printInfo->m_rectDraw.right = sz.x;
	m_printInfo->m_rectDraw.bottom = sz.y;

	CDC dc;
	dc.SetDC(GetDC(),m_pView);
	dc.m_bPrinting = true;

	m_pView->OnPrint(&dc, m_printInfo);
	return true;
}

bool CPrintout::HasPage(int pageNum)
{
	return pageNum >= (int)m_printInfo->GetMinPage() &&
		pageNum <= (int)m_printInfo->GetMaxPage();
}

void CPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)\
{
	*minPage = m_printInfo->m_pPD->GetMinPage();
	*maxPage = m_printInfo->m_pPD->GetMaxPage();
	*selPageFrom = m_printInfo->m_pPD->GetFromPage();
	*selPageTo = m_printInfo->m_pPD->GetToPage();
}

void CPrintout::OnPreparePrinting()
{
	wxPrintout::OnPreparePrinting();
	m_printInfo->m_bPreview = IsPreview();

	m_pView->OnPreparePrinting(m_printInfo);
}

void CPrintout::OnBeginPrinting()
{
	wxPrintout::OnBeginPrinting();

	CDC dc;
	dc.SetDC(GetDC(), m_pView);
	dc.m_bPrinting = true;

	m_pView->OnBeginPrinting(&dc, m_printInfo);
}

void CPrintout::OnEndPrinting()
{
	wxPrintout::OnEndPrinting();

	CDC dc;
	dc.SetDC(GetDC(), m_pView);
	dc.m_bPrinting = true;
	m_pView->OnEndPrinting(&dc, m_printInfo);
}
