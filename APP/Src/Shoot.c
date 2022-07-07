//
// Created by 14685 on 2022/7/3.
//

#include "Shoot.h"
#include "bsp_can.h"
#include "pid.h"
#include "bsp_uart.h"
#include "keyboard.h"
#include "Detect.h"
#include "bsp_tim.h"

/* �ϴε�ң���������� */
uint8_t   last_left_key;
uint8_t   last_right_key;
uint8_t   last_sw1;
uint8_t   last_sw2;
int16_t   last_wheel_value;

/*���ָǵ���ز���*/
int cap_open_flag = 0;
int cap_ok = 0;

/* �����ز��� */
enum ShootState shoot_state;
uint8_t   shoot_cmd = 0;
uint32_t  shoot_continue_time;
uint16_t  fric_wheel_speed = SHOT_FRIC_WHEEL_SPEED;
bool_t fric_wheel_run = 0;
uint8_t shooter_output;

/* �����������λ��(��λ�Ǳ�������ֵencoder) */
int32_t trigger_moto_position_ref;
/* �����������ת��(rpm) */
int16_t trigger_moto_speed_ref;
/* ����������� */
int16_t trigger_moto_current;
/* Ħ���ֵ������ */
int16_t shoot_moto_current_left;
int16_t shoot_moto_current_right;
/* ����������ز��� */
uint32_t stall_count = 0;
uint32_t stall_inv_count = 0;
uint8_t  stall_f = 0;

float speedInNumsPerSec;
uint32_t numsOfOneShot;
uint32_t delayTimeInMs;


void shoot_task(const void* argu){
    /* �������PID������ʼ�� */
    PID_Init(&pid_trigger, 4500, 2000, 0.15f, 0.005, 0);
    PID_Init(&pid_trigger_speed, 7000, 3000, 7.0, 0.5, 0.1);
    /* Ħ���ֵ��PID������ʼ�� */
    PID_Init(&pid_shoot_left, 7000, 3000, 9.0f, 0.02, 0.00);
    PID_Init(&pid_shoot_right, 7000, 3000, 9.0f, 0.02, 0.00);

    uint32_t shoot_wake_time = osKernelSysTick();

    while (1){
        /* ����Ħ���� */
        if (rc.kb.bit.Q && rc.sw2 != RC_DN)
            fric_wheel_run = 1;

        if ((rc.kb.bit.Q && rc.kb.bit.SHIFT) || rc.sw2 == RC_DN)
            fric_wheel_run = 0;

        if (glb_err.err_list[REMOTE_CTRL_OFFLINE].err_exist)
            fric_wheel_run = 0;

        //* ���ص��ָ� */
        if (rc.kb.bit.R||rc.sw1!=RC_DN){
            cap_open_flag = -1;
            cap_ok = 0;
        }
        if ((rc.kb.bit.R &&rc.kb.bit.SHIFT) || rc.sw1==RC_DN){
            cap_open_flag = 1;
            cap_ok = 0;
        }

//		GPIOE->ODR|=data_recv.shootCommand<<5;//ʹ��PE5�����cmd
        //HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,(GPIO_PinState)data_recv.shootCommand);

        /* bullet single or continue trigger command control  */
        if (( RC_SINGLE_TRIG ||rc.mouse.l         //ң��������굥��
              ||auto_rx_data.shootCommand)     //��λ������
            && shoot_state == DONT_SHOOT){

            auto_rx_data.shootCommand=0;
            shoot_cmd=1;
            shoot_continue_time = HAL_GetTick();
            if(rc.kb.bit.SHIFT)
                shoot_state=TRIBLE_SHOOT;
            else
                shoot_state=SINGLE_SHOOT;
        }
        else if ( RC_CONTIN_TRIG     //ң��������
                  || (rc.mouse.r && rc.kb.bit.SHIFT) ) {  //�������
            shoot_state=CONTINUOUS_SHOOT;
            trigger_moto_position_ref=moto_trigger.total_ecd;
        }
        else if(HAL_GetTick()-shoot_continue_time>600){
            shoot_state=DONT_SHOOT;
        }

        if (fric_wheel_run == 0){
            shoot_state=DONT_SHOOT;
        }

        /* �����������ʵ�ֺ��� */
        Shoot_Custom_control();
        /* ���ָǿ��ƺ��� */
        Cap_Control_open();

        /* ����Ħ���ֲ������� */
        if(rc.sw1&&(last_sw1!=rc.sw1)){
            last_sw1 = rc.sw1;
            if(rc.sw1==RC_UP)
                fric_wheel_run = !fric_wheel_run;
        }
        /* ����Ħ����ʵ�ֺ��� */
        FrictionWheel_Turn_on_off();

        last_sw2 = rc.sw2;
        last_left_key    = km.lk_sta;
        last_right_key   = km.rk_sta;
        last_wheel_value = rc.wheel;
        //GPIOE->BSRR=0x10000000;
        osDelayUntil(&shoot_wake_time, SHOOT_PERIOD);
    }
}

void Cap_Control_open(){
    if(!cap_ok){
        if(cap_open_flag == 1){
            PWM_Set_param(3, 1000);
        }
        else{
            PWM_Set_param(3, 2350);
        }
        start_pwm_output(3);
        cap_ok = 1;
    }
}

