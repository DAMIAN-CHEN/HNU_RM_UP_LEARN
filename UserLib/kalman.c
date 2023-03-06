#include "Kalman.h"

void Kalman_Init()
{
    kfp_pitch.Last_P = 1;
    kfp_pitch.Now_P = 0;
    kfp_pitch.out = 0;
    kfp_pitch.Kg = 0;
    kfp_pitch.Q = 0.001;
    kfp_pitch.R = 0.01;

    kfp_yaw.Last_P = 1;
    kfp_yaw.Now_P = 0;
    kfp_yaw.out = 0;
    kfp_yaw.Kg = 0;
    kfp_yaw.Q = 0.001;
    kfp_yaw.R = 0.01;
}

/**
 *�������˲���
 *@param 	Kalman *kfp �������ṹ�����
 *   			float input ��Ҫ�˲��Ĳ����Ĳ���ֵ�����������Ĳɼ�ֵ��
 *@return �˲���Ĳ���������ֵ��
 */
float KalmanFilter(Kalman *kfp,float input)
{
    //Ԥ��Э����̣�kʱ��ϵͳ����Э���� = k-1ʱ�̵�ϵͳЭ���� + ��������Э����
    kfp->Now_P = kfp->Last_P + kfp->Q;
    //���������淽�̣����������� = kʱ��ϵͳ����Э���� / ��kʱ��ϵͳ����Э���� + �۲�����Э���
    kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
    //��������ֵ���̣�kʱ��״̬����������ֵ = ״̬������Ԥ��ֵ + ���������� * ������ֵ - ״̬������Ԥ��ֵ��
    kfp->out = kfp->out + kfp->Kg * (input -kfp->out);//��Ϊ��һ�ε�Ԥ��ֵ������һ�ε����ֵ
    //����Э�����: ���ε�ϵͳЭ����� kfp->LastP ����һ������׼����
    kfp->Last_P = (1-kfp->Kg) * kfp->Now_P;
    return kfp->out;
}
