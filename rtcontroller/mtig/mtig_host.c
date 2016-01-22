/**
An MT message contains the following fields:
￼
PRE BID MID LEN DATA    CS
1   1   1   1   0-254   1
*/

#define PRE_V   0xFA
#define BID_V   0xFF
#define MID_MTDATA 0x32
#define LEN_MTDATA 0x4b
#define MTDATA_HEAD ( (PRE_V)+(BID_V<<8)+(MID_MTDATA<<16)+(LEN_MTDATA<<24) )

/**
 * Calibrated data output (36 bytes)
 *  加速度 accX accY accZ
    陀螺仪 gyrX gyrY gyrZ
    地磁场 magX magY magZ
 * Orientation data output (12 bytes)
 *  roll pitch yaw
 * Position (12 bytes)
 *  ￼￼￼￼Lat Lon Alt
 * Velocity (12 bytes)
 *  vx vy vz
 * Status   (1 byte)
 * Sample counter (2 bytes)
 *
*/

#include "mtig_host.h"

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <errno.h>
#include <string.h>


#if defined(__LINUX__)
#define MTIG_DEVICE "/dev/ttyS1"
#elif defined(__APPLE__)
//#define MTIG_DEVICE "/dev/cu.usbserial-ftE00UTF"
#define MTIG_DEVICE "/dev/cu.usbserial-ftE01UU0"

#include <sys/time.h>
/** for htonl */
#include <arpa/inet.h>
/** for Mti-G*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <curses.h>

#elif defined(__WIN32)
#include <windows.h>
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#endif
#define MTIG_BAUDRATE B115200


#define MTIG_BUFF_LEN 256
#define MTIG_DATA_LEN sizeof(mtig_data_t)+4+1


static unsigned char mtig_buff[MTIG_BUFF_LEN];
static int mtig_data_wpos;
static int mtig_data_rpos;
static int mtig_parse_state;
static int mtig_thread_state;
static mtig_data_t mtig_data;
static volatile uint64_t mtig_data_lock;

pthread_t mtig_trd;

struct timeval tv_time;

int mtig_spin_lock()
{
    uint64_t locked = 1;
    while(locked == 0)
    {
#if defined(__WIN32)
        locked = InterlockedCompareExchange64(&mtig_data_lock, 1, 0);
#else
        locked = __sync_val_compare_and_swap(&mtig_data_lock, 0, 1);
#endif
    }
    return 0;
}

int mtig_spin_unlock()
{
    mtig_data_lock = 0;
    return 0;
}

void mtig_read_data(mtig_data_t* data)
{
    mtig_spin_lock();
    memcpy(data, &mtig_data, sizeof(mtig_data_t));
    mtig_spin_unlock();
}

void mtig_write_data(const mtig_data_t* data)
{
    mtig_spin_lock();
    memcpy(&mtig_data, data, sizeof(mtig_data_t));
    mtig_spin_unlock();
}

int open_mtig()
{
    int fd, ret;
    struct termios newtio;
    /*    char mtig_dev_name[128];
    memset(mtig_dev_name, 0, 128);
    sprintf(mtig_dev_name, "/dev/ttymxc%d", port);
*/
    printf("call open ... ");
    fd = open(MTIG_DEVICE, O_RDONLY | O_NOCTTY);
    if(fd <0)
        return 0;
    printf("fd is %d\n", fd);
    memset(&newtio, 0, sizeof(newtio));
    ret = tcgetattr(fd, &newtio);
    //printf("getattr %d, errno %d\n", ret, errno);
    speed_t s = cfgetospeed(&newtio);
    //printf("speed is %d\n", s);
    newtio.c_cflag |= CLOCAL | CREAD;

    cfmakeraw(&newtio);
    ret = cfsetispeed(&newtio, MTIG_BAUDRATE);
    ret = cfsetospeed(&newtio, MTIG_BAUDRATE);
    printf("setospeed(%d) return %d, errno %d\n", MTIG_BAUDRATE, ret, errno);
    
    newtio.c_cflag &= ~CSIZE;
    newtio.c_cflag |= CS8;
    newtio.c_cflag &= ~CSTOPB;
    
    s = cfgetospeed(&newtio);
    printf("set speed to %d\n", s);
    ret = tcflush(fd, TCIOFLUSH);
    if(ret != 0)
    {
        fd = -1;
        return fd;
    }
    ret = tcsetattr(fd,TCSANOW,&newtio);
    if(ret != 0)
    {
        fd = -2;
        return fd;
    }
    tcgetattr(fd, &newtio);
    s = cfgetospeed(&newtio);
    printf("now speed is %d\n", s);
    //tcflush(fd, TCIOFLUSH);
    return fd;
}

