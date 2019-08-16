
// RichEditDblClkTestDlg.h : header file
//

#pragma once


// CRichEditDblClkTestDlg dialog
class CRichEditDblClkTestDlg : public CDialogEx
{
// Construction
public:
	CRichEditDblClkTestDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RICHEDITDBLCLKTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    HACCEL m_hAccelTable;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CRichEditCtrl m_richedit;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedLeft();
    afx_msg void OnBnClickedLeftbreak();
    afx_msg void OnBnClickedRight();
    afx_msg void OnBnClickedRightbreak();
};
