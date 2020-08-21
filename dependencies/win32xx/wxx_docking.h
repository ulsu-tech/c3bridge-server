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


// Acknowledgements:
// ================
// A special thanks to James Brown and Jeff Glatt for publishing tutorials
// on docking and making these freely available. This code uses some of the
// ideas presented in those tutorials.
//
// Catch22 Docking Tutorials by James Brown:
// http://www.catch22.net/tuts
//
// Docking Toolbars in Plain C by Jeff Glatt
// http://www.codeproject.com/Articles/10224/Docking-Toolbars-in-Plain-C


///////////////////////////////////////////////////////
// wxx_docking.h
//  Declaration of the CDocker class

#ifndef _WIN32XX_DOCKING_H_
#define _WIN32XX_DOCKING_H_


#include "wxx_wincore.h"
#include "wxx_toolbar.h"
#include "wxx_tab.h"
#include "wxx_regkey.h"
#include "wxx_themes.h"
#include "default_resource.h"


// Ensure these are defined
#ifndef GA_ROOT
#define GA_ROOT                 2
#endif

#ifndef WM_UNINITMENUPOPUP
#define WM_UNINITMENUPOPUP      0x0125
#endif

#ifndef WM_MENURBUTTONUP
#define WM_MENURBUTTONUP        0x0122
#endif


// Docking Styles
#define DS_DOCKED_LEFT          0x0001  // Dock the child left
#define DS_DOCKED_RIGHT         0x0002  // Dock the child right
#define DS_DOCKED_TOP           0x0004  // Dock the child top
#define DS_DOCKED_BOTTOM        0x0008  // Dock the child bottom
#define DS_NO_DOCKCHILD_LEFT    0x0010  // Prevent a child docking left
#define DS_NO_DOCKCHILD_RIGHT   0x0020  // Prevent a child docking right
#define DS_NO_DOCKCHILD_TOP     0x0040  // Prevent a child docking at the top
#define DS_NO_DOCKCHILD_BOTTOM  0x0080  // Prevent a child docking at the bottom
#define DS_NO_RESIZE            0x0100  // Prevent resizing
#define DS_NO_CAPTION           0x0200  // Prevent display of caption when docked
#define DS_NO_CLOSE             0x0400  // Prevent closing of a docker while docked
#define DS_NO_UNDOCK            0x0800  // Prevent undocking of a docker
#define DS_CLIENTEDGE           0x1000  // Has a 3D border when docked
#define DS_NO_FIXED_RESIZE      0x2000  // Perform a proportional resize instead of a fixed size resize on dock children
#define DS_DOCKED_CONTAINER     0x4000  // Dock a container within a container
#define DS_DOCKED_LEFTMOST      0x10000 // Leftmost outer docking
#define DS_DOCKED_RIGHTMOST     0x20000 // Rightmost outer docking
#define DS_DOCKED_TOPMOST       0x40000 // Topmost outer docking
#define DS_DOCKED_BOTTOMMOST    0x80000 // Bottommost outer docking

// Required for Dev-C++
#ifndef TME_NONCLIENT
  #define TME_NONCLIENT 0x00000010
#endif
#ifndef TME_LEAVE
  #define TME_LEAVE 0x000000002
#endif
#ifndef WM_NCMOUSELEAVE
  #define WM_NCMOUSELEAVE 0x000002A2
#endif


namespace Win32xx
{
    // Class declarations
    class CDockContainer;
    class CDocker;

    typedef Shared_Ptr<CDocker> DockPtr;

    struct ContainerInfo
    {
        CString title;
        int image;
        CDockContainer* pContainer;
        ContainerInfo() : image(0), pContainer(0) {}
    };

    ///////////////////////////////////////
    // Declaration of the CDockContainer class
    //  A CDockContainer is a CTab window. A CTab has a view window, and optionally a toolbar control.
    //  A top level CDockContainer can contain other CDockContainers. The view for each container
    //  (including the top level container) along with possibly its toolbar, is displayed
    //  within the container parent's view page.
    class CDockContainer : public CTab
    {
    public:

        // Nested class. This is the Wnd for the window displayed over the client area
        // of the tab control.  The toolbar and view window are child windows of the
        // viewpage window. Only the ViewPage of the parent CDockContainer is displayed. It's
        // contents are updated with the view window of the relevant container whenever
        // a different tab is selected.
        class CViewPage : public CWnd
        {

        public:
            CViewPage() : m_pContainer(NULL), m_pView(NULL), m_pTab(NULL) {}
            virtual ~CViewPage() {}
            virtual CToolBar& GetToolBar() const    {return m_toolBar;}
            virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam);
            virtual int OnCreate(CREATESTRUCT& cs);
            virtual LRESULT OnNotify(WPARAM wparam, LPARAM lparam);
            virtual void PreRegisterClass(WNDCLASS& wc);
            virtual void RecalcLayout();
            LRESULT WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam);

            CWnd* GetTabCtrl() const                        { return m_pTab;}
            CWnd* GetView() const { return m_pView; }
            CDockContainer* GetContainer() const;
            void SetContainer(CDockContainer* pContainer)   { m_pContainer = pContainer; }
            void SetView(CWnd& wndView);

