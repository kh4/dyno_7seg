#include "board.h"

/*
    2 channel ADC
*/

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
__IO uint32_t ADC_DualConvertedValueTab[4];


void __processADC(bool isFull)
{
  uint16_t _values[2];
  for (int i = 0; i < 2; i++) {
    uint8_t idx = (isFull ? 2 : 0) + i;
    _values[i] = ADC_DualConvertedValueTab[idx] & 0xfff;
  }
  handleValuesFromADC(_values);
}


void DMA1_Channel1_IRQHandler(void)
{
  if (DMA_GetITStatus(DMA1_IT_HT1)) {
    DMA_ClearITPendingBit(DMA1_IT_HT1);
    __processADC(0);
  }
  if (DMA_GetITStatus(DMA1_IT_TC1)) {
    DMA_ClearITPendingBit(DMA1_IT_TC1);
    __processADC(1);
  }
}

void adcInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ADC on PA0 - PA1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* DMA1 channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_DualConvertedValueTab;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    /* Enable DMA1 Channel1 */
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_HT, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // DMA Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 2;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);

  ADC_DMACmd(ADC1, ENABLE);

  ADC_Cmd(ADC1, ENABLE);
  ADC_TempSensorVrefintCmd(ENABLE);
  ADC_ResetCalibration(ADC1);

  while(ADC_GetResetCalibrationStatus(ADC1));

  ADC_StartCalibration(ADC1);

  while(ADC_GetCalibrationStatus(ADC1));

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
