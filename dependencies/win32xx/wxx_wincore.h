// Win32++   Version 8.7.0
// Release Date: 12th August 2019
//
//      David Nash
//      email: dnash@bigpond.net.au
//      url: https://sourceforge.net/projects/win32-framework
//
//
// Copyright (c) 2005-2019  David Nash
//
// Permission is hereby granted, free of charge, to
// any person obtaining a copy of this software and
// associated documentation files (the "Software"),
// to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice
// shall be included in all copies or substantial portions
// of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// wxx_wincore.h
//  This file contains the definition of the CWnd class.
//  Including this file adds all the code necessary to start Win32++
//  and create simple windows.
//
//  Refer to the following example code.
//


// Example code
/*

///////////////////////////////////
// main.cpp

// To configure your project:
// 1) Add Win32++'s include directory to the project's additional include directories for C/C++
// 2) Add Win32++'s include directory to the project's additional include directories for Resources

#include "wxx_wincore.h"


// A class that inherits from CWnd. It is used to create the window.
class CView : public CWnd
{
public:
    CView() {}
    virtual ~CView() {}
    virtual void OnDestroy() { PostQuitMessage(0); }    // Ends the program
};



int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Start Win32++
    CWinApp theApp;

    // Create our view window
    CView MyWin;
    MyWin.Create();

    // Run the application's message loop
    return theApp.Run();
}

*/


#ifndef _WIN32XX_WINCORE_H_
#define _WIN32XX_WINCORE_H_



//////////////////////////////////////
//  Include the Win32++ header files

#include "wxx_appcore.h"
#include "wxx_wincore0.h"
#include "wxx_gdi.h"
#ifndef _WIN32_WCE
  #include "wxx_menu.h"
#endif
#include "wxx_ddx.h"



namespace Win32xx
{

    ////////////////////////////////////////
    // Definitions for the CWnd class
    //

    inline CWnd::CWnd() : m_wnd(NULL), m_prevWindowProc(NULL)
    {
        // Note: m_wnd is set in CWnd::CreateEx(...)
    }

    inline CWnd::CWnd(HWND wnd) : m_prevWindowProc(NULL)
    {
        // A private constructor, used internally.

        m_wnd = wnd;
    }

    inline CWnd::~CWnd()
    {
        // Destroys the window for this object and cleans up resources.
        Destroy();
    }

    // Store the window handle and CWnd pointer in the HWND map
    inline void CWnd::AddToMap()
    {
        // The framework must be started
        assert(GetApp());

        // This HWND is should not be in the map yet
        assert (NULL == GetApp()->GetCWndFromMap(*this));

        // Remove any old map entry for this CWnd (required when the CWnd is reused)
        RemoveFromMap();

        // Add the (HWND, CWnd*) pair to the map
        CThreadLock mapLock(GetApp()->m_wndLock);
        GetApp()->m_mapHWND.insert(std::make_pair(GetHwnd(), this));
    }

    // Attaches a CWnd object to an existing window and calls the OnAttach virtual function.
    inline BOOL CWnd::Attach(HWND wnd)
    {
        assert( GetApp() );
        assert( ::IsWindow(wnd) );
        assert( !IsWindow() );

        // Ensure this thread has the TLS index set
        // Note: Perform the attach from the same thread as the window's message loop
        GetApp()->SetTlsData();

        Subclass(wnd);     // Set the window's callback to CWnd::StaticWindowProc

        OnAttach();

        // Post a message to trigger a call of OnInitialUpdate
        PostMessage(UWM_WINDOWCREATED);

        return TRUE;
    }

    // Attaches a CWnd object to a dialog item.
    inline BOOL CWnd::AttachDlgItem(UINT id, HWND parent)
    {
        assert(::IsWindow(parent));

        HWND wnd = ::GetDlgItem(parent, id);
        return Attach(wnd);
    }

    // Positions the window over the center of its parent.
    inline void CWnd::CenterWindow() const
    {

    // required for multi-monitor support with Dev-C++ and VC6
#ifndef _WIN32_WCE
  #ifndef MONITOR_DEFAULTTONEAREST
    #define MONITOR_DEFAULTTONEAREST    0x00000002

        DECLARE_HANDLE(HMONITOR);

        typedef struct tagMONITORINFO
        {
            DWORD   cbSize;
            RECT    rcMonitor;
            RECT    rcWork;
            DWORD   dwFlags;
        } MONITORINFO, *LPMONITORINFO;

  #endif    // MONITOR_DEFAULTTONEAREST
#endif  // _WIN32_WCE

        assert(IsWindow());

        CRect rc = GetWindowRect();
        CRect parentRect;
        CRect desktopRect;

        // Get screen dimensions excluding task bar
        VERIFY(::SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, 0) != 0);

        // Get the parent window dimensions (parent could be the desktop)
        if (GetParent().GetHwnd() != 0) parentRect = GetParent().GetWindowRect();
        else parentRect = desktopRect;

 #ifndef _WIN32_WCE
        // Import the GetMonitorInfo and MonitorFromWindow functions
        typedef BOOL(WINAPI* LPGMI)(HMONITOR hMonitor, LPMONITORINFO lpmi);
        typedef HMONITOR(WINAPI* LPMFW)(HWND hwnd, DWORD flags);
        LPMFW pfnMonitorFromWindow = 0;
        HMODULE hUser32 = LoadLibrary(_T("USER32.DLL"));
        LPGMI pfnGetMonitorInfo = 0;
        if (hUser32)
        {

            pfnMonitorFromWindow = (LPMFW)::GetProcAddress(hUser32, "MonitorFromWindow");
  #ifdef UNICODE
            pfnGetMonitorInfo = (LPGMI)::GetProcAddress(hUser32, "GetMonitorInfoW");
  #else
            pfnGetMonitorInfo = (LPGMI)::GetProcAddress(hUser32, "GetMonitorInfoA");
  #endif

            // Take multi-monitor systems into account
            if (pfnGetMonitorInfo && pfnMonitorFromWindow)
            {
                HMONITOR hActiveMonitor = pfnMonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi;
                ZeroMemory(&mi, sizeof(mi));
                mi.cbSize = sizeof(mi);

                if (pfnGetMonitorInfo(hActiveMonitor, &mi))
                {
                    desktopRect = mi.rcWork;
                    if (GetParent().GetHwnd() == NULL) desktopRect = mi.rcWork;
                }
            }
            ::FreeLibrary(hUser32);

        }
 #endif

        // Calculate point to center the dialog over the portion of parent window on this monitor
        parentRect.IntersectRect(parentRect, desktopRect);
        int x = parentRect.left + (parentRect.Width() - rc.Width())/2;
        int y = parentRect.top + (parentRect.Height() - rc.Height())/2;

        // Keep the dialog wholly on the monitor display
        x = (x < desktopRect.left)? desktopRect.left : x;
        x = (x > desktopRect.right - rc.Width())? desktopRect.right - rc.Width() : x;
        y = (y < desktopRect.top) ? desktopRect.top: y;
        y = (y > desktopRect.bottom - rc.Height())? desktopRect.bottom - rc.Height() : y;

        SetWindowPos(0, x, y, 0, 0, SWP_NOSIZE);
    }

    // Returns the CWnd to its default state
    inline void CWnd::Cleanup()
    {
        if ( GetApp() )
            RemoveFromMap();

        m_wnd = 0;
        m_prevWindowProc = 0;
    }

    // Creates the window with default parameters. PreRegisterClass can be
    //  used to register a new window class for the window, otherwise a 
    //  default window class is used. PreCreate can be used to specify the
    //  CREATESTRUCT parameters, otherwise default parameters are used. 
    //  A failure to create a window throws an exception.
    inline HWND CWnd::Create(HWND parent /* = 0 */)
    {
        // Test if Win32++ has been started
        assert( GetApp() );

        WNDCLASS wc;
        ZeroMemory(&wc, sizeof(wc));

        CREATESTRUCT cs;
        ZeroMemory(&cs, sizeof(cs));

        // Set the WNDCLASS parameters
        PreRegisterClass(wc);
        if (wc.lpszClassName)
        {
            RegisterClass(wc);
            cs.lpszClass = wc.lpszClassName;
        }
        else
            cs.lpszClass = _T("Win32++ Window");

        // Set a reasonable default window style
        DWORD dwOverlappedStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
        cs.style = WS_VISIBLE | ((parent)? WS_CHILD : dwOverlappedStyle );

        // Set a reasonable default window position
        if (NULL == parent)
        {
            cs.x  = CW_USEDEFAULT;
            cs.cx = CW_USEDEFAULT;
            cs.y  = CW_USEDEFAULT;
            cs.cy = CW_USEDEFAULT;
        }

        // Allow the CREATESTRUCT parameters to be modified
        PreCreate(cs);

        DWORD style = cs.style & ~WS_VISIBLE;
        HWND wnd;

        // Create the window
#ifndef _WIN32_WCE
        wnd = CreateEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, style,
                cs.x, cs.y, cs.cx, cs.cy, parent,
                cs.hMenu, cs.lpCreateParams);

        if (cs.style & WS_VISIBLE)
        {
            if      (cs.style & WS_MAXIMIZE) ShowWindow(SW_MAXIMIZE);
            else if (cs.style & WS_MINIMIZE) ShowWindow(SW_MINIMIZE);
            else    ShowWindow();
        }

#else
        wnd = CreateEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
                cs.x, cs.y, cs.cx, cs.cy, parent,
                0, cs.lpCreateParams);
#endif

        return wnd;
    }

    // Creates the window by specifying each parameter. The lpszClassName must
    //  be a predefined class name or registered with RegisterClass. A failure
    //  to create a window throws an exception.
    inline HWND CWnd::CreateEx(DWORD exStyle, LPCTSTR pClassName, LPCTSTR pWindowName, DWORD style, const RECT& rc, HWND parent, UINT id, LPVOID lparam /*= NULL*/)
    {
        int x = rc.left;
        int y = rc.top;
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;

        INT_PTR idMenu = id;
        HMENU menu = parent ? reinterpret_cast<HMENU>(idMenu) : ::LoadMenu(GetApp()->GetResourceHandle(), MAKEINTRESOURCE(id));

        return CreateEx(exStyle, pClassName, pWindowName, style, x, y, cx, cy, parent, menu, lparam);
    }

    // Creates the window by specifying each parameter. The lpszClassName must
    //  be a predefined class name or registered with RegisterClass. A failure
    //  to create a window throws an exception.
    inline HWND CWnd::CreateEx(DWORD exStyle, LPCTSTR pClassName, LPCTSTR pWindowName, DWORD style, int x, int y, int width, int height, HWND hWParent, HMENU idOrMenu, LPVOID lparam /*= NULL*/)
    {
        assert( GetApp() );        // Test if Win32++ has been started
        assert( !IsWindow() );      // Only one window per CWnd instance allowed

        // Ensure a window class is registered
        CString className;
        if (pClassName == 0 || pClassName[0] == _T('\0'))
            className = _T("Win32++ Window");
        else
            className = pClassName;

        WNDCLASS wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.lpszClassName = className;
        wc.hbrBackground = reinterpret_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
        wc.hCursor       = ::LoadCursor(NULL, IDC_ARROW);

        // Register the window class (if not already registered)
        if (RegisterClass(wc) == 0)
        {
            TRACE("*** RegisterClass failed ***\n");
            assert( 0 );
        }

        // Ensure this thread has the TLS index set
        TLSData* pTLSData = GetApp()->SetTlsData();

        // Store the CWnd pointer in thread local storage
        pTLSData->pWnd = this;
        m_wnd = 0;

        // Create window
        HWND wnd = ::CreateWindowEx(exStyle, className, pWindowName, style, x, y, width, height,
                                hWParent, idOrMenu, GetApp()->GetInstanceHandle(), lparam);

        // Tidy up
        pTLSData->pWnd = NULL;

        if (wnd == 0)
        {
            // Throw an exception when window creation fails
            throw CWinException(g_msgWndCreateEx);
        }

        // Automatically subclass predefined window class types
        if (pClassName)
        {
            ::GetClassInfo(GetApp()->GetInstanceHandle(), pClassName, &wc);
            if (wc.lpfnWndProc != GetApp()->m_callback)
            {
                Subclass(wnd);

                // Override this to perform tasks after the window is attached.
                OnAttach();
            }
        }

        // Clear the CWnd pointer from TLS
        pTLSData->pWnd = NULL;

        // Post a message to trigger a call of OnInitialUpdate
        PostMessage(UWM_WINDOWCREATED);

        return wnd;
    }

    // Destroys the window and returns the CWnd back to its default state, ready for reuse.
    inline void CWnd::Destroy()
    {
        if (GetApp())          // Is the CWinApp object still valid?
        {
            if (GetCWndPtr(*this) == this)
            {
                if (IsWindow())
                    ::DestroyWindow(*this);
            }
        }

        // Return the CWnd to its default state
        Cleanup();
    }

    // Reverse an Attach
    inline HWND CWnd::Detach()
    {
        assert(m_prevWindowProc);   // Only previously attached CWnds can be detached

        if (IsWindow())
            SetWindowLongPtr(GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_prevWindowProc));

        HWND wnd = GetHwnd();
        Cleanup();

        return wnd;
    }

