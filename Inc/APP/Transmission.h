//
// Created by 14685 on 2022/7/3.
//

#ifndef HNU_RM_UP_TRANSMISSION_H
#define HNU_RM_UP_TRANSMISSION_H


#include "gpio.h"
#include "Gimbal.h"
#include "cmsis_os.h"


#define COM_CAN hcan2
#define CONTROL_CAN hcan1
#define CAN_UP_TX_INFO 0x134

/**
  * @brief  ���鷢�ͽṹ��
  */
typedef  struct
{
    uint16_t head;				    /*! ֡ͷ */
    float pitchAngleGet;    	    /*! pitch��Ƕ� */
    float yawAngleGet;      	    /*! yaw��Ƕ� */
    uint8_t rotateDirection;        /*! ��ת���� */
    float timeBais;         	    /*! Ԥ��ʱ��ƫ�� */
    float compensateBais;   	    /*! ��������ƫ�� */
    uint8_t gimbal_mode;	 	    /*! ��̨ģʽ */
    uint32_t index;                 /*! ֡��� */
}__attribute__((packed)) SendFrameTypeDef;


/**
  * @brief  ������սṹ��
  */
typedef  struct
{
    uint16_t head;  				/*! ֡ͷ */
    float pitchAngleSet;            /*! pitch��Ƕ��趨ֵ*/
    float yawAngleSet;              /*! yaw��Ƕ��趨ֵ */
    float targetAngle;
    uint8_t shootCommand;
    uint32_t index;                 /*! ֡��� */
}__attribute__((packed)) RecvFrameTypeDef;



extern RecvFrameTypeDef auto_rx_data;
extern SendFrameTypeDef auto_tx_data;

/**
  * @brief     ��ȡ�ϰ���°巢�͵�����
  * @param[in] data: �����洢���ݵ�����
  * @retval    װ�����ݵ�����
  */
void Get_Communicate_data(uint8_t* data);

/**
  * @brief     �����ϰ���°崫�������
  * @param[in] hcan: ����ͨ�ŵ�CAN
  * @param[in] data: �ϰ���°巢�͵�����
  * @retval    װ�����ݵ�����
  */
void Send_Communicate_data(CAN_HandleTypeDef *_hcan, uint8_t *data);

#endif //HNU_RM_UP_TRANSMISSION_H