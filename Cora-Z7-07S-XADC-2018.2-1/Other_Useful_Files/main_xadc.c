/*
 * main.c
 *
 *  Created on: Apr 18, 2018
 *      Author: arthur
 */


#include "xsysmon.h"
#include "xparameters.h"
#include "sleep.h"
#include "stdio.h"
#include "xil_types.h"
#include "xil_printf.h"

#define XADC_DEVICE_ID XPAR_XADC_WIZ_0_DEVICE_ID
#define XADC_SEQ_CHANNELS 0x02020000
#define XADC_CHANNELS 0x02020000
#define Test_Bit(VEC,BIT) ((VEC&(1<<BIT))!=0)


void Xadc_Init(XSysMon *InstancePtr, u32 DeviceId) {
	XSysMon_Config *ConfigPtr;
	ConfigPtr = XSysMon_LookupConfig(DeviceId);
	XSysMon_CfgInitialize(InstancePtr, ConfigPtr, ConfigPtr->BaseAddress);

	// Disable the Channel Sequencer before configuring the Sequence registers.
	XSysMon_SetSequencerMode(InstancePtr, XSM_SEQ_MODE_SAFE);
	// Disable all alarms
	XSysMon_SetAlarmEnables(InstancePtr, 0x0);
	// Set averaging for all channels to 16 samples
	XSysMon_SetAvg(InstancePtr, XSM_AVG_16_SAMPLES);
	// Set differential input mode for all channels
//	XSysMon_SetSeqInputMode(InstancePtr, XADC_SEQ_CHANNELS);
//	XSysMon_SetSeqInputMode(InstancePtr, XSM_SEQ_CH_AUX01 | XSM_SEQ_CH_AUX09 );
	// Set 6ADCCLK acquisition time in all channels
	XSysMon_SetSeqAcqTime(InstancePtr, XADC_SEQ_CHANNELS);
	// Disable averaging in all channels
	XSysMon_SetSeqAvgEnables(InstancePtr, XADC_SEQ_CHANNELS);
	// Enable all channels
	XSysMon_SetSeqChEnables(InstancePtr, XADC_SEQ_CHANNELS);
	// Set the ADCCLK frequency equal to 1/32 of System clock
	XSysMon_SetAdcClkDivisor(InstancePtr, 32);
	// Enable Calibration
	XSysMon_SetCalibEnables(InstancePtr, XSM_CFR1_CAL_PS_GAIN_OFFSET_MASK | XSM_CFR1_CAL_ADC_GAIN_OFFSET_MASK);
	// Enable the Channel Sequencer in continuous sequencer cycling mode
	XSysMon_SetSequencerMode(InstancePtr, XSM_SEQ_MODE_CONTINPASS);
	// Clear the old status
//	XSysMon_GetStatus(InstancePtr);
}


u8 Channel;
u16 RawData;

int main () {
	XSysMon Xadc;
	u32 time_count = 0;

	Xadc_Init(&Xadc, XADC_DEVICE_ID);

	printf("Cora XADC Demo Initialized!\r\n");
    Channel = 17; // or 25
	while(1) {


		time_count ++;
		if (time_count == 100000) { // print channel reading approx. 10x per second
			time_count = 0;
			RawData = XSysMon_GetAdcData(&Xadc, Channel);
			xil_printf("Xadc_RawData = %u\r\n", RawData);
		}
		usleep(1);
	}
}