#ifndef _WIN32_WCE

    //  This function performs dialog data exchange and dialog data
    //  validation using DDX and DDV functions. Typically this is done for
    //  controls in a dialogs, but controls in any window support DDX and DDV.
    //  Never call this function directly. It is called by the UpdateData()
    //  member function. Call UpdateData() to write data into, or retrieve
    //  validated data from the controls.  Override this method in the dialog
    //  or other window that utilize the DDX/DDV functions.
    inline void CWnd::DoDataExchange(CDataExchange& dx)
    {
        // Any dialog or window using DDX/DDV for its controls should
        // override of this member, and put calls to the DDX and DDV functions
        // there.  For example:
        
        // connect to edit box holding int and specify limits.
        // dx.DDX_Text(IDC_EDIT_UINT,       m_iUINT);
        // dx.DDV_MinMaxUInt(               m_iUINT, 10, 10000);        
          
        // connect to edit box holding double and specify limits.
        // dx.DDX_Text(IDC_EDIT_DOUBLE,     m_double);
        // dx.DDV_MinMaxDouble(             m_double, -10.0, 100000.);
        //
        // connect to rich edit box holding a string and specify length.
        // dx.DDX_Text(IDC_EDIT_RICHEDIT,   m_richEdit);
        // dx.DDV_MaxChars(                 m_richEdit, 25); // limit length
        //
        // connect to slider control and specify limits.
        // dx.DDX_Slider(IDC_SLIDER,        m_slider);
        // dx.DDV_MinMaxSlider(             m_slider, 0, 1000);
        //
        // connect to progress bar.
        // dx.DDX_Progress(IDC_PROGRESSBAR, m_progress);
        //
        // connect to bar to scroll bar.
        // dx.DDX_Scroll(  IDC_SCROLLBAR,   m_scrollBar);
        //
        // connect to radio boxes.
        // dx.DDX_Radio( IDC_RADIO_A,       m_radioA);
        //
        // connect to check boxes.
        // dx.DDX_Check(IDC_CHECK_A,        m_checkA);
        // dx.DDX_Check(IDC_CHECK_B,        m_checkB);
        // dx.DDX_Check(IDC_CHECK_C,        m_checkC);

        UNREFERENCED_PARAMETER(dx);
    }
#endif

    // Pass messages on to the appropriate default window procedure
    // CMDIChild and CMDIFrame override this function.
    inline LRESULT CWnd::FinalWindowProc(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (m_prevWindowProc)
            return ::CallWindowProc(m_prevWindowProc, *this, msg, wparam, lparam);
        else
            return ::DefWindowProc(*this, msg, wparam, lparam);
    }

    // Retrieves the pointer to the CWnd associated with the specified HWND.
    // Returns NULL if a CWnd object doesn't already exist for this HWND. 
    inline CWnd* CWnd::GetCWndPtr(HWND wnd)
    {
        assert( GetApp() );
        return wnd? GetApp()->GetCWndFromMap(wnd) : 0;
    }

    // The GetAncestor function retrieves the ancestor (root parent)
    // of the window. Supports Win95.
    inline CWnd CWnd::GetAncestor(UINT flags /*= GA_ROOTOWNER*/) const
    {
        assert(IsWindow());
        HWND wnd = GetHwnd();

        // Load the User32 DLL
        typedef HWND WINAPI GETANCESTOR(HWND, UINT);
        GETANCESTOR* pfnGetAncestor = NULL;
        HMODULE user32 = ::LoadLibrary(_T("USER32.DLL"));

        if (user32 != 0)
        {
            // Declare a pointer to the GetAncestor function
#ifndef _WIN32_WCE
            pfnGetAncestor = reinterpret_cast<GETANCESTOR*>(::GetProcAddress(user32, "GetAncestor"));
#else
            pfnGetAncestor = reinterpret_cast<GETANCESTOR*>(::GetProcAddress(user32, L"GetAncestor"));
#endif

            if (pfnGetAncestor)
                wnd = (*pfnGetAncestor)(*this, flags);

            ::FreeLibrary(user32);
        }

        if (!pfnGetAncestor)
        {
            // Provide our own GetAncestor if necessary
            HWND parent = ::GetParent(wnd);
            while (::IsChild(parent, wnd))
            {
                wnd = parent;
                parent = ::GetParent(wnd);
            }
        }

        return CWnd(wnd);
    }

    // Retrieves the class name of this object's window.
    inline CString CWnd::GetClassName() const
    {
        assert(IsWindow());

        CString str;
        VERIFY(::GetClassName(*this, str.GetBuffer(MAX_STRING_SIZE), MAX_STRING_SIZE) != 0);
        str.ReleaseBuffer();
        return str;
    }

    // Retrieves the title or text associated with a control in a dialog box.
    inline CString CWnd::GetDlgItemText(int dlgItemID) const
    {
        assert(IsWindow());

        int nLength = ::GetWindowTextLength(::GetDlgItem(*this, dlgItemID));
        CString str;
        VERIFY(::GetDlgItemText(*this, dlgItemID, str.GetBuffer(nLength), nLength+1) != 0);
        str.ReleaseBuffer();
        return str;
    }

    // Retrieves the text of the window's title bar.
    inline CString CWnd::GetWindowText() const
    {
        assert(IsWindow());

        CString str;
        str.GetWindowText(*this);
        return str;
    }

    // Called in response to WM_CLOSE. Override to suppress destroying the window
    // WM_CLOSE is sent by SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0) or by clicking X
    //  in the top right corner. Usually, only top level windows receive WM_CLOSE.
    inline void CWnd::OnClose()
    {
        Destroy();
    }

    // Called when the user interacts with the menu or toolbar.
    inline BOOL CWnd::OnCommand(WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);

        // Override this to handle WM_COMMAND messages, for example

        //  UINT id = LOWORD(wparam);
        //  switch (id)
        //  {
        //  case IDM_FILE_NEW:
        //      OnFileNew();
        //      TRUE;   // return TRUE for handled commands
        //  }

        // return FALSE for unhandled commands
        return FALSE;
    }

    // This function is called when a window is attached to the CWnd.
    // Override it to automatically perform tasks when the window is attached.
    // Note:  Window controls are attached.
    inline void CWnd::OnAttach()
    {
    }

    // Called during window creation. Override this functions to perform tasks
    // such as creating child windows.
    inline int CWnd::OnCreate(CREATESTRUCT& cs)
    {
        // This function is called when a WM_CREATE message is received
        // Override it to automatically perform tasks during window creation.
        // Return 0 to continue creating the window.

        // Note: Window controls don't call OnCreate. They are sublcassed (attached)
        //  after their window is created.

        UNREFERENCED_PARAMETER (cs);
        return 0;
    }

    // This function is called when a window is destroyed.
    // Override it to do additional tasks, such as ending the application
    //  with PostQuitMessage.
    inline void CWnd::OnDestroy()
    {
    }

    // Called when part of the client area of the window needs to be drawn.
    // Override this function in your derived class to perform drawing tasks.
    inline void CWnd::OnDraw(CDC& dc)
    {
        UNREFERENCED_PARAMETER(dc);
    }

    // Called when the background of the window's client area needs to be erased.
    // Override this function in your derived class to perform drawing tasks.
    // Return Value: Return FALSE to also permit default erasure of the background
    //               Return TRUE to prevent default erasure of the background
    inline BOOL CWnd::OnEraseBkgnd(CDC& dc)
    {
        UNREFERENCED_PARAMETER(dc);

        return FALSE;
    }

    // This function is called automatically once the window is created
    // Override it in your derived class to automatically perform tasks
    // after window creation.
    inline void CWnd::OnInitialUpdate()
    {
    }

    // Called when menu items are about to be displayed. Override this function to
    // enable/disable the menu item, or add/remove the check box or radio button
    // to menu items.
    inline void CWnd::OnMenuUpdate(UINT id)
    {
        UNREFERENCED_PARAMETER(id);

        // Override this function to modify the behaviour of menu items,
        // such as adding or removing checkmarks
    }

    // A function used internally to call OnMessageReflect. Don't call or override this function.
    inline LRESULT CWnd::MessageReflect(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        HWND wnd = 0;
        switch (msg)
        {
        case WM_COMMAND:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
        case WM_CHARTOITEM:
        case WM_VKEYTOITEM:
        case WM_HSCROLL:
        case WM_VSCROLL:
            wnd = reinterpret_cast<HWND>(lparam);
            break;

        case WM_DRAWITEM:
        case WM_MEASUREITEM:
        case WM_DELETEITEM:
        case WM_COMPAREITEM:
            wnd = GetDlgItem(static_cast<int>(wparam));
            break;

        case WM_PARENTNOTIFY:
            switch(LOWORD(wparam))
            {
            case WM_CREATE:
            case WM_DESTROY:
                wnd = reinterpret_cast<HWND>(lparam);
                break;
            }
        }

        CWnd* pWnd = GetApp()->GetCWndFromMap(wnd);

        if (pWnd != NULL)
            return pWnd->OnMessageReflect(msg, wparam, lparam);

        return 0;
    }

    // This function processes those special messages sent by some older controls,
    // and reflects them back to the originating CWnd object.
    // Override this function in your derived class to handle these special messages:
    // WM_COMMAND, WM_CTLCOLORBTN, WM_CTLCOLOREDIT, WM_CTLCOLORDLG, WM_CTLCOLORLISTBOX,
    // WM_CTLCOLORSCROLLBAR, WM_CTLCOLORSTATIC, WM_CHARTOITEM,  WM_VKEYTOITEM,
    // WM_HSCROLL, WM_VSCROLL, WM_DRAWITEM, WM_MEASUREITEM, WM_DELETEITEM,
    // WM_COMPAREITEM, WM_PARENTNOTIFY.
    inline LRESULT CWnd::OnMessageReflect(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);
        // This function processes those special messages (see above) sent
        // by some older controls, and reflects them back to the originating CWnd object.
        // Override this function in your derived class to handle these special messages.

        // Your overriding function should look like this ...

        // switch (msg)
        // {
        //      Handle your reflected messages here
        // }

        // return 0 for unhandled messages
        return 0;
    }

    // Processes notification (WM_NOTIFY) messages from a child window.
    inline LRESULT CWnd::OnNotify(WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);

        // You can use either OnNotifyReflect or OnNotify to handle notifications
        // Override OnNotifyReflect to handle notifications in the CWnd class that
        //   generated the notification.   OR
        // Override OnNotify to handle notifications in the PARENT of the CWnd class
        //   that generated the notification.

        // Your overriding function should look like this ...

        // switch (((LPNMHDR)lparam)->code)
        // {
        //      Handle your notifications from the CHILD window here
        //      Return the value recommended by the Windows API documentation.
        //      For many notifications, the return value doesn't matter, but for some it does.
        // }

        // return 0 for unhandled notifications
        // The framework will call SetWindowLongPtr(DWLP_MSGRESULT, result) for dialogs
        return 0;
    }

    // Processes the notification (WM_NOTIFY) messages in the child window that originated them.
    inline LRESULT CWnd::OnNotifyReflect(WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);

        // Override OnNotifyReflect to handle notifications in the CWnd class that
        //   generated the notification.

        // Your overriding function should look like this ...

        // switch (((LPNMHDR)lparam)->code)
        // {
        //      Handle your notifications from this window here
        //      Return the value recommended by the Windows API documentation.
        // }

        // return 0 for unhandled notifications
        // The framework will call SetWindowLongPtr(DWLP_MSGRESULT, result) for dialogs
        return 0;
    }

    // Called when the window paints its client area.
    inline LRESULT CWnd::OnPaint(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Window controls and other subclassed windows are expected to do their own
        // drawing, so we don't call OnDraw for those.

        // Note: CustomDraw or OwnerDraw are normally used to modify the drawing of
        //       controls, but overriding OnPaint is also an option.

        if (!m_prevWindowProc)
        {
            if (::GetUpdateRect(*this, NULL, FALSE))
            {
                CPaintDC dc(*this);
                OnDraw(dc);
            }
            else
            // RedrawWindow can require repainting without an update rect
            {
                CClientDC dc(*this);
                OnDraw(dc);
            }

            // No more drawing required
            return 0;
        }

        // Allow window controls to do their default drawing
        return FinalWindowProc(msg, wparam, lparam);
    }

    // Called by CWnd::Create to set some window creation parameters.
    inline void CWnd::PreCreate(CREATESTRUCT& cs)
    {
        UNREFERENCED_PARAMETER(cs);

        // Override this function to set the CREATESTRUCT values prior to window creation.
        // Here we set the initial values for the following:
        //  window styles (WS_VISABLE, WS_CHILD, WS_WS_MAXIMIZEBOX etc.)
        //  window extended styles
        //  window position
        //  window menu
        //  window class name
        //  window name (caption)
    }

    // Called by CWnd::Create to set some window class parameters.
    // Used to set the window type (ClassName) and for setting the background brush and cursor.
    inline void CWnd::PreRegisterClass(WNDCLASS& wc)
    {
        UNREFERENCED_PARAMETER(wc);

        // Override this function to set the WNDCLASS values prior to window creation.
        // for example, for a ToolBar, we use this:
        // wc.lpszClassName = TOOLBARCLASSNAME;

        // ADDITIONAL NOTES:
        // 1) The lpszClassName must be set for this function to take effect.
        // 2) No other defaults are set, so the following settings might prove useful
        //     wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        //     wc.hbrBackground = reinterpret_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
        //     wc.icon = ::LoadIcon(NULL, IDI_APPLICATION);
        // 3) The styles that can be set here are WNDCLASS styles. These are a different
        //     set of styles to those set by CREATESTRUCT (used in PreCreate).
        // 4) RegisterClassEx is not used because its not supported on WinCE.
        //     To set a small icon for the window, use SetIconSmall.
    }

    // Override this function if your class requires input messages to be
    // translated before normal processing. Function which translate messages
    // include TranslateAccelerator, TranslateMDISysAccel and IsDialogMessage.
    // Return TRUE if the message is translated.
    inline BOOL CWnd::PreTranslateMessage(MSG& msg)
    {
        UNREFERENCED_PARAMETER(msg);

        return FALSE;
    }

    // A private function used by the CreateEx function to register a window
    // class prior to window creation.
    inline BOOL CWnd::RegisterClass(WNDCLASS& wc)
    {
        assert( GetApp() );
        assert( ('\0' != wc.lpszClassName[0] && ( lstrlen(wc.lpszClassName) <=  MAX_STRING_SIZE) ) );

        // Check to see if this classname is already registered
        WNDCLASS wcTest;
        ZeroMemory(&wcTest, sizeof(wcTest));
        BOOL done = FALSE;

        if (::GetClassInfo(GetApp()->GetInstanceHandle(), wc.lpszClassName, &wcTest))
        {
            wc = wcTest;
            done = TRUE;
        }

        if (!done)
        {
            // Set defaults
            wc.hInstance    = GetApp()->GetInstanceHandle();
            wc.lpfnWndProc  = CWnd::StaticWindowProc;

            // Register the WNDCLASS structure
            VERIFY ( ::RegisterClass(&wc) != 0 );

            done = TRUE;
        }

        return done;
    }

    // Removes this CWnd's pointer from the application's map.
    inline BOOL CWnd::RemoveFromMap()
    {
        BOOL success = FALSE;

        if ( GetApp() )
        {
            // Allocate an iterator for our HWND map
            std::map<HWND, CWnd*, CompareHWND>::iterator m;

            CWinApp* pApp = GetApp();

            // Erase the CWnd pointer entry from the map
            CThreadLock mapLock(pApp->m_wndLock);
            for (m = pApp->m_mapHWND.begin(); m != pApp->m_mapHWND.end(); ++m)
            {
                if (this == m->second)
                {
                    pApp->m_mapHWND.erase(m);
                    success = TRUE;
                    break;
                }
            }

        }

        return success;
    }

    // Sets the large icon associated with the window.
    inline HICON CWnd::SetIconLarge(int iconID)
    {
        assert( GetApp() );
        assert(IsWindow());

        int cxIcon = ::GetSystemMetrics(SM_CXICON);
        int cyIcon = ::GetSystemMetrics(SM_CYICON);

#ifndef _WIN32_WCE
        HICON hIconLarge = reinterpret_cast<HICON>(GetApp()->LoadImage(iconID, IMAGE_ICON, cxIcon, cyIcon, LR_SHARED));
#else
        HICON hIconLarge = reinterpret_cast<HICON>(GetApp()->LoadImage(iconID, IMAGE_ICON, cxIcon, cyIcon, 0));
#endif

        if (hIconLarge != 0)
            SendMessage (WM_SETICON, WPARAM (ICON_BIG), LPARAM (hIconLarge));
        else
            TRACE("**WARNING** SetIconLarge Failed\n");

        return hIconLarge;
    }

    // Sets the small icon associated with the window.
    inline HICON CWnd::SetIconSmall(int iconID)
    {
        assert( GetApp() );
        assert(IsWindow());

        int cxSmallIcon = ::GetSystemMetrics(SM_CXSMICON);
        int cySmallIcon = ::GetSystemMetrics(SM_CYSMICON);

#ifndef _WIN32_WCE
        HICON hIconSmall = reinterpret_cast<HICON>(GetApp()->LoadImage(iconID, IMAGE_ICON, cxSmallIcon, cySmallIcon, LR_SHARED));
#else
        HICON hIconSmall = reinterpret_cast<HICON>(GetApp()->LoadImage(iconID, IMAGE_ICON, cxSmallIcon, cySmallIcon, 0));
#endif

        if (hIconSmall != 0)
            SendMessage (WM_SETICON, WPARAM (ICON_SMALL), LPARAM (hIconSmall));
        else
            TRACE("**WARNING** SetIconSmall Failed\n");

        return hIconSmall;
    }

    // All CWnd windows direct their messages here. This function redirects the message
    // to the CWnd's WndProc function.
    inline LRESULT CALLBACK CWnd::StaticWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        assert( GetApp() );

        CWnd* w = GetApp()->GetCWndFromMap(wnd);
        if (w == 0)
        {
            // The CWnd pointer wasn't found in the map, so add it now

            // Retrieve the pointer to the TLS Data
            TLSData* pTLSData = GetApp()->GetTlsData();
            assert(pTLSData);

            // Retrieve pointer to CWnd object from Thread Local Storage TLS
            w = pTLSData->pWnd;
            assert(w);              // pTLSData->pCWnd is assigned in CreateEx
            pTLSData->pWnd = NULL;

            // Store the CWnd pointer in the HWND map
            w->m_wnd = wnd;
            w->AddToMap();
        }

        return w->WndProc(msg, wparam, lparam);

    } // LRESULT CALLBACK StaticWindowProc(...)

    // A private function used by CreateEx, Attach and AttachDlgItem.
    inline void CWnd::Subclass(HWND wnd)
    {
        assert(::IsWindow(wnd));

        m_wnd = wnd;
        AddToMap();         // Store the CWnd pointer in the HWND map
        LONG_PTR pWndProc = reinterpret_cast<LONG_PTR>(CWnd::StaticWindowProc);
        LONG_PTR pRes = ::SetWindowLongPtr(wnd, GWLP_WNDPROC, pWndProc);
        m_prevWindowProc = reinterpret_cast<WNDPROC>(pRes);
    }

