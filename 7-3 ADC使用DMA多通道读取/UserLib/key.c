#include "key.h"
#include "delay.h"
#include <stdio.h>

void Key_Init(void)
{
    /* ����GPIOAʱ�ӡ���ʱ��TIM2ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    /* ����GPIO */    
    GPIO_InitTypeDef GPIOInitStructure;
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIOInitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB, &GPIOInitStructure);

    /* GPIO�����ⲿ�ж��� */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

    /* �����ⲿ�жϽṹ�� */
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;     //�ⲿ�¼�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_Init(&EXTI_InitStructure);

    // /* ����NVIC */
    // NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);

    // NVIC_InitTypeDef NVIC_InitStructure;
    // NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
}

uint8_t getKeyNum(void)
{
    if(GPIO_ReadInputDataBit(GPIOB, KEY1) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, KEY1) == RESET);		//�ȴ���������
        delay_ms(20);
        return 1;
    }
    if(GPIO_ReadInputDataBit(GPIOA, KEY2) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA, KEY2) == RESET);		//�ȴ���������
        delay_ms(20);
        return 2;
    }
    if(GPIO_ReadInputDataBit(GPIOB, KEY3) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, KEY3) == RESET);		//�ȴ���������
        delay_ms(20);
        return 3;
    }
    return 0;
}

/* �ⲿ�ж���EXIT11�жϴ����� */
void EXTI15_10_IRQHandler(void)
{
    printf("hello");
}