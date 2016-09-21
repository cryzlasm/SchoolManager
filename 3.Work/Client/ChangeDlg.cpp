// ChangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ChangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeDlg dialog


CChangeDlg::CChangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeDlg)
	m_strFour = _T("");
	m_strOne = _T("");
	m_strThree = _T("");
	m_strTwo = _T("");
	//}}AFX_DATA_INIT

    dwState = 0;
    strOne = TEXT("");
    strTwo = TEXT("");
    strThree = TEXT("");
    strFour = TEXT("");
}


void CChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeDlg)
	DDX_Control(pDX, IDC_EDIT_TWO2, m_strTwoCtl);
	DDX_Control(pDX, IDC_EDIT_ONE2, m_strOneCtl);
	DDX_Control(pDX, IDC_EDIT_FOUR2, m_strFourCtl);
	DDX_Control(pDX, IDC_EDIT_THREE2, m_strThreeCtl);
	DDX_Control(pDX, IDC_STATIC_TWO2, m_Two2);
	DDX_Control(pDX, IDC_STATIC_TWO, m_Two);
	DDX_Control(pDX, IDC_STATIC_THREE2, m_Three2);
	DDX_Control(pDX, IDC_STATIC_THREE, m_Three);
	DDX_Control(pDX, IDC_STATIC_ONE2, m_One2);
	DDX_Control(pDX, IDC_STATIC_ONE, m_One);
	DDX_Control(pDX, IDC_STATIC_FOUR2, m_Four2);
	DDX_Control(pDX, IDC_STATIC_FOUR, m_Four);
	DDX_Control(pDX, IDC_EDIT_TWO, m_TwoCtl);
	DDX_Control(pDX, IDC_EDIT_THREE, m_ThreeCtl);
	DDX_Control(pDX, IDC_EDIT_ONE, m_OneCtl);
	DDX_Control(pDX, IDC_EDIT_FOUR, m_FourCtl);
	DDX_Text(pDX, IDC_EDIT_FOUR2, m_strFour);
	DDX_Text(pDX, IDC_EDIT_ONE2, m_strOne);
	DDX_Text(pDX, IDC_EDIT_THREE2, m_strThree);
	DDX_Text(pDX, IDC_EDIT_TWO2, m_strTwo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeDlg)
	ON_BN_CLICKED(IDC_BTN_CHANGE, OnBtnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeDlg message handlers

void CChangeDlg::OnBtnChange() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

    this->OnOK();
}

//�޸�ѧ��, ѧ��ID ѧ���� ѧ���Ա� �༶ID
BOOL CChangeDlg::SetChangeStu(CString& strSID, CString& strSName, CString& strSGender, CString& strSCid)
{
    strOne = strSID;
    strTwo = strSName;
    strThree = strSGender;
    strFour = strSCid;

    dwState = ONE_STU;

    return TRUE;
}

//�޸Ŀγ� �γ�ID �γ���
BOOL CChangeDlg::SetChangeCourse(CString& strCouID, CString& strCouName)
{
    strOne = strCouID;
    strTwo = strCouName;
    strThree = TEXT("");
    strFour = TEXT("");
    
    dwState = TWO_COURSE;

    return TRUE;
}

//�޸İ༶ �༶ID �༶��
BOOL CChangeDlg::SetChangeClass(CString& strCID, CString& strCName)
{
    strOne = strCID;
    strTwo = strCName;
    strThree = TEXT("");
    strFour = TEXT("");
    
    dwState = THREE_CLS;

    return TRUE;
}

BOOL CChangeDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}


//�޸�ѧ��, ѧ��ID ѧ���� ѧ���Ա� �༶ID
BOOL CChangeDlg::GetChangeStu(CString& strSID, CString& strSName, CString& strSGender, CString& strSCid)
{
    strSID = m_strOne;
    strSName = m_strTwo;
    strSGender = m_strThree;
    strSCid = m_strFour;
    
    dwState = 0;
    
    return TRUE;
}

//�޸Ŀγ� �γ�ID �γ���
BOOL CChangeDlg::GetChangeCourse(CString& strCouID, CString& strCouName)
{
    strCouID = m_strOne;
    strCouName = m_strTwo;
    
    dwState = 0;
    
    return TRUE;
}

//�޸İ༶ �༶ID �༶��
BOOL CChangeDlg::GetChangeClass(CString& strCID, CString& strCName)
{
    strCID = m_strOne;
    strCName = m_strTwo;

    dwState = 0;
    
    return TRUE;
}