        private:
            mutable CToolBar m_toolBar;
            CDockContainer* m_pContainer;
            CString m_tooltip;
            CWnd* m_pView;
            CWnd* m_pTab;
        };

    public:
        CDockContainer();
        virtual ~CDockContainer();
        virtual void AddContainer(CDockContainer* pContainer, BOOL insert = FALSE, BOOL selecPage = TRUE);
        virtual void AddToolBarButton(UINT id, BOOL isEnabled = TRUE);
        virtual CDockContainer* GetContainerFromIndex(UINT page);
        virtual CDockContainer* GetContainerFromView(CWnd* pView) const;
        virtual int GetContainerIndex(CDockContainer* pContainer);
        virtual SIZE GetMaxTabTextSize();
        virtual CViewPage& GetViewPage() const  { return m_viewPage; }
        virtual int GetTabImageID(UINT tab) const;
        virtual CString GetTabText(UINT tab) const;
        virtual void RecalcLayout();
        virtual void RemoveContainer(CDockContainer* pWnd, BOOL updateParent = TRUE);
        virtual void SelectPage(int page);
        virtual void SetTabSize();
        virtual void SetupToolBar();
        virtual void SwapTabs(UINT tab1, UINT tab2);

        // Attributes
        CDockContainer* GetActiveContainer() const;
        CWnd* GetActiveView() const;
        const std::vector<ContainerInfo>& GetAllContainers() const {return m_pContainerParent->m_allInfo;}
        CDockContainer* GetContainerParent() const { return m_pContainerParent; }
        const CString& GetDockCaption() const { return m_caption; }
        CDocker* GetDocker() const      { return m_pDocker; }
        HICON GetTabIcon() const        { return m_tabIcon; }
        LPCTSTR GetTabText() const      { return m_tabText; }
        virtual CToolBar& GetToolBar()  const { return GetViewPage().GetToolBar(); }
        std::vector<UINT>& GetToolBarData() { return m_toolBarData; }
        CWnd* GetView() const           { return GetViewPage().GetView(); }
        void SetActiveContainer(CDockContainer* pContainer);
        void SetDockCaption(LPCTSTR pCaption) { m_caption = pCaption; }
        void SetHideSingleTab(BOOL hideSingle);
        void SetTabIcon(HICON tabIcon) { m_tabIcon = tabIcon; }
        void SetTabIcon(UINT iconID);
        void SetTabIcon(int i, HICON icon) { CTab::SetTabIcon(i, icon); }
        void SetTabText(LPCTSTR pText) { m_tabText = pText; }
        void SetTabText(UINT tab, LPCTSTR pText);
        void SetView(CWnd& wnd);


    protected:
        virtual void OnAttach();
        virtual LRESULT OnLButtonDown(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnLButtonUp(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnMouseLeave(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnMouseMove(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnNotifyReflect(WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnSetFocus(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnSize(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnTCNSelChange(LPNMHDR pNMHDR);
        virtual void PreCreate(CREATESTRUCT& cs);
        virtual void SetToolBarImages(COLORREF mask, UINT normalID, UINT hotID, UINT disabledID);
        LRESULT WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam);

    private:
        CDockContainer(const CDockContainer&);              // Disable copy construction
        CDockContainer& operator = (const CDockContainer&); // Disable assignment operator

        std::vector<ContainerInfo>& GetAll() const {return m_pContainerParent->m_allInfo;}
        std::vector<ContainerInfo> m_allInfo;          // vector of ContainerInfo structs
        std::vector<UINT> m_toolBarData;               // vector of resource IDs for ToolBar buttons
        CString m_tabText;
        mutable CString m_caption;
        CImageList m_normalImages;
        CImageList m_hotImages;
        CImageList m_disabledImages;
        mutable CViewPage m_viewPage;
        int m_currentPage;
        CDocker* m_pDocker;
        CDockContainer* m_pContainerParent;
        HICON m_tabIcon;
        int m_pressedTab;
        BOOL m_isHideSingleTab;
        CPoint m_oldMousePos;
    };

    typedef struct DRAGPOS
    {
        NMHDR hdr;
        POINT pos;
        UINT dockZone;
        CDocker* pDocker;
    } *LPDRAGPOS;


    /////////////////////////////////////////
    // Declaration of the CDocker class
    //  A CDocker window allows other CDocker windows to be "docked" inside it.
    //  A CDocker can dock on the top, left, right or bottom side of a parent CDocker.
    //  There is no theoretical limit to the number of CDockers within CDockers.
    class CDocker : public CWnd
    {
    public:
        //  A nested class for the splitter bar that separates the docked panes.
        class CDockBar : public CWnd
        {
        public:
            CDockBar();
            virtual ~CDockBar();
            virtual void OnDraw(CDC& dc);
            virtual void PreCreate(CREATESTRUCT& cs);
            virtual void PreRegisterClass(WNDCLASS& wc);
            virtual void SendNotify(UINT messageID);
            virtual void SetColor(COLORREF color);
            LRESULT WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam);

            CBrush GetBrushBkgnd() const    {return m_brBackground;}
            CDocker& GetDocker() const      {assert (m_pDocker); return *m_pDocker;}
            int GetWidth() const            {return m_dockBarWidth;}
            void SetDocker(CDocker& Docker) {m_pDocker = &Docker;}
            void SetWidth(int width)        {m_dockBarWidth = width;}

        protected:
            virtual LRESULT OnLButtonDown(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnLButtonUp(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnMouseMove(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnSetCursor(UINT msg, WPARAM wparam, LPARAM lparam);

        private:
            CDockBar(const CDockBar&);              // Disable copy construction
            CDockBar& operator = (const CDockBar&); // Disable assignment operator

            CDocker* m_pDocker;
            DRAGPOS m_dragPos;
            CBrush m_brBackground;
            int m_dockBarWidth;
        };

        //  A nested class for the window inside a CDocker which includes all of this docked client.
        //  It's the remaining part of the CDocker that doesn't belong to the CDocker's children.
        //  The Docker's view window is a child window of CDockClient.
        class CDockClient : public CWnd
        {
        public:
            CDockClient();
            virtual ~CDockClient() {}
            virtual void Draw3DBorder(const RECT& rect);
            virtual void DrawCaption();
            virtual void DrawCloseButton(CDC& drawDC, BOOL focus);
            virtual CRect GetCloseRect() const;
            virtual void SendNotify(UINT messageID);

            const CString& GetCaption() const     { return m_caption; }
            CWnd& GetView() const                 { assert (m_pView); return *m_pView; }
            void SetDocker(CDocker* pDocker)      { m_pDocker = pDocker;}
            void SetCaption(LPCTSTR pCaption)     { m_caption = pCaption; }
            void SetCaptionColors(COLORREF foregnd1, COLORREF backgnd1, COLORREF foreGnd2, COLORREF backGnd2, COLORREF penColor);
            void SetView(CWnd& wndView);

        protected:
            virtual LRESULT OnLButtonDown(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnLButtonUp(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnMouseMove(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnNCCalcSize(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnNCHitTest(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnNCLButtonDblClk(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnNCLButtonDown(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnNCMouseLeave(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnNCMouseMove(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnNCPaint(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual LRESULT OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam);
            virtual void    PreRegisterClass(WNDCLASS& wc);
            virtual void    PreCreate(CREATESTRUCT& cs);
            LRESULT WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam);

        private:
            LRESULT MouseMove(UINT msg, WPARAM wparam, LPARAM lparam);
            CDockClient(const CDockClient&);                // Disable copy construction
            CDockClient& operator = (const CDockClient&);   // Disable assignment operator

            mutable CString m_caption;
            CPoint m_oldPoint;
            CDocker* m_pDocker;
            CWnd* m_pView;
            BOOL m_isClosePressed;
            BOOL m_isOldFocusStored;
            BOOL m_isCaptionPressed;
            BOOL m_isTracking;
            COLORREF m_foregnd1;
            COLORREF m_backgnd1;
            COLORREF m_foregnd2;
            COLORREF m_backgnd2;
            COLORREF m_penColor;
        };

        //  This nested class is used to indicate where a window could dock by
        //  displaying a blue tinted window.
        class CDockHint : public CWnd
        {
        public:
            CDockHint();
            virtual ~CDockHint();
            virtual RECT CalcHintRectContainer(CDocker* pDockTarget);
            virtual RECT CalcHintRectInner(CDocker* pDockTarget, CDocker* pDockDrag, UINT dockSide);
            virtual RECT CalcHintRectOuter(CDocker* pDockDrag, UINT dockSide);
            virtual void DisplayHint(CDocker* pDockTarget, CDocker* pDockDrag, UINT dockSide);
            virtual void OnDraw(CDC& dc);
            virtual void PreCreate(CREATESTRUCT& cs);

        private:
            CDockHint(const CDockHint&);                // Disable copy construction
            CDockHint& operator = (const CDockHint&); // Disable assignment operator

            CBitmap m_bmBlueTint;
            UINT m_uDockSideOld;
        };

        class CTarget : public CWnd
        {
        public:
            CTarget() {}
            virtual ~CTarget();
            virtual void OnDraw(CDC& dc);
            virtual void PreCreate(CREATESTRUCT& cs);

        protected:
            CBitmap m_bmImage;

        private:
            CTarget(const CTarget&);                // Disable copy construction
            CTarget& operator = (const CTarget&); // Disable assignment operator
        };

        class CTargetCentre : public CTarget
        {
        public:
            CTargetCentre();
            virtual ~CTargetCentre();
            virtual void OnDraw(CDC& dc);
            virtual int  OnCreate(CREATESTRUCT& cs);
            virtual BOOL CheckTarget(LPDRAGPOS pDragPos);
            BOOL IsOverContainer() { return m_IsOverContainer; }

        private:
            CTargetCentre(const CTargetCentre&);                // Disable copy construction
            CTargetCentre& operator = (const CTargetCentre&); // Disable assignment operator

            BOOL m_IsOverContainer;
            CDocker* m_pOldDockTarget;
        };

        class CTargetLeft : public CTarget
        {
        public:
            CTargetLeft() {m_bmImage.LoadBitmap(IDW_SDLEFT);}
            virtual BOOL CheckTarget(LPDRAGPOS pDragPos);

        private:
            CTargetLeft(const CTargetLeft&);                // Disable copy construction
            CTargetLeft& operator = (const CTargetLeft&); // Disable assignment operator
        };

        class CTargetTop : public CTarget
        {
        public:
            CTargetTop() {m_bmImage.LoadBitmap(IDW_SDTOP);}
            virtual BOOL CheckTarget(LPDRAGPOS pDragPos);
        private:
            CTargetTop(const CTargetTop&);              // Disable copy construction
            CTargetTop& operator = (const CTargetTop&); // Disable assignment operator
        };

        class CTargetRight : public CTarget
        {
        public:
            CTargetRight() {m_bmImage.LoadBitmap(IDW_SDRIGHT);}
            virtual BOOL CheckTarget(LPDRAGPOS pDragPos);

        private:
            CTargetRight(const CTargetRight&);              // Disable copy construction
            CTargetRight& operator = (const CTargetRight&); // Disable assignment operator
        };

        class CTargetBottom : public CTarget
        {
        public:
            CTargetBottom() {m_bmImage.LoadBitmap(IDW_SDBOTTOM);}
            virtual BOOL CheckTarget(LPDRAGPOS pDragPos);
        };

        friend class CTargetCentre;
        friend class CTargetLeft;
        friend class CTargetTop;
        friend class CTargetRight;
        friend class CTargetBottom;
        friend class CDockClient;
        friend class CDockContainer;

    public:
        // Operations
        CDocker();
        virtual ~CDocker();
        virtual CDocker* AddDockedChild(CDocker* pDocker, DWORD dockStyle, int dockSize, int dockID = 0);
        virtual CDocker* AddUndockedChild(CDocker* pDocker, DWORD dockStyle, int dockSize, const RECT& rc, int dockID = 0);
        virtual void Close();
        virtual void CloseAllDockers();
        virtual void Dock(CDocker* pDocker, UINT dockSide);
        virtual void DockInContainer(CDocker* pDocker, DWORD dockStyle, BOOL selectPage = TRUE);
        virtual CDockContainer* GetContainer() const;
        virtual CDocker* GetActiveDocker() const;
        virtual CWnd*    GetActiveView() const;
        virtual CDocker* GetDockAncestor() const;
        virtual CDocker* GetDockFromID(int dockID) const;
        virtual CDocker* GetDockFromView(CWnd* pView) const;
        virtual CDocker* GetTopmostDocker() const;
        virtual int GetDockSize() const;
        virtual CTabbedMDI* GetTabbedMDI() const;
        virtual int GetTextHeight();
        virtual void Hide();
        virtual BOOL LoadContainerRegistrySettings(LPCTSTR pRegistryKeyName);
        virtual BOOL LoadDockRegistrySettings(LPCTSTR pRegistryKeyName);
        virtual void RecalcDockLayout();
        virtual BOOL SaveDockRegistrySettings(LPCTSTR pRegistryKeyName);
        virtual void SaveContainerRegistrySettings(CRegKey& keyDock, CDockContainer* pContainer, UINT& container);
        virtual void Undock(CPoint pt, BOOL showUndocked = TRUE);
        virtual void UndockContainer(CDockContainer* pContainer, CPoint pt, BOOL showUndocked);
        virtual BOOL VerifyDockers();

        // Attributes
        virtual CDockBar& GetDockBar() const        { return m_dockBar; }
        virtual CDockClient& GetDockClient() const  { return m_dockClient; }
        virtual CDockHint& GetDockHint() const      { return m_pDockAncestor->m_dockHint; }
        virtual CRect GetViewRect() const           { return GetClientRect(); }

        virtual CWnd& GetView() const                       { return GetDockClient().GetView(); }
        virtual void SetView(CWnd& wndView);

        const std::vector <DockPtr> & GetAllDockChildren() const    {return GetDockAncestor()->m_allDockChildren;}
        const std::vector <CDocker*> & GetDockChildren() const      {return m_dockChildren;}
        const std::vector <CDocker*> & GetAllDockers()  const       {return m_allDockers;}
        int GetBarWidth() const             {return GetDockBar().GetWidth();}
        const CString& GetCaption() const         {return GetDockClient().GetCaption();}
        int GetDockID() const               {return m_dockID;}
        CDocker* GetDockParent() const      {return m_pDockParent;}
        DWORD GetDockStyle() const          {return m_dockStyle;}
        BOOL IsChildOfDocker(HWND wnd) const;
        BOOL IsDocked() const;
        BOOL IsDragAutoResize() const;
        BOOL IsRelated(HWND wnd) const;
        BOOL IsUndocked() const;
        BOOL IsUndockable() const;
        void SetBarColor(COLORREF color) {GetDockBar().SetColor(color);}
        void SetBarWidth(int width) {GetDockBar().SetWidth(width);}
        void SetCaption(LPCTSTR pCaption);
        void SetCaptionColors(COLORREF foregnd1, COLORREF backgnd1, COLORREF foreGnd2, COLORREF backGnd2, COLORREF penColor = RGB(160, 150, 140));
        void SetCaptionHeight(int height);
        void SetDockStyle(DWORD dockStyle);
        void SetDockSize(int dockSize);
        void SetDragAutoResize(BOOL autoResize);
        BOOL SetRedraw(BOOL redraw = TRUE);

    protected:
        virtual CDocker* NewDockerFromID(int dockID);
        virtual int  OnCreate(CREATESTRUCT& cs);
        virtual void OnDestroy();
        virtual LRESULT OnBarEnd(LPDRAGPOS pDragPos);
        virtual LRESULT OnBarMove(LPDRAGPOS pDragPos);
        virtual LRESULT OnBarStart(LPDRAGPOS pDragPos);
        virtual LRESULT OnDockEnd(LPDRAGPOS pDragPos);
        virtual LRESULT OnDockMove(LPDRAGPOS pDragPos);
        virtual LRESULT OnDockStart(LPDRAGPOS pDragPos);
        virtual LRESULT OnNotify(WPARAM wparam, LPARAM lparam);
        virtual void PreCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

        // Not intended to be overwritten
        LRESULT WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam);

        // Current declarations of message handlers
        virtual LRESULT OnActivate(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnDockActivated(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnDockDestroyed(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnExitSizeMove(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnMouseActivate(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnNCLButtonDblClk(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnSize(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnSysColorChange(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnSysCommand(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnWindowPosChanging(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam);

    private:
        CDocker(const CDocker&);                // Disable copy construction
        CDocker& operator = (const CDocker&);   // Disable assignment operator
        std::vector <DockPtr> & GetAllChildren() const {return GetDockAncestor()->m_allDockChildren;}
        virtual CDocker* GetDockUnderDragPoint(POINT pt);
        void CheckAllTargets(LPDRAGPOS pDragPos);
        void CloseAllTargets();
        void DockOuter(CDocker* pDocker, DWORD dockStyle);
        void DrawAllCaptions();
        void DrawHashBar(HWND bar, POINT pos);
        void ConvertToChild(HWND hWndParent);
        void ConvertToPopup(const RECT& rc, BOOL showUndocked);
        void MoveDockChildren(CDocker* pDockTarget);
        void PromoteFirstChild();
        void RecalcDockChildLayout(CRect& rc);
        void ResizeDockers(LPDRAGPOS pDragPos);
        CDocker* SeparateFromDock();
        void SendNotify(UINT messageID);
        void SetUndockPosition(CPoint pt, BOOL showUndocked);
        std::vector<CDocker*> SortDockers();
        static BOOL CALLBACK EnumWindowsProc(HWND hTop, LPARAM lparam);

        mutable CDockBar        m_dockBar;
        mutable CDockHint       m_dockHint;
        mutable CDockClient     m_dockClient;
        CTargetCentre   m_targetCentre;
        CTargetLeft     m_targetLeft;
        CTargetTop      m_targetTop;
        CTargetRight    m_targetRight;
        CPoint          m_oldPoint;
        CTargetBottom   m_targetBottom;
        CDocker*        m_pDockParent;
        CDocker*        m_pDockAncestor;

        std::vector <CDocker*> m_dockChildren;     // Docker's immediate children
        std::vector <DockPtr> m_allDockChildren;   // All descendants of the DockAncestor (only used by the DockAncestor)
        std::vector <CDocker*> m_allDockers;       // DockAncestor + all descendants (only used by the DockAncestor)

        CRect m_barRect;
        CRect m_childRect;

        BOOL m_isBlockMove;
        BOOL m_isUndocking;
        BOOL m_isClosing;
        BOOL m_isDragging;
        BOOL m_isDragAutoResize;
        int m_dockStartSize;
        int m_dockID;
        int m_redrawCount;
        int m_ncHeight;
        DWORD m_dockZone;
        double m_dockSizeRatio;
        DWORD m_dockStyle;
        HWND m_dockUnderPoint;
        CPoint m_dockPoint;

    }; // class CDocker

    struct DockInfo
    {
        DWORD dockStyle;
        int dockSize;
        int dockID;
        int dockParentID;
        BOOL isInAncestor;
        RECT rect;
    };

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace Win32xx
{

    /////////////////////////////////////////////////////////////
    // Definitions for the CDockBar class nested within CDocker
    //
    inline CDocker::CDockBar::CDockBar() : m_pDocker(NULL), m_dockBarWidth(4)
    {
        ZeroMemory(&m_dragPos, sizeof(m_dragPos));
    }

    inline CDocker::CDockBar::~CDockBar()
    {
    }

    inline void CDocker::CDockBar::OnDraw(CDC& dc)
    {
        CRect rcClient = GetClientRect();
        dc.SelectObject(m_brBackground);
        dc.PatBlt(0, 0, rcClient.Width(), rcClient.Height(), PATCOPY);
    }

    inline LRESULT CDocker::CDockBar::OnLButtonDown(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (!(m_pDocker->GetDockStyle() & DS_NO_RESIZE))
        {
            SendNotify(UWN_BARSTART);
            SetCapture();
        }

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockBar::OnLButtonUp(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (!(m_pDocker->GetDockStyle() & DS_NO_RESIZE) && (GetCapture() == *this))
        {
            SendNotify(UWN_BAREND);
            ReleaseCapture();
        }

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockBar::OnMouseMove(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (!(m_pDocker->GetDockStyle() & DS_NO_RESIZE) && (GetCapture() == *this))
        {
            SendNotify(UWN_BARMOVE);
        }

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockBar::OnSetCursor(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (!(m_pDocker->GetDockStyle() & DS_NO_RESIZE))
        {
            HCURSOR cursor;
            DWORD side = GetDocker().GetDockStyle() & 0xF;
            if ((side == DS_DOCKED_LEFT) || (side == DS_DOCKED_RIGHT))
                cursor = GetApp()->LoadCursor(IDW_SPLITH);
            else
                cursor = GetApp()->LoadCursor(IDW_SPLITV);

            if (cursor) SetCursor(cursor);
            else TRACE("**WARNING** Missing cursor resource for slider bar\n");

            return TRUE;
        }
        else
            SetCursor(LoadCursor(NULL, IDC_ARROW));

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline void CDocker::CDockBar::PreCreate(CREATESTRUCT& cs)
    {
        // Create a child window, initially hidden
        cs.style = WS_CHILD;
    }

    inline void CDocker::CDockBar::PreRegisterClass(WNDCLASS& wc)
    {
        wc.lpszClassName = _T("Win32++ Bar");
        wc.hbrBackground = m_brBackground;
    }

    inline void CDocker::CDockBar::SendNotify(UINT messageID)
    {
        // Send a splitter bar notification to the parent
        m_dragPos.hdr.code = messageID;
        m_dragPos.hdr.hwndFrom = GetHwnd();
        m_dragPos.pos = GetCursorPos();
        m_dragPos.pos.x += 1;
        m_dragPos.pDocker = m_pDocker;
        GetParent().SendMessage(WM_NOTIFY, 0, (LPARAM)&m_dragPos);
    }

    inline void CDocker::CDockBar::SetColor(COLORREF color)
    {
        // Useful colors:
        // GetSysColor(COLOR_BTNFACE)   // Default Grey
        // RGB(196, 215, 250)           // Default Blue

        m_brBackground.DeleteObject();
        m_brBackground.CreateSolidBrush(color);
    }

    inline LRESULT CDocker::CDockBar::WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_SETCURSOR:      return OnSetCursor(msg, wparam, lparam);
        case WM_ERASEBKGND:     return 0;
        case WM_LBUTTONDOWN:    return OnLButtonDown(msg, wparam, lparam);
        case WM_LBUTTONUP:      return OnLButtonUp(msg, wparam, lparam);
        case WM_MOUSEMOVE:      return OnMouseMove(msg, wparam, lparam);
        }

        // pass unhandled messages on for default processing
        return CWnd::WndProcDefault(msg, wparam, lparam);
    }


    ////////////////////////////////////////////////////////////////
    // Definitions for the CDockClient class nested within CDocker
    //
    inline CDocker::CDockClient::CDockClient() : m_pDocker(0), m_pView(0), m_isClosePressed(FALSE),
                        m_isOldFocusStored(FALSE), m_isCaptionPressed(FALSE), m_isTracking(FALSE)
    {
        m_foregnd1 = RGB(32,32,32);
        m_backgnd1 = RGB(190,207,227);
        m_foregnd2 = GetSysColor(COLOR_BTNTEXT);
        m_backgnd2 = GetSysColor(COLOR_BTNFACE);
        m_penColor = RGB(160, 150, 140);
    }

    inline void CDocker::CDockClient::Draw3DBorder(const RECT& rect)
    {
        // Imitates the drawing of the WS_EX_CLIENTEDGE extended style
        // This draws a 2 pixel border around the specified Rect
        CWindowDC dc(*this);
        CRect rcw = rect;
        dc.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
        dc.MoveTo(0, rcw.Height());
        dc.LineTo(0, 0);
        dc.LineTo(rcw.Width(), 0);
        dc.CreatePen(PS_SOLID,1, GetSysColor(COLOR_3DDKSHADOW));
        dc.MoveTo(1, rcw.Height()-2);
        dc.LineTo(1, 1);
        dc.LineTo(rcw.Width()-2, 1);
        dc.CreatePen(PS_SOLID,1, GetSysColor(COLOR_3DHILIGHT));
        dc.MoveTo(rcw.Width()-1, 0);
        dc.LineTo(rcw.Width()-1, rcw.Height()-1);
        dc.LineTo(0, rcw.Height()-1);
        dc.CreatePen(PS_SOLID,1, GetSysColor(COLOR_3DLIGHT));
        dc.MoveTo(rcw.Width()-2, 1);
        dc.LineTo(rcw.Width()-2, rcw.Height()-2);
        dc.LineTo(1, rcw.Height()-2);
    }

    inline CRect CDocker::CDockClient::GetCloseRect() const
    {
        // Calculate the close rect position in screen co-ordinates
        CRect rcClose;

        int gap = 4;
        CRect rc = GetWindowRect();
        int cx = GetSystemMetrics(SM_CXSMICON);
        int cy = GetSystemMetrics(SM_CYSMICON);

        rcClose.top = 2 + rc.top + m_pDocker->m_ncHeight/2 - cy/2;
        rcClose.bottom = 2 + rc.top + m_pDocker->m_ncHeight/2 + cy/2;
        rcClose.right = rc.right - gap;
        rcClose.left = rcClose.right - cx;

#if (WINVER >= 0x0500)
        if (GetExStyle() & WS_EX_LAYOUTRTL)
        {
            rcClose.left = rc.left + gap;
            rcClose.right = rcClose.left + cx;
        }
#endif


        return rcClose;
    }

    inline void CDocker::CDockClient::DrawCaption()
    {

        if (IsWindow() && !(m_pDocker->GetDockStyle() & DS_NO_CAPTION))
        {
            if (m_pDocker->IsUndockable())
            {
                BOOL Focus = m_pDocker->IsChildOfDocker(GetFocus());
                m_isOldFocusStored = FALSE;

                // Acquire the DC for our NonClient painting
                CWindowDC dc(*this);

                // Create and set up our memory DC
                CRect rc = GetWindowRect();
                CMemDC dcMem(dc);
                int rcAdjust = (GetExStyle() & WS_EX_CLIENTEDGE) ? 2 : 0;
                int Width = MAX(rc.Width() - rcAdjust, 0);

                int Height = m_pDocker->m_ncHeight + rcAdjust;
                dcMem.CreateCompatibleBitmap(dc, Width, Height);
                m_isOldFocusStored = Focus;

                // Set the font for the title
                NONCLIENTMETRICS info = GetNonClientMetrics();
                dcMem.CreateFontIndirect(info.lfStatusFont);

                // Set the Colours
                if (m_pDocker->GetActiveDocker() == m_pDocker)
                {
                    dcMem.SetTextColor(m_foregnd1);
                    dcMem.CreateSolidBrush(m_backgnd1);
                    dcMem.SetBkColor(m_backgnd1);
                }
                else
                {
                    dcMem.SetTextColor(m_foregnd2);
                    dcMem.CreateSolidBrush(m_backgnd2);
                    dcMem.SetBkColor(m_backgnd2);
                }

                // Draw the rectangle
                dcMem.CreatePen(PS_SOLID, 1, m_penColor);
                dcMem.Rectangle(rcAdjust, rcAdjust, rc.Width() - rcAdjust, m_pDocker->m_ncHeight + rcAdjust);

                // Display the caption
                int cx = (m_pDocker->GetDockStyle() & DS_NO_CLOSE) ? 0 : GetSystemMetrics(SM_CXSMICON);
                CRect rcText(4 + rcAdjust, rcAdjust, rc.Width() - 4 - cx - rcAdjust, m_pDocker->m_ncHeight + rcAdjust);
                dcMem.DrawText(m_caption, m_caption.GetLength(), rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

                // Draw the close button
                if (!(m_pDocker->GetDockStyle() & DS_NO_CLOSE))
                    DrawCloseButton(dcMem, Focus);

                // Draw the 3D border
                if (GetExStyle() & WS_EX_CLIENTEDGE)
                    Draw3DBorder(rc);

                // Copy the Memory DC to the window's DC
                dc.BitBlt(rcAdjust, rcAdjust, Width, Height, dcMem, rcAdjust, rcAdjust, SRCCOPY);
            }
        }
    }

    inline void CDocker::CDockClient::DrawCloseButton(CDC& drawDC, BOOL focus)
    {
        // The close button isn't displayed on Win95
        if (GetWinVersion() == 1400)  return;

        if (m_pDocker->IsUndockable() && !(m_pDocker->GetDockStyle() & DS_NO_CAPTION))
        {
            CDockContainer* pContainer = m_pDocker->GetContainer();
            if ((m_pDocker->IsDocked()) || ((m_pDocker == m_pDocker->GetDockAncestor()) && pContainer && pContainer->GetItemCount() > 0))
            {
                // Determine the close button's drawing position relative to the window
                CRect rcClose = GetCloseRect();
                UINT uState = GetCloseRect().PtInRect(GetCursorPos()) ? m_isClosePressed && IsLeftButtonDown() ? 2 : 1 : 0;
                ScreenToClient(rcClose);

                if (GetExStyle() & WS_EX_CLIENTEDGE)
                {
                    rcClose.OffsetRect(2, m_pDocker->m_ncHeight + 2);
                    if (GetWindowRect().Height() < (m_pDocker->m_ncHeight + 4))
                        rcClose.OffsetRect(-2, -2);
                }
                else
                    rcClose.OffsetRect(0, m_pDocker->m_ncHeight - 2);

                // Draw the outer highlight for the close button
                if (!IsRectEmpty(&rcClose))
                {
                    switch (uState)
                    {
                    case 0:
                    {
                        // Normal button
                        drawDC.CreatePen(PS_SOLID, 1, RGB(232, 228, 220));
                        drawDC.MoveTo(rcClose.left, rcClose.bottom);
                        drawDC.LineTo(rcClose.right, rcClose.bottom);
                        drawDC.LineTo(rcClose.right, rcClose.top);
                        drawDC.LineTo(rcClose.left, rcClose.top);
                        drawDC.LineTo(rcClose.left, rcClose.bottom);
                        break;
                    }

                    case 1:
                    {
                        // Popped up button
                        // Draw outline, white at top, black on bottom
                        drawDC.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                        drawDC.MoveTo(rcClose.left, rcClose.bottom);
                        drawDC.LineTo(rcClose.right, rcClose.bottom);
                        drawDC.LineTo(rcClose.right, rcClose.top);
                        drawDC.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                        drawDC.LineTo(rcClose.left, rcClose.top);
                        drawDC.LineTo(rcClose.left, rcClose.bottom);
                    }

                    break;
                    case 2:
                    {
                        // Pressed button
                        // Draw outline, black on top, white on bottom
                        drawDC.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                        drawDC.MoveTo(rcClose.left, rcClose.bottom);
                        drawDC.LineTo(rcClose.right, rcClose.bottom);
                        drawDC.LineTo(rcClose.right, rcClose.top);
                        drawDC.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                        drawDC.LineTo(rcClose.left, rcClose.top);
                        drawDC.LineTo(rcClose.left, rcClose.bottom);
                    }
                    break;
                    }

                    // Manually Draw Close Button
                    if (focus)
                        drawDC.CreatePen(PS_SOLID, 1, m_foregnd1);
                    else
                        drawDC.CreatePen(PS_SOLID, 1, m_foregnd2);

                    drawDC.MoveTo(rcClose.left + 3, rcClose.top + 3);
                    drawDC.LineTo(rcClose.right - 2, rcClose.bottom - 2);

                    drawDC.MoveTo(rcClose.left + 4, rcClose.top + 3);
                    drawDC.LineTo(rcClose.right - 2, rcClose.bottom - 3);

                    drawDC.MoveTo(rcClose.left + 3, rcClose.top + 4);
                    drawDC.LineTo(rcClose.right - 3, rcClose.bottom - 2);

                    drawDC.MoveTo(rcClose.right - 3, rcClose.top + 3);
                    drawDC.LineTo(rcClose.left + 2, rcClose.bottom - 2);

                    drawDC.MoveTo(rcClose.right - 3, rcClose.top + 4);
                    drawDC.LineTo(rcClose.left + 3, rcClose.bottom - 2);

                    drawDC.MoveTo(rcClose.right - 4, rcClose.top + 3);
                    drawDC.LineTo(rcClose.left + 2, rcClose.bottom - 3);
                }
            }
        }
    }

    inline LRESULT CDocker::CDockClient::OnNCCalcSize(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Sets the non-client area (and hence sets the client area)
        // This function modifies lparam

        if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CAPTION))
        {
            if (m_pDocker->IsUndockable())
            {
                LPRECT rc = (LPRECT)lparam;
                rc->top += m_pDocker->m_ncHeight;
            }
        }

        return CWnd::WndProcDefault(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnNCHitTest(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Identify which part of the non-client area the cursor is over
        if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CAPTION))
        {
            if (m_pDocker->IsUndockable())
            {
                CPoint pt(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));

                // Indicate if the point is in the close button (except for Win95)
                if ((GetWinVersion() > 1400) && (GetCloseRect().PtInRect(pt)))
                    return HTCLOSE;

                ScreenToClient(pt);

                // Indicate if the point is in the caption
                if (pt.y < 0)
                    return HTCAPTION;
            }
        }
        return CWnd::WndProcDefault(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnNCLButtonDblClk(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CAPTION))
        {
            if ((HTCLOSE == wparam) && !(m_pDocker->GetDockStyle() & DS_NO_CLOSE))
            {
                m_isClosePressed = TRUE;
                SetCapture();
            }

            m_isCaptionPressed = TRUE;
            m_oldPoint.x = GET_X_LPARAM(lparam);
            m_oldPoint.y = GET_Y_LPARAM(lparam);
            if (m_pDocker->IsUndockable())
            {
                // Give the view window focus unless its child already has it
                if (!GetView().IsChild(GetFocus()))
                    m_pDocker->GetView().SetFocus();

                // Update the close button
                if ( !(m_pDocker->GetDockStyle() & DS_NO_CLOSE))
                {
                    CWindowDC dc(*this);
                    DrawCloseButton(dc, m_isOldFocusStored);
                }

                return 0;
            }
        }
        return CWnd::WndProcDefault(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnNCLButtonDown(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CAPTION))
        {
            if ((HTCLOSE == wparam) && !(m_pDocker->GetDockStyle() & DS_NO_CLOSE))
            {
                m_isClosePressed = TRUE;
                SetCapture();
            }

            m_isCaptionPressed = TRUE;
            m_oldPoint.x = GET_X_LPARAM(lparam);
            m_oldPoint.y = GET_Y_LPARAM(lparam);
            if (m_pDocker->IsUndockable())
            {
                // Give the view window focus unless its child already has it
                if (!GetView().IsChild(GetFocus()))
                    m_pDocker->GetView().SetFocus();

                // Update the close button
                if ( !(m_pDocker->GetDockStyle() & DS_NO_CLOSE))
                {
                    CWindowDC dc(*this);
                    DrawCloseButton(dc, m_isOldFocusStored);
                }

                return 0;
            }
        }

        return CWnd::WndProcDefault(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnLButtonUp(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        ReleaseCapture();

        if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CLOSE))
        {
            CWindowDC dc(*this);
            DrawCloseButton(dc, m_isOldFocusStored);
            if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & (DS_NO_CAPTION|DS_NO_CLOSE)))
            {
                m_isCaptionPressed = FALSE;
                if (m_isClosePressed && GetCloseRect().PtInRect(GetCursorPos()))
                {
                    // Destroy the docker
                    if (m_pDocker->GetContainer())
                    {
                        CDockContainer* pContainer = m_pDocker->GetContainer()->GetActiveContainer();
                        if (pContainer)
                        {
                            CDockContainer* pParentC = pContainer->GetContainerParent();
                            CDocker* pDocker = m_pDocker->GetDockFromView(pContainer);

                            assert(pDocker);
                            pDocker->Close();

                            if (pContainer != pParentC)
                                if (pParentC->GetItemCount() == 0)
                                    pParentC->GetDocker()->RecalcDockLayout();

                                if (m_pDocker->IsWindow())
                                    m_pDocker->RecalcDockLayout();
                        }
                    }
                    else
                    {
                        m_pDocker->Close();
                    }
                }
            }
        }

        return CWnd::WndProcDefault(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnLButtonDown(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        m_isClosePressed = FALSE;
        ReleaseCapture();
        CWindowDC dc(*this);
        DrawCloseButton(dc, m_isOldFocusStored);

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::MouseMove(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (!m_isTracking)
        {
            TRACKMOUSEEVENT TrackMouseEventStruct;
            ZeroMemory(&TrackMouseEventStruct, sizeof(TrackMouseEventStruct));
            TrackMouseEventStruct.cbSize = sizeof(TrackMouseEventStruct);
            TrackMouseEventStruct.dwFlags = TME_LEAVE|TME_NONCLIENT;
            TrackMouseEventStruct.hwndTrack = *this;
            _TrackMouseEvent(&TrackMouseEventStruct);
            m_isTracking = TRUE;
        }

        if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CAPTION))
        {
            if (m_pDocker->IsDocked())
            {
                // Discard phantom mouse move messages
                if ( (m_oldPoint.x == GET_X_LPARAM(lparam) ) && (m_oldPoint.y == GET_Y_LPARAM(lparam)))
                    return 0;

                if (IsLeftButtonDown() && (wparam == HTCAPTION)  && (m_isCaptionPressed))
                {
                    assert(m_pDocker);
                    m_pDocker->Undock(GetCursorPos());
                }

                // Update the close button
                if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CLOSE))
                {
                    CWindowDC dc(*this);
                    DrawCloseButton(dc, m_isOldFocusStored);
                }
            }
            else if (m_pDocker->IsUndockable())
            {
                // Discard phantom mouse move messages
                if ((m_oldPoint.x == GET_X_LPARAM(lparam)) && (m_oldPoint.y == GET_Y_LPARAM(lparam)))
                    return 0;

                if (IsLeftButtonDown() && (wparam == HTCAPTION) && (m_isCaptionPressed))
                {
                    CDockContainer* pContainer = m_pDocker->GetContainer();
                    assert(pContainer);

                    CDockContainer* pParent = pContainer->GetContainerParent();
                    CDockContainer* pActive = pContainer->GetActiveContainer();
                    if (pActive)
                    {
                        int lastTab = static_cast<int>(pParent->GetAllContainers().size()) - 1;
                        assert(lastTab >= 0);
                        CDockContainer* pContainerLast = pContainer->GetContainerFromIndex(lastTab);
                        m_pDocker->GetDockAncestor()->UndockContainer(pContainerLast, GetCursorPos(), FALSE);

                        while (pParent->GetAllContainers().size() > 0)
                        {
                            lastTab = static_cast<int>(pParent->GetAllContainers().size()) - 1;
                            assert(lastTab >= 0);
                            CDockContainer* pContainerNext = pContainer->GetContainerFromIndex(lastTab);

                            CDocker* pDocker = pContainerNext->GetDocker();
                            pDocker->Hide();
                            pContainerLast->GetDocker()->DockInContainer(pDocker, pDocker->GetDockStyle());
                        }

                        pContainerLast->SetActiveContainer(pActive);
                        pContainerLast->GetDocker()->ShowWindow(SW_SHOW);
                        pContainerLast->RedrawWindow();
                        m_pDocker->GetDockAncestor()->RecalcDockLayout();
                    }

                }

                // Update the close button
                if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CLOSE))
                {
                    CWindowDC dc(*this);
                    DrawCloseButton(dc, m_isOldFocusStored);
                }
            }

            m_isCaptionPressed = FALSE;
        }

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnMouseMove(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        return MouseMove(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnNCMouseLeave(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        m_isTracking = FALSE;
        CWindowDC dc(*this);
        if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & (DS_NO_CAPTION|DS_NO_CLOSE)) && m_pDocker->IsUndockable())
            DrawCloseButton(dc, m_isOldFocusStored);

        m_isTracking = FALSE;

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnNCMouseMove(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        return MouseMove(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnNCPaint(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if ((0 != m_pDocker) && !(m_pDocker->GetDockStyle() & DS_NO_CAPTION))
        {
            if (m_pDocker->IsUndockable())
            {
                DefWindowProc(WM_NCPAINT, wparam, lparam);
                DrawCaption();
                return 0;
            }
        }
        return FinalWindowProc(msg, wparam, lparam);
    }

    inline LRESULT CDocker::CDockClient::OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Reposition the View window to cover the DockClient's client area
        CRect rc = GetClientRect();
        GetView().SetWindowPos(NULL, rc, SWP_SHOWWINDOW);

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline void CDocker::CDockClient::PreRegisterClass(WNDCLASS& wc)
    {
        wc.lpszClassName = _T("Win32++ DockClient");
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    }

    inline void CDocker::CDockClient::PreCreate(CREATESTRUCT& cs)
    {
        assert(m_pDocker);
        DWORD style = m_pDocker->GetDockStyle();
        if (style & DS_CLIENTEDGE)
            cs.dwExStyle = WS_EX_CLIENTEDGE;

#if (WINVER >= 0x0500)
        if (m_pDocker->GetExStyle() & WS_EX_LAYOUTRTL)
            cs.dwExStyle |= WS_EX_LAYOUTRTL;
#endif

    }

    inline void CDocker::CDockClient::SendNotify(UINT messageID)
    {
        // Fill the DragPos structure with data
        DRAGPOS DragPos;
        DragPos.hdr.code = messageID;
        DragPos.hdr.hwndFrom = GetHwnd();
        DragPos.pos = GetCursorPos();
        DragPos.pDocker = m_pDocker;

        // Send a DragPos notification to the docker
        GetParent().SendMessage(WM_NOTIFY, 0, (LPARAM)&DragPos);
    }

    inline void CDocker::CDockClient::SetCaptionColors(COLORREF foregnd1, COLORREF backgnd1, COLORREF foregnd2, COLORREF backgnd2, COLORREF penColor)
    {
        // Set the colors used when drawing the caption
        // m_Foregnd1 Foreground colour (focused).  m_Backgnd1 Background colour (focused)
        // m_Foregnd2 Foreground colour (not focused). m_Backgnd2 Foreground colour (not focused)
        // m_PenColor Pen color used for drawing the outline.
        m_foregnd1 = foregnd1;
        m_backgnd1 = backgnd1;
        m_foregnd2 = foregnd2;
        m_backgnd2 = backgnd2;
        m_penColor = penColor;
    }

    inline void CDocker::CDockClient::SetView(CWnd& view)
    {
        if (m_pView != &view)
        {
            // Hide the existing view window (if any)
            if (m_pView && m_pView->IsWindow())
            {
                // We can't change docker view if it is a DockContainer
                // Use CDockContainer::SetView to change the DockContainer's view instead
                assert(m_pView->SendMessage(UWM_GETCDOCKCONTAINER) == 0);

                m_pView->ShowWindow(SW_HIDE);
            }

            // Assign the view window
            m_pView = &view;

            if (IsWindow())
            {
                // The docker is already created, so create and position the new view too

                if (!GetView().IsWindow())
                    GetView().Create(*this);
                else
                {
                    GetView().SetParent(*this);
                }

                CRect rc = GetClientRect();
                GetView().SetWindowPos(NULL, rc, SWP_SHOWWINDOW);
            }
        }
    }

    inline LRESULT CDocker::CDockClient::WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_LBUTTONUP:          return OnLButtonUp(msg, wparam, lparam);
        case WM_MOUSEMOVE:          return OnMouseMove(msg, wparam, lparam);
        case WM_NCCALCSIZE:         return OnNCCalcSize(msg, wparam, lparam);
        case WM_NCHITTEST:          return OnNCHitTest(msg, wparam, lparam);
        case WM_NCLBUTTONDBLCLK:    return OnNCLButtonDblClk(msg, wparam, lparam);
        case WM_NCLBUTTONDOWN:      return OnNCLButtonDown(msg, wparam, lparam);
        case WM_NCMOUSEMOVE:        return OnNCMouseMove(msg, wparam, lparam);
        case WM_NCPAINT:            return OnNCPaint(msg, wparam, lparam);
        case WM_NCMOUSELEAVE:       return OnNCMouseLeave(msg, wparam, lparam);
        case WM_NOTIFY:
        {
            // Perform default handling for WM_NOTIFY
            LRESULT result = CWnd::WndProcDefault(msg, wparam, lparam);

            // Also forward WM_NOTIFY to the docker
            if (result == 0)
                result = m_pDocker->SendMessage(msg, wparam, lparam);

            return result;
        }
        case WM_WINDOWPOSCHANGED:   return OnWindowPosChanged(msg, wparam, lparam);
        }

        return CWnd::WndProcDefault(msg, wparam, lparam);
    }


    //////////////////////////////////////////////////////////////
    // Definitions for the CDockHint class nested within CDocker
    //
    inline CDocker::CDockHint::CDockHint() : m_uDockSideOld(0)
    {
    }

    inline CDocker::CDockHint::~CDockHint()
    {
    }

    inline RECT CDocker::CDockHint::CalcHintRectContainer(CDocker* pDockTarget)
    {
        // Calculate the hint window's position for container docking
        CRect rcHint = pDockTarget->GetDockClient().GetWindowRect();
        if (pDockTarget->GetDockClient().GetExStyle() & WS_EX_CLIENTEDGE)
            rcHint.InflateRect(-2, -2);
        pDockTarget->ScreenToClient(rcHint);

        return rcHint;
    }

    inline RECT CDocker::CDockHint::CalcHintRectInner(CDocker* pDockTarget, CDocker* pDockDrag, UINT dockSide)
    {
        assert(pDockTarget);
        assert(pDockDrag);

        BOOL RTL = FALSE;
#if (WINVER >= 0x0500)
        RTL = (pDockTarget->GetExStyle() & WS_EX_LAYOUTRTL);
#endif

        // Calculate the hint window's position for inner docking
        CDockClient* pDockClient = &pDockTarget->GetDockClient();
        CRect rcHint = pDockClient->GetWindowRect();

        if (pDockClient->GetExStyle() & WS_EX_CLIENTEDGE)
            rcHint.InflateRect(-2, -2);
        pDockTarget->ScreenToClient(rcHint);

        int Width;
        CRect rcDockDrag = pDockDrag->GetWindowRect();
        CRect rcDockTarget = pDockClient->GetWindowRect();
        if ((dockSide  == DS_DOCKED_LEFT) || (dockSide  == DS_DOCKED_RIGHT))
        {
            Width = rcDockDrag.Width();
            if (Width >= (rcDockTarget.Width() - pDockDrag->GetBarWidth()))
                Width = MAX(rcDockTarget.Width()/2 - pDockDrag->GetBarWidth(), pDockDrag->GetBarWidth());
        }
        else
        {
            Width = rcDockDrag.Height();
            if (Width >= (rcDockTarget.Height() - pDockDrag->GetBarWidth()))
                Width = MAX(rcDockTarget.Height()/2 - pDockDrag->GetBarWidth(), pDockDrag->GetBarWidth());
        }
        switch (dockSide)
        {
        case DS_DOCKED_LEFT:
            if (RTL)    rcHint.left = rcHint.right - Width;
            else        rcHint.right = rcHint.left + Width;

            break;
        case DS_DOCKED_RIGHT:
            if (RTL)    rcHint.right = rcHint.left + Width;
            else        rcHint.left = rcHint.right - Width;

            break;
        case DS_DOCKED_TOP:
            rcHint.bottom = rcHint.top + Width;
            break;
        case DS_DOCKED_BOTTOM:
            rcHint.top = rcHint.bottom - Width;
            break;
        }

        return rcHint;
    }

    inline RECT CDocker::CDockHint::CalcHintRectOuter(CDocker* pDockDrag, UINT dockSide)
    {
        assert(pDockDrag);

        // Calculate the hint window's position for outer docking
        CDocker* pDockTarget = pDockDrag->GetDockAncestor();
        CRect rcHint = pDockTarget->GetViewRect();
        if (pDockTarget->GetDockClient().GetExStyle() & WS_EX_CLIENTEDGE)
            rcHint.InflateRect(-2, -2);

        int Width;
        CRect rcDockDrag = pDockDrag->GetWindowRect();

        BOOL RTL = FALSE;
#ifdef WS_EX_LAYOUTRTL
        RTL = (pDockTarget->GetExStyle() & WS_EX_LAYOUTRTL);
#endif

        int BarWidth = pDockDrag->GetBarWidth();
        CRect rcDockClient = pDockTarget->GetDockClient().GetClientRect();

        // Limit the docked size to half the parent's size if it won't fit inside parent
        if ((dockSide == DS_DOCKED_LEFTMOST) || (dockSide  == DS_DOCKED_RIGHTMOST))
        {
            Width = rcDockDrag.Width();
            if (Width >= rcDockClient.Width() - BarWidth)
                Width = MAX(rcDockClient.Width()/2 - BarWidth, BarWidth);
        }
        else
        {
            Width = rcDockDrag.Height();
            if (Width >= rcDockClient.Height() - BarWidth)
                Width = MAX(rcDockClient.Height()/2 - BarWidth, BarWidth);
        }
        switch (dockSide)
        {
        case DS_DOCKED_LEFTMOST:
            if (RTL) rcHint.left = rcHint.right - Width;
            else     rcHint.right = rcHint.left + Width;

            break;
        case DS_DOCKED_RIGHTMOST:
            if (RTL) rcHint.right = rcHint.left + Width;
            else     rcHint.left = rcHint.right - Width;

            break;
        case DS_DOCKED_TOPMOST:
            rcHint.bottom = rcHint.top + Width;
            break;
        case DS_DOCKED_BOTTOMMOST:
            rcHint.top = rcHint.bottom - Width;
            break;
        }

        return rcHint;
    }

    inline void CDocker::CDockHint::DisplayHint(CDocker* pDockTarget, CDocker* pDockDrag, UINT dockSide)
    {
        // Ensure a new hint window is created if dock side changes
        if (dockSide != m_uDockSideOld)
        {
            Destroy();
            pDockTarget->RedrawWindow(RDW_NOERASE | RDW_UPDATENOW );
            pDockDrag->RedrawWindow();
        }
        m_uDockSideOld = dockSide;

        if (!IsWindow())
        {
            CRect rcHint;

            if (dockSide & 0xF)
                rcHint = CalcHintRectInner(pDockTarget, pDockDrag, dockSide);
            else if (dockSide & 0xF0000)
                rcHint = CalcHintRectOuter(pDockDrag, dockSide);
            else if (dockSide & DS_DOCKED_CONTAINER)
                rcHint = CalcHintRectContainer(pDockTarget);
            else
                return;

            // Save the Dock window's blue tinted bitmap
            CClientDC dcDesktop(NULL);
            CMemDC dcMem(dcDesktop);
            CRect rcBitmap = rcHint;
            CRect rcTarget = rcHint;
            pDockTarget->ClientToScreen(rcTarget);

            m_bmBlueTint.DeleteObject();
            m_bmBlueTint.CreateCompatibleBitmap(dcDesktop, rcBitmap.Width(), rcBitmap.Height());
            dcMem.SelectObject(m_bmBlueTint);
            dcMem.BitBlt(0, 0, rcBitmap.Width(), rcBitmap.Height(), dcDesktop, rcTarget.left, rcTarget.top, SRCCOPY);
            m_bmBlueTint = dcMem.DetachBitmap();
            m_bmBlueTint.TintBitmap(-64, -24, +128);

            // Create the Hint window
            if (!IsWindow())
            {
                Create(*pDockTarget);
            }

            // Adjust hint shape for container in container docking
            if ((dockSide & DS_DOCKED_CONTAINER) && rcHint.Height() > 50)
            {
                CRgn Rgn;
                Rgn.CreateRectRgn(0, 0, rcHint.Width(), rcHint.Height() -25);
                assert(Rgn.GetHandle());
                CRgn Rgn2;
                Rgn2.CreateRectRgn(5, rcHint.Height() -25, 60, rcHint.Height());
                Rgn.CombineRgn(Rgn2, RGN_OR);
                SetWindowRgn(Rgn, FALSE);
            }

            pDockTarget->ClientToScreen(rcHint);
            SetWindowPos(NULL, rcHint, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOACTIVATE);
        }
    }

    inline void CDocker::CDockHint::OnDraw(CDC& dc)
    {
        // Display the blue tinted bitmap
        CRect rc = GetClientRect();
        CMemDC dcMem(dc);
        dcMem.SelectObject(m_bmBlueTint);
        dc.BitBlt(0, 0, rc.Width(), rc.Height(), dcMem, 0, 0, SRCCOPY);
    }

    inline void CDocker::CDockHint::PreCreate(CREATESTRUCT& cs)
    {
        cs.style = WS_POPUP;

        // WS_EX_TOOLWINDOW prevents the window being displayed on the taskbar
        cs.dwExStyle = WS_EX_TOOLWINDOW;

        cs.lpszClass = _T("Win32++ DockHint");
    }


    ////////////////////////////////////////////////////////////////
    // Definitions for the CTargetCentre class nested within CDocker
    //
    inline CDocker::CTargetCentre::CTargetCentre() : m_IsOverContainer(FALSE), m_pOldDockTarget(0)
    {
    }

    inline CDocker::CTargetCentre::~CTargetCentre()
    {
    }

    inline void CDocker::CTargetCentre::OnDraw(CDC& dc)
    {
        // Load the target bitmaps
        CBitmap bmCentre(IDW_SDCENTER);
        CBitmap bmLeft(IDW_SDLEFT);
        CBitmap bmRight(IDW_SDRIGHT);
        CBitmap bmTop(IDW_SDTOP);
        CBitmap bmBottom(IDW_SDBOTTOM);

        // Grey out invalid dock targets
        DWORD style = m_pOldDockTarget->GetDockStyle();
        if (style & DS_NO_DOCKCHILD_LEFT)  bmLeft.TintBitmap(150, 150, 150);
        if (style & DS_NO_DOCKCHILD_TOP)   bmTop.TintBitmap(150, 150, 150);
        if (style & DS_NO_DOCKCHILD_RIGHT) bmRight.TintBitmap(150, 150, 150);
        if (style & DS_NO_DOCKCHILD_BOTTOM) bmBottom.TintBitmap(150, 150, 150);

        // Draw the dock targets
        if (bmCentre.GetHandle())   dc.DrawBitmap(0, 0, 88, 88, bmCentre, RGB(255,0,255));
        else TRACE("Missing docking resource: Target Centre\n");

        if (bmLeft.GetHandle()) dc.DrawBitmap(0, 29, 31, 29, bmLeft, RGB(255,0,255));
        else TRACE("Missing docking resource: Target Left\n");

        if (bmTop.GetHandle()) dc.DrawBitmap(29, 0, 29, 31, bmTop, RGB(255,0,255));
        else TRACE("Missing docking resource: Target Top\n");

        if (bmRight.GetHandle()) dc.DrawBitmap(55, 29, 31, 29, bmRight, RGB(255,0,255));
        else TRACE("Missing docking resource: Target Right\n");

        if (bmBottom.GetHandle()) dc.DrawBitmap(29, 55, 29, 31, bmBottom, RGB(255,0,255));
        else TRACE("Missing docking resource: Target Bottom\n");

        if (IsOverContainer())
        {
            CBitmap bmMiddle(IDW_SDMIDDLE);
            dc.DrawBitmap(31, 31, 25, 26, bmMiddle, RGB(255,0,255));
        }
    }

    inline int CDocker::CTargetCentre::OnCreate(CREATESTRUCT& cs)
    {
        UNREFERENCED_PARAMETER(cs);

        // Use a region to create an irregularly shapped window
        POINT ptArray[16] = { {0,29}, {22, 29}, {29, 22}, {29, 0},
                              {58, 0}, {58, 22}, {64, 29}, {87, 29},
                              {87, 58}, {64, 58}, {58, 64}, {58, 87},
                              {29, 87}, {29, 64}, {23, 58}, {0, 58} };

        CRgn rgnPoly;
        rgnPoly.CreatePolygonRgn(ptArray, 16, WINDING);
        SetWindowRgn(rgnPoly, FALSE);
        return 0;
    }

    inline BOOL CDocker::CTargetCentre::CheckTarget(LPDRAGPOS pDragPos)
    {
        CDocker* pDockDrag = pDragPos->pDocker;
        assert( ::SendMessage(*pDockDrag, UWM_GETCDOCKER, 0, 0) );

        CDocker* pDockTarget = pDockDrag->GetDockUnderDragPoint(pDragPos->pos);
        if (NULL == pDockTarget) return FALSE;

        if (!IsWindow())    Create();
        m_IsOverContainer = (pDockTarget->GetView().SendMessage(UWM_GETCDOCKCONTAINER) != 0);

        // Redraw the target if the dock target changes
        if (m_pOldDockTarget != pDockTarget)    Invalidate();
        m_pOldDockTarget = pDockTarget;

        int cxImage = 88;
        int cyImage = 88;

        CRect rcTarget = pDockTarget->GetDockClient().GetWindowRect();
        int xMid = rcTarget.left + (rcTarget.Width() - cxImage)/2;
        int yMid = rcTarget.top + (rcTarget.Height() - cyImage)/2;
        SetWindowPos(HWND_TOPMOST, xMid, yMid, cxImage, cyImage, SWP_NOACTIVATE|SWP_SHOWWINDOW);

        // Create the docking zone rectangles
        CPoint pt = pDragPos->pos;
        ScreenToClient(pt);
        CRect rcLeft(0, 29, 31, 58);
        CRect rcTop(29, 0, 58, 31);
        CRect rcRight(55, 29, 87, 58);
        CRect rcBottom(29, 55, 58, 87);
        CRect rcMiddle(31, 31, 56, 57);

        // Test if our cursor is in one of the docking zones
        if ((rcLeft.PtInRect(pt)) && !(pDockTarget->GetDockStyle() & DS_NO_DOCKCHILD_LEFT))
        {
            pDockDrag->m_isBlockMove = TRUE;

            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_LEFT);
            pDockDrag->m_dockZone = DS_DOCKED_LEFT;

            return TRUE;
        }
        else if ((rcTop.PtInRect(pt)) && !(pDockTarget->GetDockStyle() & DS_NO_DOCKCHILD_TOP))
        {
            pDockDrag->m_isBlockMove = TRUE;
            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_TOP);
            pDockDrag->m_dockZone = DS_DOCKED_TOP;
            return TRUE;
        }
        else if ((rcRight.PtInRect(pt)) && !(pDockTarget->GetDockStyle() & DS_NO_DOCKCHILD_RIGHT))
        {
            pDockDrag->m_isBlockMove = TRUE;

            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_RIGHT);
            pDockDrag->m_dockZone = DS_DOCKED_RIGHT;

            return TRUE;
        }
        else if ((rcBottom.PtInRect(pt)) && !(pDockTarget->GetDockStyle() & DS_NO_DOCKCHILD_BOTTOM))
        {
            pDockDrag->m_isBlockMove = TRUE;
            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_BOTTOM);
            pDockDrag->m_dockZone = DS_DOCKED_BOTTOM;
            return TRUE;
        }
        else if ((rcMiddle.PtInRect(pt)) && (IsOverContainer()))
        {
            pDockDrag->m_isBlockMove = TRUE;
            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_CONTAINER);
            pDockDrag->m_dockZone = DS_DOCKED_CONTAINER;
            return TRUE;
        }
        else
            return FALSE;
    }

    ////////////////////////////////////////////////////////////////
    // Definitions for the CTarget class nested within CDocker
    // CTarget is the base class for a number of CTargetXXX classes
    inline CDocker::CTarget::~CTarget()
    {
    }

    inline void CDocker::CTarget::OnDraw(CDC& dc)
    {
        BITMAP data = m_bmImage.GetBitmapData();
        int cxImage = data.bmWidth;
        int cyImage = data.bmHeight;

        if (m_bmImage != 0)
            dc.DrawBitmap(0, 0, cxImage, cyImage, m_bmImage, RGB(255,0,255));
        else
            TRACE("Missing docking resource\n");
    }

    inline void CDocker::CTarget::PreCreate(CREATESTRUCT& cs)
    {
        cs.style = WS_POPUP;
        cs.dwExStyle = WS_EX_TOPMOST|WS_EX_TOOLWINDOW;
        cs.lpszClass = _T("Win32++ DockTargeting");
    }


    ////////////////////////////////////////////////////////////////
    // Definitions for the CTargetLeft class nested within CDocker
    //
    inline BOOL CDocker::CTargetLeft::CheckTarget(LPDRAGPOS pDragPos)
    {
        CDocker* pDockDrag = pDragPos->pDocker;
        assert( pDockDrag );

        CPoint pt = pDragPos->pos;
        CDocker* pDockTarget = pDockDrag->GetDockUnderDragPoint(pt)->GetTopmostDocker();
        if (pDockTarget != pDockDrag->GetDockAncestor())
        {
            Destroy();
            return FALSE;
        }

        if (pDockTarget->GetDockStyle() & DS_NO_DOCKCHILD_LEFT)
            return FALSE;

        BITMAP data = m_bmImage.GetBitmapData();
        int cxImage = data.bmWidth;
        int cyImage = data.bmHeight;

        if (!IsWindow())
        {
            Create();
            CRect rc = pDockTarget->GetViewRect();
            pDockTarget->ClientToScreen(rc);
            int yMid = rc.top + (rc.Height() - cyImage)/2;
            SetWindowPos(HWND_TOPMOST, rc.left + 8, yMid, cxImage, cyImage, SWP_NOACTIVATE|SWP_SHOWWINDOW);
        }

        CRect rcLeft(0, 0, cxImage, cyImage);
        ScreenToClient(pt);

        // Test if our cursor is in one of the docking zones
        if (rcLeft.PtInRect(pt))
        {
            pDockDrag->m_isBlockMove = TRUE;
            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_LEFTMOST);
            pDockDrag->m_dockZone = DS_DOCKED_LEFTMOST;
            return TRUE;
        }

        return FALSE;
    }


    ////////////////////////////////////////////////////////////////
    // Definitions for the CTargetTop class nested within CDocker
    //
    inline BOOL CDocker::CTargetTop::CheckTarget(LPDRAGPOS pDragPos)
    {
        CDocker* pDockDrag = pDragPos->pDocker;
        assert( pDockDrag );

        CPoint pt = pDragPos->pos;
        CDocker* pDockTarget = pDockDrag->GetDockUnderDragPoint(pt)->GetTopmostDocker();
        if (pDockTarget != pDockDrag->GetDockAncestor())
        {
            Destroy();
            return FALSE;
        }

        if (pDockTarget->GetDockStyle() & DS_NO_DOCKCHILD_TOP)
            return FALSE;

        BITMAP data = m_bmImage.GetBitmapData();
        int cxImage = data.bmWidth;
        int cyImage = data.bmHeight;

        if (!IsWindow())
        {
            Create();
            CRect rc = pDockTarget->GetViewRect();
            pDockTarget->ClientToScreen(rc);
            int xMid = rc.left + (rc.Width() - cxImage)/2;
            SetWindowPos(HWND_TOPMOST, xMid, rc.top + 8, cxImage, cyImage, SWP_NOACTIVATE|SWP_SHOWWINDOW);
        }

        CRect rcTop(0, 0, cxImage, cyImage);
        ScreenToClient(pt);

        // Test if our cursor is in one of the docking zones
        if (rcTop.PtInRect(pt))
        {
            pDockDrag->m_isBlockMove = TRUE;
            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_TOPMOST);
            pDockDrag->m_dockZone = DS_DOCKED_TOPMOST;
            return TRUE;
        }

        return FALSE;
    }


    ////////////////////////////////////////////////////////////////
    // Definitions for the CTargetRight class nested within CDocker
    //
    inline BOOL CDocker::CTargetRight::CheckTarget(LPDRAGPOS pDragPos)
    {
        CDocker* pDockDrag = pDragPos->pDocker;
        assert( pDockDrag );

        CPoint pt = pDragPos->pos;
        CDocker* pDockTarget = pDockDrag->GetDockUnderDragPoint(pt)->GetTopmostDocker();
        if (pDockTarget != pDockDrag->GetDockAncestor())
        {
            Destroy();
            return FALSE;
        }

        if (pDockTarget->GetDockStyle() & DS_NO_DOCKCHILD_RIGHT)
            return FALSE;

        BITMAP data = m_bmImage.GetBitmapData();
        int cxImage = data.bmWidth;
        int cyImage = data.bmHeight;

        if (!IsWindow())
        {
            Create();
            CRect rc = pDockTarget->GetViewRect();
            pDockTarget->ClientToScreen(rc);
            int yMid = rc.top + (rc.Height() - cyImage)/2;
            SetWindowPos(HWND_TOPMOST, rc.right - 8 - cxImage, yMid, cxImage, cyImage, SWP_NOACTIVATE|SWP_SHOWWINDOW);
        }

        CRect rcRight(0, 0, cxImage, cyImage);
        ScreenToClient(pt);

        // Test if our cursor is in one of the docking zones
        if (rcRight.PtInRect(pt))
        {
            pDockDrag->m_isBlockMove = TRUE;
            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_RIGHTMOST);
            pDockDrag->m_dockZone = DS_DOCKED_RIGHTMOST;
            return TRUE;
        }

        return FALSE;
    }


    ////////////////////////////////////////////////////////////////
    // Definitions for the CTargetBottom class nested within CDocker
    //
    inline BOOL CDocker::CTargetBottom::CheckTarget(LPDRAGPOS pDragPos)
    {
        CDocker* pDockDrag = pDragPos->pDocker;
        assert( pDockDrag );

        CPoint pt = pDragPos->pos;
        CDocker* pDockTarget = pDockDrag->GetDockUnderDragPoint(pt)->GetTopmostDocker();
        if (pDockTarget != pDockDrag->GetDockAncestor())
        {
            Destroy();
            return FALSE;
        }

        if (pDockTarget->GetDockStyle() & DS_NO_DOCKCHILD_BOTTOM)
            return FALSE;

        BITMAP data = m_bmImage.GetBitmapData();
        int cxImage = data.bmWidth;
        int cyImage = data.bmHeight;

        if (!IsWindow())
        {
            Create();
            CRect rc = pDockTarget->GetViewRect();
            pDockTarget->ClientToScreen(rc);
            int xMid = rc.left + (rc.Width() - cxImage)/2;
            SetWindowPos(HWND_TOPMOST, xMid, rc.bottom - 8 - cyImage, cxImage, cyImage, SWP_NOACTIVATE|SWP_SHOWWINDOW);
        }
        CRect rcBottom(0, 0, cxImage, cyImage);
        ScreenToClient(pt);

        // Test if our cursor is in one of the docking zones
        if (rcBottom.PtInRect(pt))
        {
            pDockDrag->m_isBlockMove = TRUE;
            pDockTarget->GetDockHint().DisplayHint(pDockTarget, pDockDrag, DS_DOCKED_BOTTOMMOST);
            pDockDrag->m_dockZone = DS_DOCKED_BOTTOMMOST;
            return TRUE;
        }

        return FALSE;
    }


    /////////////////////////////////////////
    // Definitions for the CDocker class
    //
    inline CDocker::CDocker() : m_pDockParent(NULL), m_pDockAncestor(NULL), m_isBlockMove(FALSE),
                    m_isUndocking(FALSE), m_isClosing(FALSE), m_isDragging(FALSE),
                    m_isDragAutoResize(TRUE), m_dockStartSize(0), m_dockID(0), m_redrawCount(0),
                    m_ncHeight(0), m_dockZone(0), m_dockSizeRatio(1.0), m_dockStyle(0), m_dockUnderPoint(0)
    {
        // Assume this docker is the DockAncestor for now.
        m_pDockAncestor = this;
        m_allDockers.push_back(this);
        m_dockClient.SetDocker(this);
    }

    inline CDocker::~CDocker()
    {
        GetDockBar().Destroy();

        std::vector<DockPtr>::const_iterator iter;
        if (GetDockAncestor() == this)
        {
            // Destroy all dock descendants of this dock ancestor
            for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
            {
                (*iter)->Destroy();
            }
        }
    }


    // This function creates the docker, and adds it to the docker hierarchy as docked.
    inline CDocker* CDocker::AddDockedChild(CDocker* pDocker, DWORD dockStyle, int dockSize, int dockID /* = 0*/)
    {
        // Create the docker window as a child of the frame window.
        // This permanently sets the frame window as the docker window's owner,
        // even when its parent is subsequently changed.

        assert(pDocker);

        // Store the Docker's pointer in the DockAncestor's vector for later deletion
        GetAllChildren().push_back(DockPtr(pDocker));
        GetDockAncestor()->m_allDockers.push_back(pDocker);

        pDocker->SetDockStyle(dockStyle);
        pDocker->m_dockID = dockID;
        pDocker->m_pDockAncestor = GetDockAncestor();
        pDocker->m_pDockParent = this;
        HWND frame = GetDockAncestor()->GetAncestor();
        pDocker->Create(frame);
        pDocker->SetParent(*this);

        // Dock the docker window
        if (dockStyle & DS_DOCKED_CONTAINER)
            DockInContainer(pDocker, dockStyle);
        else
            Dock(pDocker, dockStyle);

        pDocker->SetDockSize(dockSize);

        // Issue TRACE warnings for any missing resources
        HMODULE module= GetApp()->GetResourceHandle();

        if (!(dockStyle & DS_NO_RESIZE))
        {
            if (!FindResource(module, MAKEINTRESOURCE(IDW_SPLITH), RT_GROUP_CURSOR))
                TRACE("**WARNING** Horizontal cursor resource missing\n");
            if (!FindResource(module, MAKEINTRESOURCE(IDW_SPLITV), RT_GROUP_CURSOR))
                TRACE("**WARNING** Vertical cursor resource missing\n");
        }

        if (!(dockStyle & DS_NO_UNDOCK))
        {
            if (!FindResource(module, MAKEINTRESOURCE(IDW_SDCENTER), RT_BITMAP))
                TRACE("**WARNING** Docking center bitmap resource missing\n");
            if (!FindResource(module, MAKEINTRESOURCE(IDW_SDLEFT), RT_BITMAP))
                TRACE("**WARNING** Docking left bitmap resource missing\n");
            if (!FindResource(module, MAKEINTRESOURCE(IDW_SDRIGHT), RT_BITMAP))
                TRACE("**WARNING** Docking right bitmap resource missing\n");
            if (!FindResource(module, MAKEINTRESOURCE(IDW_SDTOP), RT_BITMAP))
                TRACE("**WARNING** Docking top bitmap resource missing\n");
            if (!FindResource(module, MAKEINTRESOURCE(IDW_SDBOTTOM), RT_BITMAP))
                TRACE("**WARNING** Docking center bottom resource missing\n");
        }

        if (dockStyle & DS_DOCKED_CONTAINER)
        {
            if (!FindResource(module, MAKEINTRESOURCE(IDW_SDMIDDLE), RT_BITMAP))
                TRACE("**WARNING** Docking container bitmap resource missing\n");
        }

        return pDocker;
    }


    // This function creates the docker, and adds it to the docker hierarchy as undocked.
    inline CDocker* CDocker::AddUndockedChild(CDocker* pDocker, DWORD dockStyle, int dockSize, const RECT& rc, int dockID /* = 0*/)
    {
        assert(pDocker);

        // Store the Docker's pointer in the DockAncestor's vector for later deletion
        GetAllChildren().push_back(DockPtr(pDocker));
        GetDockAncestor()->m_allDockers.push_back(pDocker);

        pDocker->SetDockSize(dockSize);
        pDocker->SetDockStyle(dockStyle & 0XFFFFFF0);
        pDocker->m_dockID = dockID;
        pDocker->m_pDockAncestor = GetDockAncestor();

        // Initially create the as a child window of the frame
        // This makes the frame window the owner of our docker
        HWND hFrame = GetDockAncestor()->GetAncestor();
        pDocker->Create(hFrame);
        pDocker->SetParent(*this);

        // Change the Docker to a POPUP window
        DWORD style = WS_POPUP| WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE;
        pDocker->SetStyle(style);
        pDocker->SetRedraw(FALSE);
        pDocker->SetParent(0);
        pDocker->SetWindowPos(0, rc, SWP_SHOWWINDOW|SWP_FRAMECHANGED);
        pDocker->SetRedraw(TRUE);
        pDocker->RedrawWindow(RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ALLCHILDREN);
        pDocker->SetWindowText(pDocker->GetCaption().c_str());

        return pDocker;
    }


    // Calls CheckTarget for each possible target zone.
    inline void CDocker::CheckAllTargets(LPDRAGPOS pDragPos)
    {
        if (!GetDockAncestor()->m_targetCentre.CheckTarget(pDragPos))
        {
            if (!GetDockAncestor()->m_targetLeft.CheckTarget(pDragPos))
            {
                if (!GetDockAncestor()->m_targetTop.CheckTarget(pDragPos))
                {
                    if (!GetDockAncestor()->m_targetRight.CheckTarget(pDragPos))
                    {
                        if (!GetDockAncestor()->m_targetBottom.CheckTarget(pDragPos))
                        {
                            // Not in a docking zone, so clean up
                            CDocker* pDockDrag = pDragPos->pDocker;
                            if (pDockDrag)
                            {
                                if (pDockDrag->m_isBlockMove)
                                    pDockDrag->RedrawWindow(RDW_FRAME|RDW_INVALIDATE);

                                GetDockHint().Destroy();
                                pDockDrag->m_dockZone = 0;
                                pDockDrag->m_isBlockMove = FALSE;
                            }
                        }
                    }
                }
            }
        }
    }


    // A diagnostic routine which verifies the integrity of the docking layout.
    inline BOOL CDocker::VerifyDockers()
    {
        BOOL Verified = TRUE;

        // Check dock ancestor
        std::vector<DockPtr>::const_iterator iter;

        for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
        {
            if (GetDockAncestor() != (*iter)->m_pDockAncestor)
            {
                TRACE("Invalid Dock Ancestor\n");
                Verified = FALSE;
            }
        }

        // Check presence of dock parent
        for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
        {
            if ((*iter)->IsUndocked() && (*iter)->m_pDockParent != 0)
            {
                TRACE("Error: Undocked dockers should not have a dock parent\n");
                Verified = FALSE;
            }

            if ((*iter)->IsDocked() && (*iter)->m_pDockParent == 0)
            {
                TRACE("Error: Docked dockers should have a dock parent\n");
                Verified = FALSE;
            }
        }

        // Check dock parent/child relationship
        for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
        {
            std::vector<CDocker*>::const_iterator iterChild;
            for (iterChild = (*iter)->m_dockChildren.begin(); iterChild != (*iter)->m_dockChildren.end(); ++iterChild)
            {
                if ((*iterChild)->m_pDockParent != (*iter).get())
                {
                    TRACE("Error: Docking parent/Child information mismatch\n");
                    Verified = FALSE;
                }

                if ((*iterChild)->GetParent() != (*iter).get()->GetHwnd())
                {
                    TRACE("Error: Incorrect windows child parent relationship\n");
                    Verified = FALSE;
                }
            }
        }

        // Check dock parent chain
        for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
        {
            CDocker* pDockTopLevel = (*iter)->GetTopmostDocker();
            if (pDockTopLevel->IsDocked())
                TRACE("Error: Top level parent should be undocked\n");
        }

        return Verified;
    }


    // Called when the close button is pressed.
    inline void CDocker::Close()
    {
        if (IsDocked())
        {
            // Undock the docker and hide it
            Hide();
        }

        // Destroy the docker and its children
        Destroy();
    }


    // Closes all the child dockers of this dock ancestor.
    inline void CDocker::CloseAllDockers()
    {
        assert(this == GetDockAncestor());  // Must call CloseAllDockers from the DockAncestor

        std::vector <DockPtr>::const_iterator v;

        for (v = GetAllChildren().begin(); v != GetAllChildren().end(); ++v)
        {
            // The CDocker is destroyed when the window is destroyed
            (*v)->m_isClosing = TRUE;
            (*v)->Destroy();    // Destroy the window
        }

        m_dockChildren.clear();

        // Delete any child containers this container might have
        if (GetContainer())
        {
            std::vector<ContainerInfo> AllContainers = GetContainer()->GetAllContainers();
            std::vector<ContainerInfo>::const_iterator iter;
            for (iter = AllContainers.begin(); iter != AllContainers.end(); ++iter)
            {
                if (GetContainer() != (*iter).pContainer)
                    GetContainer()->RemoveContainer((*iter).pContainer);
            }
        }


        RecalcDockLayout();
    }

    inline void CDocker::CloseAllTargets()
    {
        GetDockAncestor()->m_targetCentre.Destroy();
        GetDockAncestor()->m_targetLeft.Destroy();
        GetDockAncestor()->m_targetTop.Destroy();
        GetDockAncestor()->m_targetRight.Destroy();
        GetDockAncestor()->m_targetBottom.Destroy();
    }


    // Docks the specified docker inside this docker.
    inline void CDocker::Dock(CDocker* pDocker, UINT dockStyle)
    {
        assert(pDocker);

        pDocker->m_pDockParent = this;
        pDocker->m_isBlockMove = FALSE;
        pDocker->SetDockStyle(dockStyle);
        m_dockChildren.push_back(pDocker);
        pDocker->ConvertToChild(*this);

        // Limit the docked size to half the parent's size if it won't fit inside parent
        if (((dockStyle & 0xF)  == DS_DOCKED_LEFT) || ((dockStyle &0xF)  == DS_DOCKED_RIGHT))
        {
            int width = GetDockClient().GetWindowRect().Width();
            int barWidth = pDocker->GetBarWidth();
            if (pDocker->m_dockStartSize >= (width - barWidth))
                pDocker->SetDockSize(MAX(width/2 - barWidth, barWidth));

            pDocker->m_dockSizeRatio = static_cast<double>(pDocker->m_dockStartSize) / static_cast<double>(GetWindowRect().Width());
        }
        else
        {
            int height = GetDockClient().GetWindowRect().Height();
            int barWidth = pDocker->GetBarWidth();
            if (pDocker->m_dockStartSize >= (height - barWidth))
                pDocker->SetDockSize(MAX(height/2 - barWidth, barWidth));

            pDocker->m_dockSizeRatio = static_cast<double>(pDocker->m_dockStartSize) / static_cast<double>(GetWindowRect().Height());
        }

        // Redraw the docked windows
        if (GetAncestor().IsWindowVisible())
        {
            GetTopmostDocker()->SetForegroundWindow();

            // Give the view window focus unless its child already has it
            if (!pDocker->GetView().IsChild(GetFocus()))
                pDocker->GetView().SetFocus();

            GetTopmostDocker()->SetRedraw(FALSE);
            RecalcDockLayout();
            GetTopmostDocker()->SetRedraw(TRUE);
            GetTopmostDocker()->RedrawWindow();

            // Update the Dock captions
            GetDockAncestor()->PostMessage(UWM_DOCKACTIVATE);
        }
    }


    // Add a container to an existing container.
    inline void CDocker::DockInContainer(CDocker* pDocker, DWORD dockStyle, BOOL selectPage)
    {
        if ((dockStyle & DS_DOCKED_CONTAINER) && GetContainer())
        {
            // Transfer any dock children to this docker
            pDocker->MoveDockChildren(this);

            // Transfer container children to the target container
            CDockContainer* pContainer = GetContainer();
            CDockContainer* pContainerSource = pDocker->GetContainer();

            std::vector<ContainerInfo>::reverse_iterator riter;
            std::vector<ContainerInfo> AllContainers = pContainerSource->GetAllContainers();
            for (riter = AllContainers.rbegin(); riter < AllContainers.rend(); ++riter)
            {
                CDockContainer* pContainerChild = (*riter).pContainer;
                if (pContainerChild != pContainerSource)
                {
                    // Remove child container from pContainerSource
                    pContainerChild->ShowWindow(SW_HIDE);
                    pContainerSource->RemoveContainer(pContainerChild);
                    CDocker* pDockChild = GetDockFromView(pContainerChild);
                    assert(pDockChild);
                    pDockChild->SetParent(*this);
                    pDockChild->m_pDockParent = this;
                }
            }

            pDocker->m_pDockParent = this;
            pDocker->m_isBlockMove = FALSE;
            pDocker->ShowWindow(SW_HIDE);
            pDocker->SetStyle(WS_CHILD);
            pDocker->SetDockStyle(dockStyle);
            pDocker->SetParent(*this);
            pDocker->GetDockBar().SetParent(*GetDockAncestor());

            // Insert the containers in reverse order
            for (riter = AllContainers.rbegin(); riter < AllContainers.rend(); ++riter)
            {
                pContainer->AddContainer( (*riter).pContainer, TRUE, selectPage);
            }
        }

        // Redraw the docked windows
        pDocker->RecalcDockLayout();
    }


    // Docks the specified docker inside the dock ancestor.
    inline void CDocker::DockOuter(CDocker* pDocker, DWORD dockStyle)
    {
        assert(pDocker);

        pDocker->m_pDockParent = GetDockAncestor();

        DWORD outerDocking = dockStyle & 0xF0000;
        DWORD dockSide = outerDocking / 0x10000;
        dockStyle &= 0xFFF0FFFF;
        dockStyle |= dockSide;

        // Set the dock styles
        DWORD style = WS_CHILD | WS_VISIBLE;
        pDocker->m_isBlockMove = FALSE;
        pDocker->SetStyle(style);
        pDocker->ShowWindow(SW_HIDE);
        pDocker->SetDockStyle(dockStyle);

        // Set the docking relationships
        std::vector<CDocker*>::iterator iter = GetDockAncestor()->m_dockChildren.begin();
        GetDockAncestor()->m_dockChildren.insert(iter, pDocker);
        pDocker->SetParent(*GetDockAncestor());
        pDocker->GetDockBar().SetParent(*GetDockAncestor());

        double dockStartSize = static_cast<double>(pDocker->m_dockStartSize);
        double ancestorHeight = static_cast<double>(GetDockAncestor()->GetWindowRect().Height());
        double ancestorWidth = static_cast<double>(GetDockAncestor()->GetWindowRect().Width());

        // Limit the docked size to half the parent's size if it won't fit inside parent
        if (((dockStyle & 0xF)  == DS_DOCKED_LEFT) || ((dockStyle &0xF)  == DS_DOCKED_RIGHT))
        {
            int width = GetDockAncestor()->GetDockClient().GetWindowRect().Width();
            int barWidth = pDocker->GetBarWidth();
            if (pDocker->m_dockStartSize >= (width - barWidth))
                pDocker->SetDockSize(MAX(width/2 - barWidth, barWidth));

            pDocker->m_dockSizeRatio = dockStartSize / ancestorWidth;
        }
        else
        {
            int height = GetDockAncestor()->GetDockClient().GetWindowRect().Height();
            int barWidth = pDocker->GetBarWidth();
            if (pDocker->m_dockStartSize >= (height - barWidth))
                pDocker->SetDockSize(MAX(height/2 - barWidth, barWidth));

            pDocker->m_dockSizeRatio = dockStartSize / ancestorHeight;
        }

        // Redraw the docked windows
        if (GetAncestor().IsWindowVisible())
        {
            GetTopmostDocker()->SetForegroundWindow();

            // Give the view window focus unless its child already has it
            if (!pDocker->GetView().IsChild(GetFocus()))
                pDocker->GetView().SetFocus();

            GetTopmostDocker()->SetRedraw(FALSE);
            RecalcDockLayout();
            GetTopmostDocker()->SetRedraw(TRUE);
            GetTopmostDocker()->RedrawWindow();

            // Update the Dock captions
            GetDockAncestor()->PostMessage(UWM_DOCKACTIVATE);
        }
    }

    inline void CDocker::DrawAllCaptions()
    {
        std::vector<CDocker*>::const_iterator iter;
        for (iter = GetAllDockers().begin(); iter != GetAllDockers().end(); ++iter)
        {
            if ((*iter)->IsDocked() || (*iter) == GetDockAncestor())
                (*iter)->GetDockClient().DrawCaption();
        }
    }


    // Draws a hashed bar while the splitter bar is being dragged.
    inline void CDocker::DrawHashBar(HWND bar, POINT pos)
    {
        CDockBar* pDockBar = static_cast<CDockBar*>(GetCWndPtr(bar));
        if (NULL == pDockBar) return;
        CDocker& Docker = pDockBar->GetDocker();

        BOOL isVertical = ((Docker.GetDockStyle() & 0xF) == DS_DOCKED_LEFT) || ((Docker.GetDockStyle() & 0xF) == DS_DOCKED_RIGHT);
        CClientDC dcBar(*this);

        WORD hashPattern[] = {0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA};
        CBitmap hash;
        hash.CreateBitmap(8, 8, 1, 1, hashPattern);
        dcBar.CreatePatternBrush(hash);

        CRect rc = pDockBar->GetWindowRect();
        ScreenToClient(rc);
        int cx = rc.Width();
        int cy = rc.Height();
        int barWidth = Docker.GetDockBar().GetWidth();

        if (isVertical)
            dcBar.PatBlt(pos.x - barWidth/2, rc.top, barWidth, cy, PATINVERT);
        else
            dcBar.PatBlt(rc.left, pos.y - barWidth/2, cx, barWidth, PATINVERT);
    }


    // Returns a pointer to the view if it is a CDockContainer.
    // Returns NULL if the view is not a CDockContainer.
    inline CDockContainer* CDocker::GetContainer() const
    {
        // returns NULL if not a CDockContainer*
        if (IsWindow() && GetView().IsWindow())
            return reinterpret_cast<CDockContainer*>(GetView().SendMessage(UWM_GETCDOCKCONTAINER));

        return NULL;
    }


    // Returns the docker that has a child window with keyboard focus.
    // The active docker's caption is drawn with a different color.
    inline CDocker* CDocker::GetActiveDocker() const
    {
        CDocker* pDockActive = NULL;
        HWND test = ::GetFocus();

        while (test != 0)
        {
            HWND parent = ::GetParent(test);
            if (parent == test) break;      // could be owned window, not parent
            test = parent;

            CDocker* pDock = reinterpret_cast<CDocker*>(::SendMessage(test, UWM_GETCDOCKER, 0, 0));
            if (pDock)
            {
                pDockActive = pDock;
                break;
            }
        }

        return pDockActive;
    }


    // Returns a Docker's active view. Returns the container's active view for a docker with
    // a DockContainer, returns the the active MDI child for a docker with a TabbedMDI,
    // or the docker's view for a simple docker. Can return NULL.
    inline CWnd* CDocker::GetActiveView() const
    {
        CWnd* pView;
        if (GetContainer())
            pView = GetContainer()->GetActiveView();
        else if (GetTabbedMDI())
            pView = GetTabbedMDI()->GetActiveMDIChild();
        else
            pView = &GetView();

        return pView;
    }


    // The GetDockAncestor function retrieves the pointer to the
    // ancestor (root docker parent) of the Docker.
    inline CDocker* CDocker::GetDockAncestor() const
    {
        return m_pDockAncestor;
    }


    // Retrieves the Docker whose view window contains the specified point.
    // Used when dragging undocked dockers over other dockers to provide
    // the docker which needs to display the dock targets and dock hints.
    inline CDocker* CDocker::GetDockUnderDragPoint(POINT pt)
    {
        // Step 1: Find the top level Docker under the point
        // EnumWindows assigns the Docker under the point to m_dockUnderPoint

        m_dockUnderPoint = 0;
        m_dockPoint = pt;
        EnumWindows(EnumWindowsProc, (LPARAM)this);

        // Step 2: Find the docker child whose view window has the point
        CDocker* pDockTarget = NULL;

        HWND dockTest = m_dockUnderPoint;
        HWND dockParent = m_dockUnderPoint;

        if (m_dockUnderPoint != 0)
        {
            while (IsRelated(dockTest))
            {
                dockParent = dockTest;
                CPoint ptLocal = pt;
                ::ScreenToClient(dockParent, &ptLocal);
                dockTest = ::ChildWindowFromPoint(dockParent, ptLocal);
                if (dockTest == dockParent) break;
            }

            CDocker* pDockParent = reinterpret_cast<CDocker*>(::SendMessage(dockParent, UWM_GETCDOCKER, 0, 0));
            assert(pDockParent);

            CRect rc = pDockParent->GetDockClient().GetWindowRect();
            if (rc.PtInRect(pt))
                pDockTarget = pDockParent;

        }

        return pDockTarget;
    }


    // Returns the docker that has the specified Dock ID.
    inline CDocker* CDocker::GetDockFromID(int dockID) const
    {
        std::vector<DockPtr>::const_iterator v;

        if (GetDockAncestor())
        {
            for (v = GetAllChildren().begin(); v != GetAllChildren().end(); ++v)
            {
                if (dockID == (*v)->GetDockID())
                    return (*v).get();
            }
        }

        return 0;
    }


    // Returns the child docker that has the specified view.
    inline CDocker* CDocker::GetDockFromView(CWnd* pView) const
    {
        CDocker* pDocker = 0;
        std::vector<DockPtr>::const_iterator iter;
        for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
        {
            if (&(*iter)->GetView() == pView)
                pDocker = (*iter).get();
        }

        if (&GetDockAncestor()->GetView() == pView)
            pDocker = GetDockAncestor();

        return pDocker;
    }


    // Returns the size of the docker to be used if it is redocked.
    // Note: This function returns 0 if the docker has the DS_DOCKED_CONTAINER style.
    inline int CDocker::GetDockSize() const
    {
        CRect parent;
        if (GetDockParent())
            parent = GetDockParent()->GetWindowRect();
        else
            parent = GetDockAncestor()->GetWindowRect();

        double dockSize = 0;
        if ((GetDockStyle() & DS_DOCKED_LEFT) || (GetDockStyle() & DS_DOCKED_RIGHT))
            dockSize = parent.Width()*m_dockSizeRatio;
        else if ((GetDockStyle() & DS_DOCKED_TOP) || (GetDockStyle() & DS_DOCKED_BOTTOM))
            dockSize = parent.Height()*m_dockSizeRatio;
        else if ((GetDockStyle() & DS_DOCKED_CONTAINER))
            dockSize = 0;

        return static_cast<int>(dockSize);
    }


    // Returns the docker's parent at the top of the Z order.
    // Could be the dock ancestor or an undocked docker.
    inline CDocker* CDocker::GetTopmostDocker() const
    {
        CDocker* pDockTopLevel = (CDocker* const)this;

        while (pDockTopLevel->GetDockParent())
        {
            assert (pDockTopLevel != pDockTopLevel->GetDockParent());
            pDockTopLevel = pDockTopLevel->GetDockParent();
        }

        return pDockTopLevel;
    }


    // Returns the pointer to the view window if it is a CTabbedMDI.
    // Returns NULL if not a CTabbedMDI.
    inline CTabbedMDI* CDocker::GetTabbedMDI() const
    {
        // returns NULL if not a CTabbedMDI*
        if (IsWindow())
            return reinterpret_cast<CTabbedMDI*>(GetView().SendMessage(UWM_GETCTABBEDMDI));

        return NULL;
    }

    inline int CDocker::GetTextHeight()
    {
        NONCLIENTMETRICS info = GetNonClientMetrics();
        LOGFONT lf = info.lfStatusFont;

        CClientDC dc(*this);
        dc.CreateFontIndirect(lf);
        CSize textSize = dc.GetTextExtentPoint32(_T("Text"), lstrlen(_T("Text")));
        return textSize.cy;
    }


    // Undocks a docker (if needed) and hides it.
    // To unhide the docker, dock it.
    inline void CDocker::Hide()
    {
        if (IsDocked())
        {
            CDockContainer* pContainer = GetContainer();

            if (pContainer)
            {
                if (pContainer == pContainer->GetContainerParent())
                    UndockContainer(pContainer, GetCursorPos(), FALSE);
                else
                    pContainer->GetContainerParent()->RemoveContainer(pContainer, FALSE);
            }
            else
            {
                CDocker* pDockUndockedFrom = SeparateFromDock();
                pDockUndockedFrom->RecalcDockLayout();
            }
        }

        ShowWindow(SW_HIDE);
    }


    // Returns true if the specified window is a child of this docker.
    inline BOOL CDocker::IsChildOfDocker(HWND wnd) const
    {
        while ((wnd != NULL) && (wnd != *GetDockAncestor()))
        {
            if ( wnd == *this ) return TRUE;
            if (IsRelated(wnd)) break;
            wnd = ::GetParent(wnd);
        }

        return FALSE;
    }


    // Returns TRUE if this docker is docked.
    inline BOOL CDocker::IsDocked() const
    {
        return (((m_dockStyle&0xF) || (m_dockStyle & DS_DOCKED_CONTAINER)) && !m_isUndocking); // Boolean expression
    }


    // Returns true if this docker resizes child dockers dynamically.
    inline BOOL CDocker::IsDragAutoResize() const
    {
        return m_isDragAutoResize;
    }


    // Returns TRUE if the wnd is a docker within this dock family.
    inline BOOL CDocker::IsRelated(HWND wnd) const
    {
        if (*GetDockAncestor() == wnd) return TRUE;

        std::vector<DockPtr>::const_iterator iter;
        for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
        {
            if ((*iter).get()->GetHwnd() == wnd) return TRUE;
        }

        return FALSE;
    }

    inline BOOL CDocker::IsUndockable() const
    {
        // Returns TRUE if the docker is docked, or a dock ancestor that has a CDockContainer with tabs.
        BOOL isUndockable = IsDocked();

        CDockContainer* pContainer = GetContainer();
        if (this == GetDockAncestor())
        {
            if (pContainer && (pContainer->GetItemCount() > 0))
                isUndockable = TRUE;
        }

        return isUndockable;
    }

    inline BOOL CDocker::IsUndocked() const
    {
        return (!((m_dockStyle&0xF)|| (m_dockStyle & DS_DOCKED_CONTAINER)) && !m_isUndocking); // Boolean expression
    }


    // Recreates the docker layout based on information stored in the registry.
    // Assumes the DockAncestor window is already created.
    inline BOOL CDocker::LoadDockRegistrySettings(LPCTSTR pRegistryKeyName)
    {
        BOOL isLoaded = FALSE;

        if (pRegistryKeyName)
        {
            std::vector<DockInfo> dockList;
            CString keyName = _T("Software\\") + CString(pRegistryKeyName) + _T("\\Dock Windows");
            CRegKey key;
            if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, keyName, KEY_READ))
            {
                DWORD bufferSize = sizeof(DockInfo);
                DockInfo di;
                int i = 0;
                CString subKeyName;
                subKeyName.Format(_T("DockChild%d"), i);

                // Fill the DockList vector from the registry
                while (ERROR_SUCCESS == key.QueryBinaryValue(subKeyName, &di, &bufferSize))
                {
                    dockList.push_back(di);
                    i++;
                    subKeyName.Format(_T("DockChild%d"), i);
                }

                key.Close();
                if (dockList.size() > 0) isLoaded = TRUE;
            }

            try
            {
                // Add dockers without parents first
                std::vector<DockInfo>::iterator iter;
                for (iter = dockList.begin(); iter != dockList.end() ; ++iter)
                {
                    DockInfo di = (*iter);
                    if ((di.dockParentID == 0) || (di.isInAncestor))
                    {
                        CDocker* pDocker = NewDockerFromID(di.dockID);
                        if (!pDocker)
                            throw CUserException(_T("Failed to add dockers without parents from registry"));

                        if ((di.dockStyle & 0xF) || (di.isInAncestor))
                            AddDockedChild(pDocker, di.dockStyle, di.dockSize, di.dockID);
                        else
                            AddUndockedChild(pDocker, di.dockStyle, di.dockSize, di.rect, di.dockID);
                    }
                }

                // Remove dockers without parents from vDockList
                for (UINT n = static_cast<UINT>(dockList.size()); n > 0; --n)
                {
                    iter = dockList.begin() + n-1;
                    if (((*iter).dockParentID == 0) || ((*iter).isInAncestor))
                        dockList.erase(iter);
                }

                // Add remaining dockers
                while (dockList.size() > 0)
                {
                    bool found = false;
                    for (iter = dockList.begin(); iter != dockList.end(); ++iter)
                    {
                        DockInfo di = *iter;
                        CDocker* pDockParent = GetDockFromID(di.dockParentID);

                        if (pDockParent != 0)
                        {
                            CDocker* pDocker = NewDockerFromID(di.dockID);
                            if (!pDocker)
                                throw CUserException(_T("Failed to add dockers with parents from registry"));

                            pDockParent->AddDockedChild(pDocker, di.dockStyle, di.dockSize, di.dockID);
                            found = true;
                            dockList.erase(iter);
                            break;
                        }
                    }

                    if (!found)
                        throw CUserException(_T("Orphaned dockers stored in registry"));

                    if (!VerifyDockers())
                        throw CUserException(_T("Dockers are in an inconsistant state"));
                }
            }

            catch (const CUserException& e)
            {
                Trace(e.GetText()); Trace("\n");
                isLoaded = FALSE;
                CloseAllDockers();

                // Delete the bad key from the registry
                CString parentKeyName = _T("Software\\") + CString(pRegistryKeyName);
                CRegKey parentKey;
                if (ERROR_SUCCESS == parentKey.Open(HKEY_CURRENT_USER, parentKeyName, KEY_READ))
                    parentKey.RecurseDeleteKey(_T("Dock Windows"));
            }

        }

        if (isLoaded)
            LoadContainerRegistrySettings(pRegistryKeyName);

        return isLoaded;
    }


    // Loads the information for CDockContainers from the registry.
    inline BOOL CDocker::LoadContainerRegistrySettings(LPCTSTR pRegistryKeyName)
    {
        BOOL isLoaded = FALSE;
        if (pRegistryKeyName)
        {
            // Load Dock container tab order and active container
            CString keyName = _T("Software\\") + CString(pRegistryKeyName) + _T("\\Dock Windows");
            CRegKey key;

            if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, keyName, KEY_READ))
            {
                try
                {
                    UINT container = 0;
                    CString subKeyName;
                    subKeyName.Format(_T("DockContainer%u"), container);
                    CRegKey containerKey;

                    while (ERROR_SUCCESS == containerKey.Open(key, subKeyName, KEY_READ))
                    {
                        // Load tab order
                        isLoaded = TRUE;
                        UINT tabNumber = 0;
                        DWORD tabID;
                        std::vector<UINT> tabOrder;
                        CString tabKeyName;
                        tabKeyName.Format(_T("Tab%u"), tabNumber);
                        while (ERROR_SUCCESS == containerKey.QueryDWORDValue(tabKeyName, tabID))
                        {
                            tabOrder.push_back(tabID);
                            tabKeyName.Format(_T("Tab%u"), ++tabNumber);
                        }

                        // Set tab order
                        DWORD parentID;
                        if (ERROR_SUCCESS == containerKey.QueryDWORDValue(_T("Parent Container"), parentID))
                        {
                            CDocker* pDocker = GetDockFromID(parentID);
                            if (!pDocker)
                                pDocker = this;

                            CDockContainer* pParentContainer = pDocker->GetContainer();
                            if (!pParentContainer)
                                throw CUserException(_T("Failed to get parent container"));

                            for (UINT tab = 0; tab < tabOrder.size(); ++tab)
                            {
                                CDocker* pOldDocker = GetDockFromView(pParentContainer->GetContainerFromIndex(tab));
                                if (!pOldDocker)
                                    throw CUserException(_T("Failed to get docker from view"));

                                UINT oldID = pOldDocker->GetDockID();

                                std::vector<UINT>::const_iterator it = std::find(tabOrder.begin(), tabOrder.end(), oldID);
                                UINT oldTab = static_cast<UINT>((it - tabOrder.begin()));

                                if (tab >= pParentContainer->GetAllContainers().size())
                                    throw CUserException(_T("Invalid Container configuration"));

                                if (oldTab >= pParentContainer->GetAllContainers().size())
                                    throw CUserException(_T("Invalid Container configuration"));

                                if (tab != oldTab)
                                    pParentContainer->SwapTabs(tab, oldTab);
                            }
                        }

                        // Set the active container
                        DWORD activeContainer;
                        if (ERROR_SUCCESS == containerKey.QueryDWORDValue(_T("Active Container"), activeContainer))
                        {
                            CDocker* pDocker = GetDockFromID(activeContainer);
                            if (pDocker)
                            {
                                CDockContainer* pContainer = pDocker->GetContainer();
                                if (!pContainer)
                                    throw CUserException(_T("Failed to get container view"));

                                int page = pContainer->GetContainerIndex(pContainer);
                                if (page >= 0)
                                    pContainer->SelectPage(page);
                            }
                        }

                        subKeyName.Format(_T("DockContainer%u"), ++container);
                    }
                }

                catch (const CUserException& e)
                {
                    Trace("*** Failed to load values from registry, using defaults. ***\n");
                    Trace(e.GetText()); Trace("\n");
                    CloseAllDockers();

                    // Delete the bad key from the registry
                    CString parentKeyName = _T("Software\\") + CString(pRegistryKeyName);
                    CRegKey parentKey;
                    if (ERROR_SUCCESS == parentKey.Open(HKEY_CURRENT_USER, parentKeyName, KEY_READ))
                        parentKey.RecurseDeleteKey(_T("Dock Windows"));
                }
            }
        }

        return isLoaded;
    }


    // Used internally by Dock and Undock.
    inline void CDocker::MoveDockChildren(CDocker* pDockTarget)
    {
        assert(pDockTarget);

        // Transfer any dock children from the current docker to the target docker
        std::vector<CDocker*>::const_iterator iter;
        for (iter = m_dockChildren.begin(); iter != m_dockChildren.end(); ++iter)
        {
            pDockTarget->m_dockChildren.push_back(*iter);
            (*iter)->m_pDockParent = pDockTarget;
            (*iter)->SetParent(*pDockTarget);
            (*iter)->GetDockBar().SetParent(*pDockTarget);
        }
        m_dockChildren.clear();
    }


    // Used in LoadRegistrySettings. Creates a new Docker from the specified ID.
    inline CDocker* CDocker::NewDockerFromID(int id)
    {
        UNREFERENCED_PARAMETER(id);

        // Override this function to create the Docker objects as shown below

        CDocker* pDocker = NULL;
    /*  switch(id)
        {
        case ID_CLASSES:
            pDocker = new CDockClasses;
            break;
        case ID_FILES:
            pDocker = new CDockFiles;
            break;
        default:
            TRACE("Unknown Dock ID\n");
            break;
        } */

        return pDocker;
    }


    // Called when the this docker is activated.
    inline LRESULT CDocker::OnActivate(UINT, WPARAM wparam, LPARAM)
    {
        if ((this != GetDockAncestor()) && IsUndocked())
        {
            GetDockAncestor()->PostMessage(UWM_DOCKACTIVATE);

            // Give the view window focus unless its child already has it
            if ((wparam != WA_INACTIVE) && !GetView().IsChild(GetFocus()))
                GetView().SetFocus();
        }

        return 0;
    }

    inline LRESULT CDocker::OnBarStart(LPDRAGPOS pDragPos)
    {
        CPoint pt = pDragPos->pos;
        ScreenToClient(pt);
        if (!IsDragAutoResize())
            DrawHashBar(pDragPos->hdr.hwndFrom, pt);
        m_oldPoint = pt;

        return 0;
    }

    inline LRESULT CDocker::OnBarMove(LPDRAGPOS pDragPos)
    {
        CPoint pt = pDragPos->pos;
        ScreenToClient(pt);

        if (pt != m_oldPoint)
        {
            if (IsDragAutoResize())
                ResizeDockers(pDragPos);
            else
            {
                DrawHashBar(pDragPos->hdr.hwndFrom, m_oldPoint);
                DrawHashBar(pDragPos->hdr.hwndFrom, pt);
            }

            m_oldPoint = pt;
        }

        return 0;
    }

    inline LRESULT CDocker::OnBarEnd(LPDRAGPOS pDragPos)
    {
        POINT pt = pDragPos->pos;
        ScreenToClient(pt);

        if (!IsDragAutoResize())
            DrawHashBar(pDragPos->hdr.hwndFrom, pt);

        ResizeDockers(pDragPos);
        return 0;
    }


    // Called when this docker is created
    inline int CDocker::OnCreate(CREATESTRUCT& cs)
    {
        UNREFERENCED_PARAMETER(cs);

#if (WINVER >= 0x0500)
        if (GetParent().GetExStyle() & WS_EX_LAYOUTRTL)
        {
            DWORD exStyle = static_cast<DWORD>(GetExStyle());
            SetExStyle(exStyle | WS_EX_LAYOUTRTL);
        }
#endif

        // Create the various child windows
        GetDockClient().Create(*this);

        assert(&GetView());         // Use SetView in the docker's constructor to set the view window
        GetView().Create(GetDockClient());

        // Create the slider bar belonging to this docker
        GetDockBar().SetDocker(*this);
        if (GetDockAncestor() != this)
            GetDockBar().Create(*GetDockAncestor());

        // Now remove the WS_POPUP style. It was required to allow this window
        // to be owned by the frame window.
        SetStyle(WS_CHILD);
        SetParent(GetParent());     // Reinstate the window's parent

        // Set the default colour for the splitter bar if it hasn't already been set
        if (!GetDockBar().GetBrushBkgnd().GetHandle())
        {
            COLORREF rgbColour = GetSysColor(COLOR_BTNFACE);
            HWND hFrame = GetDockAncestor()->GetAncestor();

            ReBarTheme* pTheme = reinterpret_cast<ReBarTheme*>(::SendMessage(hFrame, UWM_GETRBTHEME, 0, 0));

            if (pTheme && pTheme->UseThemes && pTheme->clrBkgnd2 != 0)
                rgbColour = pTheme->clrBkgnd2;

            SetBarColor(rgbColour);
        }

        // Set the caption height based on text height
        m_ncHeight = MAX(20, GetTextHeight() + 5);

        CDockContainer* pContainer = GetContainer();
        if (pContainer)
        {
            SetCaption(pContainer->GetDockCaption().c_str());
        }

        return 0;
    }


    // Called when the window is about to be destroyed.
    inline void CDocker::OnDestroy()
    {
        // Destroy any dock children first
        std::vector<CDocker*>::const_iterator iter;
        for (iter = m_dockChildren.begin(); iter != m_dockChildren.end(); ++iter)
        {
            (*iter)->Destroy();
        }

        CDockContainer* pContainer = GetContainer();
        if (pContainer && IsUndocked())
        {
            if (pContainer->GetAllContainers().size() > 1)
            {
                // This container has children, so destroy them now
                const std::vector<ContainerInfo>& AllContainers = pContainer->GetAllContainers();
                std::vector<ContainerInfo>::const_iterator iter1;
                for (iter1 = AllContainers.begin(); iter1 < AllContainers.end(); ++iter1)
                {
                    if ((*iter1).pContainer != pContainer)
                    {
                        // Reset container parent before destroying the dock window
                        CDocker* pDocker = GetDockFromView((*iter1).pContainer);
                        assert(pDocker);
                        if (pContainer->IsWindow())
                            pContainer->SetParent(pDocker->GetDockClient());

                        pDocker->Destroy();
                    }
                }
            }
        }

        GetDockBar().Destroy();

        // Post a docker destroyed message
        if ( GetDockAncestor()->IsWindow() )
            GetDockAncestor()->PostMessage(UWM_DOCKDESTROYED, (WPARAM)this, 0);
    }


    // Called when this docker is destroyed.
    inline LRESULT CDocker::OnDockDestroyed(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(lparam);
        CDocker* pDocker = reinterpret_cast<CDocker*>(wparam);

        assert( this == GetDockAncestor() );
        std::vector<DockPtr>::iterator iter;
        for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
        {
            if ((*iter).get() == pDocker)
            {
                GetAllChildren().erase(iter);
                break;
            }
        }

        std::vector<CDocker*>& Dockers = GetDockAncestor()->m_allDockers;
        for (std::vector<CDocker*>::iterator it = Dockers.begin(); it < Dockers.end(); ++it)
        {
            if ((*it) == pDocker)
            {
                Dockers.erase(it);
                break;
            }
        }

        return 0;
    }


    // Called in response to a UWM_DOCKACTIVATE message
    inline LRESULT CDocker::OnDockActivated(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Redraw captions to take account of focus change
        if (this == GetDockAncestor())
            DrawAllCaptions();

        return CWnd::WndProcDefault(msg, wparam, lparam);
    }


    inline LRESULT CDocker::OnDockStart(LPDRAGPOS pDragPos)
    {
        if (IsDocked())
        {
            Undock(GetCursorPos());
            SendMessage(WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, MAKELPARAM(pDragPos->pos.x, pDragPos->pos.y));
        }

        return 0;
    }


    inline LRESULT CDocker::OnDockMove(LPDRAGPOS pDragPos)
    {
        CheckAllTargets(pDragPos);
        return 0;
    }


    inline LRESULT CDocker::OnDockEnd(LPDRAGPOS pDragPos)
    {
        CDocker* pDocker = pDragPos->pDocker;
        assert(pDocker);

        UINT DockZone = pDragPos->dockZone;
        CRect rc = pDocker->GetWindowRect();

        switch(DockZone)
        {
        case DS_DOCKED_LEFT:
        case DS_DOCKED_RIGHT:
            pDocker->SetDockSize(rc.Width());
            Dock(pDocker, pDocker->GetDockStyle() | DockZone);
            break;
        case DS_DOCKED_TOP:
        case DS_DOCKED_BOTTOM:
            pDocker->SetDockSize(rc.Height());
            Dock(pDocker, pDocker->GetDockStyle() | DockZone);
            break;
        case DS_DOCKED_CONTAINER:
            {
                CDockContainer* pContainer = GetContainer();
                assert(pContainer);
                CDockContainer* pActive = 0;
                if (pDocker->GetContainer())
                    pActive = pDocker->GetContainer()->GetActiveContainer();

        //      pDocker->MoveDockChildren(this);
                DockInContainer(pDocker, pDocker->GetDockStyle() | DockZone, FALSE);
                if (pActive)
                    pContainer->SetActiveContainer(pActive);
                else
                    pContainer->SelectPage(0);
            }
            break;
        case DS_DOCKED_LEFTMOST:
        case DS_DOCKED_RIGHTMOST:
            pDocker->SetDockSize(rc.Width());
            DockOuter(pDocker, pDocker->GetDockStyle() | DockZone);
            break;
        case DS_DOCKED_TOPMOST:
        case DS_DOCKED_BOTTOMMOST:
            pDocker->SetDockSize(rc.Height());
            DockOuter(pDocker, pDocker->GetDockStyle() | DockZone);
            break;
        }

        GetDockHint().Destroy();
        CloseAllTargets();

        return 0;
    }


    inline LRESULT CDocker::OnExitSizeMove(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);

        m_isBlockMove = FALSE;
        m_isDragging = FALSE;
        SendNotify(UWN_DOCKEND);

        return 0;
    }


    inline LRESULT CDocker::OnMouseActivate(UINT, WPARAM, LPARAM)
    {
        CPoint pt = GetCursorPos();

        if (PtInRect(GetDockClient().GetWindowRect(), pt)) // only for this docker
        {
            GetDockAncestor()->PostMessage(UWM_DOCKACTIVATE);
        }

        return 0;  // Return 0 to stop propogating this message to parent windows
    }


    inline LRESULT CDocker::OnNCLButtonDblClk(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        m_isDragging = FALSE;
        return FinalWindowProc(msg, wparam, lparam);
    }


    inline LRESULT CDocker::OnNotify(WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);
        LPDRAGPOS pdp = (LPDRAGPOS)lparam;

        if (IsWindowVisible())
        {
            switch (((LPNMHDR)lparam)->code)
            {
            case UWN_BARSTART:      return OnBarStart(pdp);
            case UWN_BARMOVE:       return OnBarMove(pdp);
            case UWN_BAREND:        return OnBarEnd(pdp);
            case UWN_DOCKSTART:     return OnDockStart(pdp);
            case UWN_DOCKMOVE:      return OnDockMove(pdp);
            case UWN_DOCKEND:       return OnDockEnd(pdp);
            }
        }

        return 0;
    }


    inline LRESULT CDocker::OnSize(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (this == GetTopmostDocker())
        {
            // Reposition the dock children
            if (IsUndocked() && IsWindowVisible() && !m_isClosing)
                RecalcDockLayout();
        }

        return CWnd::WndProcDefault(msg, wparam, lparam);
    }


    inline LRESULT CDocker::OnSysColorChange(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);

        if (this == GetDockAncestor())
        {
            COLORREF colour = GetSysColor(COLOR_BTNFACE);
            HWND hFrame = GetDockAncestor()->GetAncestor();

            ReBarTheme* pTheme = reinterpret_cast<ReBarTheme*>(::SendMessage(hFrame, UWM_GETRBTHEME, 0, 0));

            if (pTheme && pTheme->UseThemes && pTheme->clrBand2 != 0)
                colour = pTheme->clrBkgnd2;
            else
                colour = GetSysColor(COLOR_BTNFACE);

            // Set the splitter bar colour for each docker descendant
            std::vector<DockPtr>::const_iterator iter;
            for (iter = GetAllChildren().begin(); iter != GetAllChildren().end(); ++iter)
                (*iter)->SetBarColor(colour);

            // Set the splitter bar colour for the docker ancestor
            SetBarColor(colour);
        }

        return FinalWindowProc(msg, wparam, lparam);
    }


    inline LRESULT CDocker::OnSysCommand(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch(wparam&0xFFF0)
        {
        case SC_MOVE:
            // An undocked docker is being moved
            {
                BOOL isEnabled = FALSE;
                m_isDragging = TRUE;
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                ::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &isEnabled, 0);

                // Turn on DragFullWindows for this move
                ::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, 0, 0);

                // Process this message
                DefWindowProc(WM_SYSCOMMAND, wparam, lparam);

                // Return DragFullWindows to its previous state
                ::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, isEnabled, 0, 0);
                return 0;
            }
        case SC_CLOSE:
            // The close button is pressed on an undocked docker
            m_isClosing = TRUE;
            break;
        }
        return FinalWindowProc(msg, wparam, lparam);
    }


    inline LRESULT CDocker::OnWindowPosChanging(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Suspend dock drag moving while over dock zone
        if (m_isBlockMove)
        {
            LPWINDOWPOS pWndPos = (LPWINDOWPOS)lparam;
            pWndPos->flags |= SWP_NOMOVE|SWP_FRAMECHANGED;
            return 0;
        }

        return FinalWindowProc(msg, wparam, lparam);
    }


    inline LRESULT CDocker::OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);

        if (m_isDragging)
        {
            // Send a Move notification to the parent
            if ( IsLeftButtonDown() )
            {
                LPWINDOWPOS wPos = (LPWINDOWPOS)lparam;
                if ((!(wPos->flags & SWP_NOMOVE)) || m_isBlockMove)
                    SendNotify(UWN_DOCKMOVE);
            }
            else
            {
                CloseAllTargets();
                m_isBlockMove = FALSE;
            }
        }

        return FinalWindowProc(msg, wparam, lparam);
    }


    inline void CDocker::PreCreate(CREATESTRUCT& cs)
    {
        // Specify the WS_POPUP style to have this window owned
        if (this != GetDockAncestor())
            cs.style = WS_POPUP;

        cs.dwExStyle = WS_EX_TOOLWINDOW;
    }


    inline void CDocker::PreRegisterClass(WNDCLASS& wc)
    {
        wc.lpszClassName = _T("Win32++ Docker");
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    }


    // Repositions child windows
    inline void CDocker::RecalcDockChildLayout(CRect& rc)
    {
        // This function positions the Docker's dock children, the Dockers client area
        //  and draws the dockbar bars.

        // Notes:
        // 1) This function is called recursively.
        // 2) The client area and child dockers are positioned simultaneously with
        //      DeferWindowPos to avoid drawing errors in complex docker arrangements.
        // 3) The docker's client area contains the docker's caption (if any) and the docker's view window.

        // Note: All top level dockers are undocked, including the dock ancestor.

        BOOL rtl = FALSE;
#ifdef WS_EX_LAYOUTRTL
        rtl = (GetExStyle() & WS_EX_LAYOUTRTL);
#endif

        if (IsDocked())
        {
            rc.OffsetRect(-rc.left, -rc.top);
        }

        HDWP hdwp = BeginDeferWindowPos(static_cast<int>(m_dockChildren.size()) +2);

        // Step 1: Calculate the position of each Docker child, DockBar, and Client window.
        //   The Client area = the docker rect minus the area of dock children and the dock bar (splitter bar).
        std::vector<CDocker*>::const_iterator iter;
        for (iter = m_dockChildren.begin(); iter != m_dockChildren.end(); ++iter)
        {
            CRect rcChild = rc;
            double dockSize = (*iter)->m_dockStartSize;
            int minSize = 30;

            // Calculate the size of the Docker children
            switch ((*iter)->GetDockStyle() & 0xF)
            {
            case DS_DOCKED_LEFT:
                if ((*iter)->GetDockStyle() & DS_NO_FIXED_RESIZE)
                    dockSize = MIN((*iter)->m_dockSizeRatio*(GetWindowRect().Width()), rcChild.Width());

                if (rtl)
                {
                    rcChild.left = rcChild.right - static_cast<int>(dockSize);
                    rcChild.left = MIN(rcChild.left, rc.right - minSize);
                    rcChild.left = MAX(rcChild.left, rc.left + minSize);
                }
                else
                {
                    rcChild.right = rcChild.left + static_cast<int>(dockSize);
                    rcChild.right = MAX(rcChild.right, rc.left + minSize);
                    rcChild.right = MIN(rcChild.right, rc.right - minSize);
                }
                break;
            case DS_DOCKED_RIGHT:
                if ((*iter)->GetDockStyle() & DS_NO_FIXED_RESIZE)
                    dockSize = MIN((*iter)->m_dockSizeRatio*(GetWindowRect().Width()), rcChild.Width());

                if (rtl)
                {
                    rcChild.right = rcChild.left + static_cast<int>(dockSize);
                    rcChild.right = MAX(rcChild.right, rc.left + minSize);
                    rcChild.right = MIN(rcChild.right, rc.right - minSize);
                }
                else
                {
                    rcChild.left = rcChild.right - static_cast<int>(dockSize);
                    rcChild.left = MIN(rcChild.left, rc.right - minSize);
                    rcChild.left = MAX(rcChild.left, rc.left + minSize);
                }

                break;
            case DS_DOCKED_TOP:
                if ((*iter)->GetDockStyle() & DS_NO_FIXED_RESIZE)
                    dockSize = MIN((*iter)->m_dockSizeRatio*(GetWindowRect().Height()), rcChild.Height());

                rcChild.bottom = rcChild.top + static_cast<int>(dockSize);
                rcChild.bottom = MAX(rcChild.bottom, rc.top + minSize);
                rcChild.bottom = MIN(rcChild.bottom, rc.bottom - minSize);
                break;
            case DS_DOCKED_BOTTOM:
                if ((*iter)->GetDockStyle() & DS_NO_FIXED_RESIZE)
                    dockSize = MIN((*iter)->m_dockSizeRatio*(GetWindowRect().Height()), rcChild.Height());

                rcChild.top = rcChild.bottom - static_cast<int>(dockSize);
                rcChild.top = MIN(rcChild.top, rc.bottom - minSize);
                rcChild.top = MAX(rcChild.top, rc.top + minSize);

                break;
            }

            if ((*iter)->IsDocked())
            {
                // Position this docker's children
                hdwp = (*iter)->DeferWindowPos(hdwp, NULL, rcChild, SWP_SHOWWINDOW|SWP_FRAMECHANGED);
                (*iter)->m_childRect = rcChild;

                rc.SubtractRect(rc, rcChild);

                // Calculate the dimensions of the splitter bar
                CRect barRect = rc;
                DWORD DockSide = (*iter)->GetDockStyle() & 0xF;

                if (DS_DOCKED_LEFT   == DockSide)
                {
                    if (rtl) barRect.left   = barRect.right - (*iter)->GetBarWidth();
                    else     barRect.right  = barRect.left + (*iter)->GetBarWidth();
                }

                if (DS_DOCKED_RIGHT  == DockSide)
                {
                    if (rtl) barRect.right  = barRect.left + (*iter)->GetBarWidth();
                    else     barRect.left   = barRect.right - (*iter)->GetBarWidth();
                }

                if (DS_DOCKED_TOP    == DockSide) barRect.bottom = barRect.top + (*iter)->GetBarWidth();
                if (DS_DOCKED_BOTTOM == DockSide) barRect.top    = barRect.bottom - (*iter)->GetBarWidth();

                // Save the splitter bar position. We will reposition it later.
                (*iter)->m_barRect = barRect;
                rc.SubtractRect(rc, barRect);
            }
        }

        // Step 2: Position the Dock client and dock bar
        if (GetDockClient().IsWindow())
            hdwp = GetDockClient().DeferWindowPos(hdwp, NULL, rc, SWP_SHOWWINDOW |SWP_FRAMECHANGED);
        EndDeferWindowPos(hdwp);

        // Position the dockbar. Only docked dockers have a dock bar.
        if (IsDocked())
        {
            CRect barRect;
            barRect.IntersectRect(m_barRect, GetDockParent()->GetViewRect());

            // The SWP_NOCOPYBITS forces a redraw of the dock bar.
            GetDockBar().SetWindowPos(NULL, barRect, SWP_SHOWWINDOW|SWP_FRAMECHANGED|SWP_NOCOPYBITS);
        }

        // Step 3: Now recurse through the docker's children. They might have children of their own.
        for (iter = m_dockChildren.begin(); iter != m_dockChildren.end(); ++iter)
        {
            (*iter)->RecalcDockChildLayout((*iter)->m_childRect);
        }
    }


    // Repositions the dock children of a top level docker.
    inline void CDocker::RecalcDockLayout()
    {
        if (GetDockAncestor()->IsWindow())
        {
            CRect rc = GetTopmostDocker()->GetViewRect();
            GetTopmostDocker()->RecalcDockChildLayout(rc);
            GetTopmostDocker()->UpdateWindow();
        }
    }


    // Returns a vector of sorted dockers, used by SaveRegistrySettings.
    inline std::vector<CDocker*> CDocker::SortDockers()
    {
        std::vector<CDocker*> vSorted;
        std::vector<CDocker*>::const_iterator itSort;
        std::vector<DockPtr>::const_iterator itAll;

        // Add undocked top level dockers
        for (itAll = GetAllChildren().begin(); itAll !=  GetAllChildren().end(); ++itAll)
        {
            if (!(*itAll)->GetDockParent())
                vSorted.push_back((*itAll).get());
        }

        // Add dock ancestor's children
        vSorted.insert(vSorted.end(), GetDockAncestor()->GetDockChildren().begin(), GetDockAncestor()->GetDockChildren().end());

        // Add other dock children
        int index = 0;
        itSort = vSorted.begin();
        while (itSort < vSorted.end())
        {
            vSorted.insert(vSorted.end(), (*itSort)->GetDockChildren().begin(), (*itSort)->GetDockChildren().end());
            itSort = vSorted.begin() + (++index);
        }

        // Add dockers docked in containers
        std::vector<CDocker*> vDockContainers;
        for (itSort = vSorted.begin(); itSort< vSorted.end(); ++itSort)
        {
            if ((*itSort)->GetContainer())
                vDockContainers.push_back(*itSort);
        }

        for (itSort = vDockContainers.begin(); itSort < vDockContainers.end(); ++itSort)
        {
            CDockContainer* pContainer = (*itSort)->GetContainer();

            for (UINT i = 0; i < pContainer->GetAllContainers().size(); ++i)
            {
                CDockContainer* pChild = pContainer->GetContainerFromIndex(i);

                if (pChild != pContainer)
                {
                    CDocker* pDocker = GetDockFromView(pChild);
                    assert(pDocker);
                    vSorted.push_back(pDocker);
                }
            }
        }

        if (GetContainer())
        {
            CDockContainer* pContainer = GetContainer();

            for (UINT i = 0; i < pContainer->GetAllContainers().size(); ++i)
            {
                CDockContainer* pChild = pContainer->GetContainerFromIndex(i);

                if (pChild != pContainer)
                {
                    CDocker* pDocker = GetDockFromView(pChild);
                    assert(pDocker);
                    vSorted.push_back(pDocker);
                }
            }
        }

        return vSorted;
    }


    // Called when the docker's splitter bar is dragged
    inline void CDocker::ResizeDockers(LPDRAGPOS pDragPos)
    {
        assert(pDragPos);

        POINT pt = pDragPos->pos;
        ScreenToClient(pt);

        CDocker* pDocker = pDragPos->pDocker;
        assert(pDocker);

        RECT rcDock = pDocker->GetWindowRect();
        ScreenToClient(rcDock);

        int barWidth = pDocker->GetDockBar().GetWidth();
        int dockSize;

        BOOL rtl = FALSE;
#ifdef WS_EX_LAYOUTRTL
        rtl = (GetExStyle() & WS_EX_LAYOUTRTL);
#endif

        CRect rcDockParent = pDocker->m_pDockParent->GetWindowRect();

        double dockStartSize = static_cast<double>(pDocker->m_dockStartSize);
        double parentWidth = static_cast<double>(rcDockParent.Width());
        double parentHeight = static_cast<double>(rcDockParent.Height());

        switch (pDocker->GetDockStyle() & 0xF)
        {
        case DS_DOCKED_LEFT:
            if (rtl) dockSize = rcDock.right - MAX(pt.x, barWidth / 2) - (barWidth / 2);
            else     dockSize = MAX(pt.x, barWidth / 2) - rcDock.left - (barWidth / 2);

            dockSize = MAX(-barWidth, dockSize);
            pDocker->SetDockSize(dockSize);
            pDocker->m_dockSizeRatio = dockStartSize / parentWidth;
            break;
        case DS_DOCKED_RIGHT:
            if (rtl)  dockSize = MAX(pt.x, barWidth / 2) - rcDock.left - (barWidth / 2);
            else      dockSize = rcDock.right - MAX(pt.x, barWidth / 2) - (barWidth / 2);

            dockSize = MAX(-barWidth, dockSize);
            pDocker->SetDockSize(dockSize);
            pDocker->m_dockSizeRatio = dockStartSize / parentWidth;
            break;
        case DS_DOCKED_TOP:
            dockSize = MAX(pt.y, barWidth / 2) - rcDock.top - (barWidth / 2);
            dockSize = MAX(-barWidth, dockSize);
            pDocker->SetDockSize(dockSize);
            pDocker->m_dockSizeRatio = dockStartSize / parentHeight;
            break;
        case DS_DOCKED_BOTTOM:
            dockSize = rcDock.bottom - MAX(pt.y, barWidth / 2) - (barWidth / 2);
            dockSize = MAX(-barWidth, dockSize);
            pDocker->SetDockSize(dockSize);
            pDocker->m_dockSizeRatio = dockStartSize / parentHeight;
            break;
        }

        RecalcDockLayout();
    }


    // Stores the docking configuration in the registry
    // NOTE: This function assumes that each docker has a unique DockID
    inline BOOL CDocker::SaveDockRegistrySettings(LPCTSTR pRegistryKeyName)
    {
        std::vector<CDocker*> sortedDockers = SortDockers();
        std::vector<CDocker*>::const_iterator iter;
        std::vector<DockInfo> allDockInfo;

        if (pRegistryKeyName)
        {
            CRegKey key;
            CRegKey keyDock;
            CString keyName = _T("Software\\") + CString(pRegistryKeyName);

            try
            {
                if (!VerifyDockers())
                    throw (CUserException(_T("Dockers are in an inconsistant state")));

                // Create the App's registry key
                if (ERROR_SUCCESS != key.Create(HKEY_CURRENT_USER, keyName))
                    throw (CUserException(_T("Create Key failed")));

                if (ERROR_SUCCESS != key.Open(HKEY_CURRENT_USER, keyName))
                    throw (CUserException(_T("Open Key failed")));

                // Remove Old Docking info ...
                key.RecurseDeleteKey(_T("Dock Windows"));

                // Fill the DockInfo vector with the docking information
                for (iter = sortedDockers.begin(); iter != sortedDockers.end(); ++iter)
                {
                    DockInfo di;
                    ZeroMemory(&di, sizeof(DockInfo));
                    if (! (*iter)->IsWindow())
                        throw (CUserException(_T("Can't save Docker in registry. \n")));

                    di.dockID    = (*iter)->GetDockID();
                    di.dockStyle = (*iter)->GetDockStyle();
                    di.dockSize  = (*iter)->GetDockSize();
                    di.rect      = (*iter)->GetWindowRect();
                    if ((*iter)->GetDockParent())
                        di.dockParentID = (*iter)->GetDockParent()->GetDockID();

                    di.isInAncestor = ((*iter)->GetDockParent() == GetDockAncestor());

                    allDockInfo.push_back(di);
                }

                if (ERROR_SUCCESS != key.Create(key, _T("Dock Windows")))
                    throw (CUserException(_T("Create KeyDock failed")));

                if (ERROR_SUCCESS != keyDock.Open(key, _T("Dock Windows")))
                    throw (CUserException(_T("Open KeyDock failed")));

                CString SubKeyName;

                // Add the Dock windows information to the registry
                for (UINT u = 0; u < allDockInfo.size(); ++u)
                {
                    DockInfo di = allDockInfo[u];
                    SubKeyName.Format(_T("DockChild%u"), u);
                    if (ERROR_SUCCESS != keyDock.SetBinaryValue(SubKeyName, &di, sizeof(DockInfo)))
                        throw (CUserException(_T("KeyDock SetBinaryValue failed")));
                }

                // Add dock container info to the registry
                UINT container = 0;

                if (GetContainer())
                    SaveContainerRegistrySettings(keyDock, GetContainer(), container);


                for (iter = sortedDockers.begin(); iter != sortedDockers.end(); ++iter)
                {
                    CDockContainer* pContainer = (*iter)->GetContainer();

                    if (pContainer && ( !((*iter)->GetDockStyle() & DS_DOCKED_CONTAINER) ))
                    {
                        SaveContainerRegistrySettings(keyDock, pContainer, container);
                    }
                }
            }

            catch (const CUserException& e)
            {
                Trace("*** Failed to save dock settings in registry. ***");
                Trace(e.GetText()); Trace("\n");

                // Roll back the registry changes by deleting the subkeys
                if (key.GetKey())
                {
                    key.RecurseDeleteKey(_T("Dock Windows"));
                }

                return FALSE;
            }
        }

        return TRUE;
    }


    // Stores the docking container configuration in the registry.
    inline void CDocker::SaveContainerRegistrySettings(CRegKey& keyDock, CDockContainer* pContainer, UINT& container)
    {
        CRegKey keyContainer;
        CString subKeyName;
        subKeyName.Format(_T("DockContainer%u"), container++);
        if (ERROR_SUCCESS != keyDock.Create(keyDock, subKeyName))
            throw (CUserException(_T("Create KeyDockContainer failed")));

        if (ERROR_SUCCESS != keyContainer.Open(keyDock, subKeyName))
            throw (CUserException(_T("Open KeyContainer failed")));

        // Store the container group's parent
        CDocker* pDocker = GetDockFromView(pContainer);
        if (pDocker == 0)
            throw CUserException(_T("Failed to get docker from container view"));
        int id = pDocker->GetDockID();
        if (ERROR_SUCCESS != keyContainer.SetDWORDValue(_T("Parent Container"), id))
            throw (CUserException(_T("KeyContainer SetDWORDValue failed")));

        // Store the active (selected) container
        pDocker = GetDockFromView(pContainer->GetActiveContainer());
        if (pDocker == 0)
            id = 0;
        else
            id = pDocker->GetDockID();

        if (ERROR_SUCCESS != keyContainer.SetDWORDValue(_T("Active Container"), id))
            throw (CUserException(_T("KeyContainer SetDWORDValue failed")));

        // Store the tab order
        for (UINT u2 = 0; u2 < pContainer->GetAllContainers().size(); ++u2)
        {
            subKeyName.Format(_T("Tab%u"), u2);
            CDockContainer* pTab = pContainer->GetContainerFromIndex(u2);
            if (pTab == 0)
                throw CUserException(_T("Failed to get container from index"));
            pDocker = GetDockFromView(pTab);
            if (pDocker == 0)
                throw CUserException(_T("Failed to get docker from view"));
            int tabID = pDocker->GetDockID();

            if (ERROR_SUCCESS != keyContainer.SetDWORDValue(subKeyName, tabID))
                throw (CUserException(_T("RegSetValueEx failed")));
        }
    }


    // Sends a docking notification to the docker below the cursor
    inline void CDocker::SendNotify(UINT messageID)
    {
        DRAGPOS dragPos;
        dragPos.hdr.code = messageID;
        dragPos.hdr.hwndFrom = GetHwnd();
        dragPos.pos = GetCursorPos();
        dragPos.dockZone = m_dockZone;
        dragPos.pDocker = this;
        m_dockZone = 0;

        CDocker* pDocker = GetDockUnderDragPoint(dragPos.pos);

        if (pDocker)
            pDocker->SendMessage(WM_NOTIFY, 0, (LPARAM)&dragPos);
        else
        {
            if (GetDockHint().IsWindow())
                GetDockHint().Destroy();

            CloseAllTargets();
            m_isBlockMove = FALSE;
        }
    }


    inline void CDocker::SetDockStyle(DWORD dockStyle)
    {
        if (IsWindow())
        {
            if ((dockStyle & DS_CLIENTEDGE) != (m_dockStyle & DS_CLIENTEDGE))
            {
                if (dockStyle & DS_CLIENTEDGE)
                {
                    DWORD exStyle = GetDockClient().GetExStyle() | WS_EX_CLIENTEDGE;
                    GetDockClient().SetExStyle(exStyle);
                    GetDockClient().RedrawWindow(RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
                }
                else
                {
                    DWORD exStyle = GetDockClient().GetExStyle();
                    exStyle &= ~WS_EX_CLIENTEDGE;
                    GetDockClient().SetExStyle(exStyle);
                    GetDockClient().RedrawWindow(RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_FRAME);
                }
            }

            RecalcDockLayout();
        }

        m_dockStyle = dockStyle;
    }


    // Sets the caption text
    inline void CDocker::SetCaption(LPCTSTR pCaption)
    {
        GetDockClient().SetCaption(pCaption);

        if (IsWindow() && (this != GetDockAncestor()))
            SetWindowText(pCaption);
    }


    inline void CDocker::SetCaptionColors(COLORREF foregnd1, COLORREF backgnd1, COLORREF foreGnd2, COLORREF backGnd2, COLORREF penColor /*= RGB(160, 150, 140)*/)
    {
        GetDockClient().SetCaptionColors(foregnd1, backgnd1, foreGnd2, backGnd2, penColor);
    }


    // Sets the height of the caption
    inline void CDocker::SetCaptionHeight(int height)
    {
        m_ncHeight = height;
        if (IsWindow())
        {
            RedrawWindow();
            RecalcDockLayout();
        }
    }


    // Sets the size of a docked docker
    inline void CDocker::SetDockSize(int dockSize)
    {
        if (IsDocked())
        {
            assert (m_pDockParent);
            CRect rc = m_pDockParent->GetWindowRect();
            switch (GetDockStyle() & 0xF)
            {
            case DS_DOCKED_LEFT:
            case DS_DOCKED_RIGHT:
                m_dockStartSize = MIN(dockSize,rc.Width());
                m_dockSizeRatio = static_cast<double>(m_dockStartSize) / static_cast<double>(rc.Width());
                break;
            case DS_DOCKED_TOP:
            case DS_DOCKED_BOTTOM:
                m_dockStartSize = MIN(dockSize,rc.Height());
                m_dockSizeRatio = static_cast<double>(m_dockStartSize) / static_cast<double>(rc.Height());
                break;
            }

            RecalcDockLayout();
        }
        else
        {
            m_dockStartSize = dockSize;
            m_dockSizeRatio = 1.0;
        }
    }


    inline void CDocker::SetDragAutoResize(BOOL autoResize)
    {
        m_isDragAutoResize = autoResize;
    }


    // Assigns the view window to the docker
    inline void CDocker::SetView(CWnd& wndView)
    {
        GetDockClient().SetView(wndView);
        CDockContainer* pContainer = GetContainer();
        if (pContainer)
        {
            SetCaption(pContainer->GetDockCaption().c_str());
        }
    }


    // One of the steps required for undocking
    inline void CDocker::PromoteFirstChild()
    {
        // Promote our first child to replace ourself
        if (m_pDockParent)
        {
            std::vector<CDocker*>::iterator iter;
            std::vector<CDocker*>& children = m_pDockParent->m_dockChildren;

            for (iter = children.begin(); iter != children.end(); ++iter)
            {
                if ((*iter) == this)
                {
                    if (m_dockChildren.size() > 0)
                        // swap our first child for ourself as a child of the parent
                        (*iter) = m_dockChildren[0];
                    else
                        // remove ourself as a child of the parent
                        children.erase(iter);

                    // Done
                    break;
                }
            }
        }

        // Transfer styles and data and children to the child docker
        CDocker* pDockFirstChild = NULL;
        if (m_dockChildren.size() > 0)
        {
            pDockFirstChild = m_dockChildren[0];
            pDockFirstChild->m_dockStyle = (pDockFirstChild->m_dockStyle & 0xFFFFFFF0 ) | (m_dockStyle & 0xF);
            pDockFirstChild->m_dockStartSize = m_dockStartSize;
            pDockFirstChild->m_dockSizeRatio = m_dockSizeRatio;

            if (m_pDockParent)
            {
                pDockFirstChild->m_pDockParent = m_pDockParent;
                pDockFirstChild->SetParent(*m_pDockParent);
                pDockFirstChild->GetDockBar().SetParent(*m_pDockParent);
            }
            else
            {
                std::vector<CDocker*>::const_iterator iter;
                for (iter = m_dockChildren.begin() + 1; iter != m_dockChildren.end(); ++iter)
                    (*iter)->ShowWindow(SW_HIDE);

                pDockFirstChild->ConvertToPopup(GetWindowRect(), TRUE);
                pDockFirstChild->GetDockBar().ShowWindow(SW_HIDE);
            }

            m_dockChildren.erase(m_dockChildren.begin());
            MoveDockChildren(pDockFirstChild);
        }
    }


    inline void CDocker::ConvertToChild(HWND parent)
    {
        DWORD style = WS_CHILD | WS_VISIBLE;
        SetStyle(style);
        SetParent(parent);
        GetDockBar().SetParent(parent);
    }


    inline void CDocker::ConvertToPopup(const RECT& rc, BOOL showUndocked)
    {
        // Change the window to an "undocked" style
        ShowWindow(SW_HIDE);
        DWORD style = WS_POPUP| WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
        SetStyle(style);

        // Change the window's parent and reposition it
        if (GetDockBar().IsWindow())
            GetDockBar().ShowWindow(SW_HIDE);

        SetWindowPos(0, 0, 0, 0, 0, SWP_NOSENDCHANGING|SWP_HIDEWINDOW|SWP_NOREDRAW);
        m_pDockParent = 0;
        SetParent(0);

        DWORD styleShow = showUndocked? SWP_SHOWWINDOW : 0;
        SetWindowPos(NULL, rc, styleShow | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
        GetDockClient().SetWindowPos(NULL, GetClientRect(), SWP_SHOWWINDOW);
        SetWindowText(GetCaption().c_str());
    }


    inline CDocker* CDocker::SeparateFromDock()
    {
        // This performs some of the tasks required for undocking.
        // It is also used when a docker is hidden.
        CDocker* pDockUndockedFrom = GetDockParent();
        if (!pDockUndockedFrom && (m_dockChildren.size() > 0))
            pDockUndockedFrom = m_dockChildren[0];

        PromoteFirstChild();
        m_pDockParent = 0;

        if (GetDockBar().IsWindow())
            GetDockBar().ShowWindow(SW_HIDE);

        m_dockStyle = m_dockStyle & 0xFFFFFFF0;
        m_dockStyle &= ~DS_DOCKED_CONTAINER;

        return pDockUndockedFrom;
    }


    // Allows nested calls to SetRedraw.
    inline BOOL CDocker::SetRedraw(BOOL redraw /* = TRUE*/)
    {
        redraw? ++m_redrawCount : --m_redrawCount ;

        return (SendMessage(WM_SETREDRAW, (WPARAM)(m_redrawCount >= 0), 0) != 0);
    }


    inline void CDocker::SetUndockPosition(CPoint pt, BOOL showUndocked)
    {
        m_isUndocking = TRUE;
        CRect rc;
        rc = GetDockClient().GetWindowRect();
        CRect testRect = rc;
        testRect.bottom = MIN(testRect.bottom, testRect.top + m_ncHeight);
        if ( !testRect.PtInRect(pt))
            rc.SetRect(pt.x - rc.Width()/2, pt.y - m_ncHeight/2, pt.x + rc.Width()/2, pt.y - m_ncHeight/2 + rc.Height());

        ConvertToPopup(rc, showUndocked);

        m_isUndocking = FALSE;

        // Send the undock notification to the frame
        NMHDR nmhdr;
        ZeroMemory(&nmhdr, sizeof(nmhdr));
        nmhdr.hwndFrom = GetHwnd();
        nmhdr.code = UWN_UNDOCKED;
        nmhdr.idFrom = m_dockID;
        HWND hFrame = GetDockAncestor()->GetAncestor();
        assert(hFrame);

        ::SendMessage(hFrame, WM_NOTIFY, (WPARAM)m_dockID, (LPARAM)&nmhdr);

        // Initiate the window move
        SetCursorPos(pt.x, pt.y);
        ScreenToClient(pt);
        PostMessage(WM_SYSCOMMAND, (WPARAM)(SC_MOVE|0x0002), MAKELPARAM(pt.x, pt.y));
    }


    // Undocks a docker
    inline void CDocker::Undock(CPoint pt, BOOL showUndocked)
    {
        // Return if we shouldn't undock
        if (GetDockStyle() & DS_NO_UNDOCK) return;

        // Undocking isn't supported on Win95
        if (1400 == GetWinVersion()) return;

        CDocker* pDockUndockedFrom = SeparateFromDock();

        // Position and draw the undocked window, unless it is about to be closed
        SetUndockPosition(pt, showUndocked);

        // Give the view window focus unless its child already has it
        if (!GetView().IsChild(GetFocus()))
            GetView().SetFocus();

        RecalcDockLayout();
        if ((pDockUndockedFrom) && (pDockUndockedFrom->GetTopmostDocker() != GetTopmostDocker()))
            pDockUndockedFrom->RecalcDockLayout();
    }


    // Undocks a CDockContainer
    inline void CDocker::UndockContainer(CDockContainer* pContainer, CPoint pt, BOOL showUndocked)
    {
        assert(pContainer);
        assert(this == GetDockFromView(pContainer->GetContainerParent()));

        // Return if we shouldn't undock
        if (GetDockFromView(pContainer)->GetDockStyle() & DS_NO_UNDOCK) return;

        if (GetDockFromView(pContainer) == GetDockAncestor()) return;

        // Undocking isn't supported on Win95
        if (1400 == GetWinVersion()) return;

        CDocker* pDockUndockedFrom = this;
        if (&GetView() == pContainer)
        {
            // The parent container is being undocked, so we need
            // to transfer our child containers to a different docker

            // Choose a new docker from among the dockers for child containers
            CDocker* pDockNew = 0;
            CDocker* pDockOld = GetDockFromView(pContainer);
            assert(pDockOld);
            std::vector<ContainerInfo> AllContainers = pContainer->GetAllContainers();
            std::vector<ContainerInfo>::const_iterator iter = AllContainers.begin();
            while ((0 == pDockNew) && (iter != AllContainers.end()))
            {
                if ((*iter).pContainer != pContainer)
                {
                    pDockNew = (*iter).pContainer->GetDocker();
                    assert(pDockNew);
                }

                ++iter;
            }

            if (pDockNew)
            {
                // Move containers from the old docker to the new docker
                CDockContainer* pContainerNew = pDockNew->GetContainer();
                assert(pContainerNew);
                for (iter = AllContainers.begin(); iter != AllContainers.end(); ++iter)
                {
                    if ((*iter).pContainer != pContainer)
                    {
                        CDockContainer* pChildContainer = (*iter).pContainer;
                        pContainer->RemoveContainer(pChildContainer);
                        if (pContainerNew != pChildContainer)
                        {
                            pContainerNew->AddContainer(pChildContainer);
                            CDocker* pDocker = GetDockFromView(pChildContainer);
                            assert(pDocker);
                            pDocker->SetParent(*pDockNew);
                            pDocker->m_pDockParent = pDockNew;
                        }
                    }
                }

                // Now transfer the old docker's settings to the new one.
                pDockUndockedFrom = pDockNew;
                pDockNew->m_dockStyle       = pDockOld->m_dockStyle;
                pDockNew->m_dockStartSize   = pDockOld->m_dockStartSize;
                pDockNew->m_dockSizeRatio   = pDockOld->m_dockSizeRatio;
                if (pDockOld->IsDocked())
                {
                    pDockNew->m_pDockParent     = pDockOld->m_pDockParent;
                    pDockNew->SetParent(pDockOld->GetParent());
                }
                else
                {
                    CRect rc = pDockOld->GetWindowRect();
                    pDockNew->ShowWindow(SW_HIDE);
                    DWORD style = WS_POPUP| WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE;
                    pDockNew->SetStyle(style);
                    pDockNew->m_pDockParent = 0;
                    pDockNew->SetParent(0);
                    pDockNew->SetWindowPos(NULL, rc, SWP_SHOWWINDOW|SWP_FRAMECHANGED| SWP_NOOWNERZORDER);
                }
                pDockNew->GetDockBar().SetParent(pDockOld->GetParent());
                pDockNew->GetView().SetFocus();

                // Transfer the Dock children to the new docker
                pDockOld->MoveDockChildren(pDockNew);

                // insert pDockNew into its DockParent's DockChildren vector
                if (pDockNew->m_pDockParent)
                {
                    std::vector<CDocker*>::iterator p;
                    for (p = pDockNew->m_pDockParent->m_dockChildren.begin(); p != pDockNew->m_pDockParent->m_dockChildren.end(); ++p)
                    {
                        if (*p == this)
                        {
                            pDockNew->m_pDockParent->m_dockChildren.insert(p, pDockNew);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            // This is a child container, so simply remove it from the parent
            CDockContainer* pContainerParent = GetContainer();
            assert(pContainerParent);
            pContainerParent->RemoveContainer(pContainer, showUndocked);
            pContainerParent->SetTabSize();
            pContainerParent->SetFocus();
            if (pContainerParent->GetViewPage().IsWindow())
                pContainerParent->GetViewPage().SetParent(*pContainerParent);
        }

        // Finally do the actual undocking
        CDocker* pDocker = GetDockFromView(pContainer);
        assert(pDocker);
        CRect rc = GetDockClient().GetWindowRect();
        ScreenToClient(rc);
        pDocker->GetDockClient().SetWindowPos(NULL, rc, SWP_SHOWWINDOW);
        pDocker->Undock(pt, showUndocked);
        pDocker->GetDockBar().SetParent(*GetDockAncestor());
        pDockUndockedFrom->ShowWindow();
        pDockUndockedFrom->RecalcDockLayout();
        pDocker->BringWindowToTop();
    }


    inline LRESULT CDocker::WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_ACTIVATE:           return OnActivate(msg, wparam, lparam);
        case WM_SYSCOMMAND:         return OnSysCommand(msg, wparam, lparam);
        case WM_EXITSIZEMOVE:       return OnExitSizeMove(msg, wparam, lparam);
        case WM_MOUSEACTIVATE:      return OnMouseActivate(msg, wparam, lparam);
        case WM_NCLBUTTONDBLCLK:    return OnNCLButtonDblClk(msg, wparam, lparam);
        case WM_SIZE:               return OnSize(msg, wparam, lparam);
        case WM_SYSCOLORCHANGE:     return OnSysColorChange(msg, wparam, lparam);
        case WM_WINDOWPOSCHANGING:  return OnWindowPosChanging(msg, wparam, lparam);
        case WM_WINDOWPOSCHANGED:   return OnWindowPosChanged(msg, wparam, lparam);

        // Messages defined by Win32++
        case UWM_DOCKACTIVATE:      return OnDockActivated(msg, wparam, lparam);
        case UWM_DOCKDESTROYED:     return OnDockDestroyed(msg, wparam, lparam);
        case UWM_GETCDOCKER:        return reinterpret_cast<LRESULT>(this);
        }

        return CWnd::WndProcDefault(msg, wparam, lparam);
    }


    // Static callback function to enumerate top level dockers excluding
    // the one being dragged. Top level windows are enumurated in Z order.
    inline BOOL CALLBACK CDocker::EnumWindowsProc(HWND hTop, LPARAM lparam)
    {
        CDocker* pThis = reinterpret_cast<CDocker*>(lparam);
        assert(dynamic_cast<CDocker*>(pThis));
        CPoint pt = pThis->m_dockPoint;

        // Update hWndTop if the DockAncestor is a child of the top level window
        if (::IsChild(hTop, pThis->GetDockAncestor()->GetHwnd()))
            hTop = pThis->GetDockAncestor()->GetHwnd();

        // Assign this docker's m_dockUnderPoint
        if (pThis->IsRelated(hTop) && hTop != pThis->GetHwnd())
        {
            CRect rc;
            ::GetWindowRect(hTop, &rc);
            if ( rc.PtInRect(pt) )
            {
                pThis->m_dockUnderPoint = hTop;
                return FALSE;   // Stop enumerating
            }
        }

        return TRUE;    // Continue enumerating
    }


    //////////////////////////////////////
    // Declaration of the CDockContainer class
    inline CDockContainer::CDockContainer() : m_currentPage(0), m_pDocker(0),
                                         m_pContainerParent(0), m_tabIcon(0),
                                         m_pressedTab(-1), m_isHideSingleTab(FALSE)
    {
        m_pContainerParent = this;
        m_viewPage.SetContainer(this);
    }


    inline CDockContainer::~CDockContainer()
    {
        if (m_tabIcon != 0)
            DestroyIcon(m_tabIcon);
    }

    // Adds a container to the group. Set Insert to TRUE to insert the container as the
    //  first tab, or FALSE to add it as the last tab.
    inline void CDockContainer::AddContainer(CDockContainer* pContainer, BOOL insert /* = FALSE */, BOOL selecPage)
    {
        assert(pContainer);
        assert(this == m_pContainerParent); // Must be performed by parent container

        ContainerInfo ci;
        ci.pContainer = pContainer;
        ci.title = pContainer->GetTabText();
        ci.image = GetODImageList().Add( pContainer->GetTabIcon() );
        int newPage = 0;
        if (insert)
        {
            m_allInfo.insert(m_allInfo.begin(), ci);
        }
        else
        {
            newPage = static_cast<int>(m_allInfo.size());
            m_allInfo.push_back(ci);
        }

        if (IsWindow())
        {
            TCITEM tie;
            ZeroMemory(&tie, sizeof(tie));
            tie.mask = TCIF_TEXT | TCIF_IMAGE;
            tie.iImage = ci.image;
            tie.pszText = const_cast<LPTSTR>(m_allInfo[newPage].title.c_str());
            InsertItem(newPage, &tie);

            if (selecPage)
                SelectPage(newPage);

            SetTabSize();
        }

        pContainer->m_pContainerParent = this;
        if (pContainer->IsWindow())
        {
            // Set the parent container relationships
            pContainer->GetViewPage().SetParent(*this);
            pContainer->GetViewPage().ShowWindow(SW_HIDE);
            RecalcLayout();
        }
    }


    // Adds Resource IDs to toolbar buttons.
    // A resource ID of 0 is a separator
    inline void CDockContainer::AddToolBarButton(UINT id, BOOL isEnabled /* = TRUE */)
    {
        GetToolBarData().push_back(id);
        GetToolBar().AddButton(id, isEnabled);
    }


    inline CDockContainer* CDockContainer::GetContainerFromIndex(UINT page)
    {
        CDockContainer* pContainer = NULL;
        if (page < m_allInfo.size())
            pContainer = m_allInfo[page].pContainer;

        return pContainer;
    }


    inline CDockContainer* CDockContainer::GetActiveContainer() const
    {
        assert(m_pContainerParent);

        if (m_pContainerParent->m_allInfo.size() > 0)
            return m_pContainerParent->m_allInfo[m_pContainerParent->m_currentPage].pContainer;
        else
            return 0;
    }


    // Returns a pointer to the active view window, or NULL if there is no active view.
    inline CWnd* CDockContainer::GetActiveView() const
    {
        if (GetActiveContainer())
            return GetActiveContainer()->GetView();
        else
            return 0;
    }


    inline CDockContainer* CDockContainer::GetContainerFromView(CWnd* pView) const
    {
        assert(pView);

        std::vector<ContainerInfo>::const_iterator iter;
        CDockContainer* pViewContainer = 0;
        for (iter = GetAll().begin(); iter != GetAll().end(); ++iter)
        {
            CDockContainer* pContainer = (*iter).pContainer;
            if (pContainer->GetView() == pView)
                pViewContainer = pContainer;
        }

        return pViewContainer;
    }


    inline int CDockContainer::GetContainerIndex(CDockContainer* pContainer)
    {
        assert(pContainer);
        int result = -1;

        for (int i = 0; i < static_cast<int>(m_pContainerParent->m_allInfo.size()); ++i)
        {
            if (m_pContainerParent->m_allInfo[i].pContainer == pContainer)
                result = i;
        }

        return result;
    }


    inline SIZE CDockContainer::GetMaxTabTextSize()
    {
        CSize sz;

        // Allocate an iterator for the ContainerInfo vector
        std::vector<ContainerInfo>::const_iterator iter;

        for (iter = m_allInfo.begin(); iter != m_allInfo.end(); ++iter)
        {
            CSize tempSize;
            CClientDC dc(*this);
            NONCLIENTMETRICS info = GetNonClientMetrics();
            dc.CreateFontIndirect(info.lfStatusFont);
            tempSize = dc.GetTextExtentPoint32(iter->title, lstrlen(iter->title));
            if (tempSize.cx > sz.cx)
                sz = tempSize;
        }

        return sz;
    }


    // Returns a tab's image index.
    inline int CDockContainer::GetTabImageID(UINT tab) const
    {
        assert (tab < GetAllContainers().size());
        return GetAllContainers()[tab].image;
    }


    // Returns a tab's text.
    inline CString CDockContainer::GetTabText(UINT tab) const
    {
        assert (tab < GetAllContainers().size());
        return GetAllContainers()[tab].title;
    }


    // Use this function to set the Resource IDs for the toolbar(s).
    inline void CDockContainer::SetupToolBar()
    {
/*      // Set the Resource IDs for the toolbar buttons
        AddToolBarButton( IDM_FILE_NEW   );
        AddToolBarButton( IDM_FILE_OPEN  );
        AddToolBarButton( IDM_FILE_SAVE  );
        AddToolBarButton( 0 );              // Separator
        AddToolBarButton( IDM_EDIT_CUT   );
        AddToolBarButton( IDM_EDIT_COPY  );
        AddToolBarButton( IDM_EDIT_PASTE );
        AddToolBarButton( 0 );              // Separator
        AddToolBarButton( IDM_FILE_PRINT );
        AddToolBarButton( 0 );              // Separator
        AddToolBarButton( IDM_HELP_ABOUT );
*/
    }


    // Called when a HWND is attached to this CWnd
    inline void CDockContainer::OnAttach()
    {
        m_pDocker = reinterpret_cast<CDocker*>((GetParent().GetParent().SendMessage(UWM_GETCDOCKER)));

        // Create and assign the tab's image list
        GetODImageList().Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0);

        // Add a tab for this container except for the DockAncestor
        if (!GetDocker() || GetDocker()->GetDockAncestor() != GetDocker())
        {
            assert(GetView());          // Use SetView in the constructor to set the view window

            ContainerInfo ci;
            ci.pContainer = this;
            ci.title = GetTabText();
            ci.image = GetODImageList().Add(GetTabIcon());
            m_allInfo.push_back(ci);

            // Create the page window
            GetViewPage().Create(*this);

            // Create the toolbar
            GetToolBar().Create(GetViewPage());
            DWORD style = GetToolBar().GetStyle();
            style |= CCS_NODIVIDER;
            GetToolBar().SetStyle(style);
            SetupToolBar();
            if (GetToolBarData().size() > 0)
            {
                // Set the toolbar images
                // A mask of 192,192,192 is compatible with AddBitmap (for Win95)
                if (!GetToolBar().SendMessage(TB_GETIMAGELIST, 0, 0))
                    SetToolBarImages(RGB(192, 192, 192), IDW_MAIN, 0, 0);

                GetToolBar().SendMessage(TB_AUTOSIZE, 0, 0);
            }
            else
                GetToolBar().Destroy();
        }

        // Set the tab control's font
        NONCLIENTMETRICS info = GetNonClientMetrics();
        GetTabFont().CreateFontIndirect(info.lfStatusFont);
        SetFont(GetTabFont());

        SetFixedWidth(TRUE);
        SetOwnerDraw(TRUE);

        // Add tabs for each container.
        for (int i = 0; i < static_cast<int>(m_allInfo.size()); ++i)
        {
            // Add tabs for each view.
            TCITEM tie;
            ZeroMemory(&tie, sizeof(tie));
            tie.mask = TCIF_TEXT | TCIF_IMAGE;
            tie.iImage = m_allInfo[i].image;
            tie.pszText = const_cast<LPTSTR>(m_allInfo[i].title.c_str());
            InsertItem(i, &tie);
        }

    }


    // Called when the left mouse button is pressed
    // Overrides CTab::OnLButtonDown
    inline LRESULT CDockContainer::OnLButtonDown(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);

        CPoint pt(lparam);
        TCHITTESTINFO info;
        ZeroMemory(&info, sizeof(info));
        info.pt = pt;
        m_pressedTab = HitTest(info);

        return FinalWindowProc(msg, wparam, lparam);
    }


    // Called when the left mouse button is released
    // Overrides CTab::OnLButtonUp and takes no action
    inline LRESULT CDockContainer::OnLButtonUp(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);
        return FinalWindowProc(msg, wparam, lparam);
    }


    inline LRESULT CDockContainer::OnMouseLeave(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Overrides CTab::OnMouseLeave

        if (IsLeftButtonDown() && (m_pressedTab >= 0))
        {
            if (GetDocker())
            {
                CDockContainer* pContainer = GetContainerFromIndex(m_currentPage);
                GetDocker()->UndockContainer(pContainer, GetCursorPos(), TRUE);
            }
        }

        m_pressedTab = -1;
        return CTab::OnMouseLeave(msg, wparam, lparam);
    }


    inline LRESULT CDockContainer::OnMouseMove(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        CPoint pt;
        pt.x = GET_X_LPARAM(lparam);
        pt.y = GET_Y_LPARAM(lparam);

        // Skip if mouse hasn't moved
        if ((pt.x == m_oldMousePos.x) && (pt.y == m_oldMousePos.y))
            return FALSE;

        m_oldMousePos.x = pt.x;
        m_oldMousePos.y = pt.y;

        if (IsLeftButtonDown())
        {
            TCHITTESTINFO info;
            ZeroMemory(&info, sizeof(info));
            info.pt = CPoint(static_cast<DWORD_PTR>(lparam));
            int nTab = HitTest(info);
            if (nTab >= 0 && m_pressedTab >= 0)
            {
                if (nTab != m_pressedTab)
                {
                    SwapTabs(nTab, m_pressedTab);
                    m_pressedTab = nTab;
                    SelectPage(nTab);
                }
            }
        }

        return CTab::OnMouseMove(msg, wparam, lparam);
    }


    inline LRESULT CDockContainer::OnNotifyReflect(WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);

        LPNMHDR pNMHDR = (LPNMHDR)lparam;
        switch (pNMHDR->code)
        {
        case TCN_SELCHANGE: return OnTCNSelChange(pNMHDR);
        }

        return 0;
    }


    inline LRESULT CDockContainer::OnTCNSelChange(LPNMHDR pNMHDR)
    {
        UNREFERENCED_PARAMETER(pNMHDR);

        // Display the newly selected tab page
        int page = GetCurSel();
        SelectPage(page);

        return 0;
    }


    inline LRESULT CDockContainer::OnSetFocus(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // Sets the focus to the active view (or its child)
        HWND prevFocus = reinterpret_cast<HWND>(wparam);
        if (GetActiveView() && GetActiveView()->IsWindow())
        {
            if (GetActiveView()->IsChild(prevFocus))
            {
                // return focus back to the child of the active view that had it before
                ::SetFocus(prevFocus);
            }
            else
                GetActiveView()->SetFocus();
        }

        return FinalWindowProc(msg, wparam, lparam);
    }


    inline LRESULT CDockContainer::OnSize(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);

        RecalcLayout();
        return 0;
    }


    inline void CDockContainer::PreCreate(CREATESTRUCT& cs)
    {
        // For Tabs on the bottom, add the TCS_BOTTOM style
        CTab::PreCreate(cs);
        cs.style |= TCS_BOTTOM;
    }


    inline void CDockContainer::RecalcLayout()
    {
        if (GetContainerParent() == this)
        {
            // Set the tab sizes
            SetTabSize();

            // Position the View over the tab control's display area
            CRect rc = GetClientRect();
            AdjustRect(FALSE, &rc);

            if (m_allInfo.size() > 0)
            {
                CDockContainer* pContainer = m_allInfo[m_currentPage].pContainer;

                if (pContainer->GetViewPage().IsWindow())
                    pContainer->GetViewPage().SetWindowPos(0, rc, SWP_SHOWWINDOW);

            }
            RedrawWindow(RDW_INVALIDATE | RDW_NOCHILDREN);
        }
    }


    inline void CDockContainer::RemoveContainer(CDockContainer* pWnd, BOOL updateParent)
    {
        assert(pWnd);

        // Remove the tab
        int iTab = GetContainerIndex(pWnd);
        DeleteItem(iTab);

        // Remove the ContainerInfo entry
        std::vector<ContainerInfo>::iterator iter;
        int iImage = -1;
        for (iter = m_allInfo.begin(); iter != m_allInfo.end(); ++iter)
        {
            if (iter->pContainer == pWnd)
            {
                iImage = (*iter).image;
                if (iImage >= 0)
                    RemoveImage(iImage);

                m_allInfo.erase(iter);
                break;
            }
        }

        // Set the parent container relationships
        pWnd->GetViewPage().SetParent(*pWnd);
        pWnd->m_pContainerParent = pWnd;

        // Display next lowest page
        m_currentPage = MAX(iTab - 1, 0);
        if (IsWindow() && updateParent)
        {
            if (GetItemCount() > 0)
                SelectPage(m_currentPage);
            else
                m_pDocker->SetCaption(_T(""));

            RecalcLayout();
        }
    }


    inline void CDockContainer::SelectPage(int page)
    {
        if (this != m_pContainerParent)
            m_pContainerParent->SelectPage(page);
        else
        {
            if ((page >= 0) && (page < static_cast<int>(m_allInfo.size())))
            {
                if (GetCurSel() != page)
                    SetCurSel(page);

                // Create the new container window if required
                if (!m_allInfo[page].pContainer->IsWindow())
                {
                    CDockContainer* pContainer = m_allInfo[page].pContainer;
                    pContainer->Create(GetParent());
                    pContainer->GetViewPage().SetParent(*this);
                }

                // Determine the size of the tab page's view area
                CRect rc = GetClientRect();
                AdjustRect(FALSE, &rc);

                // Swap the pages over
                CDockContainer* pNewContainer = m_allInfo[page].pContainer;
                std::vector<ContainerInfo>::const_iterator it;
                for (it = m_allInfo.begin(); it != m_allInfo.end(); ++it)
                {
                    (*it).pContainer->GetViewPage().ShowWindow(SW_HIDE);
                }

                pNewContainer->GetViewPage().SetWindowPos(0, rc, SWP_SHOWWINDOW);
                pNewContainer->GetViewPage().GetView()->SetFocus();

                // Adjust the docking caption
                if (GetDocker())
                {
                    GetDocker()->SetCaption(pNewContainer->GetDockCaption());
                    GetDocker()->RedrawWindow();
                }

                m_currentPage = page;
            }
        }
    }


    inline void CDockContainer::SetActiveContainer(CDockContainer* pContainer)
    {
        int nPage = GetContainerIndex(pContainer);
        assert (0 <= nPage);
        SelectPage(nPage);
    }


    inline void CDockContainer::SetHideSingleTab(BOOL hideSingle)
    // Only display tabs if there are two or more.
    {
        m_isHideSingleTab = hideSingle;
        RecalcLayout();
    }


    inline void CDockContainer::SetTabIcon(UINT iconID)
    {
        HICON icon = reinterpret_cast<HICON>(GetApp()->LoadImage(iconID, IMAGE_ICON, 0, 0, LR_SHARED));
        SetTabIcon(icon);
    }


    inline void CDockContainer::SetTabSize()
    {
        CRect rc = GetClientRect();
        AdjustRect(FALSE, &rc);
        if (rc.Width() < 0)
            rc.SetRectEmpty();

        int itemWidth = 0;
        int itemHeight = 1;
        if ((m_allInfo.size() > 0) && ((GetItemCount() != 1) || !m_isHideSingleTab))
        {
            itemWidth = MIN(25 + GetMaxTabTextSize().cx, (rc.Width() - 2) / static_cast<int>(m_allInfo.size()));
            itemHeight = MAX(20, GetTextHeight() + 5);
        }
        SendMessage(TCM_SETITEMSIZE, 0, MAKELPARAM(itemWidth, itemHeight));
    }


    inline void CDockContainer::SetTabText(UINT tab, LPCTSTR pText)
    {
        CDockContainer* pContainer = GetContainerParent()->GetContainerFromIndex(tab);
        pContainer->SetTabText(pText);

        CTab::SetTabText(tab, pText);
    }


    // Either sets the imagelist or adds/replaces bitmap depending on ComCtl32.dll version
    // Assumes the width of the button image = height, minimum width = 16
    // The colour mask is ignored for 32bit bitmaps, but is required for 24bit bitmaps
    // The colour mask is often grey RGB(192,192,192) or magenta (255,0,255)
    // The hot and disabled bitmap resources can be 0
    // A disabled image list is created from the normal image list if one isn't provided.
    inline void CDockContainer::SetToolBarImages(COLORREF mask, UINT normalID, UINT hotID, UINT disabledID)
    {
        // ToolBar ImageLists require Comctl32.dll version 4.7 or later
        if (GetComCtlVersion() < 470)
        {
            // We are using COMCTL32.DLL version 4.0, so we can't use an ImageList.
            // Instead we simply set the bitmap.
            GetToolBar().AddReplaceBitmap(normalID);
            return;
        }

        // Set the button images
        CBitmap Bitmap(normalID);
        assert(Bitmap.GetHandle());

        BITMAP data = Bitmap.GetBitmapData();
        int cy = data.bmHeight;
        int cx  = MAX(data.bmHeight, 16);

        m_normalImages.DeleteImageList();
        m_normalImages.Create(cx, cy, ILC_COLOR32 | ILC_MASK, 0, 0);
        m_normalImages.Add(Bitmap, mask);
        GetToolBar().SetImageList(m_normalImages);

        if (hotID)
        {
            CBitmap BitmapHot(hotID);
            assert(BitmapHot);

            m_hotImages.DeleteImageList();
            m_hotImages.Create(cx, cy, ILC_COLOR32 | ILC_MASK, 0, 0);
            m_hotImages.Add(BitmapHot, mask);
            GetToolBar().SetHotImageList(m_hotImages);
        }

        if (disabledID)
        {
            CBitmap BitmapDisabled(disabledID);
            assert(BitmapDisabled);

            m_disabledImages.DeleteImageList();
            m_disabledImages.Create(cx, cy, ILC_COLOR32 | ILC_MASK, 0, 0);
            m_disabledImages.Add(BitmapDisabled, mask);
            GetToolBar().SetDisableImageList(m_disabledImages);
        }
        else
        {
            m_disabledImages.DeleteImageList();
            m_disabledImages.CreateDisabledImageList(m_normalImages);
            GetToolBar().SetDisableImageList(m_disabledImages);
        }
    }


    inline void CDockContainer::SetView(CWnd& Wnd)
    {
        GetViewPage().SetView(Wnd);
    }


    inline void CDockContainer::SwapTabs(UINT tab1, UINT tab2)
    {
        assert (tab1 < GetContainerParent()->m_allInfo.size());
        assert (tab2 < GetContainerParent()->m_allInfo.size());

        if (tab1 != tab2)
        {
            ContainerInfo CI1 = GetContainerParent()->m_allInfo[tab1];
            ContainerInfo CI2 = GetContainerParent()->m_allInfo[tab2];

            TCITEM Item1;
            ZeroMemory(&Item1, sizeof(Item1));
            Item1.mask = TCIF_IMAGE | TCIF_PARAM | TCIF_RTLREADING | TCIF_STATE | TCIF_TEXT;
            Item1.cchTextMax = CI1.title.GetLength()+1;
            Item1.pszText = const_cast<LPTSTR>(CI1.title.c_str());
            GetItem(tab1, &Item1);

            TCITEM Item2;
            ZeroMemory(&Item2, sizeof(Item2));
            Item2.mask = TCIF_IMAGE | TCIF_PARAM | TCIF_RTLREADING | TCIF_STATE | TCIF_TEXT;
            Item2.cchTextMax = CI2.title.GetLength()+1;
            Item2.pszText = const_cast<LPTSTR>(CI2.title.c_str());
            GetItem(tab2, &Item2);

            SetItem(tab1, &Item2);
            SetItem(tab2, &Item1);
            GetContainerParent()->m_allInfo[tab1] = CI2;
            GetContainerParent()->m_allInfo[tab2] = CI1;
        }
    }


    inline LRESULT CDockContainer::WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_LBUTTONDOWN:    return OnLButtonDown(msg, wparam, lparam);
        case WM_LBUTTONUP:      return OnLButtonUp(msg, wparam, lparam);
        case WM_MOUSELEAVE:     return OnMouseLeave(msg, wparam, lparam);
        case WM_MOUSEMOVE:      return OnMouseMove(msg, wparam, lparam);
        case WM_SETFOCUS:       return OnSetFocus(msg, wparam, lparam);
        case WM_SIZE:           return OnSize(msg, wparam, lparam);
        case UWM_GETCDOCKCONTAINER: return reinterpret_cast<LRESULT>(this);
        }

        // pass unhandled messages on to CTab for processing
        return CTab::WndProcDefault(msg, wparam, lparam);
    }


    ///////////////////////////////////////////
    // Declaration of the nested CViewPage class

    inline CDockContainer* CDockContainer::CViewPage::GetContainer() const
    {
        assert(m_pContainer);
        return m_pContainer;
    }


    inline BOOL CDockContainer::CViewPage::OnCommand(WPARAM wparam, LPARAM lparam)
    {
        BOOL IsHandled = FALSE;
        if (GetContainer()->GetActiveContainer() && GetContainer()->GetActiveContainer()->IsWindow())
            IsHandled = (GetContainer()->GetActiveContainer()->SendMessage(WM_COMMAND, wparam, lparam) != 0);

        return IsHandled;
    }


    inline int CDockContainer::CViewPage::OnCreate(CREATESTRUCT& cs)
    {
        UNREFERENCED_PARAMETER(cs);
        if (m_pView)
            m_pView->Create(*this);

        return 0;
    }


    inline LRESULT CDockContainer::CViewPage::OnNotify(WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(wparam);

        switch (((LPNMHDR)lparam)->code)
        {

        // Display tooltips for the toolbar
        case TTN_GETDISPINFO:
            {
                int index =  GetToolBar().HitTest();
                LPNMTTDISPINFO lpDispInfo = (LPNMTTDISPINFO)lparam;
                if (index >= 0)
                {
                    int id = GetToolBar().GetCommandID(index);
                    if (id > 0)
                    {
                        m_tooltip = LoadString(id);
                        lpDispInfo->lpszText = const_cast<LPTSTR>(m_tooltip.c_str());
                    }
                    else
                        m_tooltip = _T("");
                }
            }
            break;
        case NM_CUSTOMDRAW:
            {
                if (((LPNMHDR)lparam)->hwndFrom == GetToolBar())
                {
                    // Pass Toolbar's custom draw up to CFrame
                    return GetAncestor().SendMessage(WM_NOTIFY, wparam, lparam);
                }
            }
            break;
        } // switch LPNMHDR

        return 0;
    }


    inline void CDockContainer::CViewPage::PreRegisterClass(WNDCLASS& wc)
    {
        wc.lpszClassName = _T("Win32++ TabPage");
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    }


    inline void CDockContainer::CViewPage::RecalcLayout()
    {
        CRect rc = GetClientRect();
        if (GetToolBar().IsWindow())
        {
            GetToolBar().SendMessage(TB_AUTOSIZE, 0, 0);
            CRect rcToolBar = GetToolBar().GetClientRect();
            rc.top += rcToolBar.Height();
        }

        if (GetView())
            GetView()->SetWindowPos(NULL, rc, SWP_SHOWWINDOW);
    }


    inline void CDockContainer::CViewPage::SetView(CWnd& wndView)
    // Sets or changes the View window displayed within the container
    {
        if (m_pView != &wndView)
        {
            // Hide the existing view window (if any)
            if (m_pView && m_pView->IsWindow())
                m_pView->ShowWindow(SW_HIDE);

            // Assign the view window
            m_pView = &wndView;

            if (IsWindow())
            {
                if (!GetView()->IsWindow())
                    GetView()->Create(*this);
                else
                {
                    GetView()->SetParent(*this);
                    GetView()->ShowWindow();
                }

                // The new view must not be a dockcontainer
                assert(GetView()->SendMessage(UWM_GETCDOCKCONTAINER) == 0);

                RecalcLayout();
            }
        }
    }


    inline LRESULT CDockContainer::CViewPage::WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_SIZE:
            RecalcLayout();
            break;
        }

        // pass unhandled messages on for default processing
        return CWnd::WndProcDefault(msg, wparam, lparam);
    }

} // namespace Win32xx

#endif // _WIN32XX_DOCKING_H_

