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

/* BCPͨѶЭ����� */
#define FRAME_MAX_LEN 36        /* ͨѶ֡����󳤶� */
/* Ŀ���ַ�� */
#define broadcast   0x00        /* �㲥 */
#define mainfold    0x01        /* ��λ�� */
#define sentry_up   0x02        /* �ڱ�����������̨ */
#define sentry_down 0x03        /* �ڱ�����������̨ */
#define infantry    0x04        /* ���������� */
#define engineer    0x05        /* ���̻����� */
#define hero        0x06        /* Ӣ�ۻ����� */
#define air         0x07        /* ���л����� */
#define radar       0x08        /* �״�վ */
#define gather      0x09        /* �Ӿ��ɼ�̨ */
#define standard    0x10        /* AI������/ȫ�Զ����������� */
/* ������� */
#define chassis                 0x10        /* �ٶȷ�ʽ���� */
#define chassis_odom            0x11        /* ��̼Ʒ�ʽ���� */
#define chassis_ctrl            0x12        /* ��/���ٶȷ�ʽ���� */
#define gimbal                  0x20        /* ŷ����rpy��ʽ���� */
#define game_status             0x30        /* ������������*/
#define robot_HP                0x31        /* ������Ѫ������ */
#define ICRA_buff_debuff_zone   0x32        /* ���������� */
#define game_mode               0x33        /* ��������ɫ���� */
#define robot_command           0x34        /* ������λ����Ϣ */
#define client_map_command      0x35        /* �״﷢��Ŀ��λ����Ϣ */
#define barrel                  0x40        /* ����������� */
#define manifold_ctrl           0x50        /* ����ģʽ */
#define mode                    0x60        /* ģʽ���� */
#define dev_error               0xE0        /* ������Ϣ */
#define heartbeat               0xF0        /* �������� */


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


/**
  * @brief  ͨѶ֡�ṹ�� ��BCPͨѶЭ�飩
  */
typedef  struct
{
    uint16_t HEAD;  				/*! ֡ͷ */
    uint16_t D_ADDR;                /*! Ŀ���ַ */
    uint16_t ID;                    /*! ������ */
    uint16_t LEN;                   /*! ���ݳ��� */
    int8_t DATA[FRAME_MAX_LEN];     /*! �������� */
    uint8_t SC;                     /*! ��У�� */
    uint8_t AC;                     /*! ����У�� */
}__attribute__((packed)) BCPFrameTypeDef;

extern BCPFrameTypeDef upper_rx_data;  //������λ������֡

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

/**
 * @brief BCP��У���㷨
 * @param frame
 * @retval �ϲ�Ϊһ�����ݵ�У���� SC:��У�飨��8λ�� AC:���Ӻ�У�飨��8λ��
 */
uint16_t Sumcheck_Cal(BCPFrameTypeDef frame);

#endif //HNU_RM_UP_TRANSMISSION_H