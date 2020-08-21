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
// wxx_rect.h
//  Definitions of the CSize, CPoint and CRect classes


#ifndef _WIN32XX_RECT_H_
#define _WIN32XX_RECT_H_

#include <winsock2.h>       // must include before windows.h
#include <windows.h>        // For SIZE, POINT, RECT etc.


// define useful macros from WindowsX.h
#ifndef GET_X_LPARAM
  #define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
  #define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))
#endif


namespace Win32xx
{
    // Forward declarations
    class CPoint;
    class CRect;


    /////////////////////////////////////////
    // This class can be used to replace the SIZE structure
    class CSize : public SIZE
    {
    public:
        CSize()                             { cx = 0; cy = 0; }
        CSize(int CX, int CY)               { cx = CX; cy = CY; }
        CSize(SIZE sz)                      { cx = sz.cx; cy = sz.cy; }
        CSize(POINT pt)                     { cx = pt.x;  cy = pt.y; }
        CSize(DWORD dw)                     { cx = (short)LOWORD(dw); cy = (short)HIWORD(dw); }
        void SetSize(int CX, int CY)        { cx = CX; cy = CY; }

        // Operators
        operator LPSIZE()                   { return this; }
        BOOL operator == (SIZE sz) const    { return (cx == sz.cx && cy == sz.cy); }
        BOOL operator != (SIZE sz) const    { return (cx != sz.cx || cy != sz.cy); }
        void operator += (SIZE sz)          { cx += sz.cx; cy += sz.cy; }
        void operator -= (SIZE sz)          { cx -= sz.cx; cy -= sz.cy; }

        // Operators returning CSize
        CSize operator - () const           { return CSize (-cx, -cy); }
        CSize operator + (SIZE sz) const    { return CSize (cx + sz.cx, cy + sz.cy); }
        CSize operator - (SIZE sz) const    { return CSize (cx - sz.cx, cy - sz.cy); }

        // Operators returning CPoint
        CPoint operator + (POINT point) const;
        CPoint operator - (POINT point) const;

        // Operators returning CRect
        CRect operator + (LPCRECT prc) const;
        CRect operator - (LPCRECT prc) const;
    };


    /////////////////////////////////////////
    // This class can be used to replace the POINT structure
    class CPoint : public POINT
    {
    public:
        CPoint()                            { x = 0; y = 0; }
        CPoint(int X, int Y)                { x = X; y = Y; }
        CPoint(POINT pt)                    { x = pt.x ; y = pt.y; }
        CPoint(POINTS pts)                  { x = pts.x; y = pts.y; }
        CPoint(SIZE sz)                     { x = sz.cx; y = sz.cy; }
        CPoint(LPARAM dwPos)                { x = GET_X_LPARAM(dwPos); y = GET_Y_LPARAM(dwPos); }

        void Offset(int dx, int dy)         { x += dx; y += dy; }
        void Offset(POINT pt)               { x += pt.x; y += pt.y; }
        void Offset(SIZE sz)                { x += sz.cx; y += sz.cy; }
        void SetPoint(int X, int Y)         { x = X; y = Y; }

        // Operators
        operator LPPOINT()                  { return this; }
        BOOL operator == (POINT pt) const   { return ((x == pt.x) && (y == pt.y)); }
        BOOL operator != (POINT pt) const   { return ((x != pt.x) || (y != pt.y)); }
        void operator += (SIZE sz)          { x += sz.cx; y += sz.cy; }
        void operator -= (SIZE sz)          { x -= sz.cx; y -= sz.cy; }
        void operator += (POINT pt)         { x += pt.x; y += pt.y; }
        void operator -= (POINT pt)         { x -= pt.x; y -= pt.y; }

        // Operators returning CPoint
        CPoint operator - () const          { return CPoint(-x, -y); }
        CPoint operator + (SIZE sz) const   { return CPoint(x + sz.cx, y + sz.cy); }
        CPoint operator - (SIZE sz) const   { return CPoint(x - sz.cx, y - sz.cy); }
        CPoint operator + (POINT pt) const  { return CPoint(x + pt.x, y + pt.y); }
        CPoint operator - (POINT pt) const  { return CPoint(x - pt.x, y - pt.y); }

