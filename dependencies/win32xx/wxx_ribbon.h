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
// wxx_ribbon.h
//  Declaration of the following classes:
//  CRibbon, CRibbonFrameT, CRibbonFrame, CRibbonDockFrame
//  CRibbonMDIFrame and CRibbonMDIDockFrame.
//

#ifndef _WIN32XX_RIBBON_H_
#define _WIN32XX_RIBBON_H_


// Notes :
//  1) To compile code using the Ribbon UI you will need a Microsoft compiler.
//     Visual Studio Community 2013 (or later) is recommended, but older
//     Microsoft compilers can be used with the Windows 7 SDK.
//  2) The Ribbon UI only runs on Windows 7 or later operating systems.If the
//     code is run on an earlier operating system, it reverts back to a menu
//     and toolbar.

#include <UIRibbon.h>                   // Contained within the Windows 7 SDK
#include <UIRibbonPropertyHelpers.h>

#include "wxx_frame.h"
#include "wxx_dockframe.h"

namespace Win32xx
{
    //////////////////////////////////////////////
    // The CRibbon class is used to add the Ribbon framework to a window.
    class CRibbon : public IUICommandHandler, public IUIApplication
    {
    public:
        CRibbon();
        ~CRibbon();

        // IUnknown methods.
        STDMETHOD_(ULONG, AddRef());
        STDMETHOD_(ULONG, Release());
        STDMETHOD(QueryInterface(REFIID iid, void** ppObject));

        // IUIApplication methods
        STDMETHOD(OnCreateUICommand)(UINT nCmdID, __in UI_COMMANDTYPE typeID,
            __deref_out IUICommandHandler** ppCommandHandler);

        STDMETHOD(OnDestroyUICommand)(UINT32 commandId, __in UI_COMMANDTYPE typeID,
            __in_opt IUICommandHandler* commandHandler);

        STDMETHOD(OnViewChanged)(UINT viewId, __in UI_VIEWTYPE typeId, __in IUnknown* pView,
            UI_VIEWVERB verb, INT uReasonCode);

        // IUICommandHandle methods
        STDMETHODIMP Execute(UINT nCmdID, UI_EXECUTIONVERB verb, __in_opt const PROPERTYKEY* key, __in_opt const PROPVARIANT* value,
                                          __in_opt IUISimplePropertySet* pCommandExecutionProperties);

        STDMETHODIMP UpdateProperty(UINT nCmdID, __in REFPROPERTYKEY key, __in_opt const PROPVARIANT* currentValue,
                                                 __out PROPVARIANT* newValue);

        bool virtual CreateRibbon(HWND wnd);
        void virtual DestroyRibbon();
        IUIFramework* GetRibbonFramework() const { return m_pRibbonFramework; }
        UINT GetRibbonHeight() const;

    private:
        CRibbon(const CRibbon&);              // Disable copy construction
        CRibbon& operator = (const CRibbon&); // Disable assignment operator

        IUIFramework* m_pRibbonFramework;
        LONG m_count;                            // Reference count.

    };

    //////////////////////////////////////////////
    // Declaration of the CRibbonFrameT class template
    //

    // The CRibbonFrameT is the base class for frames that support the Ribbon Framework.
    // The T parameter can be either CWnd or CDocker.
    template <class T>
    class CRibbonFrameT : public T, public CRibbon
    {
    public:
        // A nested class for the MRU item properties
        class CRecentFiles : public IUISimplePropertySet
        {
        public:
            CRecentFiles(PWSTR pFullPath);
            ~CRecentFiles() {}

            // IUnknown methods.
            STDMETHODIMP_(ULONG) AddRef();
            STDMETHODIMP_(ULONG) Release();
            STDMETHODIMP QueryInterface(REFIID iid, void** ppObject);

            // IUISimplePropertySet methods
            STDMETHODIMP GetValue(__in REFPROPERTYKEY key, __out PROPVARIANT* value);