#ifndef _WIN32_WCE

    //  Dialog Data Exchange support. Call this function to retrieve values from
    //  (retrieveAndValidate is TRUE) or assign values to (retrieveAndValidate
    //  is FALSE) a set of controls appearing in DDX/DDV statements in an
    //  override of the DoDataExchange() member method.
    //
    //  Returns TRUE if the operation is successful, or FALSE otherwise. If
    //  called when retrieveValidate is TRUE, success means the data has
    //  been validated.
    inline BOOL CWnd::UpdateData(CDataExchange& dx, BOOL retrieveAndValidate)
    {
        // must not update data before the window is created
        assert(IsWindow());

        // A critical section ensures threads update the data separately
        CThreadLock lock(GetApp()->m_appLock);

        dx.Init(*this, retrieveAndValidate);

        BOOL ok = FALSE;  // Remains FALSE if DoDataExchange throws a CUserException
        try
        {
            DoDataExchange(dx);
            if (dx.GetLastControl() != NULL && dx.GetLastEditControl() != NULL)
            {
                // select all characters in the edit control
                ::SetFocus(dx.GetLastEditControl());
                ::SendMessage(dx.GetLastEditControl(), EM_SETSEL, 0, -1);
            }
            ok = TRUE; // DoDataExchage completed successfully
        }
        catch(const CUserException& e)
        {
            // Validation has failed. Call the Fail to display the error.
            dx.Fail( e.GetText() );
        }

        return ok;
    }