        // Operators returning CRect
        CRect operator + (LPCRECT prc) const;
        CRect operator - (LPCRECT prc) const;
    };


    /////////////////////////////////////////
    // This class can be used to replace the RECT structure.
    class CRect : public RECT
    {
    public:
        CRect()                                     { left = top = right = bottom = 0; }
        CRect(int l, int t, int r, int b)           { left = l; top = t; right = r; bottom = b; }
        CRect(const RECT& rc)                       { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; }
        CRect(POINT pt, SIZE sz)                    { right = (left = pt.x) + sz.cx; bottom = (top = pt.y) + sz.cy; }
        CRect(POINT topLeft, POINT bottomRight)     { left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }

        BOOL CopyRect(LPCRECT prc)                  { return ::CopyRect(this, prc); }
        BOOL DeflateRect(int x, int y)              { return ::InflateRect(this, -x, -y); }
        BOOL DeflateRect(SIZE size)                 { return ::InflateRect(this, -size.cx, -size.cy); }
        void DeflateRect(LPCRECT prc)               { left += prc->left; top += prc->top; right -= prc->right; bottom -= prc->bottom; }
        void DeflateRect(int l, int t, int r, int b){ left += l; top += t; right -= r; bottom -= b; }
        BOOL EqualRect(LPRECT prc) const            { return ::EqualRect(prc, this); }
        BOOL InflateRect(int dx, int dy)            { return ::InflateRect(this, dx, dy); }
        BOOL InflateRect(SIZE sz)                   { return ::InflateRect(this, sz.cx, sz.cy); }
        void InflateRect(LPCRECT prc)               { left -= prc->left; top -= prc->top; right += prc->right; bottom += prc->bottom; }
        void InflateRect(int l, int t, int r, int b){ left -= l; top -= t; right += r; bottom += b; }
        BOOL IntersectRect(LPCRECT prc1, LPCRECT prc2)  { return ::IntersectRect(this, prc1, prc2); }
        BOOL IsRectEmpty() const                    { return ::IsRectEmpty(this);}
        BOOL IsRectNull() const                     { return (left == 0 && right == 0 && top == 0 && bottom == 0); }
        CRect MulDiv(int nMult, int nDiv) const     { return CRect ((left * nMult) / nDiv, (top * nMult) / nDiv,
                                                        (right * nMult) / nDiv, (bottom * nMult) / nDiv); }
        void NormalizeRect()                        { int nTemp; if (left > right) { nTemp = left; left = right; right = nTemp; }
                                                        if (top > bottom) { nTemp = top; top = bottom; bottom = nTemp; } }
        BOOL OffsetRect(int dx, int dy)             { return ::OffsetRect(this, dx, dy); }
        BOOL OffsetRect(POINT pt)                   { return ::OffsetRect(this, pt.x, pt.y); }
        BOOL OffsetRect(SIZE size)                  { return ::OffsetRect(this, size.cx, size.cy); }
        BOOL PtInRect(POINT pt) const               { return ::PtInRect(this, pt); }
        BOOL SetRect(int l, int t, int r, int b)    { return ::SetRect(this, l, t, r, b); }
        BOOL SetRect(POINT TopLeft, POINT BtmRight) { return ::SetRect(this, TopLeft.x, TopLeft.y, BtmRight.x, BtmRight.y); }
        BOOL SetRectEmpty()                         { return ::SetRectEmpty(this); }
        BOOL SubtractRect(LPCRECT prc1, LPCRECT prc2)   { return ::SubtractRect(this, prc1, prc2); }
        BOOL UnionRect(LPCRECT prc1, LPCRECT prc2)  { return ::UnionRect(this, prc1, prc2); }