        private:
            LONG m_count;                        // Reference count.
            WCHAR m_displayName[MAX_PATH];
            WCHAR m_fullPath[MAX_PATH];
        };

        typedef Shared_Ptr<CRecentFiles> RecentFilesPtr;

        CRibbonFrameT() {}
        virtual ~CRibbonFrameT() {}
        virtual CRect GetViewRect() const;
        virtual int  OnCreate(CREATESTRUCT& cs);
        virtual void OnDestroy();
        virtual STDMETHODIMP OnViewChanged(UINT32 viewId, UI_VIEWTYPE typeId, IUnknown* pView, UI_VIEWVERB verb, INT32 reasonCode);
        virtual HRESULT PopulateRibbonRecentItems(PROPVARIANT* value);
        virtual void UpdateMRUMenu();

    private:
        CRibbonFrameT(const CRibbonFrameT&);              // Disable copy construction
        CRibbonFrameT& operator = (const CRibbonFrameT&); // Disable assignment operator

        std::vector<RecentFilesPtr> m_recentFiles;
    };


    // This class provides an SDI frame with a Ribbon Framework
    class CRibbonFrame : public CRibbonFrameT<CFrame>
    {
    public:
        CRibbonFrame() {}
        virtual ~CRibbonFrame() {}
    };

    // This class provide an SDI frame with a Ribbon Framework that supports docking.
    class CRibbonDockFrame : public CRibbonFrameT<CDockFrame>
    {
    public:
        CRibbonDockFrame() {}
        virtual ~CRibbonDockFrame() {}
    };

    // This class provides a MDI frame with a Ribbon Framework.
    class CRibbonMDIFrame : public CRibbonFrameT<CMDIFrame>
    {
    public:
        CRibbonMDIFrame() {}
        virtual ~CRibbonMDIFrame() {}
    };

    // This class provides a MDI frame with a Ribbon Framework that supports docking.
    class CRibbonMDIDockFrame : public CRibbonFrameT<CMDIDockFrame>
    {
    public:
        CRibbonMDIDockFrame() {}
        virtual ~CRibbonMDIDockFrame() {}
    };

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


namespace Win32xx
{
    //////////////////////////////////////////////
    // Definitions for the CRibbon class
    //

    inline CRibbon::CRibbon() : m_count(0), m_pRibbonFramework(NULL)
    {
        if (FAILED(::CoInitialize(NULL)))
            throw CWinException(g_msgCoInitialize);
    }

    inline CRibbon::~CRibbon()
    {
        // Reference count must be 0 or we have a leak!
        assert(m_count == 0);
        ::CoUninitialize();
    }

    //////////////////////////////////
    // IUnknown method implementations.


    inline STDMETHODIMP_(ULONG) CRibbon::AddRef()
    {
        return InterlockedIncrement(&m_count);
    }

    inline STDMETHODIMP_(ULONG) CRibbon::Release()
    {
        LONG cRef = InterlockedDecrement(&m_count);
        return cRef;
    }


    // Responds to execute events on Commands bound to the Command handler.
    inline STDMETHODIMP CRibbon::Execute(UINT cmdID, UI_EXECUTIONVERB verb, __in_opt const PROPERTYKEY* key, __in_opt const PROPVARIANT* value,
                                          __in_opt IUISimplePropertySet* pCommandExecutionProperties)
    {
        UNREFERENCED_PARAMETER (cmdID);
        UNREFERENCED_PARAMETER (verb);
        UNREFERENCED_PARAMETER (key);
        UNREFERENCED_PARAMETER (value);
        UNREFERENCED_PARAMETER (pCommandExecutionProperties);

        return E_NOTIMPL;
    }