BOOL CChangeDlg::InitSomeThings()
{
    //�����ϲ���ǩ
    m_One.SetWindowText(TEXT(""));
    m_Two.SetWindowText(TEXT(""));
    m_Three.SetWindowText(TEXT(""));
    m_Four.SetWindowText(TEXT(""));
    
    //�����²���ǩ
    m_One2.SetWindowText(TEXT(""));
    m_Two2.SetWindowText(TEXT(""));
    m_Three2.SetWindowText(TEXT(""));
    m_Four2.SetWindowText(TEXT(""));
    
    //�����ϲ�Edit��Ϣ
    m_OneCtl.SetWindowText(TEXT(""));
    m_TwoCtl.SetWindowText(TEXT(""));
    m_ThreeCtl.SetWindowText(TEXT(""));
    m_FourCtl.SetWindowText(TEXT(""));

    //�����²�Edit
    m_strOneCtl.SetWindowText(TEXT(""));
    m_strTwoCtl.SetWindowText(TEXT(""));
    m_strThreeCtl.SetWindowText(TEXT(""));
    m_strFourCtl.SetWindowText(TEXT(""));


    //�����ϲ���ǩ
    m_One.EnableWindow(TRUE);
    m_Two.EnableWindow(TRUE);
    m_Three.EnableWindow(TRUE);
    m_Four.EnableWindow(TRUE);
    
    //�����²���ǩ
    m_One2.EnableWindow(TRUE);
    m_Two2.EnableWindow(TRUE);
    m_Three2.EnableWindow(TRUE);
    m_Four2.EnableWindow(TRUE);
    
    //�����ϲ�Edit��Ϣ
    m_OneCtl.EnableWindow(FALSE);
    m_TwoCtl.EnableWindow(FALSE);
    m_ThreeCtl.EnableWindow(FALSE);
    m_FourCtl.EnableWindow(FALSE);
    
    //�����²�Edit
    m_strOneCtl.EnableWindow(TRUE);
    m_strTwoCtl.EnableWindow(TRUE);
    m_strThreeCtl.EnableWindow(TRUE);
    m_strFourCtl.EnableWindow(TRUE);

    return TRUE;
}

BOOL CChangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    InitSomeThings();
    
	
	if(dwState == ONE_STU)
	{
        //�����ϲ���ǩ
        m_One.SetWindowText(TEXT("ѧ��"));
        m_Two.SetWindowText(TEXT("����"));
        m_Three.SetWindowText(TEXT("�Ա�"));
        m_Four.SetWindowText(TEXT("�༶ID"));
        
        //�����²���ǩ
        m_One2.SetWindowText(TEXT("ѧ��"));
        m_Two2.SetWindowText(TEXT("����"));
        m_Three2.SetWindowText(TEXT("�Ա�"));
        m_Four2.SetWindowText(TEXT("�༶ID"));
        
        //�����ϲ�Edit��Ϣ
        m_OneCtl.SetWindowText(strOne);
        m_TwoCtl.SetWindowText(strTwo);
        m_ThreeCtl.SetWindowText(strThree);
        m_FourCtl.SetWindowText(strFour);
	}
    else if(dwState == TWO_COURSE)
    {
        //�����ϲ���ǩ
        m_One.SetWindowText(TEXT("�γ̺�"));
        m_Two.SetWindowText(TEXT("�γ���"));
        m_Three.EnableWindow(FALSE);
        m_Four.EnableWindow(FALSE);
        
        //�����²���ǩ
        m_One2.SetWindowText(TEXT("�γ̺�"));
        m_Two2.SetWindowText(TEXT("�γ���"));
        m_Three2.EnableWindow(FALSE);
        m_Four2.EnableWindow(FALSE);
        
        //�����ϲ�Edit��Ϣ
        m_OneCtl.SetWindowText(strOne);
        m_TwoCtl.SetWindowText(strTwo);
        m_ThreeCtl.EnableWindow(FALSE);
        m_FourCtl.EnableWindow(FALSE);

        //�����²�Edit
        m_strThreeCtl.EnableWindow(FALSE);
        m_strFourCtl.EnableWindow(FALSE);
    }
    else if(dwState == THREE_CLS)
    {
        //�����ϲ���ǩ
        m_One.SetWindowText(TEXT("�༶��"));
        m_Two.SetWindowText(TEXT("�༶��"));
        m_Three.EnableWindow(FALSE);
        m_Four.EnableWindow(FALSE);
        
        //�����²���ǩ
        m_One2.SetWindowText(TEXT("�༶��"));
        m_Two2.SetWindowText(TEXT("�༶��"));
        m_Three2.EnableWindow(FALSE);
        m_Four2.EnableWindow(FALSE);
        
        //�����ϲ�Edit��Ϣ
        m_OneCtl.SetWindowText(strOne);
        m_TwoCtl.SetWindowText(strTwo);
        m_ThreeCtl.EnableWindow(FALSE);
        m_FourCtl.EnableWindow(FALSE);
        
        //�����²�Edit
        m_strThreeCtl.EnableWindow(FALSE);
        m_strFourCtl.EnableWindow(FALSE);
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
