//
// Created by 14685 on 2022/7/4.
//

#ifndef HNU_RM_UP_BSP_RC_H
#define HNU_RM_UP_BSP_RC_H

#include "cmsis_os.h"
/**
  * @brief     �������ң�������ݽṹ��
  */
typedef struct
{
    /* ң������ͨ�����ݣ���ֵ��Χ��-660 ~ 660 */
    int16_t ch1;   //�Ҳ�����
    int16_t ch2;   //�Ҳ�����
    int16_t ch3;   //�������
    int16_t ch4;   //�������

    /* ң�����Ĳ������ݣ������·ֱ�Ϊ��1��3��2 */
    uint8_t sw1;   //��ದ��
    uint8_t sw2;   //�Ҳದ��

    /* PC ������� */
    struct
    {
        /* ����ƶ���� */
        int16_t x;   //���ƽ��
        int16_t y;   //�������
        /* ��갴����أ�1Ϊ���£�0Ϊ�ɿ� */
        uint8_t l;   //��ఴ��
        uint8_t r;   //�Ҳఴ��
    }mouse;

    /* PC ���̰������� */
    union
    {
        uint16_t key_code;
        struct
        {
            uint16_t W:1;
            uint16_t S:1;
            uint16_t A:1;
            uint16_t D:1;
            uint16_t SHIFT:1;
            uint16_t CTRL:1;
            uint16_t Q:1;
            uint16_t E:1;
            uint16_t R:1;
            uint16_t F:1;
            uint16_t G:1;
            uint16_t Z:1;
            uint16_t X:1;
            uint16_t C:1;
            uint16_t V:1;
            uint16_t B:1;
        }bit;
    }kb;

    /* ң������ದ������ */
    int16_t wheel;
}RcTypeDef;
/**
  * @brief     ң������������ö��
  */
enum
{
    RC_UP = 1,
    RC_MI = 3,
    RC_DN = 2,
};

#endif //HNU_RM_UP_BSP_RC_H
