/******************************************************************************
**
** Module:       		ipdetect.exe
** File:         		ipdetvw.cpp
** Descriptions:		implementation of the CIpdetectView class
** Contains:    		
**
** Copyright (c) 1996, Microsoft Corporation, all rights reserved
**
** This document is provided for informational purposes only and Microsoft 
** Corporation makes no warranties, either expressed or implied, in this document.
** Information in this document may be substantially changed without notice in
** subsequent versions of windows and does not represent a commitment on the 
** part of Microsoft Corporation. 
**
******************************************************************************/

#include "stdafx.h"
#include "ipdetect.h"

#include "ipdetdoc.h"
#include "ipdetvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIpdetectView

IMPLEMENT_DYNCREATE(CIpdetectView, CView)

BEGIN_MESSAGE_MAP(CIpdetectView, CView)
	//{{AFX_MSG_MAP(CIpdetectView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpdetectView construction/destruction

CIpdetectView::CIpdetectView()
{
	// TODO: add construction code here
}

CIpdetectView::~CIpdetectView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CIpdetectView drawing

void CIpdetectView::OnDraw(CDC* pDC)
{
	CEditView::OnDraw( pDC );

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CIpdetectView diagnostics

#ifdef _DEBUG
void CIpdetectView::AssertValid() const
{
	CEditView::AssertValid();
}

void CIpdetectView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CIpdetectDoc* CIpdetectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIpdetectDoc)));
	return (CIpdetectDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIpdetectView message handlers


void CIpdetectView::OnPaint()
{
	// 10 pixel high Courier font
	LOGFONT logfont;
	CFont	cfont;
	
	memset( &logfont, 0, sizeof(logfont) );
	
	logfont.lfHeight = -10;							// less than zero indicates pixel height
	logfont.lfWeight = FW_NORMAL;					
	logfont.lfPitchAndFamily = FIXED_PITCH;
	lstrcpy( logfont.lfFaceName, "Courier" );
	  
	cfont.CreateFontIndirect( &logfont );
	SetFont( &cfont );


	CEditView::OnPaint();
}
    
    
int CIpdetectView::OnCreate( LPCREATESTRUCT lpcs )
{
	return CEditView::OnCreate( lpcs );
}