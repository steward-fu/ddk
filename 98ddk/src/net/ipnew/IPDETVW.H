/******************************************************************************
**
** Module:       		ipdetect.exe
** File:         		ipdetvw.h
** Descriptions:		interface of the CIpdetectView class
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

class CIpdetectView : public CEditView
{
protected: // create from serialization only
	CIpdetectView();
	DECLARE_DYNCREATE(CIpdetectView)

// Attributes
public:
	CIpdetectDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CIpdetectView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CIpdetectView)
	afx_msg void OnPaint();
	afx_msg int OnCreate( LPCREATESTRUCT lpcs );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ipdetvw.cpp
inline CIpdetectDoc* CIpdetectView::GetDocument()
   { return (CIpdetectDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
