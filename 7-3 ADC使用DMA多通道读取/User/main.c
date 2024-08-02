/**
 * 
 * 	ADC��˫ADCģʽ�£�ʹ��DMA���ж�ͨ��������
 *  
 *  ADCͨ�������Ŷ�Ӧ��ϵ��
 * 	ADC1_IN0   ---->  PA0 
 *  ADC1_IN16  ---->  �ڲ��¶ȴ��������������
 *  ADC2_IN7   ---->  PA7
 *  
 * 
 */


#include "stm32f10x.h"                  
#include "delay.h"
#include "usart.h"
#include <stdio.h>

void parseData(void);					//����ADC������������
void ADC_Configuration(void);			//����ADC
void DMA_Configuration(void);			//����DMA
void GPIO_Configuration(void);			//����GPIO
void RCC_Configuration(void);			//����ADCCLK��GPIO��ADCʱ��

__IO uint32_t ADCConvertedValue[2]; //��������DMA����ȡ�������ݴ�ŵ�����

int main(void)
{
	COM_Init();				//���ڳ�ʼ�������������������
	RCC_Configuration();	
	GPIO_Configuration();
	DMA_Configuration();	
	ADC_Configuration();
	
	while (1)
	{
		parseData();
		delay_ms(3000);	//delay 3s
	}
}


void RCC_Configuration(void)
{
    /* ��PCLK2���ж���Ƶ */
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);	//PCLK2����2��Ƶ��28 / 2 = 14MHz  = 0.083us 

	/* ����GPIOB��ADC1ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC2, ENABLE);

	/* ����DMAʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void ADC_Configuration(void)
{
	
	/* ����ADC1 */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	//����ͬ��ģʽRegular simultaneous mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//��������ת��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//����ɨ��ת��ģʽ
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//���ݶ��뷽ʽ���Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//���ⲿ����
	ADC_InitStructure.ADC_NbrOfChannel = 2;	//2��ADCͨ��
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ����ADC1ͨ��0��ͨ��16�Ĳ���˳��Ͳ���ʱ�� */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5); //����ʱ��0.083us * 239.5 = 19.88us
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 2, ADC_SampleTime_239Cycles5); //����ʱ��0.083us * 239.5 = 19.88us

	/* Enable the temperature sensor and vref internal channel */ 
	ADC_TempSensorVrefintCmd(ENABLE); 
	
	/* ��ʼADC1��DMAģʽ */
	ADC_DMACmd(ADC1, ENABLE);

	/* ����ADC2 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	//����ͬ��ģʽRegular simultaneous mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//��������ת��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//����ɨ��ת��ģʽ
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//���ݶ��뷽ʽ���Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//���ⲿ����
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//1��ADCͨ��
	ADC_Init(ADC2, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_239Cycles5); //����ʱ��0.083us * 239.5 = 19.88us

	/* ����ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* ����ADCУ׼�Ĵ��� */
	ADC_ResetCalibration(ADC1);

	/* ��鸴λADCУ׼�Ƿ���� */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* ����ADCУ׼ */
	ADC_StartCalibration(ADC1);

	/* ���У׼�Ƿ���� */
	while(ADC_GetCalibrationStatus(ADC1));


	/* ����ADC2 */
	ADC_Cmd(ADC2, ENABLE);

	/* ����ADCУ׼�Ĵ��� */
	ADC_ResetCalibration(ADC2);

	/* ��鸴λADCУ׼�Ƿ���� */
	while(ADC_GetResetCalibrationStatus(ADC2));

	/* ����ADCУ׼ */
	ADC_StartCalibration(ADC2);

	/* ���У׼�Ƿ���� */
	while(ADC_GetCalibrationStatus(ADC2));

	/* ��ʼADC���ת�� */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);

	/* Test on DMA1 channel1 transfer complete flag */
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));

	/* Clear DMA1 channel1 transfer complete flag */
	DMA_ClearFlag(DMA1_FLAG_TC1);	
}



void DMA_Configuration(void)
{
	/* ����ʼ��DMA1ͨ��1 */
	DMA_DeInit(DMA1_Channel1);

	/* ����DMA��ʼ���ṹ�� */
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);		//�����������ַ��������ADC1���ݼĴ����ĵ�ַ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//�������ݴ�С��32λ, ˫ADCģʽ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;		//�����ڴ����ַ�����������ݴ�ŵ��ڴ��ַ
	DMA_InitStructure.DMA_BufferSize = 2;		//�����С����������Ϊһ����λ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//������Ϊ���ݴ������Դ 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//�ر�DMA���ڴ浽�ڴ�ģʽ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;	//�ڴ����ݴ�С��32λ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//�ڴ��ַ����
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//ѭ������ģʽ������ζ�ȡ���ݵ�������
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;		//���ȼ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//�����ַ������
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	//��ʼ��DMA

	DMA_Cmd(DMA1_Channel1, ENABLE);	//ʹ��DMA
}


void GPIO_Configuration(void)
{
	/* ����GPIO */    
    GPIO_InitTypeDef GPIOInitStructure;
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_AIN;	//ģ������
    GPIOInitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_7;
    GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIOInitStructure);
}

void parseData(void)
{
	// Temperature (in ��C) = {(V25 - VSENSE) / Avg_Slope} + 25.
	// v25����ֵ1.43V  Avg_Slope 4.3mv/��c VSENSE=DataValue
	//����Vdda/4096

	/**
	 *   adc1_data�����ڲ��¶ȴ�����������
	 *   adc1_data2����PA0������
	 *   adc2_data����PA7������	
	 * 
	 */

	double temperature;
	uint16_t adc1_data, adc2_data, adc1_data2;
	/* ��16λ��ADC1�����ݣ���16λ��ADC2������ */
	adc1_data = ADCConvertedValue[0] & 0xffff;
	adc2_data = (ADCConvertedValue[0] >> 16);
	adc1_data2 = ADCConvertedValue[1] & 0xffff;

	/* ʹ�òο��ֲ��������Ĺ�ʽ������¶� */
	double VSENSE = (double)adc1_data*(3.3/4096);
	temperature = (1.43 - VSENSE) / (0.0043) + 25;
	
	/* ����������� */
	printf("t: %.2f, d1: %d, d2: %d", temperature, adc2_data, adc1_data2);
}