    inline STDMETHODIMP CRibbon::QueryInterface(REFIID iid, void** ppObject)
    {
        if (iid == __uuidof(IUnknown))
        {
            *ppObject = static_cast<IUnknown*>(static_cast<IUIApplication*>(this));
        }
        else if (iid == __uuidof(IUICommandHandler))
        {
            *ppObject = static_cast<IUICommandHandler*>(this);
        }
        else if (iid == __uuidof(IUIApplication))
        {
            *ppObject = static_cast<IUIApplication*>(this);
        }
        else
        {
            *ppObject = NULL;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }


    // Called by the Ribbon framework for each command specified in markup, to bind the Command to an IUICommandHandler.
    inline STDMETHODIMP CRibbon::OnCreateUICommand(UINT cmdID, __in UI_COMMANDTYPE typeID,
                                                 __deref_out IUICommandHandler** ppCommandHandler)
    {
        UNREFERENCED_PARAMETER(typeID);
        UNREFERENCED_PARAMETER(cmdID);

        // By default we use the single command handler provided as part of CRibbon.
        // Override this function to account for multiple command handlers.

        return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
    }

    // Called when the state of the Ribbon changes, for example, created, destroyed, or resized.
    inline STDMETHODIMP CRibbon::OnViewChanged(UINT viewId, __in UI_VIEWTYPE typeId, __in IUnknown* pView,
                                             UI_VIEWVERB verb, INT reasonCode)
    {
        UNREFERENCED_PARAMETER(viewId);
        UNREFERENCED_PARAMETER(typeId);
        UNREFERENCED_PARAMETER(pView);
        UNREFERENCED_PARAMETER(verb);
        UNREFERENCED_PARAMETER(reasonCode);

        return E_NOTIMPL;
    }

    // Called by the Ribbon framework for each command at the time of ribbon destruction.
    inline STDMETHODIMP CRibbon::OnDestroyUICommand(UINT32 cmdID, __in UI_COMMANDTYPE typeID,
                                                  __in_opt IUICommandHandler* commandHandler)
    {
        UNREFERENCED_PARAMETER(commandHandler);
        UNREFERENCED_PARAMETER(typeID);
        UNREFERENCED_PARAMETER(cmdID);

        return E_NOTIMPL;
    }

    // Called by the Ribbon framework when a command property (PKEY) needs to be updated.
    inline STDMETHODIMP CRibbon::UpdateProperty(UINT cmdID, __in REFPROPERTYKEY key, __in_opt const PROPVARIANT* currentValue,
                                                 __out PROPVARIANT* newValue)
    {
        UNREFERENCED_PARAMETER(cmdID);
        UNREFERENCED_PARAMETER(key);
        UNREFERENCED_PARAMETER(currentValue);
        UNREFERENCED_PARAMETER(newValue);

        return E_NOTIMPL;
    }

    // Creates the ribbon.
    inline bool CRibbon::CreateRibbon(HWND wnd)
    {
        HRESULT result;
        // Instantiate the Ribbon framework object.
        result = ::CoCreateInstance(CLSID_UIRibbonFramework, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pRibbonFramework));
        if (FAILED(result))
            return false;

        // Connect the host application to the Ribbon framework.
        assert(m_pRibbonFramework);
        result = m_pRibbonFramework->Initialize(wnd, this);
        if (FAILED(result))
            return false;

        // Load the binary markup. APPLICATION_RIBBON is the default name generated by uicc.
        result = m_pRibbonFramework->LoadUI(GetModuleHandle(NULL), L"APPLICATION_RIBBON");
        if (FAILED(result))
            return false;

        return true;
    }

    // Destroys the ribbon.
    inline void CRibbon::DestroyRibbon()
    {
        if (m_pRibbonFramework)
        {
            m_pRibbonFramework->Destroy();
            m_pRibbonFramework->Release();
            m_pRibbonFramework = NULL;
        }

    }

