/******************************************************************************
**
** Module:       		ipdetect.exe
** File:         		ipdetdoc.cpp
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
// CIpdetectDoc

IMPLEMENT_DYNCREATE(CIpdetectDoc, CDocument)

BEGIN_MESSAGE_MAP(CIpdetectDoc, CDocument)
	//{{AFX_MSG_MAP(CIpdetectDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpdetectDoc construction/destruction

CIpdetectDoc::CIpdetectDoc()
{
	// TODO: add one-time construction code here
}

CIpdetectDoc::~CIpdetectDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectDoc OnNewDocument
//

BOOL CIpdetectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIpdetectDoc SaveModified
//

BOOL CIpdetectDoc::SaveModified( void )
{
	if ( IsModified() )
	{
	    CString name = m_strPathName;
		
		if ( name.IsEmpty() )
		{
			VERIFY( name.LoadString( IDS_DEFAULTFILENAME ) );
		}                                                               
	
		if ( !name.IsEmpty() )
		{
			if ( !DoSave( name ) )
				return FALSE;
		}
	}
		
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectDoc serialization

void CIpdetectDoc::Serialize(CArchive& ar)
{
	if ( ar.IsStoring() )
	{
		POSITION 	   pos   = GetFirstViewPosition();
		CIpdetectView* pView = (CIpdetectView*)GetNextView( pos );
		
		if ( pView )
			pView->Serialize( ar );
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CIpdetectDoc diagnostics

#ifdef _DEBUG
void CIpdetectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIpdetectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIpdetectDoc commands
