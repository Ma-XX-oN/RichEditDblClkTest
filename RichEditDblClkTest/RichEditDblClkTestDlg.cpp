
// RichEditDblClkTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RichEditDblClkTest.h"
#include "RichEditDblClkTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRichEditDblClkTestDlg dialog



CRichEditDblClkTestDlg::CRichEditDblClkTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RICHEDITDBLCLKTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRichEditDblClkTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICHEDIT21, m_richedit);
}

BEGIN_MESSAGE_MAP(CRichEditDblClkTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CRichEditDblClkTestDlg message handlers

LRESULT CALLBACK RicheditSubclass(
    HWND hRichEdit,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData
)
{
    LRESULT classified, is_delim;
    if (uMsg == WM_LBUTTONDBLCLK) {
        ::SendMessageW(hRichEdit, WM_SETREDRAW, FALSE, 0);
        CHARRANGE range;
        ::SendMessageW(hRichEdit, EM_EXGETSEL, 0, (LPARAM)&range);
        classified = ::SendMessageW(hRichEdit, EM_FINDWORDBREAK, WB_CLASSIFY, range.cpMin);
        is_delim = ::SendMessageW(hRichEdit, EM_FINDWORDBREAK, WB_ISDELIMITER, range.cpMin);
    }
    auto result = DefSubclassProc(hRichEdit, uMsg, wParam, lParam);

    if (uMsg == WM_LBUTTONDBLCLK) {
        CHARRANGE range;
        ::SendMessageW(hRichEdit, EM_EXGETSEL, 0, (LPARAM)&range);
        auto is_white = classified & WBF_ISWHITE;
        if (is_delim || is_white) {
            range.cpMin = (LONG)::SendMessageW(hRichEdit, EM_FINDWORDBREAK, WB_LEFT, range.cpMax);
            TRACE("%d\n", range.cpMin);
        }
        else {
            auto new_max = (LONG)::SendMessageW(hRichEdit, EM_FINDWORDBREAK, WB_LEFTBREAK, range.cpMax);
            if (new_max > range.cpMin) {
                range.cpMax = new_max;
            }
        }
        CHARRANGE tmp{ range.cpMin, range.cpMin };
        ::SendMessageW(hRichEdit, EM_EXSETSEL, 0, (LPARAM)&tmp);
        ::SendMessageW(hRichEdit, WM_SETREDRAW, TRUE, 0);
        ::SendMessageW(hRichEdit, EM_EXSETSEL, 0, (LPARAM)&range);
#ifdef DEBUG
        ::SendMessageW(hRichEdit, EM_EXGETSEL, 0, (LPARAM)&tmp);
        ASSERT(range.cpMax == tmp.cpMax && range.cpMin == tmp.cpMin);
#endif
    }
    return result;
}

#include <algorithm>
#include <map>
HWND hRichEdit;
int CALLBACK Editwordbreakproc(
    LPWSTR lpch,
    int ichCurrent,
    int cch, // WTF!? cch is byte length, not char length.  Happens with WB_MOVEWORDLEFT and WB_MOVEWORDRIGHT. Need to check others.
    int code
)
{
#if defined DEBUG
#define MAP_VALUE(value_name) { value_name, L ## #value_name }
    static const std::map<int, LPCTSTR> name_map = {
        MAP_VALUE(WB_CLASSIFY),
        MAP_VALUE(WB_ISDELIMITER),
        MAP_VALUE(WB_LEFT),
        MAP_VALUE(WB_LEFTBREAK),
        MAP_VALUE(WB_MOVEWORDLEFT),
        MAP_VALUE(WB_RIGHT),
        MAP_VALUE(WB_RIGHTBREAK),
        MAP_VALUE(WB_MOVEWORDRIGHT),
    };
#undef MAP_VALUE

    auto found = name_map.find(code);
    if (found == name_map.end()) {
        TRACE(L"'%c' (%02x) UNKNOWN (%d)\n", lpch[ichCurrent], lpch[ichCurrent], code);
    }
    else {
        TRACE(L"'%c' (%02x) %.*s\n", lpch[ichCurrent], lpch[ichCurrent], -16, found->second);
    }
    //if (code != WB_MOVEWORDLEFT && code != WB_MOVEWORDRIGHT) {
        TRACE(L"% 3d  % *.*sv\n", ichCurrent, ichCurrent, ichCurrent, L"");
        TRACE(L"% 3d |% *.*s|\n", cch / 2, cch / 2, cch / 2, lpch);
        ASSERT(cch % 2 == 0);
    //}
#endif
    ::SendMessageW(hRichEdit, EM_SETWORDBREAKPROC, 0, 0);

    WCHAR text[200];
    TEXTRANGE range = { { 0, -1 }, text };
    int result = ::SendMessageW(hRichEdit, EM_GETTEXTRANGE, 0, (LPARAM)&range);
    auto found_str = std::search(std::begin(text), std::begin(text) + wcslen(text), lpch, lpch + cch / 2 - (lpch[cch / 2 - 1] == L'\r' ? 1 : 0));
    // When doing a WB_MOVEWORDRIGHT, the lpch string will have a leading whitespace character
    if (found_str == std::begin(text) + wcslen(text)) {
        found_str = std::search(std::begin(text), std::begin(text) + wcslen(text), lpch + 1, lpch + cch / 2 - (lpch[cch / 2 - 1] == L'\r' ? 1 : 0));
    }
    ASSERT(found_str != std::begin(text) + wcslen(text));
    // NOTE: This is a reference test.  It may not work correctly if you don't
    //       have unique string values greater than 16 characters in length.
    //       This is because the search that was just performed is to get the
    //       location of the buffer (lpch) inside of the document, and if you
    //       have multiple copies, it may pick the wrong location.
    auto document_offset = std::distance(text, found_str);
    result = ::SendMessageW(hRichEdit, EM_FINDWORDBREAK, code, document_offset + ichCurrent);
    TRACE("%d\n", result);
    if (code != WB_CLASSIFY && code != WB_ISDELIMITER) {
        // Have to adjust result from document offset to buffer offset.
        if (result < document_offset) {
            result = 0;
        }
        else if (document_offset + cch / 2 <= result) {
            result = cch / 2;
        }
        else {
            result -= document_offset;
        }
    }

    ::SendMessageW(hRichEdit, EM_SETWORDBREAKPROC, 0, (LPARAM)Editwordbreakproc);
    auto proc = (EDITWORDBREAKPROCW)::SendMessageW(hRichEdit, EM_GETWORDBREAKPROC, 0, 0);
    ASSERT(proc == Editwordbreakproc);

#if defined DEBUG
//    if (code != WB_MOVEWORDLEFT && code != WB_MOVEWORDRIGHT && code != WB_ISDELIMITER) {
    //TRACE(L"% 3d |% *.*s|\n", wcslen(text), wcslen(text), wcslen(text), text);
    TRACE(L"% 3d  % *.*s^\n", result, result, result, L"");
//    }
#endif

    return result;
}

BOOL CRichEditDblClkTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    hRichEdit = m_richedit.m_hWnd;
    ::SetWindowSubclass(hRichEdit, RicheditSubclass, 0, 0);
    ::SendMessageW(hRichEdit, EM_SETWORDBREAKPROC, 0, (LPARAM)Editwordbreakproc);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRichEditDblClkTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRichEditDblClkTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRichEditDblClkTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

