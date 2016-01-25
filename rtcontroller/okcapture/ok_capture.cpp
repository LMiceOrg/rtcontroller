#include "ok_capture.h"
#include <iostream>
#include <string.h>

LPBITMAPINFOHEADER	lpbi;
LPBYTE			lpdib;
LPSTR			lpMemory;
static int totalframe = 0;
static unsigned int elapsed = 0;

Ok_capture::Ok_capture()
{
    lParam = 0x3;
    OKDEVTYPE *lpOkDevInfo;
    iIndex = -1;
    hInstLibrary = LoadLibraryA("okapi64.dll");
    if (hInstLibrary == NULL) {
        printf("Error load okapi32.DLL!\n");
        FreeLibrary(hInstLibrary);
    }
    okGetImageDevice(&lpOkDevInfo, (LPARAM)&lParam);
    hBoard=OpenCard(lpbi, lpdib, lpMemory, iIndex);
    while (hBoard == 0) {
        printf("Wrong detect OK Board!\n");
        Sleep(2000);
        hBoard=OpenCard(lpbi, lpdib, lpMemory, iIndex);
    }
    printf("Initial Success!\n");
}

Ok_capture::~Ok_capture()
{
    if (hInstLibrary != NULL) {
        FreeLibrary(hInstLibrary);
    }
    std::cout<<"GOOD BYE!"<<std::endl;
}

long GetTargetSize(HANDLE hBoard, TARGET tgt, short *width, short *height)
{
    RECT	rect;
    long	form;

    SetRect(&rect,0,0,1280,1024);   //Change the Imagine size
    if( (tgt==SCREEN ) ||(tgt==BUFFER) ) {
      //  rect.right=-1;
        okSetTargetRect(hBoard,tgt,&rect); //get current rect
        *width=(short)(rect.right-rect.left);
        *height=(short)(rect.bottom-rect.top);

        if(tgt==SCREEN ) {
            form=okSetCaptureParam(hBoard,CAPTURE_SCRRGBFORMAT,GETCURRPARAM); //-1
            //limit to video rect
            rect.right=-1; //max. captured rect
            okSetTargetRect(hBoard,VIDEO,&rect); //get video rect
            *width=(short)min(*width,rect.right-rect.left);
            *height=(short)min(*height,rect.bottom-rect.top);
        }
        else if(tgt==BUFFER)
            form=okSetCaptureParam(hBoard,CAPTURE_BUFRGBFORMAT,FORM_GRAY8); //-1--------GETCURRPARAM---------------------------------

    } else if(tgt>BUFFER) { //from blkinfo
        LPBLOCKINFO	lpblk;
        lpblk=(LPBLOCKINFO)tgt;

        *width=lpblk->iWidth;
        *height=abs(lpblk->iHeight);
        form=MAKELONG(lpblk->iFormType,lpblk->iBitCount);
    }

    return form;
}

long SetBitmapHeader(LPBITMAPINFOHEADER lpbi,short width,short height, short bits, short form)
{
    long	wbytes;

    if(!lpbi)
        return FALSE;

    lpbi->biWidth=width;
    lpbi->biHeight=height;

    lpbi->biSize=sizeof(BITMAPINFOHEADER);
    lpbi->biPlanes=1;

    lpbi->biBitCount=bits;
    wbytes=((((lpbi->biWidth*bits)+31)&~31)>>3);//bmp need 4 align
    lpbi->biSizeImage=wbytes*lpbi->biHeight;

    lpbi->biClrUsed=0;
    //special format for 555,565 & 32
    if(form==FORM_RGB555 || form==FORM_RGB565 || form==FORM_RGB8888 )
        lpbi->biCompression=BI_BITFIELDS;
    else
        lpbi->biCompression=0;
    if(lpbi->biCompression==BI_BITFIELDS) {//
        DWORD	*lpmask;
        lpmask=(DWORD *)((LPSTR)lpbi+lpbi->biSize);

        if(form==FORM_RGB555) {
            lpmask[2]=0x001f; //blue
            lpmask[1]=0x03e0;
            lpmask[0]=0x7c00;
        }
        else if(form==FORM_RGB565) {
            lpmask[2]=0x001f; //blue
            lpmask[1]=0x07e0;
            lpmask[0]=0xf800;
        }
        else if(bits==32) {
            lpmask[2]=0x0000ff;
            lpmask[1]=0x00ff00;
            lpmask[0]=0xff0000;
        }
    }
    else if(bits<=12) { // 8,10,12
        int		i;
        RGBQUAD	*lpRGB=(RGBQUAD *)((LPSTR)lpbi+lpbi->biSize);

        lpbi->biClrUsed=(1<<bits);
        for(i=0; i<(short)lpbi->biClrUsed;i++) {
            lpRGB[i].rgbBlue=i;
            lpRGB[i].rgbGreen=i;
            lpRGB[i].rgbRed=i;
            lpRGB[i].rgbReserved=0;
        }
    }
    lpbi->biClrImportant=lpbi->biClrUsed;

    return lpbi->biClrUsed;
}

long GetBitmapHeader(HANDLE hBoard, TARGET src, LPBITMAPINFOHEADER lpbi)
{
    short		width,height;
    long		form;

    form=GetTargetSize(hBoard, src, &width, &height);

    //here take form as screen forever
    //form=okSetCaptureParam(hBoard,CAPTURE_SCRRGBFORMAT,GETCURRPARAM); //-1

    SetBitmapHeader(lpbi, width, height, HIWORD(form), LOWORD(form) );

    return LOWORD(form);
}

BOOL CALLBACK EndCapture(HANDLE hBoard)
{
    okSetSeqCallback(hBoard, NULL, NULL, NULL);
    return 0;
}