void mtig_parse_data()
{
    int i;
    mtig_data_t *pdata;
    mtig_raw_t * praw;
    unsigned char* p =mtig_buff+mtig_data_rpos;



    /** we got head, so do get data */
    praw = (mtig_raw_t *)(p+4);
    for(i=0; i<18;++i)
    {
        praw->i32[i] = ntohl(praw->i32[i]);
    }
    praw->i16 = ntohs(praw->i16);
    pdata = (mtig_data_t*)praw;

    mtig_write_data(pdata);
#if CONSOLE_DEBUG_MODE

    if(pdata->status == 1) {


#if 1
    mvprintw(0,0,"Mti-G %u, Calibrated data\n", pdata->status);
    mvprintw(1,0,"%f\t%f\t%f\t"
             "%f\t%f\t%f\t"
             "%f\t%f\t%f\t"
             "\n",
             pdata->accX, pdata->accY, pdata->accZ
             ,pdata->gyrX, pdata->gyrY, pdata->gyrZ
             ,pdata->magX, pdata->magY, pdata->magZ
             );
#endif

#if 1
    mvprintw(2,0,"Mti-G %d Orientation data  "
             "%f\t%f\t%f\t"
             "\n",
             pdata->status, pdata->roll, pdata->pitch, pdata->yaw
             );
#endif

#if 1
    mvprintw(3,0,"Mti-G %d(%d) Position data  "
             "%f\t%f\t%f\t"
             "\n",
             pdata->status,pdata->sample_counter, pdata->Lat, pdata->Lon, pdata->Alt
             );
#endif

#if 1
    mvprintw(4,0,"Mti-G Velocity data\n"
             "%f\t%f\t%f\t"
             "\n",
             pdata->vx, pdata->vy, pdata->vz
             );
#endif

#if 1
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    int64_t difft = tv.tv_sec*1000000LL+tv.tv_usec - tv_time.tv_sec*1000000LL - tv_time.tv_usec;
    double dt = difft * 1e-3;
    mvprintw(7,0, "Time: %lf (ms)\n", dt);
    tv_time = tv;
#endif

    refresh();
    }
#endif /** CONSOLE_DEBUG_MODE */



    /** reset pos */
    int len = mtig_data_wpos - mtig_data_rpos - MTIG_DATA_LEN;
    if(pdata->sample_counter == 1) {
        memcpy(mtig_buff, mtig_buff+mtig_data_rpos+MTIG_DATA_LEN, len);
    }
    mtig_data_wpos = len;
    mtig_data_rpos = 0;
    mtig_parse_state = 0;
}

void mtig_parse()
{

    unsigned char* p =mtig_buff+mtig_data_rpos;

    if(mtig_parse_state == 1)
    {
        if(mtig_data_wpos - mtig_data_rpos >= MTIG_DATA_LEN)
            mtig_parse_data();
        return;
    }
    /** only work when state is 0 and remaining data > 4 bytes */
    else if( (mtig_data_wpos - mtig_data_rpos >= 4) )
    {
        while(mtig_data_rpos <= mtig_data_wpos-4)
        {
            if(*((unsigned int*)p) == MTDATA_HEAD )
            {
                mtig_parse_state = 1;
                break;
            }
            mtig_data_rpos ++;
            p ++;
        }

        /** parse head failed so reset rpos */
        if(mtig_parse_state == 0)
        {
            if(mtig_data_rpos > 3)
                mtig_data_rpos -= 3;
            else
                mtig_data_rpos = 0;
            return;
        }
        else if(mtig_parse_state == 1 && mtig_data_wpos - mtig_data_rpos >= MTIG_DATA_LEN )
        {
            //printf("parse data\n");
            mtig_parse_data();
            return;
        }
    }


}

void mtig_test()
{
    //    struct timespec ts1, ts2;
    int ret;
    int mtig_fd ;
    printf("opening mtig\n");
    mtig_fd = open_mtig();
    if(mtig_fd <= 0)
    {
        printf("open Mti-G failed and return %d errno(%d)\n", mtig_fd, errno);
        return;
    }
    printf("open Mti-G fd %d\n", mtig_fd);
    mtig_data_rpos = 0;
    mtig_data_wpos = 0;
    //    ts1.tv_sec = 0;
    //    ts1.tv_nsec = 10*1000000L;

    tcflush(mtig_fd, TCIOFLUSH);

    while(1)
    {
        //printf("reading...");
        ret = read(mtig_fd, mtig_buff+mtig_data_wpos, MTIG_BUFF_LEN - mtig_data_wpos);
        if(ret == -1) {
            printf("\nread [%d] err[%d]\n", ret, errno);
            return;
        }
        //printf(" %d bytes\n", ret);
        mtig_data_wpos += ret;
        mtig_parse();
        if(mtig_data_wpos >200)
            break;
        if(mtig_thread_state == 1)
            break;
    }

}

void* mtig_thread(void* arg)
{
    (void)arg;
    mtig_test();
    return 0;
}

void mtig_thread_start()
{
    mtig_thread_state = 0;
    pthread_create(&mtig_trd, NULL, mtig_thread, NULL);
}

void mtig_thread_stop()
{
    if(mtig_thread_state == 0)
    {
        mtig_thread_state = 1;
        pthread_join(mtig_trd, NULL);
    }
}

#if defined(CONSOLE_DEBUG_MODE)
int main()
{

    mtig_thread_start();
    sleep(2);

    initscr();
    char line[256];
    mtig_data_t data;
    while(1)
    {
        getnstr(line, 255);
        //mvscanw(20,0,"%s", line);
        if( line[0] == 'q')
        {
            break;
        }
        //sleep(1);
        //mtig_read_data(&data);
        //mvprintw(20,0, "
        //printf("roll: %d, pitch:%f,yaw:%f\n", data.status, data.pitch, data.yaw);

    }
    endwin();

    mtig_thread_stop();

    return 0;
}

#endif /** CONSOLE_DEBUG_MODE */