#endif // _WIN32_WCE


    // Processes this window's message. Override this function in your class
    // derived from CWnd to handle window messages.
    inline LRESULT CWnd::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        //  A typical function might look like this:

        //  switch (msg)
        //  {
        //  case MESSAGE1:  return OnMessage1();
        //  case MESSAGE2:  return OnMessage2();
        //  }

        // The message functions should return a value recommended by the Windows API documentation.
        // Alternatively, return FinalWindowProc to continue with default processing.

        // Always pass unhandled messages on to WndProcDefault
        return WndProcDefault(msg, wparam, lparam);
    }

    // Provides default processing for this window's messages.
    // All WndProc functions should pass unhandled window messages to this function.
    inline LRESULT CWnd::WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        LRESULT result = 0;
        if (UWM_WINDOWCREATED == msg)
        {
            OnInitialUpdate();
            return 0;
        }

        switch (msg)
        {
        case WM_CLOSE:
            {
                OnClose();
                return 0;
            }
        case WM_COMMAND:
            {
                // Reflect this message if it's from a control
                CWnd* pWnd = GetCWndPtr(reinterpret_cast<HWND>(lparam));
                if (pWnd != NULL)
                    result = pWnd->OnCommand(wparam, lparam);

                // Handle user commands
                if (0 == result)
                    result =  OnCommand(wparam, lparam);

                if (0 != result) return 0;
            }
            break;  // Note: Some MDI commands require default processing
        case WM_CREATE:
            {
                LPCREATESTRUCT pcs = (LPCREATESTRUCT) lparam;
                if (pcs == NULL)
                    throw CWinException(_T("WM_CREATE failed"));

                return OnCreate(*pcs);
            }
        case WM_DESTROY:
            OnDestroy();
            break;  // Note: Some controls require default processing.
        case WM_NOTIFY:
            {
                // Do notification reflection if message came from a child window.
                // Restricting OnNotifyReflect to child windows avoids double handling.
                HWND from = ((LPNMHDR)lparam)->hwndFrom;
                CWnd* pWndFrom = GetApp()->GetCWndFromMap(from);

                if (pWndFrom != NULL)
                    if (::GetParent(from) == m_wnd)
                        result = pWndFrom->OnNotifyReflect(wparam, lparam);

                // Handle user notifications
                if (result == 0) result = OnNotify(wparam, lparam);
                if (result != 0) return result;
                break;
            }

        case WM_PAINT:
            {
                // OnPaint calls OnDraw when appropriate
                OnPaint(msg, wparam, lparam);
            }

            return 0;

        case WM_ERASEBKGND:
            {
                CDC dc(reinterpret_cast<HDC>(wparam));
                BOOL preventErasure;

                preventErasure = OnEraseBkgnd(dc);
                if (preventErasure) return TRUE;
            }
            break;

        // A set of messages to be reflected back to the control that generated them
        case WM_CTLCOLORBTN:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
        case WM_DELETEITEM:
        case WM_COMPAREITEM:
        case WM_CHARTOITEM:
        case WM_VKEYTOITEM:
        case WM_HSCROLL:
        case WM_VSCROLL:
        case WM_PARENTNOTIFY:
            {
                result = MessageReflect(msg, wparam, lparam);
                if (result != 0) return result;    // Message processed so return
            }
            break;              // Do default processing when message not already processed

        case UWM_UPDATECOMMAND:
            OnMenuUpdate(static_cast<UINT>(wparam)); // Perform menu updates
            break;

        case UWM_GETCWND:
            {
                assert(this == GetCWndPtr(m_wnd));
                return reinterpret_cast<LRESULT>(this);
            }

        } // switch (msg)

        // Now hand all messages to the default procedure
        return FinalWindowProc(msg, wparam, lparam);

    } // LRESULT CWnd::WindowProc(...)


    //
    // Wrappers for Win32 API functions
    //

    // The BeginPaint function prepares the specified window for painting and fills a PAINTSTRUCT
    // structure with information about the painting. Consider using CPaintDC instead.
    // Refer to BeginPaint in the Windows API documentation for more information.
    inline HDC CWnd::BeginPaint(PAINTSTRUCT& ps) const
    {
        assert(IsWindow());
        return ::BeginPaint(*this, &ps);
    }

    // The BringWindowToTop function brings the specified window to the top
    // of the Z order. If the window is a top-level window, it is activated.
    // Refer to BringWindowToTop in the Windows API documentation for more information.
    inline BOOL CWnd::BringWindowToTop() const
    {
        assert(IsWindow());
        return ::BringWindowToTop(*this);
    }

    // Passes message information to the specified window procedure.
    // Refer to CallWindowProc in the Windows API documentation for more information.
    inline LRESULT CWnd::CallWindowProc(WNDPROC pPrevWndFunc, UINT msg, WPARAM wparam, LPARAM lparam) const
    {
        assert(IsWindow());
        return ::CallWindowProc(pPrevWndFunc, *this, msg, wparam, lparam);
    }

    // The CheckDlgButton function changes the check state of a button control.
    // Refer to CheckDlgButton in the Windows API documentation for more information.
    inline BOOL CWnd::CheckDlgButton(int buttonID, UINT check) const
    {
        assert(IsWindow());
        return ::CheckDlgButton(*this, buttonID, check);
    }

    // The CheckRadioButton function adds a check mark to (checks) a specified radio button in a group
    // and removes a check mark from (clears) all other radio buttons in the group.
    // Refer to CheckRadioButton in the Windows API documentation for more information.
    inline BOOL CWnd::CheckRadioButton(int firstButtonID, int lastButtonID, int checkButtonID) const
    {
        assert(IsWindow());
        return ::CheckRadioButton(*this, firstButtonID, lastButtonID, checkButtonID);
    }

    // Determines which, if any, of the child windows belonging to a parent window contains
    // the specified point. The search is restricted to immediate child windows.
    // Grandchildren, and deeper descendant windows are not searched.
    // Refer to ChildWindowFromPoint in the Windows API documentation for more information.
    inline CWnd CWnd::ChildWindowFromPoint(POINT point) const
    {
        assert(IsWindow());
        return CWnd(::ChildWindowFromPoint(*this, point));
    }

    // The ClientToScreen function converts the client-area coordinates of a specified point to screen coordinates.
    // Refer to ClientToScreen in the Windows API documentation for more information.
    inline BOOL CWnd::ClientToScreen(POINT& point) const
    {
        assert(IsWindow());
        return ::ClientToScreen(*this, &point);
    }

    // The ClientToScreen function converts the client-area coordinates of a specified RECT to screen coordinates.
    // Refer to MapWindowPoints in the Windows API documentation for more information.
    inline BOOL CWnd::ClientToScreen(RECT& rect) const
    {
        assert(IsWindow());
        return (::MapWindowPoints(*this, NULL, (LPPOINT)&rect, 2) != 0);
    }

    // The DeferWindowPos function updates the specified multiple window position structure for the window.
    // The insertAfter can one of:  HWND_BOTTOM, HWND_NOTOPMOST, HWND_TOP, or HWND_TOPMOST.
    // Refer to DeferWindowPos in the Windows API documentation for more information.
    inline HDWP CWnd::DeferWindowPos(HDWP winPosInfo, HWND insertAfter, int x, int y, int cx, int cy, UINT flags) const
    {
        assert(IsWindow());
        return ::DeferWindowPos(winPosInfo, *this, insertAfter, x, y, cx, cy, flags);
    }

    // The DeferWindowPos function updates the specified multiple window position structure for the window.
    // The insertAfter can one of:  HWND_BOTTOM, HWND_NOTOPMOST, HWND_TOP, or HWND_TOPMOST.
    // Refer to DeferWindowPos in the Windows API documentation for more information.
    inline HDWP CWnd::DeferWindowPos(HDWP winPosInfo, HWND insertAfter, const RECT& rect, UINT flags) const
    {
        assert(IsWindow());
        return ::DeferWindowPos(winPosInfo, *this, insertAfter, rect.left, 
            rect.top, rect.right - rect.left, rect.bottom - rect.top, flags);
    }

    // This function provides default processing for any window messages that an application does not process.
    // Refer to DefWindowProc in the Windows API documentation for more information.
    inline LRESULT CWnd::DefWindowProc(UINT msg, WPARAM wparam, LPARAM lparam) const
    {
        assert(IsWindow());
        return ::DefWindowProc(*this, msg, wparam, lparam);
    }

    // The DrawMenuBar function redraws the menu bar of the specified window.
    // If the menu bar changes after the system has created the window, this function
    // must be called to draw the changed menu bar.
    // Refer to DrawMenuBar in the Windows API documentation for more information.
    inline BOOL CWnd::DrawMenuBar() const
    {
        assert(IsWindow());
        return ::DrawMenuBar(*this);
    }

    // The EnableWindow function enables or disables mouse and
    // keyboard input to the window.
    // Refer to EnableWindow in the Windows API documentation for more information.
    inline BOOL CWnd::EnableWindow(BOOL enable /*= TRUE*/) const
    {
        assert(IsWindow());
        return ::EnableWindow(*this, enable);
    }

    // The EndPaint function marks the end of painting in the specified window.
    // This function is required for each call to the BeginPaint function, 
    // but only after painting is complete.
    // Refer to EndPaint in the Windows API documentation for more information.
    inline BOOL CWnd::EndPaint(PAINTSTRUCT& ps) const
    {
        assert(IsWindow());
        return ::EndPaint(*this, &ps);
    }

    // The GetActiveWindow function retrieves the active window attached to the calling
    // thread's message queue.
    // Refer to GetActiveWindow in the Windows API documentation for more information.
    inline CWnd CWnd::GetActiveWindow() const
    {
        return CWnd(::GetActiveWindow() );
    }

    // The GetCapture function retrieves the window (if any) that has captured the mouse.
    // Refer to GetCapture in the Windows API documentation for more information.
    inline CWnd CWnd::GetCapture() const
    {
        return CWnd( ::GetCapture() );
    }

    // The GetClassLongPtr function retrieves the specified value from the
    // WNDCLASSEX structure associated with the window.
    // Possible index values: GCL_CBCLSEXTRA, GCL_CBWNDEXTRA, GCLP_ HBRBACKGROUND, GCLP_HCURSOR,
    // GCLP_HICON, GCLP_HICONSM, GCLP_HMODULE, GCLP_MENUNAME, GCL_STYLE, GCLP_WNDPROC.
    // Refer to GetClassLongPtr in the Windows API documentation for more information.
    inline ULONG_PTR CWnd::GetClassLongPtr(int index) const
    {
        assert(IsWindow());
        return ::GetClassLongPtr(*this, index);
    }

    // The GetClientRect function retrieves the coordinates of a window's client area.
    // The client coordinates specify the upper-left and lower-right corners of the
    // client area. Because client coordinates are relative to the upper-left corner
    // of a window's client area, the coordinates of the upper-left corner are (0,0).
    // Refer to GetClientRect in the Windows API documentation for more information.
    inline CRect CWnd::GetClientRect() const
    {
        assert(IsWindow());
        CRect rc;
        VERIFY(::GetClientRect(*this, &rc) != 0);
        return rc;
    }

    // The GetDC function retrieves the display device context (DC)
    // for the client area of the window. Use like this:
    // CDC dc = GetDC;
    // Refer to GetDC in the Windows API documentation for more information.
    inline CDC CWnd::GetDC() const
    {
        assert(IsWindow());
        return CClientDC(*this);
    }

    // The GetDCEx function retrieves a display device context (DC) for the
    // client area or entire area of a window. Use like this:
    // CDC dc = GetDCEx(clip, flags);
    // Refer to GetDCEx in the Windows API documentation for more information.
    inline CDC CWnd::GetDCEx(HRGN clip, DWORD flags) const
    {
        assert(IsWindow());
        return CClientDCEx(*this, clip, flags);
    }

    // The GetDesktopWindow function retrieves the desktop window.
    // Refer to GetDesktopWindow in the Windows API documentation for more information.
    inline CWnd CWnd::GetDesktopWindow() const
    {
        return CWnd( ::GetDesktopWindow() );
    }

    // Retrieves the control ID value for any child window.
    // Refer to GetDlgCtrlID in the Windows API documentation for more information.
    inline int CWnd::GetDlgCtrlID() const
    {
        assert(IsWindow());
        return ::GetDlgCtrlID(*this);
    }

    // The GetDlgItem function retrieves a handle to a control in the dialog box.
    // Refer to GetDlgItem in the Windows API documentation for more information.
    inline CWnd CWnd::GetDlgItem(int dlgItemID) const
    {
        assert(IsWindow());
        return CWnd( ::GetDlgItem(*this, dlgItemID) );
    }

    // The GetDlgItemInt function translates the text of a specified control in a dialog box into an integer value.
    // Refer to GetDlgItemInt in the Windows API documentation for more information.
    inline UINT CWnd::GetDlgItemInt(int dlgItemID, BOOL& isTranslated, BOOL isSigned) const
    {
        assert(IsWindow());
        return ::GetDlgItemInt(*this, dlgItemID, &isTranslated, isSigned);
    }

    // The GetDlgItemInt function translates the text of a specified control in a dialog box into an integer value.
    // Refer to GetDlgItemInt in the Windows API documentation for more information.
    inline UINT CWnd::GetDlgItemInt(int dlgItemID, BOOL isSigned) const
    {
        assert(IsWindow());
        return ::GetDlgItemInt(*this, dlgItemID, NULL, isSigned);
    }

    // Retrieves the window's extended window style.
    // Refer to GetWindowLongPtr in the Windows API documentation for more information.
    inline DWORD CWnd::GetExStyle() const
    {
        assert(IsWindow());
        return static_cast<DWORD>(GetWindowLongPtr(GWL_EXSTYLE));
    }

    // The GetFocus function retrieves the window that has the keyboard focus, if the window
    // is attached to the calling thread's message queue.
    // Refer to GetFocus in the Windows API documentation for more information.
    inline CWnd CWnd::GetFocus() const
    {
        return CWnd( ::GetFocus() );
    }

    // Retrieves the font with which the window is currently drawing its text.
    // Refer to WM_GETFONT in the Windows API documentation for more information.
    inline CFont CWnd::GetFont() const
    {
        assert(IsWindow());
        HFONT font = (HFONT)SendMessage(WM_GETFONT, 0, 0);
        return CFont(font);
    }

    // Retrieves a handle to the large or small icon associated with a window.
    // Refer to WM_GETICON in the Windows API documentation for more information.
    inline HICON CWnd::GetIcon(BOOL isBigIcon) const
    {
        assert(IsWindow());
        return (HICON)SendMessage(WM_GETICON, (WPARAM)isBigIcon, 0);
    }

    // The GetNextDlgGroupItem function retrieves the first control in a group of controls that
    // precedes (or follows) the specified control in a dialog box.
    // Refer to GetNextDlgGroupItem in the Windows API documentation for more information.
    inline CWnd CWnd::GetNextDlgGroupItem(HWND control, BOOL isPrevious) const
    {
        assert(IsWindow());
        return CWnd(::GetNextDlgGroupItem(*this, control, isPrevious));
    }

    // The GetNextDlgTabItem function retrieves the first control that has the WS_TABSTOP style
    // that precedes (or follows) the specified control.
    // Refer to GetNextDlgTabItem in the Windows API documentation for more information.
    inline CWnd CWnd::GetNextDlgTabItem(HWND control, BOOL isPrevious) const
    {
        assert(IsWindow());
        return CWnd(::GetNextDlgTabItem(*this, control, isPrevious));
    }

    // The GetParent function retrieves the specified window's parent or owner.
    // Refer to GetParent in the Windows API documentation for more information.
    inline CWnd CWnd::GetParent() const
    {
        assert(IsWindow());
        return CWnd(::GetParent(*this));
    }

    // The GetScrollInfo function retrieves the parameters of a scroll bar, including
    // the minimum and maximum scrolling positions, the page size, and the position
    // of the scroll box (thumb).
    // Refer to GetScrollInfo in the Windows API documentation for more information.
    inline BOOL CWnd::GetScrollInfo(int barType, SCROLLINFO& si) const
    {
        assert(IsWindow());
        return ::GetScrollInfo(*this, barType, &si);
    }

    // Retrieves the window's window style.
    // Refer to GetWindowLongPtr in the Windows API documentation for more information.
    inline DWORD CWnd::GetStyle() const
    {
        assert(IsWindow());
        return static_cast<DWORD>(GetWindowLongPtr(GWL_STYLE));
    }

    // The GetUpdateRect function retrieves the coordinates of the smallest rectangle that completely
    // encloses the update region of the specified window.
    // Refer to GetUpdateRect in the Windows API documentation for more information.
    inline CRect CWnd::GetUpdateRect(BOOL erase) const
    {
        assert(IsWindow());
        CRect rc;
        ::GetUpdateRect(*this, &rc, erase);
        return rc;
    }

    // The GetUpdateRgn function retrieves the update region of a window by copying it into the specified region.
    // Refer to GetUpdateRgn in the Windows API documentation for more information.
    inline int CWnd::GetUpdateRgn(HRGN rgn, BOOL erase) const
    {
        assert(IsWindow());
        return ::GetUpdateRgn(*this, rgn, erase);
    }

    // The GetWindow function retrieves a window that has the specified
    // relationship (Z-Order or owner) to the specified window.
    // Possible cmd values: GW_CHILD, GW_ENABLEDPOPUP, GW_HWNDFIRST, GW_HWNDLAST,
    // GW_HWNDNEXT, GW_HWNDPREV, GW_OWNER.
    // Refer to GetWindow in the Windows API documentation for more information.
    inline CWnd CWnd::GetWindow(UINT cmd) const
    {
        assert(IsWindow());
        return CWnd( ::GetWindow(*this, cmd) );
    }

    // The GetWindowDC function retrieves the device context (DC) for the entire window,
    // including title bar, menus, and scroll bars. Use like this:
    // CDC dc = GetWindowDC();
    // Refer to GetWindowDC in the Windows API documentation for more information.
    inline CDC CWnd::GetWindowDC() const
    {
        assert(IsWindow());
        return CWindowDC(*this);
    }

    // The GetWindowLongPtr function retrieves information about the window.
    // Possible index values: GWL_EXSTYLE, GWL_STYLE, GWLP_WNDPROC, GWLP_HINSTANCE
    // GWLP_HWNDPARENT, GWLP_ID, GWLP_USERDATA.
    // Additional index values for dialogs: DWLP_DLGPROC, DWLP_MSGRESULT, DWLP_USER.
    // Refer to GetWindowLongPtr in the Windows API documentation for more information.
    inline LONG_PTR CWnd::GetWindowLongPtr(int index) const
    {
        assert(IsWindow());
        return ::GetWindowLongPtr(*this, index);
    }

    // retrieves the dimensions of the bounding rectangle of the window.
    // The dimensions are given in screen coordinates that are relative to the
    // upper-left corner of the screen.
    // Refer to GetWindowRect in the Windows API documentation for more information.
    inline CRect CWnd::GetWindowRect() const
    {
        assert(IsWindow());
        CRect rc;
        VERIFY(::GetWindowRect(*this, &rc) != 0);
        return rc;
    }

    // The GetWindowTextLength function retrieves the length, in characters, of the specified window's
    // title bar text (if the window has a title bar).
    // Refer to GetWindowTextLength in the Windows API documentation for more information.
    inline int CWnd::GetWindowTextLength() const
    {
        assert(IsWindow());
        return ::GetWindowTextLength(*this);
    }

    // The Invalidate function adds the entire client area to the window's update region.
    // The update region represents the portion of the window's client area that must be redrawn.
    // Refer to InvalidateRect in the Windows API documentation for more information.
    inline void CWnd::Invalidate(BOOL erase /*= TRUE*/) const
    {
        assert(IsWindow());
        ::InvalidateRect(*this, NULL, erase);
    }

    // The InvalidateRect function adds a rectangle to the window's update region.
    // The update region represents the portion of the window's client area that must be redrawn.
    // Refer to InvalidateRect in the Windows API documentation for more information.
    inline BOOL CWnd::InvalidateRect(const RECT& rect, BOOL erase /*= TRUE*/) const
    {
        assert(IsWindow());
        return ::InvalidateRect(*this, &rect, erase);
    }

    // The InvalidateRect function adds a rectangle to the window's update region.
    // The entire window's client area is redrawn when no rectangle is specified.
    // Refer to InvalidateRect in the Windows API documentation for more information.
    inline BOOL CWnd::InvalidateRect(BOOL erase /*= TRUE*/) const
    {
        assert(IsWindow());
        return ::InvalidateRect(*this, NULL, erase);
    }

    // The InvalidateRgn function invalidates the client area within the specified region
    // by adding it to the current update region of a window. The invalidated region,
    // along with all other areas in the update region, is marked for painting when the
    // next WM_PAINT message occurs.
    // Refer to InvalidateRgn in the Windows API documentation for more information.
    inline BOOL CWnd::InvalidateRgn(HRGN rgn, BOOL erase /*= TRUE*/) const
    {
        assert(IsWindow());
        return ::InvalidateRgn(*this, rgn, erase);
    }

    // The IsChild function tests whether a window is a child window or descendant window
    // of a parent window's CWnd.
    // Refer to IsChild in the Windows API documentation for more information.
    inline BOOL CWnd::IsChild(HWND child) const
    {
        assert(IsWindow());
        return ::IsChild(*this, child);
    }

    // The IsDialogMessage function determines whether a message is intended for the specified dialog box and,
    // if it is, processes the message.
    // Refer to IsDialogMessage in the Windows API documentation for more information.
    inline BOOL CWnd::IsDialogMessage(MSG& msg) const
    {
        assert(IsWindow());
        return ::IsDialogMessage(*this, &msg);
    }

    // The IsDlgButtonChecked function determines whether a button control has a check mark next to it
    // or whether a three-state button control is grayed, checked, or neither.
    //
    // The return value from a button created with the BS_AUTOCHECKBOX, BS_AUTORADIOBUTTON, BS_AUTO3STATE, 
    // BS_CHECKBOX, BS_RADIOBUTTON, or BS_3STATE style can be one of the following:
    // BST_CHECKED Button is checked. 
    // BST_INDETERMINATE Button is grayed, indicating an indeterminate state (applies only if the button
    //      has the BS_3STATE or BS_AUTO3STATE style).
    // BST_UNCHECKED Button is cleared.
    //
    // If the button has any other style, the return value is zero.
    // Refer to IsDlgButtonChecked in the Windows API documentation for more information.
    inline UINT CWnd::IsDlgButtonChecked(int buttonID) const
    {
        assert(IsWindow());
        return ::IsDlgButtonChecked(*this, buttonID);
    }

    // The IsWindowEnabled function determines whether the window is enabled
    // for mouse and keyboard input.
    // Refer to IsWindowEnabled in the Windows API documentation for more information.
    inline BOOL CWnd::IsWindowEnabled() const
    {
        assert(IsWindow());
        return ::IsWindowEnabled(*this);
    }

    // The IsWindowVisible function retrieves the visibility state of the window.
    // Refer to IsWindowVisible in the Windows API documentation for more information.
    inline BOOL CWnd::IsWindowVisible() const
    {
        // Microsoft's IsWindowVisible is buggy, so we do it like this
        assert(IsWindow());
        return (GetWindowLongPtr(GWL_STYLE) & WS_VISIBLE) != 0;
    }

    // The IsWindow function determines whether the window exists.
    // Refer to IsWindow in the Windows API documentation for more information.
    inline BOOL CWnd::IsWindow() const
    {
        return ::IsWindow(*this);
    }

    // The MapWindowPoints function converts (maps) a set of points from a coordinate space relative to one
    // window to a coordinate space relative to another window.
    // Refer to MapWindowPoints in the Windows API documentation for more information.
    inline void  CWnd::MapWindowPoints(HWND to, POINT& point) const
    {
        assert(IsWindow());
        ::MapWindowPoints(*this, to, &point, 1);
    }

    // The MapWindowPoints function converts (maps) a set of points from a coordinate space relative to one
    // window to a coordinate space relative to another window.
    // Refer to MapWindowPoints in the Windows API documentation for more information.
    inline void CWnd::MapWindowPoints(HWND to, RECT& rect) const
    {
        assert(IsWindow());
        ::MapWindowPoints(*this, to, (LPPOINT)&rect, 2);
    }

    // The MapWindowPoints function converts (maps) a set of points from a coordinate space relative to one
    // window to a coordinate space relative to another window.
    // Refer to MapWindowPoints in the Windows API documentation for more information.
    inline void CWnd::MapWindowPoints(HWND to, LPPOINT pointsArray, UINT count) const
    {
        assert(IsWindow());
        ::MapWindowPoints(*this, to, (LPPOINT)pointsArray, count);
    }

    // The MessageBox function creates, displays, and operates a message box.
    // Possible combinations of type values include: MB_OK, MB_HELP, MB_OKCANCEL, MB_RETRYCANCEL,
    // MB_YESNO, MB_YESNOCANCEL, MB_ICONEXCLAMATION, MB_ICONWARNING, MB_ICONERROR (+ many others).
    // Refer to MessageBox in the Windows API documentation for more information.
    inline int CWnd::MessageBox(LPCTSTR pText, LPCTSTR pCaption, UINT type) const
    {
        assert(IsWindow());
        return ::MessageBox(*this, pText, pCaption, type);
    }

    // The MoveWindow function changes the position and dimensions of the window.
    // Refer to MoveWindow in the Windows API documentation for more information.
    inline BOOL CWnd::MoveWindow(int x, int y, int width, int height, BOOL repaint /* = TRUE*/) const
    {
        assert(IsWindow());
        return ::MoveWindow(*this, x, y, width, height, repaint = TRUE);
    }

    // The MoveWindow function changes the position and dimensions of the window.
    // Refer to MoveWindow in the Windows API documentation for more information.
    inline BOOL CWnd::MoveWindow(const RECT& rect, BOOL repaint /* = TRUE*/) const
    {
        assert(IsWindow());
        return ::MoveWindow(*this, rect.left, rect.top, rect.right - rect.left,
            rect.bottom - rect.top, repaint);
    }

    // The PostMessage function places (posts) a message in the message queue
    // associated with the thread that created the window and returns without
    // waiting for the thread to process the message.
    // Refer to PostMessage in the Windows API documentation for more information.
    inline BOOL CWnd::PostMessage(UINT msg, WPARAM wparam /*= 0*/, LPARAM lparam /*= 0*/) const
    {
        assert(IsWindow());
        return ::PostMessage(*this, msg, wparam, lparam);
    }

    // The PostMessage function places (posts) a message in the message queue
    // associated with the thread that created the window and returns without
    // waiting for the thread to process the message.
    // Refer to PostMessage in the Windows API documentation for more information.
    inline BOOL CWnd::PostMessage(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) const
    {
        // Required by by some macros
        assert(IsWindow());
        return ::PostMessage(wnd, msg, wparam, lparam);
    }

    // The RedrawWindow function updates the specified rectangle in a window's client area.
    // Refer to RedrawWindow in the Windows API documentation for more information.
    inline BOOL CWnd::RedrawWindow(const RECT& updateRect, UINT flags) const
    {
        assert(IsWindow());
        return ::RedrawWindow(*this, &updateRect, 0, flags);
    }

    // The RedrawWindow function updates the specified region in a window's client area.
    // Refer to RedrawWindow in the Windows API documentation for more information.
    inline BOOL CWnd::RedrawWindow(HRGN rgn, UINT flags) const
    {
        assert(IsWindow());
        return ::RedrawWindow(*this, 0, rgn, flags);
    }

    // The RedrawWindow function updates the entire window's client area.
    // Refer to RedrawWindow in the Windows API documentation for more information.
    inline BOOL CWnd::RedrawWindow(UINT flags) const
    {
        assert(IsWindow());
        return ::RedrawWindow(*this, 0, 0, flags);
    }

    // The ReleaseDC function releases a device context (DC), freeing it for use
    // by other applications.
    // Refer to ReleaseDC in the Windows API documentation for more information.
    inline int CWnd::ReleaseDC(HDC dc) const
    {
        assert(IsWindow());
        return ::ReleaseDC(*this, dc);
    }

    // The ScreenToClient function converts the screen coordinates of
    // a specified point on the screen to client-area coordinates.
    // Refer to ScreenToClient in the Windows API documentation for more information.
    inline BOOL CWnd::ScreenToClient(POINT& point) const
    {
        assert(IsWindow());
        return ::ScreenToClient(*this, &point);
    }

    // The ScreenToClient function converts the screen coordinates of
    // a specified RECT on the screen to client-area coordinates.
    // Refer to ScreenToClient in the Windows API documentation for more information.
    inline BOOL CWnd::ScreenToClient(RECT& rect) const
    {
        assert(IsWindow());
        return (::MapWindowPoints(NULL, *this, (LPPOINT)&rect, 2) != 0);
    }

    // The SendDlgItemMessage function sends a message to the specified control in a dialog box.
    // Refer to SendDlgItemMessage in the Windows API documentation for more information.
    inline LRESULT CWnd::SendDlgItemMessage(int dlgItemID, UINT msg, WPARAM wparam, LPARAM lparam) const
    {
        assert(IsWindow());
        return ::SendDlgItemMessage(*this, dlgItemID, msg, wparam, lparam);
    }

    // The SendMessage function sends the specified message to a window or windows.
    // It calls the window procedure for the window and does not return until the
    // window procedure has processed the message.
    // Refer to SendMessage in the Windows API documentation for more information.
    inline LRESULT CWnd::SendMessage(UINT msg, WPARAM wparam /*= 0*/, LPARAM lparam /*= 0*/) const
    {
        assert(IsWindow());
        return ::SendMessage(*this, msg, wparam, lparam);
    }

    // The SendMessage function sends the specified message to a window or windows.
    // It calls the window procedure for the window and does not return until the
    // window procedure has processed the message.
    // Refer to SendMessage in the Windows API documentation for more information.
    inline LRESULT CWnd::SendMessage(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) const
    {
        // Required by by some macros
        assert(IsWindow());
        return ::SendMessage(wnd, msg, wparam, lparam);
    }

    // The SendNotifyMessage function sends the specified message to a window or windows. If the window was created by the
    // calling thread, SendNotifyMessage calls the window procedure for the window and does not return until the window procedure
    // has processed the message. If the window was created by a different thread, SendNotifyMessage passes the message to the
    // window procedure and returns immediately; it does not wait for the window procedure to finish processing the message.
    // Refer to SendNotifyMessage in the Windows API documentation for more information.
    inline BOOL CWnd::SendNotifyMessage(UINT msg, WPARAM wparam, LPARAM lparam) const
    {
        assert(IsWindow());
        return ::SendNotifyMessage(*this, msg, wparam, lparam);
    }

    // The SetActiveWindow function activates the window, but
    // not if the application is in the background.
    // Refer to SetActiveWindow in the Windows API documentation for more information.
    inline HWND CWnd::SetActiveWindow() const
    {
        assert(IsWindow());
        return ::SetActiveWindow(*this);
    }

    // The SetCapture function sets the mouse capture to the window.
    // SetCapture captures mouse input either when the mouse is over the capturing
    // window, or when the mouse button was pressed while the mouse was over the
    // capturing window and the button is still down.
    // Refer to SetCapture in the Windows API documentation for more information.
    inline HWND CWnd::SetCapture() const
    {
        assert(IsWindow());
        return ::SetCapture(*this);
    }

    // The SetClassLongPtr function replaces the specified value at the specified offset in the
    // extra class memory or the WNDCLASSEX structure for the class to which the window belongs.
    // Possible index values: GCL_CBCLSEXTRA, GCL_CBWNDEXTRA, GCLP_ HBRBACKGROUND, GCLP_HCURSOR,
    // GCLP_HICON, GCLP_HICONSM, GCLP_HMODULE, GCLP_MENUNAME, GCL_STYLE, GCLP_WNDPROC.
    // Refer to SetClassLongPtr in the Windows API documentation for more information.
    inline ULONG_PTR CWnd::SetClassLongPtr(int index, LONG_PTR newLong) const
    {
        assert(IsWindow());
        return ::SetClassLongPtr(*this, index, newLong);
    }

    // Assigns an id to the window. Note that only child windows can have an ID assigned.
    // Refer to SetClassLongPtr in the Windows API documentation for more information.
    inline LONG_PTR CWnd::SetDlgCtrlID(int id) const
    {
        assert(IsWindow());
        return SetWindowLongPtr(GWLP_ID, id);
    }

    // The SetDlgItemInt function sets the text of a control in a dialog box to the string representation of a specified integer value.
    // Refer to SetDlgItemInt in the Windows API documentation for more information.
    inline BOOL CWnd::SetDlgItemInt(int dlgItemID, UINT value, BOOL isSigned) const
    {
        assert(IsWindow());
        return ::SetDlgItemInt(*this, dlgItemID, value, isSigned);
    }

    // The SetDlgItemText function sets the title or text of a control in a dialog box.
    // Refer to SetDlgItemText in the Windows API documentation for more information.
    inline BOOL CWnd::SetDlgItemText(int dlgItemID, LPCTSTR pString) const
    {
        assert(IsWindow());
        return ::SetDlgItemText(*this, dlgItemID, pString);
    }

    // Assigns a new windows extended style to the window.
    // Refer to SetWindowLongPtr in the Windows API documentation for more information.
    inline void CWnd::SetExStyle(DWORD exStyle) const
    {
        assert(IsWindow());
        ::SetWindowLongPtr(*this, GWL_EXSTYLE, exStyle);
    }

    // The SetFocus function sets the keyboard focus to the window.
    // Refer to SetFocus in the Windows API documentation for more information.
    inline HWND CWnd::SetFocus() const
    {
        assert(IsWindow());
        return ::SetFocus(*this);
    }

    // Specifies the font that the window will use when drawing text.
    // Refer to WM_SETFONT in the Windows API documentation for more information.
    inline void CWnd::SetFont(HFONT font, BOOL redraw /* = TRUE*/) const
    {
        assert(IsWindow());
        SendMessage(WM_SETFONT, (WPARAM)font, (LPARAM)redraw);
    }

    // Associates a new large or small icon with a window.
    // Refer to WM_SETICON in the Windows API documentation for more information.
    inline HICON CWnd::SetIcon(HICON icon, BOOL isBigIcon) const
    {
        assert(IsWindow());
        return (HICON)SendMessage(WM_SETICON, (WPARAM)isBigIcon, (LPARAM)icon);
    }

    // The SetForegroundWindow function puts the thread that created the window into the
    // foreground and activates the window.
    // Refer to SetForegroundWindow in the Windows API documentation for more information.
    inline BOOL CWnd::SetForegroundWindow() const
    {
        assert(IsWindow());
        return ::SetForegroundWindow(*this);
    }

    // The SetParent function changes the parent window of the child window.
    // Refer to SetParent in the Windows API documentation for more information.
    inline HWND CWnd::SetParent(HWND parent) const
    {
        assert(IsWindow());
        return ::SetParent(*this, parent);
    }

    // This function allows changes in that window to be redrawn or prevents changes
    // in that window from being redrawn.
    // Refer to WM_SETREDRAW in the Windows API documentation for more information.
    inline BOOL CWnd::SetRedraw(BOOL redraw /*= TRUE*/) const
    {
        assert(IsWindow());
        return (::SendMessage(*this, WM_SETREDRAW, (WPARAM)redraw, 0) != 0);
    }

    // Assigns a new windows style to the window.
    // Refer to SetWindowLongPtr in the Windows API documentation for more information.
    inline void CWnd::SetStyle(DWORD style) const
    {
        assert(IsWindow());
        ::SetWindowLongPtr(*this, GWL_STYLE, style);
    }

    // Creates a timer with the specified time-out value.
    // Refer to SetTimer in the Windows API documentation for more information.
    inline UINT_PTR CWnd::SetTimer(UINT_PTR eventID, UINT elapse, TIMERPROC pTimerFunc) const
    {
        assert(IsWindow());
        return ::SetTimer(*this, eventID, elapse, pTimerFunc);
    }

    // The SetWindowLongPtr function changes an attribute of the window.
    // Possible index values: GWL_EXSTYLE, GWL_STYLE, GWLP_WNDPROC, GWLP_HINSTANCE
    // GWLP_HWNDPARENT, GWLP_ID, GWLP_USERDATA.
    // Additional index values for dialogs: DWLP_DLGPROC, DWLP_MSGRESULT, DWLP_USER.
    // Refer to SetWindowLongPtr in the Windows API documentation for more information.
    inline LONG_PTR CWnd::SetWindowLongPtr(int index, LONG_PTR newLong) const
    {
        assert(IsWindow());
        return ::SetWindowLongPtr(*this, index, newLong);
    }

    // The SetWindowPos function changes the size, position, and Z order of a child, pop-up,
    // or top-level window.
    // The insertAfter can one of:  HWND_TOP, HWND_TOPMOST, HWND_BOTTOM, or HWND_NOTOPMOST.
    // Refer to SetWindowPos in the Windows API documentation for more information.
    inline BOOL CWnd::SetWindowPos(HWND insertAfter, int x, int y, int cx, int cy, UINT flags) const
    {
        assert(IsWindow());
        return ::SetWindowPos(*this, insertAfter, x, y, cx, cy, flags);
    }

    // The SetWindowPos function changes the size, position, and Z order of a child, pop-up,
    // or top-level window.
    // The insertAfter can one of:  HWND_TOP, HWND_TOPMOST, HWND_BOTTOM, or HWND_NOTOPMOST.
    // Refer to SetWindowPos in the Windows API documentation for more information.
    inline BOOL CWnd::SetWindowPos(HWND insertAfter, const RECT& rect, UINT flags) const
    {
        assert(IsWindow());
        return ::SetWindowPos(*this, insertAfter, rect.left, rect.top, rect.right - rect.left,
            rect.bottom - rect.top, flags);
    }

    // The SetWindowRgn function sets the window region of the window.
    // The window region determines the area within the window where the system permits drawing.
    // The window now owns the region so it is detached from Rgn.
    // Refer to SetWindowRgn in the Windows API documentation for more information.
    inline int CWnd::SetWindowRgn(CRgn& rgn, BOOL redraw /*= TRUE*/) const
    {
        assert(IsWindow());
        HRGN rgnHandle = reinterpret_cast<HRGN>(rgn.GetHandle());
        int iResult = ::SetWindowRgn(*this, rgnHandle, redraw);
        if (iResult && rgnHandle)
            rgn.Detach();   // The system owns the region now

        return iResult;
    }

    // The SetWindowText function changes the text of the window's title bar (if it has one).
    // Refer to SetWindowText in the Windows API documentation for more information.
    inline BOOL CWnd::SetWindowText(LPCTSTR pString) const
    {
        assert(IsWindow());
        return ::SetWindowText(*this, pString);
    }

    // Set the XP Theme for a window.
    // Examples:
    //  SetWindowTheme(NULL, NULL);     // Reverts the window's XP theme back to default.
    //  SetWindowTheme(L" ", L" ");     // Disables XP theme for the window.
    // Refer to SetWindowTheme in the Windows API documentation for more information.
    inline HRESULT CWnd::SetWindowTheme(LPCWSTR pSubAppName, LPCWSTR pSubIdList) const
    {
        HRESULT result = E_NOTIMPL;

#ifndef _WIN32_WCE

        HMODULE theme = ::LoadLibrary(_T("uxtheme.dll"));
        if (theme != 0)
        {
            typedef HRESULT (__stdcall *PFNSETWINDOWTHEME)(HWND wnd, LPCWSTR pSubAppName, LPCWSTR pSubIdList);
            PFNSETWINDOWTHEME pfn = (PFNSETWINDOWTHEME)GetProcAddress(theme, "SetWindowTheme");

            result = pfn(*this, pSubAppName, pSubIdList);

            ::FreeLibrary(theme);
        }

#endif

        return result;
    }

    // The ShowWindow function sets the window's show state.
    // Refer to ShowWindow in the Windows API documentation for more information.
    inline BOOL CWnd::ShowWindow(int showCmd /*= SW_SHOWNORMAL*/) const
    {
        assert(IsWindow());
        return ::ShowWindow(*this, showCmd);
    }

    // The UpdateWindow function updates the client area of the window by sending a
    // WM_PAINT message to the window if the window's update region is not empty.
    // If the update region is empty, no message is sent.
    // Refer to UpdateWindow in the Windows API documentation for more information.
    inline BOOL CWnd::UpdateWindow() const
    {
        assert(IsWindow());
        return ::UpdateWindow(*this);
    }

    // The ValidateRect function validates the client area within a rectangle by
    // removing the rectangle from the update region of the window.
    // Refer to ValidateRect in the Windows API documentation for more information.
    inline BOOL CWnd::ValidateRect(const RECT& rect) const
    {
        assert(IsWindow());
        return ::ValidateRect(*this, &rect);
    }

    // The ValidateRect function validates the entire client area of the window.
    // Refer to ValidateRect in the Windows API documentation for more information.
    inline BOOL CWnd::ValidateRect() const
    {
        assert(IsWindow());
        return ::ValidateRect(*this, NULL);
    }

    // The ValidateRgn function validates the client area within a region by
    // removing the region from the current update region of the window.
    // Refer to ValidateRgn in the Windows API documentation for more information.
    inline BOOL CWnd::ValidateRgn(HRGN rgn) const
    {
        assert(IsWindow());
        return ::ValidateRgn(*this, rgn);
    }

    // Retrieves the window that contains the specified point (in screen coordinates).
    // Refer to WindowFromPoint in the Windows API documentation for more information.
    inline CWnd CWnd::WindowFromPoint(POINT point)
    {
        return CWnd(::WindowFromPoint(point));
    }

    //
    // These functions aren't supported on WinCE
    //

  #ifndef _WIN32_WCE

    // The CloseWindow function minimizes (but does not destroy) the window.
    // To destroy a window, an application can use the Destroy function.
    // Refer to CloseWindow in the Windows API documentation for more information.
    inline BOOL CWnd::CloseWindow() const
    {
        assert(IsWindow());
        return ::CloseWindow(*this);
    }

    // The DlgDirList function replaces the contents of a list box with the names of the subdirectories and files
    // in a specified directory. You can filter the list of names by specifying a set of file attributes.
    // Refer to DlgDirList in the Windows API documentation for more information.
    inline int CWnd::DlgDirList(LPTSTR pPathSpec, int listBoxID, int staticPathID, UINT fileType) const
    {
        assert(IsWindow());
        return ::DlgDirList(*this, pPathSpec, listBoxID, staticPathID, fileType);
    }

    // The DlgDirListComboBox function replaces the contents of a combo box with the names of the subdirectories
    // and files in a specified directory. You can filter the list of names by specifying a set of file attributes.
    // in a specified directory. You can filter the list of names by specifying a set of file attributes.
    // Refer to DlgDirListComboBox in the Windows API documentation for more information.
    inline int CWnd::DlgDirListComboBox(LPTSTR pPathSpec, int comboBoxID, int staticPathID, UINT fileType) const
    {
        assert(IsWindow());
        return ::DlgDirListComboBox(*this, pPathSpec, comboBoxID, staticPathID, fileType);
    }

    // The DlgDirSelectEx function retrieves the current selection from a single-selection list box. It assumes that the list box
    // has been filled by the DlgDirList function and that the selection is a drive letter, filename, or directory name.
    // Refer to DlgDirSelectEx in the Windows API documentation for more information.
    inline BOOL CWnd::DlgDirSelectEx(LPTSTR pString, int count, int listBoxID) const
    {
        assert(IsWindow());
        return ::DlgDirSelectEx(*this, pString, count, listBoxID);
    }

    // The DlgDirSelectComboBoxEx function retrieves the current selection from a combo box filled by using the
    // DlgDirListComboBox function. The selection is interpreted as a drive letter, a file, or a directory name.
    // Refer to DlgDirSelectComboBoxEx in the Windows API documentation for more information.
    inline BOOL CWnd::DlgDirSelectComboBoxEx(LPTSTR pString, int count, int comboBoxID) const
    {
        assert(IsWindow());
        return ::DlgDirSelectComboBoxEx(*this, pString, count, comboBoxID);
    }

    #ifndef WIN32_LEAN_AND_MEAN

    // Registers whether a window accepts dropped files.
    // Refer to DragAcceptFiles in the Windows API documentation for more information.
    inline void CWnd::DragAcceptFiles(BOOL accept) const
    {
        assert(IsWindow());
        ::DragAcceptFiles(*this, accept);
    }

    #endif

    // The DrawAnimatedRects function draws a wire-frame rectangle and animates it to indicate the opening of
    // an icon or the minimizing or maximizing of a window.
    // Refer to DrawAnimatedRects in the Windows API documentation for more information.
    inline BOOL CWnd::DrawAnimatedRects(int aniID, const RECT& from, const RECT& to) const
    {
        assert(IsWindow());
        return ::DrawAnimatedRects(*this, aniID, &from, &to);
    }

    // The DrawCaption function draws a window caption.
    // Refer to DrawCaption in the Windows API documentation for more information.
    inline BOOL CWnd::DrawCaption(HDC dc, const RECT& rect, UINT flags) const
    {
        assert(IsWindow());
        return ::DrawCaption(*this, dc, &rect, flags);
    }

    // The EnableScrollBar function enables or disables one or both scroll bar arrows.
    // Refer to EnableScrollBar in the Windows API documentation for more information.
    inline BOOL CWnd::EnableScrollBar(UINT flags, UINT arrows) const
    {
        assert(IsWindow());
        return ::EnableScrollBar(*this, flags, arrows);
    }

    // The GetLastActivePopup function determines which pop-up window owned by the specified window was most recently active.
    // Refer to GetLastActivePopup in the Windows API documentation for more information.
    inline CWnd CWnd::GetLastActivePopup() const
    {
        assert(IsWindow());
        return CWnd( ::GetLastActivePopup(*this) );
    }

    // The GetMenu function retrieves a handle to the menu assigned to the window.
    // Refer to GetMenu in the Windows API documentation for more information.
    inline CMenu CWnd::GetMenu() const
    {
        assert(IsWindow());
        return CMenu(::GetMenu(*this));
    }

    // The GetScrollPos function retrieves the current position of the scroll box
    // (thumb) in the specified scroll bar.
    // Refer to GetScrollPos in the Windows API documentation for more information.
    inline int CWnd::GetScrollPos(int bar) const
    {
        assert(IsWindow());
        return ::GetScrollPos(*this, bar);
    }

    // The GetScrollRange function retrieves the current minimum and maximum scroll box
    // (thumb) positions for the specified scroll bar.
    // Refer to GetScrollRange in the Windows API documentation for more information.
    inline BOOL CWnd::GetScrollRange(int bar, int& minPos, int& maxPos) const
    {
        assert(IsWindow());
        return ::GetScrollRange(*this, bar, &minPos, &maxPos );
    }

    // The GetSystemMenu function allows the application to access the window menu (also known as the system menu
    // or the control menu) for copying and modifying.
    // Refer to GetSystemMenu in the Windows API documentation for more information.
    inline CMenu CWnd::GetSystemMenu(BOOL revertToDefault) const
    {
        assert(IsWindow());
        return CMenu( ::GetSystemMenu(*this, revertToDefault) );
    }

    // The GetTopWindow function examines the Z order of the child windows associated with the parent window and
    // retrieves a handle to the child window at the top of the Z order.
    // Refer to GetTopWindow in the Windows API documentation for more information.
    inline CWnd CWnd::GetTopWindow() const
    {
        assert(IsWindow());
        return CWnd( ::GetTopWindow(*this) );
    }

    // The GetWindowPlacement function retrieves the show state and the restored,
    // minimized, and maximized positions of the window.
    // Refer to GetWindowPlacement in the Windows API documentation for more information.
    inline BOOL CWnd::GetWindowPlacement(WINDOWPLACEMENT& wp) const
    {
        assert(IsWindow());
        return ::GetWindowPlacement(*this, &wp);
    }

    // The HiliteMenuItem function highlights or removes the highlighting from an item in a menu bar.
    // Refer to HiliteMenuItem in the Windows API documentation for more information.
    inline BOOL CWnd::HiliteMenuItem(HMENU menu, UINT itemID, UINT hilite) const
    {
        assert(IsWindow());
        return ::HiliteMenuItem(*this, menu, itemID, hilite);
    }

    // The IsIconic function determines whether the window is minimized (iconic).
    // Refer to IsIconic in the Windows API documentation for more information.
    inline BOOL CWnd::IsIconic() const
    {
        assert(IsWindow());
        return ::IsIconic(*this);
    }

    // The IsZoomed function determines whether the window is maximized.
    // Refer to IsZoomed in the Windows API documentation for more information.
    inline BOOL CWnd::IsZoomed() const
    {
        assert(IsWindow());
        return ::IsZoomed(*this);
    }

    // Destroys the specified timer.
    // Refer to KillTimer in the Windows API documentation for more information.
    inline BOOL CWnd::KillTimer(UINT_PTR eventID) const
    {
        assert(IsWindow());
        return ::KillTimer(*this, eventID);
    }

    // Disables drawing in the window. Only one window can be locked at a time.
    // Use UnLockWindowUpdate to re-enable drawing in the window.
    // Refer to LockWindowUpdate in the Windows API documentation for more information.
    inline BOOL CWnd::LockWindowUpdate() const
    {
        assert(IsWindow());
        return ::LockWindowUpdate(*this);
    }

    // The OpenIcon function restores a minimized (iconic) window to its previous size and position.
    // Refer to OpenIcon in the Windows API documentation for more information.
    inline BOOL CWnd::OpenIcon() const
    {
        assert(IsWindow());
        return ::OpenIcon(*this);
    }

    // Requests that the window draw itself in the specified device context, most commonly in a printer device context.
    // Refer to WM_PRINT in the Windows API documentation for more information.
    inline void CWnd::Print(HDC dc, DWORD flags) const
    {
        assert(IsWindow());
        SendMessage(*this, WM_PRINT, (WPARAM)dc, (LPARAM)flags);
    }

    // The ScrollWindow function scrolls the contents of the window's client area.
    // scrollRect specifies the portion of the client area to be scrolled.
    // pClipRect points to the clipping rectangle to scroll. Only bits inside this rectangle are scrolled.
    // If prcClip is NULL, no clipping is performed on the scroll rectangle.
    // Refer to ScrollWindow in the Windows API documentation for more information.
    inline BOOL CWnd::ScrollWindow(int xAmount, int yAmount, const RECT& scrollRect, LPCRECT pClipRect) const
    {
        assert(IsWindow());
        return ::ScrollWindow(*this, xAmount, yAmount, &scrollRect, pClipRect);
    }

    // The ScrollWindow function scrolls the contents of the window's client area.
    // The entire client area is scrolled.
    // pClipRect points to the clipping rectangle to scroll. Only bits inside this rectangle are scrolled.
    // If pClipRect is NULL, no clipping is performed on the scroll rectangle.
    // Refer to ScrollWindow in the Windows API documentation for more information.
    inline BOOL CWnd::ScrollWindow(int xAmount, int yAmount, LPCRECT pClipRect) const
    {
        assert(IsWindow());
        return ::ScrollWindow(*this, xAmount, yAmount, NULL, pClipRect);
    }

    // The ScrollWindow function scrolls the contents of the window's client area.
    // pScrollRect: Pointer to a RECT structure that specifies the portion of the client area to be scrolled.
    //              If this parameter is NULL, the entire client area is scrolled.
    // pClipRect:   Pointer to a RECT structure that contains the coordinates of the clipping rectangle.
    //              Only device bits within the clipping rectangle are affected. This parameter may be NULL.
    // update:      Handle to the region that is modified to hold the region invalidated by scrolling.
    //              This parameter may be NULL.
    // pUpdateRect: Pointer to a RECT structure that receives the boundaries of the rectangle invalidated by scrolling.
    //              This parameter may be NULL.
    // flags:       Specifies flags that control scrolling.This parameter can be one of the following values.
    //   SW_ERASE:          Erases the newly invalidated region when specified with the SW_INVALIDATE flag.
    //   SW_INVALIDATE:     Invalidates the region identified by the hrgnUpdate parameter after scrolling.
    //   SW_SCROLLCHILDREN: Scrolls all child windows that intersect the rectangle pointed to by the prcScroll parameter.
    //   SW_SMOOTHSCROLL:   Scrolls using smooth scrolling.
    // Refer to ScrollWindowEx in the Windows API documentation for more information.
    inline int CWnd::ScrollWindowEx(int dx, int dy, LPCRECT pScrollRect, LPCRECT pClipRect, HRGN update, LPRECT pUpdateRect, UINT flags) const
    {
        assert(IsWindow());
        return ::ScrollWindowEx(*this, dx, dy, pScrollRect, pClipRect, update, pUpdateRect, flags);
    }

    // The SetMenu function assigns a menu to the specified window.
    // A menu of NULL removes the menu.
    // Refer to SetMenu in the Windows API documentation for more information.
    inline BOOL CWnd::SetMenu(HMENU menu) const
    {
        assert(IsWindow());
        return ::SetMenu(*this, menu);
    }

    // The SetScrollInfo function sets the parameters of a scroll bar, including
    // the minimum and maximum scrolling positions, the page size, and the
    // position of the scroll box (thumb).
    // Refer to SetScrollInfo in the Windows API documentation for more information.
    inline int CWnd::SetScrollInfo(int barType, const SCROLLINFO& si, BOOL redraw) const
    {
        assert(IsWindow());
        return ::SetScrollInfo(*this, barType, &si, redraw);
    }

    // The SetScrollPos function sets the position of the scroll box (thumb) in
    // the specified scroll bar.
    // Refer to SetScrollPos in the Windows API documentation for more information.
    inline int CWnd::SetScrollPos(int barType, int pos, BOOL redraw) const
    {
        assert(IsWindow());
        return ::SetScrollPos(*this, barType, pos, redraw);
    }

    // The SetScrollRange function sets the minimum and maximum scroll box positions for the scroll bar.
    // Refer to SetScrollRange in the Windows API documentation for more information.
    inline BOOL CWnd::SetScrollRange(int barType, int minPos, int maxPos, BOOL redraw) const
    {
        assert(IsWindow());
        return ::SetScrollRange(*this, barType, minPos, maxPos, redraw);
    }

    // The SetWindowPlacement function sets the show state and the restored, minimized,
    // and maximized positions of the window.
    // Refer to SetWindowPlacement in the Windows API documentation for more information.
    inline BOOL CWnd::SetWindowPlacement(const WINDOWPLACEMENT& wndpl) const
    {
        assert(IsWindow());
        return ::SetWindowPlacement(*this, &wndpl);
    }

    // The ShowOwnedPopups function shows or hides all pop-up windows owned by the specified window.
    // Refer to ShowOwnedPopups in the Windows API documentation for more information.
    inline BOOL CWnd::ShowOwnedPopups(BOOL show) const
    {
        assert(IsWindow());
        return ::ShowOwnedPopups(*this, show);
    }

    // The ShowScrollBar function shows or hides the specified scroll bar.
    // Refer to ShowScrollBar in the Windows API documentation for more information.
    inline BOOL CWnd::ShowScrollBar(int bar, BOOL show) const
    {
        assert(IsWindow());
        return ::ShowScrollBar(*this, bar, show);
    }

    // The ShowWindowAsync function sets the show state of a window created by a different thread.
    // Refer to ShowWindowAsync in the Windows API documentation for more information.
    inline BOOL CWnd::ShowWindowAsync(int showCmd) const
    {
        assert(IsWindow());
        return ::ShowWindowAsync(*this, showCmd);
    }

    // Enables drawing in the window. Only one window can be locked at a time.
    // Use LockWindowUpdate to disable drawing in the window.
    // Refer to LockWindowUpdate in the Windows API documentation for more information.
    inline BOOL CWnd::UnLockWindowUpdate() const
    {
        assert(IsWindow());
        return ::LockWindowUpdate(0);
    }

    // The WindowFromDC function returns a handle to the window associated with the specified display device context (DC).
    // Refer to WindowFromDC in the Windows API documentation for more information.
    inline CWnd CWnd::WindowFromDC(HDC dc) const
    {
        return CWnd( ::WindowFromDC(dc) );
    }

  #endif    // _WIN32_WCE



    /////////////////////////////////////////////////////////
    // Definitions of CString functions that require CWinApp
    //

    // Appends formatted data to the CStringT content.
    template <class T>
    inline void CStringT<T>::AppendFormat(UINT formatID, ...)
    {
        CStringT str1;
        CStringT str2;

        if (str1.LoadString(formatID))
        {
            va_list args;
            va_start(args, formatID);
            str2.FormatV(str1.c_str(), args);
            va_end(args);

            m_str.append(str2);
        }
    }


    // Formats the string as sprintf does.
    template <class T>
    inline void CStringT<T>::Format(UINT id, ...)
    {
        CStringT str;
        if (str.LoadString(id))
        {
            va_list args;
            va_start(args, id);
            FormatV(str.c_str(), args);
            va_end(args);
        }
    }


    // Loads the string from a Windows resource.
    template <>
    inline bool CStringT<CHAR>::LoadString(UINT id)
    {
        assert (GetApp());

        int startSize = 64;
        CHAR* pTCharArray = 0;
        std::vector<CHAR> vString;
        int chars = startSize;

        Empty();

        // Increase the size of our array in a loop until we load the entire string
        // The ANSI and _UNICODE versions of LoadString behave differently. This technique works for both.
        while (startSize -1 <= chars )
        {
            startSize = startSize * 4;
            vString.assign(size_t(startSize)+1, 0);
            pTCharArray = &vString[0];
            chars = ::LoadStringA (GetApp()->GetResourceHandle(), id, pTCharArray, startSize);
        }

        if (chars > 0)
            m_str.assign(pTCharArray);

        return (chars != 0);
    }


    // Loads the string from a Windows resource.
    // Refer to LoadString in the Windows API documentation for more information.
    template <>
    inline bool CStringT<WCHAR>::LoadString(UINT id)
    {
        assert (GetApp());

        int startSize = 64;
        WCHAR* pTCharArray = 0;
        std::vector<WCHAR> vString;
        int chars = startSize;

        Empty();

        // Increase the size of our array in a loop until we load the entire string
        // The ANSI and _UNICODE versions of LoadString behave differently. This technique works for both.
        while (startSize -1 <= chars )
        {
            startSize = startSize * 4;
            vString.assign(size_t(startSize)+1, 0);
            pTCharArray = &vString[0];
            chars = ::LoadStringW (GetApp()->GetResourceHandle(), id, pTCharArray, startSize);
        }

        if (chars > 0)
            m_str.assign(pTCharArray);

        return (chars != 0);
    }

    ////////////////////////////////////////
    // Global Functions
    //


    // Returns the path to the AppData folder. Returns an empty CString if
    // the Operating System doesn't support the use of an AppData folder.
    // The AppData folder is available in Windows 2000 and above.
    inline CString GetAppDataPath()
    {
        CString AppData;

#ifndef _WIN32_WCE

        HMODULE hShell = ::LoadLibrary(_T("Shell32.dll"));
        if (hShell)
        {
            typedef HRESULT(WINAPI * MYPROC)(HWND, int, HANDLE, DWORD, LPTSTR);

            // Get the function pointer of the SHGetFolderPath function
#ifdef UNICODE
            MYPROC pSHGetFolderPath = (MYPROC)GetProcAddress(hShell, "SHGetFolderPathW");
#else
            MYPROC pSHGetFolderPath = (MYPROC)GetProcAddress(hShell, "SHGetFolderPathA");
#endif

#ifndef CSIDL_APPDATA
  #define CSIDL_APPDATA     0x001a
  #define CSIDL_PERSONAL    0x0005 /* My Documents */
#endif

#ifndef CSIDL_FLAG_CREATE
  #define CSIDL_FLAG_CREATE 0x8000
#endif

            if (pSHGetFolderPath)
            {
                // Call the SHGetFolderPath function to retrieve the AppData folder
                pSHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, AppData.GetBuffer(MAX_PATH));
                AppData.ReleaseBuffer();
            }

            // If we can't get the AppData folder, get the MyDocuments folder instead
            if (AppData.IsEmpty())
            {
                typedef HRESULT(WINAPI * GETSPECIALPATH)(HWND, LPTSTR, int, BOOL);

#ifdef UNICODE
                GETSPECIALPATH pGetSpecialPath = (GETSPECIALPATH)GetProcAddress(hShell, "SHGetSpecialFolderPathW");
#else
                GETSPECIALPATH pGetSpecialPath = (GETSPECIALPATH)GetProcAddress(hShell, "SHGetSpecialFolderPathA");
#endif

                if (pGetSpecialPath)
                {
                    // Call the SHGetSpecialFolderPath function to retrieve the MyDocuments folder
                    pGetSpecialPath(NULL, AppData.GetBuffer(MAX_PATH), CSIDL_PERSONAL, TRUE);
                    AppData.ReleaseBuffer();
                }
            }

            ::FreeLibrary(hShell);
        }