BOOL CALLBACK Process(HANDLE hBoard,MLONG no)
{

    int i, loop = 0, dt[10], frame = no;
    char szFileName[128] = "Image",tail[10] = ".bmp"; //
    while (frame > 0) {
        dt[loop] = frame % 10 ;
        frame = frame / 10;
        loop ++;
    }
    for (i=0; i<loop; i++) {
        szFileName[5+i] = dt[loop-i-1] + '0';
    }
    szFileName[loop+5] = '\0';
    strcat(szFileName, tail);
    // Image rate
    if ((totalframe>=12) ||  ((okGetTickCount()-elapsed)>2000)&&(totalframe>=2)) {
        elapsed=okGetTickCount() - elapsed;
        printf("Running time = %lf\n",((float)totalframe*1000/elapsed));
    }
    okSaveImageFile(hBoard, szFileName, 0, BUFFER, no, 1);
    if(totalframe == 0) elapsed= okGetTickCount();
    printf("Save Success %d images!!\n", totalframe);
    totalframe++; //总采集帧数
    return 0;
}

BOOL CALLBACK BeginCapture(HANDLE hBoard)
{
    short		width,height;
    long		blkform,bufform;
    BLOCKINFO	blk;
  // if(bDispToScreen) {
            bufform=GetTargetSize(hBoard, BUFFER, &width, &height);
            if( (LOWORD(bufform)==FORM_GRAY10) || (LOWORD(bufform)==FORM_GRAY12) ) { //special
                //blkform=okSetCaptureParam(hBoard,CAPTURE_SCRRGBFORMAT,GETCURRPARAM); //-1
                blkform=MAKELONG(FORM_GRAY8,8);
            }
            else //take same bits as buffer
                blkform=bufform;
                SetBitmapHeader(lpbi, width, height, HIWORD(blkform), LOWORD(blkform) );
      //  }

        blk.lpBits=lpdib;
        blk.iBitCount=lpbi->biBitCount;
        blk.iWidth=(short)lpbi->biWidth;

        if( okSetCaptureParam(hBoard,CAPTURE_SAMPLEFIELD,-1)==0 ) //sample in field by field
            if( LOWORD(okSetVideoParam(hBoard,VIDEO_SIGNALTYPE,-1))==1 ) //interlace video
                lpbi->biHeight*=2; //double size for ht

        blk.iHeight=-(short)lpbi->biHeight; //note: minus is for invert dib in y by ConvertRect

        elapsed=GetTickCount();
       // totalframe=0;

        return 1;
}

HANDLE OpenCard(LPBITMAPINFOHEADER lpbi,LPBYTE lpdib,LPSTR lpMemory,MLONG iIndex)
{

    HANDLE hBoard;
//	short	typecode;
    char	string[100];
    MEMORYSTATUS Status;
    DWORD dwMaxMemSize=30*1024*1024;
  //  okGetAvailBuffer(NULL,&dwBufSize);
    //just starting, then allocated memory for dib
    if(!lpbi) {
        //下面为应用程序分配内存
        GlobalMemoryStatus(&Status);
        //determaine how many memory will be allocated as user memory
        dwMaxMemSize=5*(1<<20); //5M
        lpbi=(LPBITMAPINFOHEADER)GlobalAlloc(GPTR,dwMaxMemSize);
        if(lpbi==NULL) {
            dwMaxMemSize=4*(1<<20); //4M
            lpbi=(LPBITMAPINFOHEADER)GlobalAlloc(GPTR,dwMaxMemSize);
        }
        lpdib=(LPBYTE)lpbi+5120; //keep 5k for bi
        lpMemory=(LPSTR)lpbi+2048*1024; //keep 2M for dib
        dwMaxMemSize-=2048*1024+5120;
        if(lpbi) {
            sprintf(string,"Allocated memory = %d",dwMaxMemSize);
        }else
            sprintf(string,"Allocate memory failed !");
    //	MessageBox(hwnd,string,"Message",MB_OK);

    }
//    for(int i =0; i<TotalBoard; i++)
//        {
//            printf("Device %d: %s\n", i, lpOkDevInfo[i].szBoardName);
//        }
    hBoard = okOpenBoard(&iIndex);
    GetBitmapHeader(hBoard, BUFFER, lpbi);
    //set initial size
  //  sampwidth=(short)lpbi->biWidth;
  //  sampheight=(short)lpbi->biHeight;
    //initailize screen
    //okSetToWndRect(hBoard,hWnd);
    //InvalidateRect(hWnd,NULL,FALSE);
    return hBoard;
}

int Ok_capture::Capture_single(int frame)
{
     int i, loop = 0, dt[10];
     char szFileName[128] = "Image",tail[10] = ".bmp"; //
     while (frame > 0) {
         dt[loop] = frame % 10 ;
         frame = frame / 10;
         loop ++;
     }
     for (i=0; i<loop; i++) {
         szFileName[5+i] = dt[loop-i-1] + '0';
     }
     szFileName[loop+5] = '\0';
     strcat(szFileName, tail);

     okCaptureSingle(hBoard, BUFFER, 0);
     okGetCaptureStatus(hBoard, 1);

     printf("Single frame is Finished! \n"); //Success
     okSaveImageFile(hBoard, szFileName, 0, BUFFER, 0, 1);
     okStopCapture(hBoard);
     //okStopCapture(hBoard);
     return 0;
}

int Ok_capture::Capture_series()
{
     okSetSeqCallback(hBoard, BeginCapture, Process, EndCapture);
     printf("Endcapture! The max frame number is %d! \n",okCaptureTo(hBoard,BUFFER, 0,-1)); //连续
     okStopCapture(hBoard);
     //okStopCapture(hBoard);
     return 0;
}

