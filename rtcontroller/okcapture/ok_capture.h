#ifndef OK_CAPTURE_H
#define OK_CAPTURE_H
#include <Windows.h>
#include <okcapture/Okapi64.h>

class Ok_capture
{
public:
    Ok_capture();
    ~Ok_capture();
    int Capture_single(int);
    int Capture_series();
    int Callsave();

private:
    HINSTANCE hInstLibrary;
    int	mBufRgbFrom;
    int mAutoGain;
    long lParam;
    HANDLE hBoard;
    MLONG iIndex;

};

HANDLE OpenCard(LPBITMAPINFOHEADER, LPBYTE ,LPSTR ,MLONG );
BOOL CALLBACK EndCapture(HANDLE );
BOOL CALLBACK Process(HANDLE ,MLONG );
BOOL CALLBACK BeginCapture(HANDLE );
long GetBitmapHeader(HANDLE , TARGET , LPBITMAPINFOHEADER );
long SetBitmapHeader(LPBITMAPINFOHEADER ,short, short , short , short);
long GetTargetSize(HANDLE , TARGET , short *, short *);

#endif // OK_CAPTRURE_H