#endif // _WIN32_WCE

        return AppData;
    }

    // Retrieves the command line arguments and stores them in a vector of CString.
    // Similar to CommandLineToArgvW, but supports all versions of Windows,
    // supports ANSI and Unicode, and doesn't require the user to use LocalFree.
    inline std::vector<CString> GetCommandLineArgs()
    {
        std::vector<CString> CommandLineArgs;
        CString CommandLine = GetCommandLine();
        int index = 0;
        int endPos = 0;

        while (index < CommandLine.GetLength())
        {
            // Is the argument quoted?
            bool IsQuoted = (CommandLine[index] == _T('\"'));

            if (IsQuoted)
            {
                // Find the terminating token (quote followed by space)
                endPos = CommandLine.Find(_T("\" "), index);
                if (endPos == -1) endPos = CommandLine.GetLength() - 1;

                // Store the argument in the CStringT vector without the quotes.
                CString s;
                if (endPos - index < 2)
                    s = _T("\"\"");     // "" for a single quote or double quote argument
                else
                    s = CommandLine.Mid(index + 1, endPos - index - 1);

                CommandLineArgs.push_back(s);
                index = endPos + 2;
            }
            else
            {
                // Find the terminating token (space character)
                endPos = CommandLine.Find(_T(' '), index);
                if (endPos == -1) endPos = CommandLine.GetLength();

                // Store the argument in the CStringT vector.
                CString s = CommandLine.Mid(index, endPos - index);
                CommandLineArgs.push_back(s);
                index = endPos + 1;
            }

            // skip excess space characters
            while (index < CommandLine.GetLength() && CommandLine[index] == _T(' '))
                index++;
        }

        // CommandLineArgs is a vector of CStringT
        return CommandLineArgs;
    }