    // Retrieves the height of the ribbon.
    inline UINT CRibbon::GetRibbonHeight() const
    {
        HRESULT result = E_FAIL;
        IUIRibbon* pRibbon = NULL;
        UINT ribbonHeight = 0;

        if (GetRibbonFramework())
        {
            result = GetRibbonFramework()->GetView(0, IID_PPV_ARGS(&pRibbon));
            if (SUCCEEDED(result))
            {
                // Call to the framework to determine the desired height of the Ribbon.
                result = pRibbon->GetHeight(&ribbonHeight);
                pRibbon->Release();
            }
        }

        return ribbonHeight;
    }


    //////////////////////////////////////////////
    // Definitions for the CRibbonFrameT class template
    //

    // Get the frame's client area
    template <class T>
    inline CRect CRibbonFrameT<T>::GetViewRect() const
    {
        CRect clientRect = GetClientRect();

        clientRect.top += GetRibbonHeight();

        if (GetStatusBar().IsWindow() && GetStatusBar().IsWindowVisible())
            clientRect = ExcludeChildRect(clientRect, GetStatusBar());

        if (GetReBar().IsWindow() && GetReBar().IsWindowVisible())
            clientRect = ExcludeChildRect(clientRect, GetReBar());
        else
            if (GetToolBar().IsWindow() && GetToolBar().IsWindowVisible())
                clientRect = ExcludeChildRect(clientRect, GetToolBar());

        return clientRect;
    }

    // OnCreate is called automatically during window creation when a
    // WM_CREATE message received.
    // Tasks such as setting the icon, creating child windows, or anything
    // associated with creating windows are normally performed here.
    template <class T>
    inline int CRibbonFrameT<T>::OnCreate(CREATESTRUCT& cs)
    {
        UNREFERENCED_PARAMETER(cs);

        if (GetWinVersion() >= 2601)    // WinVersion >= Windows 7
        {
            if (CreateRibbon(*this))
            {
                UseReBar(FALSE);     // Don't use a ReBar
                UseToolBar(FALSE);   // Don't use a ToolBar
            }
            else
            {
                TRACE("Failed to create Ribbon\n");
                DestroyRibbon();
            }
        }

        T::OnCreate(cs);
        if (GetRibbonFramework())
        {
            SetMenu(NULL);              // Disable the window menu
            SetFrameMenu(reinterpret_cast<HMENU>(0));
        }

        return 0;
    }

    // Called when the ribbon frame is destroyed.
    template <class T>
    inline void CRibbonFrameT<T>::OnDestroy()
    {
        DestroyRibbon();
        T::OnDestroy();
    }

    // Called when the ribbon's view has changed.
    template <class T>
    inline STDMETHODIMP CRibbonFrameT<T>::OnViewChanged(UINT32 viewId, UI_VIEWTYPE typeId, IUnknown* pView, UI_VIEWVERB verb, INT32 reasonCode)
    {
        UNREFERENCED_PARAMETER(viewId);
        UNREFERENCED_PARAMETER(pView);
        UNREFERENCED_PARAMETER(reasonCode);

        HRESULT result = E_NOTIMPL;

        // Checks to see if the view that was changed was a Ribbon view.
        if (UI_VIEWTYPE_RIBBON == typeId)
        {
            switch (verb)
            {
            case UI_VIEWVERB_CREATE:    // The view was newly created.
                result = S_OK;
                break;
            case UI_VIEWVERB_SIZE:      // Ribbon size has changed
                RecalcLayout();
                break;
            case UI_VIEWVERB_DESTROY:   // The view was destroyed.
                result = S_OK;
                break;
            }
        }

        return result;
    }

