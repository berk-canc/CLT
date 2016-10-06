#include "stdafx.h"
#include "CentralLimit.h"
#include "CentralLimitDlg.h"
#include <time.h>
#include <sstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CentralLimitDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_DICE_N, OnDiceNumberChange)
	ON_COMMAND(IDC_BUTTON_ROLL, OnRoll)
	ON_COMMAND(IDC_BUTTON_RESET, OnReset)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

CentralLimitDlg::CentralLimitDlg(CWnd* pParent)	: 
	CDialog(CentralLimitDlg::IDD, pParent),
	m_nOfDices(1),
	m_nOfRolls(""),
	m_paintAreaWidth(0),
	m_paintAreaHeight(0),
	m_totalScores(NULL),
	m_barWidth(0),
	m_unitHeight(0),
	m_highestScoreFreq(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Seed for random number. 
	//Without this seed you will always get the same random number.
	srand(time(0));

	m_totalScores = new int[m_nOfDices*6];

	if(m_totalScores == NULL)
	{
		AfxThrowMemoryException();
	}
	memset(m_totalScores, NULL, m_nOfDices*6*sizeof(int));
}

CentralLimitDlg::~CentralLimitDlg()
{
	if(m_totalScores != NULL)
	{
		delete[] m_totalScores;
		m_totalScores = NULL;
	}
}

void CentralLimitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_DICE_N, m_comboBox);

	DDX_Text(pDX, IDC_EDIT_ROLL_N, m_nOfRolls);
	DDV_MaxChars(pDX, m_nOfRolls, 2);
}

BOOL CentralLimitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_comboBox.LimitText(0);
	m_comboBox.AddString("1");
	m_comboBox.AddString("2");
	m_comboBox.AddString("3");
	m_comboBox.AddString("4");
	m_comboBox.AddString("5");
	m_comboBox.SetCurSel(0);
	
	SetIcon(m_hIcon, TRUE);		//Set big icon
	SetIcon(m_hIcon, FALSE);	//Set small icon

	RECT rect;
	GetClientRect(&rect);
	
	m_paintAreaWidth = rect.right;
	m_paintAreaHeight = rect.bottom - INFO_GROUP_HEIGHT;

	return TRUE;
}

void CentralLimitDlg::OnPaint() 
{
	CPaintDC dc(this);

	if ( IsIconic() )
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);

		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		//Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//draw x-axis
		dc.MoveTo(AXIS_LINE_GAP_FROM_SIDES, m_paintAreaHeight);
		dc.LineTo(m_paintAreaWidth-AXIS_LINE_GAP_FROM_SIDES, m_paintAreaHeight);

		m_barWidth = ( m_paintAreaWidth-(2*AXIS_LINE_GAP_FROM_SIDES) ) / (m_nOfDices*6);
		int nOfTicks = 0;

        //paint ticks
		for(int i=m_barWidth+AXIS_LINE_GAP_FROM_SIDES; i<m_paintAreaWidth; i+=m_barWidth)
		{
			//dont draw the last tick! For example; if there are 6 dices dont draw the 13th tick.
			if(nOfTicks >= (m_nOfDices*6)-1)
			{
				break;
			}

			dc.MoveTo(i, m_paintAreaHeight);
			dc.LineTo(i, m_paintAreaHeight+AXIS_LINE_GAP_FROM_SIDES);
			nOfTicks++;
		}

		DrawGraph(dc);
	}
}