#ifndef _WIN32_WCE

    // Retrieves the version of common control dll used.
    // return values and DLL versions
    // 400  dll ver 4.00    Windows 95/Windows NT 4.0
    // 470  dll ver 4.70    Internet Explorer 3.x
    // 471  dll ver 4.71    Internet Explorer 4.0
    // 472  dll ver 4.72    Internet Explorer 4.01 and Windows 98
    // 580  dll ver 5.80    Internet Explorer 5
    // 581  dll ver 5.81    Windows 2000 and Windows ME
    // 582  dll ver 5.82    Windows XP, Vista, Windows 7 etc. without XP themes
    // 600  dll ver 6.00    Windows XP with XP themes
    // 610  dll ver 6.10    Windows Vista with XP themes
    // 616  dll ver 6.16    Windows Vista SP1 or above with XP themes
    inline int GetComCtlVersion()
    {
        // Load the Common Controls DLL
        HMODULE comCtl = ::LoadLibrary(_T("COMCTL32.DLL"));
        if (comCtl == 0)
            return 0;

        int comCtlVer = 400;

        if (::GetProcAddress(comCtl, "InitCommonControlsEx"))
        {
            // InitCommonControlsEx is unique to 4.7 and later
            comCtlVer = 470;

            if (::GetProcAddress(comCtl, "DllGetVersion"))
            {
                typedef HRESULT CALLBACK DLLGETVERSION(DLLVERSIONINFO*);
                DLLGETVERSION* pfnDLLGetVersion = NULL;

                pfnDLLGetVersion = reinterpret_cast<DLLGETVERSION*>(::GetProcAddress(comCtl, "DllGetVersion"));
                if (pfnDLLGetVersion)
                {
                    DLLVERSIONINFO dvi;
                    dvi.cbSize = sizeof dvi;
                    if (NOERROR == pfnDLLGetVersion(&dvi))
                    {
                        DWORD verMajor = dvi.dwMajorVersion;
                        DWORD verMinor = dvi.dwMinorVersion;
                        comCtlVer = 100 * verMajor + verMinor;
                    }
                }
            }
            else if (::GetProcAddress(comCtl, "InitializeFlatSB"))
                comCtlVer = 471;    // InitializeFlatSB is unique to version 4.71
        }

        ::FreeLibrary(comCtl);

        return comCtlVer;
    }

    // Retrieves the window version
    // Return values and window versions:
    //  1400     Windows 95
    //  1410     Windows 98
    //  1490     Windows ME
    //  2400     Windows NT
    //  2500     Windows 2000
    //  2501     Windows XP
    //  2502     Windows Server 2003
    //  2600     Windows Vista and Windows Server 2008
    //  2601     Windows 7 and Windows Server 2008 r2
    //  2602     Windows 8 and Windows Server 2012
    //  2603     Windows 8.1 and Windows Server 2012 r2
    // Note: For windows 8.1 and above, the value returned is also affected by the embedded manifest
    //       Applications not manifested for Windows 8.1 or Windows 10 will return the Windows 8 OS (2602).
    inline int GetWinVersion()
    {
#if defined (_MSC_VER) && (_MSC_VER >= 1400)
  #pragma warning ( push )
  #pragma warning ( disable : 4996 )        // GetVersion declared deprecated.
  #pragma warning ( disable : 28159 )       // Deprecated function. Consider using IsWindows instead. 
#endif // (_MSC_VER) && (_MSC_VER >= 1400)

        DWORD version = GetVersion();

#if defined (_MSC_VER) && (_MSC_VER >= 1400)
  #pragma warning ( pop )
#endif // (_MSC_VER) && (_MSC_VER >= 1400)

        int platform = (version < 0x80000000) ? 2 : 1;
        int majorVer = LOBYTE(LOWORD(version));
        int minorVer = HIBYTE(LOWORD(version));

        int result = 1000 * platform + 100 * majorVer + minorVer;
        return result;
    }


    // Returns a NONCLIENTMETRICS struct filled from the system parameters.
    // Refer to NONCLIENTMETRICS in the Windows API documentation for more information.
    inline NONCLIENTMETRICS GetNonClientMetrics()
    {
        NONCLIENTMETRICS ncm;
        ZeroMemory(&ncm, sizeof(ncm));
        ncm.cbSize = sizeof(ncm);

#if (WINVER >= 0x2600)
        // Is OS version less than Vista, adjust size to correct value
        if (GetWinVersion() < 2600)
            ncm.cbSize = CCSIZEOF_STRUCT(NONCLIENTMETRICS, lfMessageFont);
#endif

        VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0) != 0);

        return ncm;
    }


    // Reports the state of the left mouse button
    // Refer to GetAsyncKeyState in the Windows API documentation for more information.
    inline BOOL IsLeftButtonDown()
    {
        SHORT state;
        if (GetSystemMetrics(SM_SWAPBUTTON))
            // Mouse buttons are swapped
            state = ::GetAsyncKeyState(VK_RBUTTON);
        else
            // Mouse buttons are not swapped
            state = ::GetAsyncKeyState(VK_LBUTTON);

        // returns true if the left mouse button is down
        return (state & 0x8000);
    }

