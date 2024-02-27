#include "infrared_sensor.h"
#include "stm32f10x.h"

#define Perfect_Normal_Running (M_SEN == 0 && ML_SEN !=0 && MR_SEN !=0 && L_SEN != 0 && R_SEN != 0)
#define Normal_Running (M_SEN == 0 && (ML_SEN ==0 || MR_SEN == 0 )&& L_SEN != 0 && R_SEN != 0)
#define  Slight_Right_Deviation (M_SEN != 0 && ML_SEN ==0)
#define  Slight_Left_Deviation (M_SEN != 0 && MR_SEN ==0)
#define  Severe_Right_Deviation (M_SEN != 0 && L_SEN ==0)
#define  Severe_Left_Deviation (M_SEN != 0 && R_SEN ==0)
#define Circle ((M_SEN == 0 && L_SEN == 0 ) || ( M_SEN == 0 && R_SEN == 0) && MR_SEN != 0 && ML_SEN != 0)
#define Crossing (M_SEN==0&&MR_SEN ==0 && ML_SEN == 0 && M_SEN == 0 &&R_SEN==0)
#define Stop (MR_SEN ==0 && ML_SEN == 0 && M_SEN == 0 &&R_SEN==0)
#define Collision CLP_SEN = 0
#define Near NEAR_SEN = 1
u8 L_SEN, ML_SEN, M_SEN, MR_SEN, R_SEN, NEAR_SEN, CLP_SEN, Circle_num = 0;
u8 running_status;

void infrared_init() {
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef infraredStr;
    infraredStr.GPIO_Speed = GPIO_Speed_50MHz;
    infraredStr.GPIO_Mode = GPIO_Mode_IPU;
    infraredStr.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_Init(GPIOA, &infraredStr);

}

/**
 * @brief 读取红外传感器状态，并根据不同情况更新运行状态值。
 */
void infrared_tell() {
    // NEAR_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    // CLP_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
    R_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    MR_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
    M_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
    ML_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
    L_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);

    if (Perfect_Normal_Running || Normal_Running) { running_status = 50; }
    else if (Slight_Left_Deviation) { running_status = 65; }
    else if (Slight_Right_Deviation) { running_status = 35; }
    else if (Severe_Left_Deviation) { running_status = 80; }
    else if (Severe_Right_Deviation) { running_status = 20; }
    else if (Crossing) { running_status = 50; }
    else if (Circle) { if (L_SEN == 0) { running_status = 30; } else { running_status = 70; }}
    else if (Stop) { running_status = 0; }
}