/* �������� */
void Block_Bullet_handle(void){
    if (pid_trigger_speed.out <= -5000) {  //��������
        if (stall_f == 0)
            stall_count ++;
    }
    else
        stall_count = 0;

    if (stall_count >= 600) { //����ʱ��3s
        stall_f = 1;
        stall_count = 0;

    }

    if (stall_f == 1){
        stall_inv_count++;

        if (stall_inv_count >= 100) { //��תʱ��0.5s
            stall_f = 0;
            stall_inv_count = 0;
        }
        else
            trigger_moto_speed_ref = 2000;
    }
}

float ShootAndDelay(float speedInNumsPerSec, uint32_t numsOfOneShot, uint32_t delayTimeInMs){
    static uint32_t ticksInMs = 0, lastNumsOfOneShot = 0, lastDelayTimeInMs = 0, count = 0;
    static int32_t lastAngle = 0;
    static float speed = 0;
    if (count == 0 || lastNumsOfOneShot != numsOfOneShot || lastDelayTimeInMs != delayTimeInMs){
        ticksInMs = HAL_GetTick() + delayTimeInMs + 1;
        lastAngle = moto_trigger.total_angle;
    }
    if (lastAngle - moto_trigger.total_angle > 8191 * TRIGGER_MOTOR_REDUCTION_RATIO / BULLETS_PER_ROUND * numsOfOneShot){
        lastAngle = moto_trigger.total_angle;
        speed = 0;
        ticksInMs = HAL_GetTick();
    }

    if (HAL_GetTick() - ticksInMs > delayTimeInMs)
        speed = speedInNumsPerSec / BULLETS_PER_ROUND * TRIGGER_MOTOR_REDUCTION_RATIO * 60;

    count++;
    lastNumsOfOneShot = numsOfOneShot;
    lastDelayTimeInMs = delayTimeInMs;
    return speed;
}

/* �ӵ��ĵ������������� */
void Shoot_Custom_control(void){
    if (fric_wheel_run
        &&shooter_output==1) {  //����ϵͳ��SHOOTû�й���ʱ��������)

        //HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,(GPIO_PinState)shoot_cmd);
        switch(shoot_state){
            case SINGLE_SHOOT:
                if(shoot_cmd){
                    /* ����ǵ������������ת45�� */
                    trigger_moto_position_ref = moto_trigger.total_ecd + DEGREE_45_TO_ENCODER;
                    shoot_cmd=0;
                }
                /* �ջ����㲦���������ת�� */
                trigger_moto_speed_ref = PID_Calc(&pid_trigger, moto_trigger.total_ecd, trigger_moto_position_ref);
                goto emmm;
            case TRIBLE_SHOOT:
                if(shoot_cmd){
                    /* ������������������ת3*45�� */
                    trigger_moto_position_ref = moto_trigger.total_ecd + 3*DEGREE_45_TO_ENCODER;
                    shoot_cmd=0;
                }
                /* �ջ����㲦���������ת�� */
                trigger_moto_speed_ref = PID_Calc(&pid_trigger, moto_trigger.total_ecd, trigger_moto_position_ref);
                goto emmm;
            case CONTINUOUS_SHOOT:
                speedInNumsPerSec=4.0f;
                numsOfOneShot=4;
                delayTimeInMs=10;
                break;
            case DONT_SHOOT:
                trigger_moto_speed_ref = 0;
                goto emmm;
        }
        trigger_moto_speed_ref=-ShootAndDelay(speedInNumsPerSec,numsOfOneShot,delayTimeInMs);
        Block_Bullet_handle();                                 //��������
        /* �ջ����㲦��������� */
        emmm:
        trigger_moto_current = PID_Calc(&pid_trigger_speed, moto_trigger.speed_rpm, trigger_moto_speed_ref);
    }
    else{
        trigger_moto_current = 0;
    }
    /* �ջ�����Ħ���ֵ������ */
    shoot_moto_current_left = PID_Calc(&pid_shoot_left, moto_shoot[0].speed_rpm, -fric_wheel_speed);
    shoot_moto_current_right = PID_Calc(&pid_shoot_right, moto_shoot[1].speed_rpm, fric_wheel_speed);

    /* ���Ͳ��������Ħ���ֵ������ */
    ShootMoto_Send_current(shoot_moto_current_left, shoot_moto_current_right, trigger_moto_current);
}

/* ����Ħ���ִ��� */
void FrictionWheel_Turn_on_off(void){
    if (fric_wheel_run){
        //��Ħ����
        fric_wheel_speed=SHOT_FRIC_WHEEL_SPEED;
        //�򿪼��⡢����װ��
        HAL_GPIO_WritePin(UVLED_GPIO_Port,UVLED_Pin,GPIO_PIN_SET);
        //TODO:���⣬����û�ӣ���ע����
        //write_led_io(LASER_IO, LED_ON);
    }
    else{
        //�ر�Ħ����
        fric_wheel_speed=0;
        //�رռ��⡢����װ��
        HAL_GPIO_WritePin(UVLED_GPIO_Port,UVLED_Pin,GPIO_PIN_RESET);
        //write_led_io(LASER_IO, LED_OFF);
    }
}