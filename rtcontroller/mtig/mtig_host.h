#ifndef MTIG_HOST_H
#define MTIG_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#pragma pack(push)

#pragma pack(1)

typedef struct mtig_data_t_ {
    float accX,accY,accZ;
    float gyrX,gyrY,gyrZ;
    float magX, magY, magZ;
    float roll, pitch, yaw;
    float Lat, Lon, Alt;
    float vx,vy,vz;
    unsigned char status;
    unsigned short sample_counter;
} mtig_data_t;

typedef struct mtig_raw_t_ {
    uint32_t i32[18];
    uint8_t i8;
    uint16_t i16;
} mtig_raw_t;

#pragma pack(pop)

void mtig_read_data(mtig_data_t* data);
void mtig_write_data(const mtig_data_t* data);
void mtig_thread_start();
void mtig_thread_stop();

#ifdef __cplusplus
}
#endif

#endif /** MTIG_HOST_H */
