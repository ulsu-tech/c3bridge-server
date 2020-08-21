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


////////////////////////////////////////////////////////
// wxx_dialog.h
//  Declaration of the CDialog class

// CDialog adds support for dialogs to Win32++. Dialogs are specialised
// windows which are a parent window for common controls. Common controls
// are special window types such as buttons, edit controls, tree views,
// list views, static text etc.

// The layout of a dialog is typically defined in a resource script file
// (often Resource.rc). While this script file can be constructed manually,
// it is often created using a resource editor. If your compiler doesn't
// include a resource editor, you might find ResEdit useful. It is a free
// resource editor available for download at:
// http://www.resedit.net/

// CDialog supports modal and modeless dialogs. It also supports the creation
// of dialogs defined in a resource script file, as well as those defined in
// a dialog template.

// Use the Dialog generic program as the starting point for your own dialog
// applications.
// The DialogDemo sample demonstrates how to use subclassing to customise
// the behaviour of common controls in a dialog.

// Note: The following functions often used with dialogs are provided by CWnd:
//  GetDlgCtrlID, GetDlgItem, GetDlgItemInt, GetDlgItemText, GetNextDlgGroupItem,
//  GetNextDlgTabItem, SendDlgItemMessage, SetDlgItemInt, SetDlgItemText


#ifndef _WIN32XX_DIALOG_H_
#define _WIN32XX_DIALOG_H_

#include "wxx_wincore.h"

#ifndef SWP_NOCOPYBITS
    #define SWP_NOCOPYBITS      0x0100
#endif


namespace Win32xx
{
    // This class displays and manages a dialog.
    class CDialog : public CWnd
    {

    public:
        CDialog(UINT resID);
        CDialog(LPCTSTR pResName);
        CDialog(LPCDLGTEMPLATE pDlgTemplate);
        virtual ~CDialog();

        // You probably won't need to override these functions
        virtual void AttachItem(int id, CWnd& wnd);
        virtual HWND Create(HWND parent = 0) { return DoModeless(parent); }
        virtual INT_PTR DoModal(HWND parent = 0);
        virtual HWND DoModeless(HWND parent = 0);
        virtual BOOL IsModal() const { return m_isModal; }
        BOOL IsIndirect() const { return (NULL != m_pDlgTemplate); }

    protected:
        // These are the functions you might wish to override
        virtual INT_PTR DialogProc(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual void EndDialog(INT_PTR result);
        virtual void OnCancel();
        virtual void OnClose();
        virtual BOOL OnInitDialog();
        virtual void OnOK();
        virtual BOOL PreTranslateMessage(MSG& msg);

        // Not intended to be overridden
        virtual INT_PTR DialogProcDefault(UINT msg, WPARAM wparam, LPARAM lparam);

        // Can't override these functions
        DWORD GetDefID() const;
        void GotoDlgCtrl(HWND control);
        BOOL MapDialogRect(RECT& rc) const;
        void NextDlgCtrl() const;
        void PrevDlgCtrl() const;
        void SetDefID(UINT id);

    private:
        CDialog(const CDialog&);              // Disable copy construction
        CDialog& operator = (const CDialog&); // Disable assignment operator

        static INT_PTR CALLBACK StaticDialogProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

#ifndef _WIN32_WCE
        static LRESULT CALLBACK StaticMsgHook(int code, WPARAM wparam, LPARAM lparam);
#endif

        BOOL m_isModal;                  // a flag for modal dialogs
        LPCTSTR m_pResName;              // the resource name for the dialog
		LPCDLGTEMPLATE m_pDlgTemplate;   // the dialog template for indirect dialogs
    };


#ifndef _WIN32_WCE

    //////////////////////////////////////
    // Declaration of the CResizer class
    //
    // The CResizer class can be used to rearrange a dialog's child
    // windows when the dialog is resized.

    // To use CResizer, follow the following steps:
    // 1) Use Initialize to specify the dialog's CWnd, and min and max size.
    // 3) Use AddChild for each child window
    // 4) Call HandleMessage from within DialogProc.
    //

    // Resize Dialog Styles
#define RD_STRETCH_WIDTH        0x0001  // The item has a variable width
#define RD_STRETCH_HEIGHT       0x0002  // The item has a variable height

    // Resize Dialog alignments
    enum Alignment {topleft, topright, bottomleft, bottomright, center, leftcenter, rightcenter, topcenter, bottomcenter };


