#ifndef CENTRAL_LIMIT_DLG_H
#define CENTRAL_LIMIT_DLG_H
#pragma once
#include <vector>

const int INFO_GROUP_HEIGHT = 70;
const int AXIS_LINE_GAP_FROM_SIDES = 5;

class CentralLimitDlg : public CDialog
{
public:
	CentralLimitDlg(CWnd* pParent = NULL);
	~CentralLimitDlg();

	enum { IDD = IDD_CENTRALLIMIT_DIALOG };

protected:
	virtual void		DoDataExchange(CDataExchange* pDX);
	HICON				m_hIcon;
	CComboBox			m_comboBox;
	int					m_nOfDices;
	CString				m_nOfRolls;
	int					m_paintAreaWidth;
	int					m_paintAreaHeight;
	int*				m_totalScores;
	int					m_barWidth;
	int					m_highestScoreFreq;
	int					m_unitHeight;

	/**
	 * 
	 * Below is m_totalScores, for 6 dices. Each index (i), holds the value of number if the (i+1) scores. If we roll
	 * 7 dices and if results are 1, 2, 3, 4, 4, 5, 6, values of m_totalScores's elements will look as following: 
	 * 
  	 *			+---+---+---+---+---+---+
	 * Index:	| 0 | 1 | 2 | 3 | 4 | 5 |
	 *			+---+---+---+---+---+---+
	 *
	 * Value:	  1   1   1   2   1   1  
	 *
	 **/

	virtual BOOL	OnInitDialog();
	afx_msg void	OnPaint();
	afx_msg HCURSOR	OnQueryDragIcon();
	void			OnOK();
	void			OnRoll();
	void			OnReset();
	void			OnDiceNumberChange();
	int				GetRandomNumber() const;
	void			ResetData(const int& diceN);
	void			DrawGraph(CPaintDC& dc);
	inline int		GetHighestScoreFreq(const int* scores) const;
	inline int		GetTotalScoreFreq(const int* scores) const;

	DECLARE_MESSAGE_MAP()
};

#endif	//CENTRAL_LIMIT_DLG_H