#endif

    // Loads the common controls using InitCommonControlsEx or InitCommonControls.
    // Returns TRUE if InitCommonControlsEx is used.
    // Refer to InitCommonControlsEx in the Windows API documentation for more information.
    inline void LoadCommonControls()
    {
        // Load the Common Controls DLL
        HMODULE comCtl = ::LoadLibrary(_T("COMCTL32.DLL"));
        if (comCtl == 0)
            comCtl = ::LoadLibrary(_T("COMMCTRL.DLL"));

        if (comCtl)
        {
            // Declare a typedef for the InItCommonControlsEx function
            typedef BOOL WINAPI INIT_EX(INITCOMMONCONTROLSEX*);

#ifdef _WIN32_WCE
            INIT_EX* pfnInitEx = reinterpret_cast<INIT_EX*>(::GetProcAddress(comCtl, _T("InitCommonControlsEx")));
#else
            INIT_EX* pfnInitEx = reinterpret_cast<INIT_EX*>(::GetProcAddress(comCtl, "InitCommonControlsEx"));
#endif

            if (pfnInitEx)
            {
                // Load the full set of common controls
                INITCOMMONCONTROLSEX InitStruct;
                InitStruct.dwSize = sizeof(InitStruct);
                InitStruct.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_DATE_CLASSES;


#if (!defined _WIN32_WCE && _WIN32_IE >= 0x0401)
                if (GetComCtlVersion() > 470)
                    InitStruct.dwICC |= ICC_INTERNET_CLASSES | ICC_NATIVEFNTCTL_CLASS | ICC_PAGESCROLLER_CLASS | ICC_USEREX_CLASSES;
#endif

                // Call InitCommonControlsEx
                if (!(pfnInitEx(&InitStruct)))
                    InitCommonControls();
            }
            else
            {
                // InitCommonControlsEx not supported. Use older InitCommonControls
                InitCommonControls();
            }

            ::FreeLibrary(comCtl);
        }
    }


    // Returns a CString containing the specified string resource.
    // Refer to LoadString in the Windows API documentation for more information.
    inline CString LoadString(UINT id)
    {
        CString str;
        str.LoadString(id);
        return str;
    }

}



#endif // _WIN32XX_WINCORE_H_