    // The CResizer class can be used to rearrange a dialog's child
    // windows when the dialog is resized.
    class CResizer
    {
    public:
        CResizer() : m_parent(0), m_xScrollPos(0), m_yScrollPos(0) {}
        virtual ~CResizer() {}

        virtual void AddChild(HWND wnd, Alignment corner, DWORD style);
        virtual void HandleMessage(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual void Initialize(HWND parent, const RECT& minRect, const RECT& maxRect = CRect(0,0,0,0));
        virtual void OnHScroll(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual void OnVScroll(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual void RecalcLayout();
        CRect GetMinRect() const { return m_minRect; }
        CRect GetMaxRect() const { return m_maxRect; }
        void  SetMinRect(const RECT& minRect) { m_minRect = minRect; }
        void  SetMaxRect(const RECT& rcMaxRect) { m_maxRect = rcMaxRect; }

        struct ResizeData
        {
            ResizeData() : corner(topleft), isFixedWidth(FALSE), isFixedHeight(FALSE), wnd(0) {}
            CRect initRect;
            CRect oldRect;
            Alignment corner;
            BOOL isFixedWidth;
            BOOL isFixedHeight;
            HWND wnd;
        };

    private:
        CResizer(const CResizer&);              // Disable copy construction
        CResizer& operator = (const CResizer&); // Disable assignment operator

        static BOOL CALLBACK EnumWindowsProc(HWND wnd, LPARAM lparam);

        HWND m_parent;
        std::vector<ResizeData> m_resizeData;

        CRect m_initRect;
        CRect m_minRect;
        CRect m_maxRect;

        int m_xScrollPos;
        int m_yScrollPos;
    };

#endif

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


namespace Win32xx
{
    ////////////////////////////////////
    // Definitions for the CDialog class
    //

    inline CDialog::CDialog(LPCTSTR pResName) : m_isModal(TRUE),
                        m_pResName(pResName), m_pDlgTemplate(NULL)
    {
        // Initialize the common controls.
        LoadCommonControls();
    }

    inline CDialog::CDialog(UINT resID) : m_isModal(TRUE),
                        m_pResName(MAKEINTRESOURCE (resID)), m_pDlgTemplate(NULL)
    {
        // Initialize the common controls.
        LoadCommonControls();
    }

    //For indirect dialogs - created from a dialog box template in memory.
    inline CDialog::CDialog(LPCDLGTEMPLATE pDlgTemplate) : m_isModal(TRUE),
                        m_pResName(NULL), m_pDlgTemplate(pDlgTemplate)
    {
        // Initialize the common controls.
        LoadCommonControls();
    }

    inline CDialog::~CDialog()
    {
        if (GetHwnd() != 0)
        {
            if (IsModal())
                ::EndDialog(GetHwnd(), 0);
            else
                Destroy();
        }
    }

    // Attaches a dialog item to a CWnd
    inline void CDialog::AttachItem(int id, CWnd& wnd)
    {
        wnd.AttachDlgItem(id, *this);
    }

    // Override this function in your class derived from CDialog if you wish to handle messages.
    inline INT_PTR CDialog::DialogProc(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // A typical function might look like this:

        //  switch (msg)
        //  {
        //  case MESSAGE1:      // Some Windows API message
        //      OnMessage1();   // A user defined function
        //      break;          // Also do default processing
        //  case MESSAGE2:
        //      OnMessage2();
        //      return x;       // Don't do default processing, but instead return
        //                      //  a value recommended by the Windows API documentation
        //  }

        // Always pass unhandled messages on to DialogProcDefault
        return DialogProcDefault(msg, wparam, lparam);
    }

    // All DialogProc functions should pass unhandled messages to this function.
    inline INT_PTR CDialog::DialogProcDefault(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        LRESULT result = 0;

        switch (msg)
        {
        case WM_INITDIALOG:
            {
                // Center the dialog
                CenterWindow();
            }
            return OnInitDialog();
        case WM_CLOSE:
            {
                OnClose();
                return 0;
            }
        case WM_COMMAND:
            {
                if (HIWORD(wparam) == BN_CLICKED)
                {
                    switch (LOWORD(wparam))
                    {
                    case IDOK:
                        OnOK();
                        return TRUE;
                    case IDCANCEL:
                        OnCancel();
                        return TRUE;
                    }
                }

                // Reflect this message if it's from a control
                CWnd* pWnd = GetCWndPtr(reinterpret_cast<HWND>(lparam));
                if (pWnd != NULL)
                    result = pWnd->OnCommand(wparam, lparam);

                // Handle user commands
                if (!result)
                    result =  OnCommand(wparam, lparam);

                if (result) return 0;
            }
            break;  // Some commands require default processing


        case WM_DESTROY:
            OnDestroy();
            break;
        case WM_NOTIFY:
            {
                // Do notification reflection if message came from a child window.
                // Restricting OnNotifyReflect to child windows avoids double handling.
                LPNMHDR pNmhdr = reinterpret_cast<LPNMHDR>(lparam);
                assert(pNmhdr);
                HWND from = pNmhdr->hwndFrom;
                CWnd* pFrom = GetApp()->GetCWndFromMap(from);

                if (pFrom != NULL)
                    if (::GetParent(from) == m_wnd)
                        result = pFrom->OnNotifyReflect(wparam, lparam);

                // Handle user notifications
                if (!result) result = OnNotify(wparam, lparam);

                // Set the return code for notifications
                if (IsWindow())
                    SetWindowLongPtr(DWLP_MSGRESULT, static_cast<LONG_PTR>(result));

                return result;
            }

        case WM_PAINT:
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

                break;
            }

        case WM_ERASEBKGND:
            {
                CDC dc(reinterpret_cast<HDC>(wparam));
                BOOL PreventErasure;

                PreventErasure = OnEraseBkgnd(dc);
                if (PreventErasure) return TRUE;
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
            return MessageReflect(msg, wparam, lparam);

        case UWM_GETCDIALOG:    // Returns a pointer to this CDialog object
        case UWM_GETCWND:
        {
            assert(this == GetCWndPtr(m_wnd));

            // Set the return code
            if (IsWindow())
                SetWindowLongPtr(DWLP_MSGRESULT, reinterpret_cast<LONG_PTR>(this));

            return TRUE;
        }

        } // switch(msg)

        return 0;

    } // INT_PTR CALLBACK CDialog::DialogProc(...)

    // Creates a modal dialog. A modal dialog box must be closed by the user
    // before the application continues.
    // Refer to DialogBox and DialogBoxIndirect in the Windows API documentation for more information.
    inline INT_PTR CDialog::DoModal(HWND parent /* = 0 */)
    {
        assert( GetApp() );        // Test if Win32++ has been started
        assert(!IsWindow());        // Only one window per CWnd instance allowed
        assert(m_pDlgTemplate || m_pResName);  // Dialog layout must be defined.

        INT_PTR result = 0;
        m_isModal=TRUE;
        m_wnd = 0;

        // Ensure this thread has the TLS index set
        TLSData* pTLSData = GetApp()->SetTlsData();

    #ifndef _WIN32_WCE
        if (NULL == pTLSData->msgHook )
        {
            pTLSData->msgHook = ::SetWindowsHookEx(WH_MSGFILTER, (HOOKPROC)StaticMsgHook, NULL, ::GetCurrentThreadId());
        }
        InterlockedIncrement(&pTLSData->dlgHooks);
    #endif

        HINSTANCE instance = GetApp()->GetInstanceHandle();
        pTLSData->pWnd = this;

        // Create a modal dialog
        if (IsIndirect() && m_pDlgTemplate != NULL)
            result = ::DialogBoxIndirect(instance, m_pDlgTemplate, parent, (DLGPROC)CDialog::StaticDialogProc);
        else
        {
            if (::FindResource(GetApp()->GetResourceHandle(), m_pResName, RT_DIALOG))
                instance = GetApp()->GetResourceHandle();
            result = ::DialogBox(instance, m_pResName, parent, (DLGPROC)CDialog::StaticDialogProc);
        }

        // Tidy up
        pTLSData->pWnd = NULL;
        m_wnd = 0;

    #ifndef _WIN32_WCE
        InterlockedDecrement(&pTLSData->dlgHooks);
        if (pTLSData->dlgHooks == 0)
        {
            ::UnhookWindowsHookEx(pTLSData->msgHook);
            pTLSData->msgHook = NULL;
        }

    #endif

        // Throw an exception if the dialog creation fails
        if (result == -1)
        {
            throw CWinException(g_msgWndDoModal);
        }

        return result;
    }

    // Creates a modeless dialog.
    // Refer to CreateDialog and CreateDialogIndirect in the Windows API documentation for more information.
    inline HWND CDialog::DoModeless(HWND parent /* = 0 */)
    {
        assert( GetApp() );        // Test if Win32++ has been started
        assert(!IsWindow());        // Only one window per CWnd instance allowed
        assert(m_pDlgTemplate || m_pResName);  // Dialog layout must be defined.

        m_isModal=FALSE;
        m_wnd = 0;

        // Ensure this thread has the TLS index set
        TLSData* pTLSData = GetApp()->SetTlsData();

        // Store the CWnd pointer in Thread Local Storage
        pTLSData->pWnd = this;

        HINSTANCE instance = GetApp()->GetInstanceHandle();
        HWND wnd;

        // Create the modeless dialog
        if (IsIndirect() && m_pDlgTemplate != NULL)
            wnd = ::CreateDialogIndirect(instance, m_pDlgTemplate, parent, (DLGPROC)CDialog::StaticDialogProc);
        else
        {
            if (::FindResource(GetApp()->GetResourceHandle(), m_pResName, RT_DIALOG))
                instance = GetApp()->GetResourceHandle();

            wnd = ::CreateDialog(instance, m_pResName, parent, (DLGPROC)CDialog::StaticDialogProc);
        }

        // Tidy up
        pTLSData->pWnd = NULL;

        // Display information on dialog creation failure
        if (wnd == 0)
        {
            throw CWinException(g_msgWndDoModal);
        }

        return wnd;
    }

    // Ends a modal or modeless dialog.
    // Refer to EndDialog in the Windows API documentation for more information.
    inline void CDialog::EndDialog(INT_PTR result)
    {
        assert(IsWindow());

        if (IsModal())
            ::EndDialog(*this, result);
        else
            Destroy();
    }

    // Called when the Cancel button is pressed. Override to customize OnCancel behaviour.
    inline void CDialog::OnCancel()
    {
        EndDialog(IDCANCEL);
    }

    // Called when the Close button is pressed.
    inline void CDialog::OnClose()
    {
        EndDialog(0);
    }

    // Called when the dialog is initialized.
    // Override it in your derived class to automatically perform tasks.
    // The return value is used by WM_INITDIALOG.
    inline BOOL CDialog::OnInitDialog()
    {
        return TRUE;
    }

    // Called when the OK button is pressed. Override to customize OnOK behaviour.
    inline void CDialog::OnOK()
    {
        if ( IsWindow() )
            EndDialog(IDOK);
    }

    // Override this function to filter mouse and keyboard messages prior to
    // being passed to the DialogProc.
    inline BOOL CDialog::PreTranslateMessage(MSG& msg)
    {
        // allow the dialog to translate keyboard input
        if ((msg.message >= WM_KEYFIRST) && (msg.message <= WM_KEYLAST))
        {
            // Process dialog keystrokes for modeless dialogs
            if (!IsModal())
            {
                TLSData* pTLSData = GetApp()->GetTlsData();
                if (NULL == pTLSData->msgHook)
                {
                    if (IsDialogMessage(msg))
                        return TRUE;
                }
                else
                {
                    // A modal message loop is running which performs IsDialogMessage
                    // for us.
                }
            }
        }

        return FALSE;
    }

    // Retrieves the identifier of the default push button control for the dialog.
    // Refer to DM_GETDEFID in the Windows API documentation for more information.
    inline DWORD CDialog::GetDefID() const
    {
        assert(IsWindow());
        DWORD id = 0;
        LRESULT result = SendMessage(DM_GETDEFID, 0, 0);
        if (DC_HASDEFID == HIWORD(result))
            id = LOWORD(result);

        return id;
    }

    // Sets the keyboard focus to the specified control.
    // Refer to WM_NEXTDLGCTL in the Windows API documentation for more information.
    inline void CDialog::GotoDlgCtrl(HWND control)
    {
        assert(IsWindow());
        assert(::IsWindow(control));
        SendMessage(WM_NEXTDLGCTL, (WPARAM)control, (LPARAM)TRUE);
    }

    // Converts the dialog box units to screen units (pixels).
    // Refer to MapDialogRect in the Windows API documentation for more information.
    inline BOOL CDialog::MapDialogRect(RECT& rc) const
    {
        assert(IsWindow());
        return ::MapDialogRect(*this, &rc);
    }

    // Sets the keyboard focus to the next dialog control.
    // Refer to WM_NEXTDLGCTL in the Windows API documentation for more information.
    inline void CDialog::NextDlgCtrl() const
    {
        assert(IsWindow());
        SendMessage(WM_NEXTDLGCTL, (WPARAM)FALSE, (LPARAM)FALSE);
    }

    // Sets the keyboard focus to the previous dialog control.
    // Refer to WM_NEXTDLGCTL in the Windows API documentation for more information.
    inline void CDialog::PrevDlgCtrl() const
    {
        assert(IsWindow());
        SendMessage(WM_NEXTDLGCTL, (WPARAM)TRUE, (LPARAM)FALSE);
    }

    // Changes the identifier of the default push button for a dialog box.
    // Refer to DM_SETDEFID in the Windows API documentation for more information.
    inline void CDialog::SetDefID(UINT id)
    {
        assert(IsWindow());
        SendMessage(DM_SETDEFID, (WPARAM)id, 0);
    }

    // This callback function passes messages to DialogProc
    inline INT_PTR CALLBACK CDialog::StaticDialogProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Find the CWnd pointer mapped to this HWND
        CDialog* pDialog = static_cast<CDialog*>(GetCWndPtr(wnd));
        if (pDialog == 0)
        {
            // The HWND wasn't in the map, so add it now
            TLSData* pTLSData = GetApp()->GetTlsData();
            assert(pTLSData);

            // Retrieve pointer to CWnd object from Thread Local Storage TLS
            pDialog = static_cast<CDialog*>(pTLSData->pWnd);
            assert(pDialog);
            pTLSData->pWnd = NULL;

            // Store the Window pointer into the HWND map
            pDialog->m_wnd = wnd;
            pDialog->AddToMap();
        }

        return pDialog->DialogProc(msg, wparam, lparam);

    } // INT_PTR CALLBACK CDialog::StaticDialogProc(...)


#ifndef _WIN32_WCE

    // Used by Modal Dialogs for idle processing and PreTranslateMessage.
    inline LRESULT CALLBACK CDialog::StaticMsgHook(int code, WPARAM wparam, LPARAM lparam)
    {
        TLSData* pTLSData = GetApp()->GetTlsData();
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        LONG count = 0;

        // While idle, perform idle processing until OnIdle returns FALSE
        // Exclude some messages to avoid calling OnIdle excessively
        while (!::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) &&
                            (msg.message != WM_TIMER) &&
                            (msg.message != WM_MOUSEMOVE) &&
                            (msg.message != WM_SETCURSOR) &&
                                GetApp()->OnIdle(count) != FALSE )
        {
            ++count;
        }
        count = 0;

        if (code == MSGF_DIALOGBOX)
        {
            MSG* pMsg = reinterpret_cast<MSG*>(lparam);
            assert(pMsg);

            // only pre-translate keyboard and mouse events
            if ((pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) ||
                (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST))
            {
                for (HWND wnd = pMsg->hwnd; wnd != NULL; wnd = ::GetParent(wnd))
                {
                    // Only CDialogs respond to this message
                    CDialog* pDialog = reinterpret_cast<CDialog*>(::SendMessage(wnd, UWM_GETCDIALOG, 0, 0));
                    if (pDialog != 0)
                    {
                        assert(GetCWndPtr(wnd));
                        if (pDialog->PreTranslateMessage(*pMsg))
                            return 1; // Eat the message

                        break;  // Pass the message on
                    }
                }
            }
        }

        return ::CallNextHookEx(pTLSData->msgHook, code, wparam, lparam);
    }
#endif



#ifndef _WIN32_WCE

    /////////////////////////////////////
    // Definitions for the CResizer class
    //

    // Adds or updates the information about a child window managed by CResizer.
    //
    // The alignment parameter determines the control's position.
    // Allowed values are:
    //  topleft:      Positions top left
    //  topright:     Positions top right
    //  bottomleft:   Positions bottom left
    //  bottomright:  Positions bottom right
    //  center:       Positions proportionally
    //  leftcenter:   Positions proportionally along the left side
    //  rightcenter:  Positions proportionally along the right side
    //  topcenter:    Positions proportionally along the top side
    //  bottomcenter: Positions proportionally along the bottom side
    //
    // The style is a combination of the following flags. If the style is
    // zero, the control's width and height is fixed:
    //  RD_STRETCH_WIDTH:   The width is variable instead of fixed.
    //  RD_STRETCH_HEIGHT:  The height is variable instead of fixed.
    void inline CResizer::AddChild(HWND wnd, Alignment corner, DWORD style)
    {
        assert(::IsWindow(wnd));
        assert(::GetParent(wnd) == m_parent);

        ResizeData rd;
        rd.corner = corner;
        rd.isFixedWidth  = !(style & RD_STRETCH_WIDTH);
        rd.isFixedHeight = !(style & RD_STRETCH_HEIGHT);
        CRect initRect;
        ::GetWindowRect(wnd, &initRect);
        ::MapWindowPoints(NULL, m_parent, (LPPOINT)&initRect, 2);
        rd.initRect = initRect;
        rd.wnd = wnd;

        std::vector<ResizeData>::iterator iter;
        for (iter = m_resizeData.begin(); iter != m_resizeData.end(); ++ iter)
        {
            if ( iter->wnd == wnd)
            {
                // Replace the value
                *iter = rd;
                break;
            }
        }

        // Add the value
        if (iter == m_resizeData.end())
            m_resizeData.push_back(rd);
    }

    // A callback function used by EnumChildWindows.
    inline BOOL CALLBACK CResizer::EnumWindowsProc(HWND wnd, LPARAM lparam)
    {
        CResizer* pResizer = reinterpret_cast<CResizer*>(lparam);

        // Only for a child, not other descendants.
        if (::GetParent(wnd) == pResizer->m_parent)
        {
            // Add the child window to set of windows managed by CResizer.
            pResizer->AddChild(wnd, topleft, 0);
        }

        return TRUE;
    }

    // Performs the resizing and scrolling. Call this function from within the window's DialogProc.
    inline void CResizer::HandleMessage(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_SIZE:
            RecalcLayout();
            break;

        case WM_HSCROLL:
            if (lparam == 0)
                OnHScroll(msg, wparam, lparam);
            break;

        case WM_VSCROLL:
            if (lparam == 0)
                OnVScroll(msg, wparam, lparam);
            break;
        }
    }