        // Reposition rectangle
        void MoveToX (int x)                        { right = Width() + x; left = x; }
        void MoveToY (int y)                        { bottom = Height() + y; top = y; }
        void MoveToXY (int x, int y)                { MoveToX(x); MoveToY(y); }
        void MoveToXY (POINT pt)                    { MoveToX (pt.x); MoveToY (pt.y); }

        // Attributes
        int Height() const                          { return bottom - top; }
        int Width() const                           { return right - left; }
        CSize Size() const                          { return CSize(Width(), Height()); }
        CPoint CenterPoint() const                  { return CPoint((left + right) / 2, (top + bottom) / 2); }
        CPoint TopLeft() const                      { return CPoint(left, top); }
        CPoint BottomRight() const                  { return CPoint(right, bottom); }

        // operators
        operator LPRECT()                           { return this; }
        operator LPCRECT() const                    { return this; }
        BOOL operator == (const RECT& rc) const     { return ::EqualRect(this, &rc); }
        BOOL operator != (const RECT& rc) const     { return !::EqualRect(this, &rc); }
        void operator += (POINT pt)                 { ::OffsetRect(this, pt.x, pt.y); }
        void operator += (SIZE size)                { ::OffsetRect(this, size.cx, size.cy); }
        void operator += (LPCRECT prc)              { ::InflateRect(this, prc->right - prc->left, prc->bottom - prc->top); }
        void operator -= (LPCRECT prc)              { ::InflateRect(this, prc->left - prc->right, prc->top - prc->bottom); }
        void operator -= (POINT pt)                 { ::OffsetRect(this, -pt.x, -pt.y); }
        void operator -= (SIZE sz)                  { ::OffsetRect(this, -sz.cx, -sz.cy); }
        void operator &= (const RECT& rc)           { ::IntersectRect(this, this, &rc); }
        void operator |= (const RECT& rc)           { ::UnionRect(this, this, &rc); }

        // Operators returning CRect
        CRect operator + (POINT pt) const           { CRect rc(*this); ::OffsetRect(&rc, pt.x, pt.y); return rc; }
        CRect operator - (POINT pt) const           { CRect rc(*this); ::OffsetRect(&rc, -pt.x, -pt.y); return rc; }
        CRect operator + (SIZE sz) const            { CRect rc(*this); ::OffsetRect(&rc, sz.cx, sz.cy); return rc; }
        CRect operator - (SIZE sz) const            { CRect rc(*this); ::OffsetRect(&rc, -sz.cx, -sz.cy); return rc; }
        CRect operator + (LPRECT prc) const         { CRect rc1(*this); rc1.InflateRect(prc); return rc1; }
        CRect operator - (LPRECT prc) const         { CRect rc1(*this); rc1.DeflateRect(prc); return rc1; }
        CRect operator & (const RECT& rc) const     { CRect rc1; ::IntersectRect(&rc1, this, &rc); return rc1; }
        CRect operator | (const RECT& rc) const     { CRect rc1; ::UnionRect(&rc1, this, &rc); return rc1; }
    };

    // CSize member function definitions
    inline CPoint CSize::operator + (POINT pt) const    { return CPoint(pt) + *this; }
    inline CPoint CSize::operator - (POINT pt) const    { return CPoint(pt) - *this; }
    inline CRect CSize::operator + (LPCRECT prc) const  { return CRect(*prc) + *this; }
    inline CRect CSize::operator - (LPCRECT prc) const  { return CRect(*prc) - *this; }

    // CPoint member function definitions
    inline CRect CPoint::operator + (LPCRECT prc) const { return CRect(*prc) + *this; }
    inline CRect CPoint::operator - (LPCRECT prc) const { return CRect(*prc) - *this; }


    ////////////////////////////////////////
    // Global Functions
    //

    // Returns a CPoint holding the current cursor position.
    inline CPoint GetCursorPos()
    {
        CPoint pt;
        ::GetCursorPos(&pt);
        return pt;
    }

} // namespace Win32xx

#endif // _WIN32XX_RECT_H_
