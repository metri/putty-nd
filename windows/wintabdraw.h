#ifndef WINTABDRAW_H
#define WINTABDRAW_H

typedef enum{	
    SIDE_LEFT, SIDE_TOP, SIDE_RIGHT, SIDE_BOTTOM
}SIDE;

int DrawMinButton(HDC hdc, RECT *pRect, COLORREF clrBorder, COLORREF clrBack, COLORREF clrBtn);
int DrawMaxButton(HDC hdc, RECT *pRect, COLORREF clrBorder, COLORREF clrBack, COLORREF clrBtn, int isZoomed);
int DrawClsButton(HDC hdc, RECT *pRect, COLORREF clrBorder, COLORREF clrBack, COLORREF clrBtn);
HRGN DrawChromeFrame(HDC hdc, RECT *pRect, COLORREF clrBorder, COLORREF clrBack);
int DrawSysButtonFrame(HDC hdc, RECT *pRect, COLORREF clrBorder, COLORREF clrBack, HRGN* hRgns);
int DrawSysButton(HDC hdc, RECT *pRect, COLORREF clrBorder);
HRGN DrawCloseButton(HDC hdc, const int x, const int y, 
    COLORREF clrBorder, COLORREF clrBack );
void DrawHalfRoundFrame(HDC hdc, RECT const *pRect, SIDE side, 
    int radius, COLORREF clrBorder, COLORREF clrBack);
void DrawFrame(HDC hdc, POINT const *pPoints, int iCount, 
    COLORREF clrBorder, COLORREF clrBack);
void _DrawFrame(HDC hdc, POINT const *pPoints, int iCount, COLORREF clrLine);
void DrawGradient(HDC hdc, RECT const *pRect, char horz, COLORREF clrTopLeft, COLORREF clrBottomRight);
void DrawLinec(HDC hdc, int x1, int y1, int x2, int y2, COLORREF clrLine);
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2);
void DrawRect4c(HDC hdc, int x1, int y1, int x2, int y2, COLORREF clrLine);
void DrawRect1c(HDC hdc, RECT const *pRect, COLORREF clrLine);
void DrawRect4(HDC hdc, int x1, int y1, int x2, int y2);
void DrawRect1(HDC hdc, RECT const *pRect);
void DrawBeveledRect(HDC hdc, RECT const *pRect, int bevel);
COLORREF PixelAlpha(COLORREF src, COLORREF dst, int percent);
void FillSolidRect(HDC hdc, RECT const *rc, COLORREF color);
int DrawMarker(HDC hdc, RECT const *pRect, HBITMAP hbmpMask, COLORREF color);
int CorrectFitSpaceString(HDC hdc, char const *strSrc, int maxLength, char *strDst);


#endif /* WINTABDRAW_H */