    // Sets up the Resizer by specifying the parent window (usually a dialog),
    // and the minimum and maximum allowed rectangle sizes.
    // Note: parent can either be a CWnd or a window handle (HWND)
    void inline CResizer::Initialize(HWND parent, const RECT& minRect, const RECT& maxRect)
    {
        assert (::IsWindow(parent));

        m_parent = parent;
        VERIFY(::GetClientRect(parent, &m_initRect) != 0);

        m_minRect = minRect;
        m_maxRect = maxRect;

        m_resizeData.clear();

        // Add scroll bar support to the parent window
        DWORD style = static_cast<DWORD>(::GetClassLongPtr(parent, GCL_STYLE));
        style |= WS_HSCROLL | WS_VSCROLL;
        ::SetClassLongPtr(parent, GCL_STYLE, style);

        // Calls AddChild for each child window with default settings.
        ::EnumChildWindows(parent, EnumWindowsProc, (LPARAM)this);
    }

    // Called to perform horizontal scrolling.
    void inline CResizer::OnHScroll(UINT, WPARAM wparam, LPARAM)
    {
        int xNewPos;

        switch (LOWORD(wparam))
        {
            case SB_PAGEUP: // User clicked the scroll bar shaft left of the scroll box.
                xNewPos = m_xScrollPos - 50;
                break;

            case SB_PAGEDOWN: // User clicked the scroll bar shaft right of the scroll box.
                xNewPos = m_xScrollPos + 50;
                break;

            case SB_LINEUP: // User clicked the left arrow.
                xNewPos = m_xScrollPos - 5;
                break;

            case SB_LINEDOWN: // User clicked the right arrow.
                xNewPos = m_xScrollPos + 5;
                break;

            case SB_THUMBPOSITION: // User dragged the scroll box.
                xNewPos = HIWORD(wparam);
                break;

            case SB_THUMBTRACK: // User dragging the scroll box.
                xNewPos = HIWORD(wparam);
                break;

            default:
                xNewPos = m_xScrollPos;
        }

        // Scroll the window.
        xNewPos = MAX(0, xNewPos);
        CRect rc;
        VERIFY(::GetClientRect(m_parent, &rc) != 0);
        xNewPos = MIN( xNewPos, GetMinRect().Width() - rc.Width() );
        int xDelta = xNewPos - m_xScrollPos;
        m_xScrollPos = xNewPos;
        ::ScrollWindow(m_parent, -xDelta, 0, NULL, NULL);

        // Reset the scroll bar.
        SCROLLINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof(si);
        si.fMask  = SIF_POS;
        si.nPos   = m_xScrollPos;
        ::SetScrollInfo(m_parent, SB_HORZ, &si, TRUE);
    }

