
// KeyboardHookDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "KeyboardHook.h"
#include "KeyboardHookDlg.h"
#include "afxdialogex.h"
#include <iostream>  
#include <string>  
#include <fstream>

using namespace std;

//全局变量
HHOOK glhHook = NULL;			//安装的鼠标勾子句柄 
BOOL g_bCapsLock = FALSE;		//大小写锁定键	
BOOL g_bShift = FALSE;			//shift键
ofstream SaveFile("key.txt");

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKeyboardHookDlg 对话框


//键盘钩子回调函数
LRESULT  CALLBACK  KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

	char c[2];
	c[1] = 0;
	if ((wParam == WM_KEYDOWN) && (HC_ACTION == nCode)) {		//有键按下

		KBDLLHOOKSTRUCT* keyNum = (KBDLLHOOKSTRUCT*)lParam;

		//处理字母大小写
		if ((keyNum->vkCode == VK_CAPITAL) || (keyNum->vkCode == VK_LSHIFT) || (keyNum->vkCode == VK_RETURN) || (keyNum->vkCode >= 65 && keyNum->vkCode <= 90)) {

			if (!GetKeyState(VK_CAPITAL)) {	//如果大写锁定键未被按下
				g_bCapsLock = FALSE;
			}
			else {
				g_bCapsLock = TRUE;
			}

			if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) { //如果shift键被按住
				g_bShift = TRUE;
			}
			else {
				g_bShift = FALSE;
			}

			if (keyNum->vkCode >= 65 && keyNum->vkCode <= 90) {
				BOOL flag = g_bCapsLock ^ g_bShift;//同假异真
				if (flag) {
					c[0] = keyNum->vkCode;
				}
				else {
					c[0] = keyNum->vkCode + 32;
				}
				SaveFile << (int)c[0] << " : " << c << endl;
			}
		}
		//处理数字小键盘
		else if (keyNum->vkCode == 144 || (keyNum->vkCode >= VK_NUMPAD0 && keyNum->vkCode <= VK_NUMPAD9)) { //144表示数字小键盘锁键

			if (GetKeyState(144)) {		//如果数字小键盘锁键被按下
				int mapKey = keyNum->vkCode - 48;
				SaveFile << keyNum->vkCode << " : " << char(mapKey) << endl;
			}

		}
		else {
			SaveFile << keyNum->vkCode << " : " << char(keyNum->vkCode) << endl;
		}
	}
	return CallNextHookEx(glhHook, nCode, wParam, lParam);
}


CKeyboardHookDlg::CKeyboardHookDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KEYBOARDHOOK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeyboardHookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKeyboardHookDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CKeyboardHookDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CKeyboardHookDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CKeyboardHookDlg 消息处理程序

BOOL CKeyboardHookDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKeyboardHookDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CKeyboardHookDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CKeyboardHookDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	string str = "start:";
	SaveFile << str << endl;

	//这里调用SetWindowsHookExA()函数，因为hook的实现不是在DLL中，而是直接在KeyboardHookDlg.cpp中实现，所有第4个参数使用GetModuleHandle(NULL)
	glhHook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), NULL);
	if (glhHook != NULL) {
		//AfxMessageBox(L"StartHook成功！");//用于打桩测试，通过后注释掉，不然太麻烦
	}
	else {
		AfxMessageBox(L"StartHook失败！");
	}
}


void CKeyboardHookDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	UnhookWindowsHookEx(glhHook);
}
