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
#include <sys/time.h>
#include <errno.h>

/** for htonl */
#include <arpa/inet.h>

/** for Mti-G*/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <pthread.h>

#include <curses.h>

#if defined(__LINUX__)
#define MTIG_DEVICE "/dev/ttyS1"
#else defined(__APPLE__)
//#define MTIG_DEVICE "/dev/cu.usbserial-ftE00UTF"
#define MTIG_DEVICE "/dev/cu.usbserial-ftE01UU0"
#endif
#define MTIG_BAUDRATE B115200


#define buff_len 256
#define mtig_package_len 75+4+1

#pragma pack(push)
#pragma pack(1)
typedef struct {
    uint32_t i32[18];
    uint8_t i8;
    uint16_t i16;
} mtig_raw_t;
#pragma pack(pop)

static unsigned char mtig_buff[buff_len];
static int wpos;
static int rpos;
static int state;
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
        locked = __sync_val_compare_and_swap(&mtig_data_lock, 0, 1);
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
    unsigned char* p =mtig_buff+rpos;



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
    int len = wpos - rpos - mtig_package_len;
    if(pdata->sample_counter == 1) {
        memcpy(mtig_buff, mtig_buff+rpos+mtig_package_len, len);
    }
    wpos = len;
    rpos = 0;
    state = 0;
}

void mtig_parse()
{

    unsigned char* p =mtig_buff+rpos;

    if(state == 1)
    {
        if(wpos - rpos >= mtig_package_len)
            mtig_parse_data();
        return;
    }
    /** only work when state is 0 and remaining data > 4 bytes */
    else if( (wpos - rpos >= 4) )
    {
        while(rpos <= wpos-4)
        {
            if(*((unsigned int*)p) == MTDATA_HEAD )
            {
                state = 1;
                break;
            }
            rpos ++;
            p ++;
        }

        /** parse head failed so reset rpos */
        if(state == 0)
        {
            if(rpos > 3)
                rpos -= 3;
            else
                rpos = 0;
            return;
        }
        else if(state == 1 && wpos - rpos >= mtig_package_len )
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
    rpos = 0;
    wpos = 0;
    //    ts1.tv_sec = 0;
    //    ts1.tv_nsec = 10*1000000L;

    tcflush(mtig_fd, TCIOFLUSH);

    while(1)
    {
        //printf("reading...");
        ret = read(mtig_fd, mtig_buff+wpos, buff_len - wpos);
        if(ret == -1) {
            printf("\nread [%d] err[%d]\n", ret, errno);
            return;
        }
        //printf(" %d bytes\n", ret);
        wpos += ret;
        mtig_parse();
        if(wpos >200)
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

//#if defined(CONSOLE_DEBUG_MODE)
#include "control_config.h"
int main()
{
    control_config_t cfg;
    int i;
    load_config("cfg.json", &cfg);
    printf("cfg loaded\narea count=%d\n", cfg.area_count);
    for(i=0; i<cfg.area_count; ++i)
    {
        printf("area [%d]: \tleft top x=%lf, y=%lf, z=%lf\n\t\tright buttom"
               "x=%lf, y=%lf, z=%lf\n\n",
               i, cfg.area[i].lt.x,cfg.area[i].lt.y,cfg.area[i].lt.z,
               cfg.area[i].rb.x,cfg.area[i].rb.y,cfg.area[i].rb.z);
    }
    printf("time period is %d\n", cfg.time_period);
    printf("picture store path is %s\n", cfg.pic_path);
    printf("There are %d channel\n", cfg.channel_count);
    for(i=0; i< cfg.channel_count; ++i)
    {
        printf("channel [%d]\t net device name :%s\n", i, cfg.net_device[i].name);
    }
    //return 0;
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

//#endif /** CONSOLE_DEBUG_MODE */