    // Called to perform vertical scrolling.
    void inline CResizer::OnVScroll(UINT, WPARAM wparam, LPARAM)
    {
        int yNewPos;

        switch (LOWORD(wparam))
        {
            case SB_PAGEUP: // User clicked the scroll bar shaft above the scroll box.
                yNewPos = m_yScrollPos - 50;
                break;

            case SB_PAGEDOWN: // User clicked the scroll bar shaft below the scroll box.
                yNewPos = m_yScrollPos + 50;
                break;

            case SB_LINEUP: // User clicked the top arrow.
                yNewPos = m_yScrollPos - 5;
                break;

            case SB_LINEDOWN: // User clicked the bottom arrow.
                yNewPos = m_yScrollPos + 5;
                break;

            case SB_THUMBPOSITION: // User dragged the scroll box.
                yNewPos = HIWORD(wparam);
                break;

            case SB_THUMBTRACK: // User dragging the scroll box.
                yNewPos = HIWORD(wparam);
                break;

            default:
                yNewPos = m_yScrollPos;
        }

        // Scroll the window.
        yNewPos = MAX(0, yNewPos);
        CRect rc;
        VERIFY(::GetClientRect(m_parent, &rc) != 0);
        yNewPos = MIN( yNewPos, GetMinRect().Height() - rc.Height() );
        int yDelta = yNewPos - m_yScrollPos;
        m_yScrollPos = yNewPos;
        ::ScrollWindow(m_parent, 0, -yDelta, NULL, NULL);

        // Reset the scroll bar.
        SCROLLINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof(si);
        si.fMask  = SIF_POS;
        si.nPos   = m_yScrollPos;
        ::SetScrollInfo(m_parent, SB_VERT, &si, TRUE);
    }