void CentralLimitDlg::DrawGraph(CPaintDC& dc)
{
	m_unitHeight		= 0;
	m_highestScoreFreq	= GetHighestScoreFreq(m_totalScores);
	bool overTheTop		= false;	//indicates if bar higher than the dialog
	int barHeight		= 0;

	if(m_highestScoreFreq == 0)
	{
		m_unitHeight = 0;
	}
	else
	{
		//initial m_unitHeight
		m_unitHeight = m_paintAreaHeight/(m_nOfDices*6);	
	}

	//when a bar hits the top, that bar's y-axis becomes the largest y-axis value for the graph
	if(m_paintAreaHeight-(m_unitHeight*m_highestScoreFreq) <= 0)
	{
		overTheTop = true;

		//determine unit height based on the highest bar
		//m_highestScoreFreq		m_paintAreaHeight
		//m_totalScores[i]			x (unit height)
		//___________________________________________
		//x = (m_totalScores[i]*m_paintAreaHeight) / m_highestScoreFreq
		
		for(int i=0; i<6*m_nOfDices; i++)
		{
			int height = 0;

			height = (m_totalScores[i]*m_paintAreaHeight) / m_highestScoreFreq;

			dc.Rectangle( (i*m_barWidth)+AXIS_LINE_GAP_FROM_SIDES,
						m_paintAreaHeight-height,
						((i+1)*m_barWidth)+AXIS_LINE_GAP_FROM_SIDES,
						m_paintAreaHeight);
		}
	}

	if(overTheTop == false)
	{
		for(int i=0; i<6*m_nOfDices; i++)
		{
			dc.Rectangle( (i*m_barWidth)+AXIS_LINE_GAP_FROM_SIDES,
						m_paintAreaHeight-(m_unitHeight*m_totalScores[i]),
						((i+1)*m_barWidth)+AXIS_LINE_GAP_FROM_SIDES,
						m_paintAreaHeight);
		}
	}
}

void CentralLimitDlg::OnOK()
{
	//over-riden to block access to default implementation.
	//if not, hitting enter will close the dialog.
}

void CentralLimitDlg::OnRoll()
{
	UpdateData(TRUE);	//controls to members

	const int TIMES_TO_ROLL	= (int) atof( m_nOfRolls.GetBuffer() );
	const int N_OF_DICES = m_comboBox.GetCurSel()+1;

	m_nOfDices = N_OF_DICES;

	//bad input
	if(	   m_nOfRolls	 == ""
		|| TIMES_TO_ROLL <= 0
		|| N_OF_DICES	 <= 0)
	{
		AfxMessageBox("Enter number of dice rolls.", MB_OK); 
	}
	else
	{
		int totalScore = 0;

		for(int i=0; i<TIMES_TO_ROLL; i++)
		{			
			for(int j=0; j<N_OF_DICES; j++)
			{
				totalScore += GetRandomNumber();
			}
			m_totalScores[totalScore-1]++;
			totalScore = 0;
		}

		Invalidate();
		UpdateWindow();
	}	//end of else
}

int CentralLimitDlg::GetHighestScoreFreq(const int* scores) const
{
	int hiScore = m_totalScores[0];

	for(int i=0; i<(m_nOfDices*6)-1; i++)
	{
		if(m_totalScores[i+1] > hiScore)
		{	
			hiScore = m_totalScores[i+1];
		}
	}

	return hiScore;
}

int CentralLimitDlg::GetTotalScoreFreq(const int* scores) const
{
	int totalScore = 0;
	
	for(int i=0; i<m_nOfDices*6; i++)
	{
		totalScore += m_totalScores[i];
	}

	return totalScore;
}

void CentralLimitDlg::OnDiceNumberChange()
{
	m_nOfDices = m_comboBox.GetCurSel() + 1;
    ResetData(m_nOfDices);

	Invalidate();
	UpdateWindow();	
}

void CentralLimitDlg::OnReset()
{
	ResetData(m_nOfDices);

	Invalidate();
	UpdateWindow();
}

HCURSOR CentralLimitDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//returns a random number 1 to 6, inclusive
int CentralLimitDlg::GetRandomNumber() const
{
	CString str = "";
	int randomNumber = 0;

	do
	{
		std::stringstream sstrm;
		sstrm << rand();						//put random number to the stream

		str = sstrm.str().c_str();				//put stream into a string
		str = str.GetAt(str.GetLength()-1);		//get string's last char

		randomNumber = atoi(str.GetBuffer());	//put that char into an int
	}
	while(   randomNumber >  6
		  || randomNumber == 0);

	return randomNumber;
}

void CentralLimitDlg::ResetData(const int& diceN)
{
	m_nOfDices	= diceN;
	m_nOfRolls	= "";

	if(m_totalScores != NULL)
	{
		delete[] m_totalScores;
		m_totalScores = NULL;
	}

	m_totalScores = new int[diceN*6];

	if(m_totalScores == NULL)
	{
		AfxThrowMemoryException();
	}
	memset(m_totalScores, NULL, diceN*6*sizeof(int));
}