    // Populates the ribbon's recent items list.
    template <class T>
    inline HRESULT CRibbonFrameT<T>::PopulateRibbonRecentItems(PROPVARIANT* pvarValue)
    {
        LONG currentFile = 0;
        std::vector<CString> fileNames = GetMRUEntries();
        std::vector<CString>::const_iterator iter;
        ULONG_PTR fileCount = fileNames.size();
        HRESULT result = E_FAIL;
        SAFEARRAY* psa = SafeArrayCreateVector(VT_UNKNOWN, 0, (ULONG)fileCount);
        m_recentFiles.clear();

        if (psa != NULL)
        {
            for (iter = fileNames.begin(); iter != fileNames.end(); ++iter)
            {
                WCHAR curFileName[MAX_PATH] = {0};
                StrCopyW(curFileName, TtoW(*iter), MAX_PATH);

                CRecentFiles* pRecentFiles = new CRecentFiles(curFileName);
                m_recentFiles.push_back(RecentFilesPtr(pRecentFiles));
                result = SafeArrayPutElement(psa, &currentFile, static_cast<void*>(pRecentFiles));
                ++currentFile;
            }

            SAFEARRAYBOUND sab = {static_cast<ULONG>(currentFile), 0};
            SafeArrayRedim(psa, &sab);
            result = UIInitPropertyFromIUnknownArray(UI_PKEY_RecentItems, psa, pvarValue);

            SafeArrayDestroy(psa);  // Calls release for each element in the array
        }

        return result;
    }

    // Updates the frame's MRU when the Ribbon Framework isn't used.
    template <class T>
    inline void CRibbonFrameT<T>::UpdateMRUMenu()
    {
        // Suppress UpdateMRUMenu when ribbon is used
        if (GetRibbonFramework() != 0) return;

        T::UpdateMRUMenu();
    }


    ////////////////////////////////////////////////////////
    // Declaration of the nested CRecentFiles class
    //
    template <class T>
    inline CRibbonFrameT<T>::CRecentFiles::CRecentFiles(PWSTR fullPath) : m_count(0)
    {
        SHFILEINFOW sfi;
        ZeroMemory(&sfi, sizeof(sfi));
        DWORD_PTR ptr = NULL;
        m_fullPath[0] = L'\0';
        m_displayName[0] = L'\0';

        StrCopyW(m_fullPath, fullPath, MAX_PATH);
        ptr = ::SHGetFileInfoW(fullPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_USEFILEATTRIBUTES);
        if (ptr != NULL)
        {
            StrCopyW(m_displayName, sfi.szDisplayName, MAX_PATH);
        }
        else // Provide a reasonable fall back.
        {
            StrCopyW(m_displayName, m_fullPath, MAX_PATH);
        }

    }

    template <class T>
    inline STDMETHODIMP_(ULONG) CRibbonFrameT<T>::CRecentFiles::AddRef()
    {
        return InterlockedIncrement(&m_count);
    }

    template <class T>
    inline STDMETHODIMP_(ULONG) CRibbonFrameT<T>::CRecentFiles::Release()
    {
        return InterlockedDecrement(&m_count);
    }

    template <class T>
    inline STDMETHODIMP CRibbonFrameT<T>::CRecentFiles::QueryInterface(REFIID iid, void** ppObject)
    {
        if (!ppObject)
        {
            return E_POINTER;
        }

        if (iid == __uuidof(IUnknown))
        {
            *ppObject = static_cast<IUnknown*>(this);
        }
        else if (iid == __uuidof(IUISimplePropertySet))
        {
            *ppObject = static_cast<IUISimplePropertySet*>(this);
        }
        else
        {
            *ppObject = NULL;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    // IUISimplePropertySet methods.
    template <class T>
    inline STDMETHODIMP CRibbonFrameT<T>::CRecentFiles::GetValue(__in REFPROPERTYKEY key, __out PROPVARIANT *ppropvar)
    {
        HRESULT result = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

        if (key == UI_PKEY_Label)
        {
            result = UIInitPropertyFromString(key, m_displayName, ppropvar);
        }
        else if (key == UI_PKEY_LabelDescription)
        {
            result = UIInitPropertyFromString(key, m_displayName, ppropvar);
        }

        return result;
    }


} // namespace Win32xx

#endif  // _WIN32XX_RIBBON_H_