    // Repositions the child windows. Call this function when handling
    // the WM_SIZE message in the parent window.
    void inline CResizer::RecalcLayout()
    {
        assert (m_initRect.Width() > 0 && m_initRect.Height() > 0);
        assert (::IsWindow(m_parent));

        CRect currentRect;
        VERIFY(::GetClientRect(m_parent, &currentRect) != 0);

        // Adjust the scrolling if required
        m_xScrollPos = MIN(m_xScrollPos, MAX(0, m_minRect.Width()  - currentRect.Width() ) );
        m_yScrollPos = MIN(m_yScrollPos, MAX(0, m_minRect.Height() - currentRect.Height()) );
        SCROLLINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof(si);
        si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMax   = m_minRect.Width();
        si.nPage  = currentRect.Width();
        si.nPos   = m_xScrollPos;
        ::SetScrollInfo(m_parent, SB_HORZ, &si, TRUE);
        si.nMax   = m_minRect.Height();
        si.nPage  = currentRect.Height();
        si.nPos   = m_yScrollPos;
        ::SetScrollInfo(m_parent, SB_VERT, &si, TRUE);

        // Note: calls to SetScrollInfo may have changed the client rect, so
        // we get it again.
        VERIFY(::GetClientRect(m_parent, &currentRect) != 0);

        currentRect.right  = MAX(currentRect.Width(),  m_minRect.Width() );
        currentRect.bottom = MAX(currentRect.Height(), m_minRect.Height() );
        if (!m_maxRect.IsRectEmpty())
        {
            currentRect.right  = MIN(currentRect.Width(),  m_maxRect.Width() );
            currentRect.bottom = MIN(currentRect.Height(), m_maxRect.Height() );
        }

        // Declare an iterator to step through the vector
        std::vector<ResizeData>::iterator iter;

        // Allocates memory for a multiple-window- position structure.
        HDWP hdwp = ::BeginDeferWindowPos(static_cast<int>(m_resizeData.size()));

        for (iter = m_resizeData.begin(); iter != m_resizeData.end(); ++iter)
        {
            int left   = 0;
            int top    = 0;
            int width  = 0;
            int height = 0;

            // Calculate the new size and position of the child window
            switch( (*iter).corner )
            {
            case topleft:      // Positions top left
                width  = (*iter).isFixedWidth?  (*iter).initRect.Width()  : (*iter).initRect.Width()  - m_initRect.Width() + currentRect.Width();
                height = (*iter).isFixedHeight? (*iter).initRect.Height() : (*iter).initRect.Height() - m_initRect.Height() + currentRect.Height();
                left   = (*iter).initRect.left;
                top    = (*iter).initRect.top;
                break;
            case topright:     // Positions top right
                width  = (*iter).isFixedWidth?  (*iter).initRect.Width()  : (*iter).initRect.Width()  - m_initRect.Width() + currentRect.Width();
                height = (*iter).isFixedHeight? (*iter).initRect.Height() : (*iter).initRect.Height() - m_initRect.Height() + currentRect.Height();
                left   = (*iter).initRect.right - width - m_initRect.Width() + currentRect.Width();
                top    = (*iter).initRect.top;
                break;
            case bottomleft:   // Positions bottom left
                width  = (*iter).isFixedWidth?  (*iter).initRect.Width()  : (*iter).initRect.Width()  - m_initRect.Width() + currentRect.Width();
                height = (*iter).isFixedHeight? (*iter).initRect.Height() : (*iter).initRect.Height() - m_initRect.Height() + currentRect.Height();
                left   = (*iter).initRect.left;
                top    = (*iter).initRect.bottom - height - m_initRect.Height() + currentRect.Height();
                break;
            case bottomright:  // Positions bottom right
                width  = (*iter).isFixedWidth?  (*iter).initRect.Width()  : (*iter).initRect.Width()  - m_initRect.Width() + currentRect.Width();
                height = (*iter).isFixedHeight? (*iter).initRect.Height() : (*iter).initRect.Height() - m_initRect.Height() + currentRect.Height();
                left   = (*iter).initRect.right   - width - m_initRect.Width() + currentRect.Width();
                top    = (*iter).initRect.bottom  - height - m_initRect.Height() + currentRect.Height();
                break;
            case center:       // Positions proportionally
                width  = (*iter).isFixedWidth ? (*iter).initRect.Width() : ((*iter).initRect.Width() * currentRect.Width()) / m_initRect.Width();
                height = (*iter).isFixedHeight ? (*iter).initRect.Height() : ((*iter).initRect.Height() * currentRect.Height()) / m_initRect.Height();
                left   = ((*iter).initRect.left * currentRect.Width()) / m_initRect.Width();
                top    = ((*iter).initRect.top * currentRect.Height()) / m_initRect.Height();
                break;
            case leftcenter:   // Positions proportionally along the left side
                width  = (*iter).isFixedWidth ? (*iter).initRect.Width() : (*iter).initRect.Width() - m_initRect.Width() + currentRect.Width();
                height = (*iter).isFixedHeight ? (*iter).initRect.Height() : ((*iter).initRect.Height() * currentRect.Height()) / m_initRect.Height();
                left   = (*iter).initRect.left;
                top    = ((*iter).initRect.top * currentRect.Height()) / m_initRect.Height();
                break;
            case rightcenter:  // Positions proportionally along the right side
                width  = (*iter).isFixedWidth ? (*iter).initRect.Width() : (*iter).initRect.Width() - m_initRect.Width() + currentRect.Width();
                height = (*iter).isFixedHeight ? (*iter).initRect.Height() : ((*iter).initRect.Height() * currentRect.Height()) / m_initRect.Height();
                left   = (*iter).initRect.right - width - m_initRect.Width() + currentRect.Width();
                top    = ((*iter).initRect.top * currentRect.Height()) / m_initRect.Height();
                break;
            case topcenter:    // Positions proportionally along the top side
                width  = (*iter).isFixedWidth ? (*iter).initRect.Width() : ((*iter).initRect.Width() * currentRect.Width()) /  m_initRect.Width();
                height = (*iter).isFixedHeight ? (*iter).initRect.Height() : (*iter).initRect.Height() - m_initRect.Height() + currentRect.Height();
                left   = ((*iter).initRect.left * currentRect.Width()) / m_initRect.Width();
                top    = (*iter).initRect.top;
                break;
            case bottomcenter: // Positions proportionally along the bottom side
                width  = (*iter).isFixedWidth ? (*iter).initRect.Width() : ((*iter).initRect.Width() * currentRect.Width()) / m_initRect.Width();
                height = (*iter).isFixedHeight ? (*iter).initRect.Height() : (*iter).initRect.Height() - m_initRect.Height() + currentRect.Height();
                left   = ((*iter).initRect.left * currentRect.Width()) / m_initRect.Width();
                top    = (*iter).initRect.bottom - height - m_initRect.Height() + currentRect.Height();
                break;
            }

            // Determine the position of the child window.
            CRect rc(left - m_xScrollPos, top - m_yScrollPos, left + width - m_xScrollPos, top + height - m_yScrollPos);
            if ( rc != (*iter).oldRect)
            {
                // Note: The tab order of the dialog's controls is determined by the order
                //       they are specified in the resource script (resource.rc).

                // Store the window's new position. Repositioning happens later.
                hdwp = ::DeferWindowPos(hdwp, (*iter).wnd, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_NOCOPYBITS);

                (*iter).oldRect = rc;
            }

        }

        // Reposition all the child windows simultaneously.
        VERIFY(::EndDeferWindowPos(hdwp) != 0);
    }

#endif // #ifndef _WIN32_WCE

} // namespace Win32xx



#endif // _WIN32XX_DIALOG_H_

