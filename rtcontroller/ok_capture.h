#ifndef OK_CAPTURE_H
#define OK_CAPTURE_H
#include <Okapi64.h>
#include <Windows.h>

class Ok_capture
{
public:
    Ok_capture();
    ~Ok_capture();
    HANDLE OpenCard(LPBITMAPINFOHEADER, LPBYTE ,LPSTR ,MLONG );
    long GetBitmapHeader(HANDLE , TARGET , LPBITMAPINFOHEADER );
    long SetBitmapHeader(LPBITMAPINFOHEADER ,short, short , short , short);
    BOOL GetTargetSize(HANDLE , TARGET , short , short );
    BOOL CALLBACK EndCapture(HANDLE );
    BOOL CALLBACK Process(HANDLE ,MLONG );
    BOOL CALLBACK BeginCapture(HANDLE );
    int Callsave();

private:
    int	mBufRgbFrom;
    int mAutoGain;
    long lParam;
    int Board;
    OKDEVTYPE *lpOkDevInfo;
    LPBITMAPINFOHEADER	lpbi;
    LPBYTE				lpdib;
    LPSTR				lpMemory;
    int totalframe;
    long elapsed;
    HANDLE hBoard;
    MLONG iIndex;

};

#endif // OK_CAPTRURE_H
