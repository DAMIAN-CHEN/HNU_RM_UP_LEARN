#ifndef __Kalman_H
#define __Kalman_H

#include "main.h"

typedef struct
{
    float Last_P;//�ϴι���Э���� ������Ϊ0 ! ! ! ! !
    float Now_P;//��ǰ����Э����
    float out;//�������˲������
    float Kg;//����������
    float Q;//��������Э����
    float R;//�۲�����Э����
}Kalman;

void Kalman_Init(void);
float KalmanFilter(Kalman *kfp,float input);

extern Kalman kfp_pitch;
extern Kalman kfp_yaw;

#endif