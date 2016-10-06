#ifndef CENTRAL_LIMIT_H
#define CENTRAL_LIMIT_H
#pragma once
#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif
#include "resource.h"

class CentralLimit : public CWinApp
{
public:
	CentralLimit();
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CentralLimit theApp;
#endif	//CENTRAL_LIMIT_H