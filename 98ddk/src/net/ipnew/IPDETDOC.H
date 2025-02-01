/******************************************************************************
**
** Module:       		ipdetect.exe
** File:         		ipdetdoc.h
** Descriptions:		implementation of CIpdetectDoc class
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


class CIpdetectDoc : public CDocument
{
protected: // create from serialization only
	CIpdetectDoc();
	DECLARE_DYNCREATE(CIpdetectDoc)

// Attributes
public:
// Operations
public:

// Advanced Overrides
	virtual BOOL SaveModified(); // return TRUE if ok to continue

// Implementation
public:
	virtual ~CIpdetectDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

// Generated message map functions
protected:
	//{{AFX_MSG(CIpdetectDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
