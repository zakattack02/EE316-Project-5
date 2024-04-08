//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvIntegration of pmod ledvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv//
	#include "xtmrctr.h"
	#include "xparameters.h"
	#include "xil_exception.h"
	#include <stdio.h>
	#include "xscugic.h"
	#include "xil_printf.h"

	#define TMRCTR_1_DEVICE_ID        XPAR_TMRCTR_2_DEVICE_ID

	#define INTC_DEVICE_ID          XPAR_SCUGIC_SINGLE_DEVICE_ID
	#define INTC                    XScuGic
	#define INTC_HANDLER            XScuGic_InterruptHandler
	#define PWM_PERIOD              20000000    /* PWM period in (20 ms) */
	#define TMRCTR_1_0                0            /* Timer 0 ID */
	#define TMRCTR_1_1                1            /* Timer 1 ID */
	#define CYCLE_PER_DUTYCYCLE     10           /* Clock cycles per duty cycle */
	#define MAX_DUTYCYCLE           100          /* Max duty cycle */
	#define DUTYCYCLE_DIVISOR       4            /* Duty cycle Divisor */
	#define WAIT_COUNT              50000000   /* Interrupt wait counter */

	/************************** Function Prototypes ******************************/
	int TmrCtrPwmExample(XTmrCtr *InstancePtr, u16 DeviceId);
	static void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);
	/************************** Variable Definitions *****************************/
	INTC InterruptController;  /* The instance of the Interrupt Controller */
	XTmrCtr TimerCounterInst;  /* The instance of the Timer Counter */

	static int   PeriodTimerHit = FALSE;
	static int   HighTimerHit = FALSE;
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Integration of pmod led^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvIntegration of xadcvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv//
#include "xsysmon.h"
#include "xparameters.h"
#include "sleep.h"
#include "stdio.h"
#include "xil_types.h"
#include "xil_printf.h"
#include "myLCD.h"

//#include "xgpio_intr_tapp_example.h"

#define XADC_DEVICE_ID XPAR_XADC_WIZ_0_DEVICE_ID
#define XADC_SEQ_CHANNELS 0x02020000 //0x00000000
#define XADC_CHANNELS 0x02020000 //0x00000000
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

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Integration of xadc^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xgpio.h"
#include "xil_exception.h"

#ifdef XPAR_INTC_0_DEVICE_ID
 #include "xintc.h"
 #include <stdio.h>
#else
 #include "xscugic.h"
 #include "xil_printf.h"
#endif

/************************** Constant Definitions *****************************/
#ifndef TESTAPP_GEN
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define GPIO_DEVICE_ID		XPAR_GPIO_0_DEVICE_ID
#define GPIO_CHANNEL1		1

#ifdef XPAR_INTC_0_DEVICE_ID
 #define INTC_GPIO_INTERRUPT_ID	XPAR_INTC_0_GPIO_0_VEC_ID
 #define INTC_DEVICE_ID	XPAR_INTC_0_DEVICE_ID
#else
 #define INTC_GPIO_INTERRUPT_ID	XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
 #define INTC_DEVICE_ID	XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif /* XPAR_INTC_0_DEVICE_ID */

/*
 * The following constants define the positions of the buttons and LEDs each
 * channel of the GPIO
 */
#define GPIO_ALL_LEDS		0xFFFF
#define GPIO_ALL_BUTTONS	0xFFFF

/*
 * The following constants define the GPIO channel that is used for the buttons
 * and the LEDs. They allow the channels to be reversed easily.
 */
#define BUTTON_CHANNEL	 1	/* Channel 1 of the GPIO Device */
#define LED_CHANNEL	 2	/* Channel 2 of the GPIO Device */
#define BUTTON_INTERRUPT XGPIO_IR_CH1_MASK  /* Channel 1 Interrupt Mask */

/*
 * The following constant determines which buttons must be pressed at the same
 * time to cause interrupt processing to stop and start
 */
#define INTERRUPT_CONTROL_VALUE 0x7

/*
 * The following constant is used to wait after an LED is turned on to make
 * sure that it is visible to the human eye.  This constant might need to be
 * tuned for faster or slower processor speeds.
 */
#define LED_DELAY	1000000

#endif /* TESTAPP_GEN */

#define INTR_DELAY	0x00FFFFFF

#ifdef XPAR_INTC_0_DEVICE_ID
 #define INTC_DEVICE_ID	XPAR_INTC_0_DEVICE_ID
 #define INTC		XIntc
 #define INTC_HANDLER	XIntc_InterruptHandler
#else
 #define INTC_DEVICE_ID	XPAR_SCUGIC_SINGLE_DEVICE_ID
 #define INTC		XScuGic
 #define INTC_HANDLER	XScuGic_InterruptHandler
#endif /* XPAR_INTC_0_DEVICE_ID */

/************************** Function Prototypes ******************************/
void GpioHandler(void *CallBackRef);

int GpioIntrExample(INTC *IntcInstancePtr, XGpio *InstancePtr,
			u16 DeviceId, u16 IntrId,
			u16 IntrMask, u32 *DataRead);

int GpioSetupIntrSystem(INTC *IntcInstancePtr, XGpio *InstancePtr,
			u16 DeviceId, u16 IntrId, u16 IntrMask);

void GpioDisableIntr(INTC *IntcInstancePtr, XGpio *InstancePtr,
			u16 IntrId, u16 IntrMask);

/************************** Variable Definitions *****************************/

/*
 * The following are declared globally so they are zeroed and so they are
 * easily accessible from a debugger
 */
XGpio Gpio; /* The Instance of the GPIO Driver */

INTC Intc; /* The Instance of the Interrupt Controller Driver */


static u16 GlobalIntrMask; /* GPIO channel mask that is needed by
			    * the Interrupt Handler */

static volatile u32 IntrFlag; /* Interrupt Handler Flag */

int count;

//*********************************************Integration of xadc & LCD*********************************************//
u8 Channel = 17;
int sysEn = 1;
u16 RawData;
XSysMon Xadc;
u32 time_count = 0;
BaseAddress = XPAR_MYLCD_0_S00_AXI_BASEADDR;

//*********************************************Integration of xadc*********************************************//

/****************************************************************************/
/**
* This function is the main function of the GPIO example.  It is responsible
* for initializing the GPIO device, setting up interrupts and providing a
* foreground loop such that interrupt can occur in the background.
*
* @param	None.
*
* @return
*		- XST_SUCCESS to indicate success.
*		- XST_FAILURE to indicate failure.
*
* @note		None.
*
*****************************************************************************/
#ifndef TESTAPP_GEN
int main(void)
{
	//------------------------LCD+BTN--------------------------------------------------
	int Status;
	u32 DataRead;
	  print(" Press button to Generate Interrupt\r\n");
	  Status = GpioIntrExample(&Intc, &Gpio,
				   GPIO_DEVICE_ID,
				   INTC_GPIO_INTERRUPT_ID,
				   GPIO_CHANNEL1, &DataRead);
	if (Status == 0 ){
		if(DataRead == 0)
			print("No button pressed. \r\n");
		else
			print("Successfully ran Gpio Interrupt Tapp Example\r\n");
	} else {
		 print("Gpio Interrupt Tapp Example Failed.\r\n");
		 return XST_FAILURE;
	}

	//------------------------PMOD LEDs--------------------------------------------------
	int Status_PMOD_LEDs;

	/* Run the Timer Counter PWM example */
	Status_PMOD_LEDs = TmrCtrPwmExample(&TimerCounterInst, XPAR_TMRCTR_2_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Tmrctr PWM Example Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran Tmrctr PWM Example\r\n");
	xil_printf("end\r\n");
	return XST_SUCCESS;

	return XST_SUCCESS;
}
#endif

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvIntegration of pmod ledvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv//
int TmrCtrPwmExample(XTmrCtr *TmrCtrInstancePtr, u16 DeviceId)
{
	float DutyCycle_percent;
	u8  Div;
	u32 Period;
	u32 HighTime;
	int Status;

	Status = XTmrCtr_Initialize(TmrCtrInstancePtr, DeviceId);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Perform a self-test to ensure that the hardware was built
		 * correctly. Timer0 is used for self test
		 */
		Status = XTmrCtr_SelfTest(TmrCtrInstancePtr, TMRCTR_1_0);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	XTmrCtr_SetHandler(TmrCtrInstancePtr, TimerCounterHandler,
							TmrCtrInstancePtr);
	u32 pwmmasks = XTC_CSR_ENABLE_INT_MASK | XTC_CSR_DOWN_COUNT_MASK
			| XTC_CSR_AUTO_RELOAD_MASK;

	Xil_Out32(0x42820000, 0); //Turn off all fields.
	Xil_Out32(0x42820010, 0); //Turn off all fields.

	Xil_Out32(0x42820000, pwmmasks); //Start timer0 with count down/auto reload capability and interrupts.
	Xil_Out32(0x42820010, pwmmasks); //Start timer1 with count down/auto reload capability and interrupts.
	RawData = 1000;
	Div = 64000/(float)RawData;

		/* Configure PWM */
		Period = 20000; //(20ms)
		HighTime = Period/Div; //it will be high for this amount of time

		DutyCycle_percent = (float)HighTime/(float)Period*100;
		Xil_Out32(0x42820000+4, Period);
		Xil_Out32(0x42820010+4, HighTime);

		printf("PWM Configured for Duty Cycle = %5.2F\r\n", DutyCycle_percent);

		/* Enable PWM */
		pwmmasks |= XTC_CSR_ENABLE_PWM_MASK | XTC_CSR_EXT_GENERATE_MASK|XTC_CSR_ENABLE_ALL_MASK;
		Xil_Out32(0x42820000, pwmmasks);
		Xil_Out32(0x42820010, pwmmasks);

	return Status;
}

static void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
{
	/* Mark if period timer expired */
	if (TmrCtrNumber == TMRCTR_1_0) {
//		xil_printf("In Tmrctr interrupt handler - TMRCTR_1_0\r\n");
		PeriodTimerHit = TRUE;
	}

	/* Mark if high time timer expired */
	if (TmrCtrNumber == TMRCTR_1_1) {
//		xil_printf("In Tmrctr interrupt handler - TMRCTR_1_1\r\n");
		HighTimerHit = TRUE;
	}
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Integration of pmod led^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

/******************************************************************************/
/**
*
* This is the entry function from the TestAppGen tool generated application
* which tests the interrupts when enabled in the GPIO
*
* @param	IntcInstancePtr is a reference to the Interrupt Controller
*		driver Instance
* @param	InstancePtr is a reference to the GPIO driver Instance
* @param	DeviceId is the XPAR_<GPIO_instance>_DEVICE_ID value from
*		xparameters.h
* @param	IntrId is XPAR_<INTC_instance>_<GPIO_instance>_IP2INTC_IRPT_INTR
*		value from xparameters.h
* @param	IntrMask is the GPIO channel mask
* @param	DataRead is the pointer where the data read from GPIO Input is
*		returned
*
* @return
*		- XST_SUCCESS if the Test is successful
*		- XST_FAILURE if the test is not successful
*
* @note		None.
*
******************************************************************************/
int GpioIntrExample(INTC *IntcInstancePtr, XGpio* InstancePtr, u16 DeviceId,
			u16 IntrId, u16 IntrMask, u32 *DataRead)
{
	int Status;
	u32 delay;

	/* Initialize the GPIO driver. If an error occurs then exit */
	Status = XGpio_Initialize(InstancePtr, DeviceId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = GpioSetupIntrSystem(IntcInstancePtr, InstancePtr, DeviceId,
					IntrId, IntrMask);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	IntrFlag = 0;
	delay = 0;

	while(1);

	/*while(!IntrFlag && (delay < INTR_DELAY)) {
		delay++;
	}

	GpioDisableIntr(IntcInstancePtr, InstancePtr, IntrId, IntrMask);

	*DataRead = IntrFlag;*/

	return Status;
}


/******************************************************************************/
/**
*
* This function performs the GPIO set up for Interrupts
*
* @param	IntcInstancePtr is a reference to the Interrupt Controller
*		driver Instance
* @param	InstancePtr is a reference to the GPIO driver Instance
* @param	DeviceId is the XPAR_<GPIO_instance>_DEVICE_ID value from
*		xparameters.h
* @param	IntrId is XPAR_<INTC_instance>_<GPIO_instance>_IP2INTC_IRPT_INTR
*		value from xparameters.h
* @param	IntrMask is the GPIO channel mask
*
* @return	XST_SUCCESS if the Test is successful, otherwise XST_FAILURE
*
* @note		None.
*
******************************************************************************/
int GpioSetupIntrSystem(INTC *IntcInstancePtr, XGpio *InstancePtr,
			u16 DeviceId, u16 IntrId, u16 IntrMask)
{
	int Result;

	GlobalIntrMask = IntrMask;

#ifdef XPAR_INTC_0_DEVICE_ID

#ifndef TESTAPP_GEN
	/*
	 * Initialize the interrupt controller driver so that it's ready to use.
	 * specify the device ID that was generated in xparameters.h
	 */
	Result = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Result != XST_SUCCESS) {
		return Result;
	}
#endif /* TESTAPP_GEN */

	/* Hook up interrupt service routine */
	XIntc_Connect(IntcInstancePtr, IntrId,
		      (Xil_ExceptionHandler)GpioHandler, InstancePtr);

	/* Enable the interrupt vector at the interrupt controller */
	XIntc_Enable(IntcInstancePtr, IntrId);

#ifndef TESTAPP_GEN
	/*
	 * Start the interrupt controller such that interrupts are recognized
	 * and handled by the processor
	 */
	Result = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Result != XST_SUCCESS) {
		return Result;
	}
#endif /* TESTAPP_GEN */

#else /* !XPAR_INTC_0_DEVICE_ID */

#ifndef TESTAPP_GEN
	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Result = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Result != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif /* TESTAPP_GEN */

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, IntrId,
					0xA0, 0x3);

	/*
	 * Connect the interrupt handler that will be called when an
	 * interrupt occurs for the device.
	 */
	Result = XScuGic_Connect(IntcInstancePtr, IntrId,
				 (Xil_ExceptionHandler)GpioHandler, InstancePtr);
	if (Result != XST_SUCCESS) {
		return Result;
	}

	/* Enable the interrupt for the GPIO device.*/
	XScuGic_Enable(IntcInstancePtr, IntrId);
#endif /* XPAR_INTC_0_DEVICE_ID */

	/*
	 * Enable the GPIO channel interrupts so that push button can be
	 * detected and enable interrupts for the GPIO device
	 */
	XGpio_InterruptEnable(InstancePtr, IntrMask);
	XGpio_InterruptGlobalEnable(InstancePtr);

	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)INTC_HANDLER, IntcInstancePtr);

	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/******************************************************************************/
/**
*
* This is the interrupt handler routine for the GPIO for this example.
*
* @param	CallbackRef is the Callback reference for the handler.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GpioHandler(void *CallbackRef)
{
	XGpio *GpioPtr = (XGpio *)CallbackRef;
	count++;
	IntrFlag = 1;
	u32 data = XGpio_DiscreteRead(GpioPtr,1);
	Xadc_Init(&Xadc, XADC_DEVICE_ID);
	//xil_printf("Count = %u; Data = %u \r\n ", count%2, data);
	//printf("Cora XADC Demo Initialized!\r\n");

//	while(1) {
//		time_count ++;
//		if (time_count == 100000) { // print channel reading approx. 10x per second
//			time_count = 0;
//			RawData = XSysMon_GetAdcData(&Xadc, Channel);
//			xil_printf("Xadc_RawData = %u\r\n", RawData);
//		}
//		usleep(1);
//	}

	int btn0_value = 1;
	int btn1_value = 2;
	int btn2_value = 4;
	int btn3_value = 8;

	/////////////////////////////////////////////////////////////////////////////////////////
	if (data == btn0_value){
		xil_printf("-----------BTN0 Press (Reset)-----------\r\n");
		if (sysEn == 1){
			//-------------------------------------------------------------------
			//top line
			MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
			MYLCD_mWriteReg(BaseAddress, 4, 0x52737420); //Rst
			MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
			MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

			//bottom line
			MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
			MYLCD_mWriteReg(BaseAddress, 20, 0x504f5420); //POT
			MYLCD_mWriteReg(BaseAddress, 24, 0x20202020); //
			MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //
			//-------------------------------------------------------------------

			if (Channel == 17){
				Channel = 17;
				xil_printf("System Reset! Channel: %d\r\n", Channel);
				RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
			}else if (Channel == 25){// if channel is 25
				Channel = 17; //change channel to 17
				xil_printf("System Reset! Channel: %d\r\n", Channel);
				RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				}
		}else if(sysEn == 0){
			if (Channel == 17){
				Channel = 17;
				sysEn = 1;
				xil_printf("System Reset! Channel: %d\r\n", Channel);
				RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				xil_printf("System Re-enabled = %u\r\n", sysEn); //print data
				//-------------------------------------------------------------------
				//top line
				MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
				MYLCD_mWriteReg(BaseAddress, 4, 0x52737420); //Rst
				MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
				MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

				//bottom line
				MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
				MYLCD_mWriteReg(BaseAddress, 20, 0x504f5420); //POT
				MYLCD_mWriteReg(BaseAddress, 24, 0x20202020); //
				MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //
				//-------------------------------------------------------------------
			}else if (Channel == 25){// if channel is 25
				Channel = 17; //change channel to 17
				sysEn = 1;
				xil_printf("System Reset! Channel: %d\r\n", Channel);
				RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				xil_printf("System Re-enabled = %u\r\n", sysEn); //print data
				//-------------------------------------------------------------------
				//top line
				MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
				MYLCD_mWriteReg(BaseAddress, 4, 0x52737420); //Rst
				MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
				MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

				//bottom line
				MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
				MYLCD_mWriteReg(BaseAddress, 20, 0x504f5420); //POT
				MYLCD_mWriteReg(BaseAddress, 24, 0x20202020); //
				MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //
				//-------------------------------------------------------------------
				}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	else if (data == btn1_value){
		xil_printf("-----------BTN1 (Chnl Switch) Press-----------\r\n");

		if (sysEn == 1){
			if (Channel == 17){
				Channel = 25;
				xil_printf("Channel = 25 (PhRes) \r\n");
				xil_printf("sysEn: %d\r\n", sysEn);
				RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				//top line
				MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
				MYLCD_mWriteReg(BaseAddress, 4, 0x456e2020); //En
				MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
				MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

				//bottom line
				MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
				MYLCD_mWriteReg(BaseAddress, 20, 0x50687472); //Phtr
				MYLCD_mWriteReg(BaseAddress, 24, 0x73747220); //str
				MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //
	//			while(1) {
	//				time_count ++;
	//				if (time_count == 100000) { // print channel reading approx. 10x per second
	//					time_count = 0;
	//					RawData = XSysMon_GetAdcData(&Xadc, Channel);
	//					xil_printf("Xadc_RawData = %u\r\n", RawData);
	//				}
	//				usleep(1);
	//			}
			}else if (Channel == 25){// if channel is 25
				Channel = 17; //change channel to 17
				xil_printf("Channel = 17 (POT) \r\n");
				xil_printf("sysEn: %d \r\n", sysEn);
				RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				//top line
					MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
					MYLCD_mWriteReg(BaseAddress, 4, 0x456e2020); //En
					MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
					MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

					//bottom line
					MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
					MYLCD_mWriteReg(BaseAddress, 20, 0x504f5420); //POT
					MYLCD_mWriteReg(BaseAddress, 24, 0x20202020); //
					MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //
	//			while(1) {
	//				time_count ++;
	//				if (time_count == 100000) { // print channel reading approx. 10x per second
	//					time_count = 0;
	//					RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
	//					xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
	//				}
	//				usleep(1);
	//			}
			}else{}//do nothing
	//		xil_printf("Channel %d \r\n", Channel);

		}else if(sysEn == 0){
			xil_printf("System Disabled\r\n");
			RawData = 0;//get data
			xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
			xil_printf("Channel = %u\r\n", Channel); //print data

//			if (Channel == 17){
//				xil_printf("System Disabled//Ch = 17\r\n");
//				RawData = 0;//get data
//				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
//			}else if (Channel == 25){
//				xil_printf("System Disabled//Ch = 25\r\n");
//				RawData = 0;//get data
//				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
//			}else{}

		}else{}//do nothing

	}
	/////////////////////////////////////////////////////////////////////////////////////////
	else if (data == btn2_value){
		xil_printf("-----------BTN2 (En/Dis) Press-----------\r\n");
		if (sysEn == 1){ //if enabled
			sysEn = 0; //disbaled
			if (Channel == 17){
				xil_printf("System Disabled//Ch = 17\r\n");
				RawData = 0;//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				//top line
				MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
				MYLCD_mWriteReg(BaseAddress, 4, 0x44697320); //Dis
				MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
				MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

				//bottom line
				MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
				MYLCD_mWriteReg(BaseAddress, 20, 0x504f5420); //POT
				MYLCD_mWriteReg(BaseAddress, 24, 0x20202020); //
				MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //
			}else if (Channel == 25){
				xil_printf("System Disabled//Ch = 25\r\n");
				RawData = 0;//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				//top line
				MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
				MYLCD_mWriteReg(BaseAddress, 4, 0x44697320); //Dis
				MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
				MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

				//bottom line
				MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
				MYLCD_mWriteReg(BaseAddress, 20, 0x50687472); //Phtr
				MYLCD_mWriteReg(BaseAddress, 24, 0x73747220); //str
				MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //

			}else{}
		}else if (sysEn == 0){ //if disabled
			sysEn = 1; //enable
			if (Channel == 17){
				xil_printf("System Re-enabled//Ch = 17(POT)\r\n");
				RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				//top line
				MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
				MYLCD_mWriteReg(BaseAddress, 4, 0x456e2020); //En
				MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
				MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

				//bottom line
				MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
				MYLCD_mWriteReg(BaseAddress, 20, 0x504f5420); //POT
				MYLCD_mWriteReg(BaseAddress, 24, 0x20202020); //
				MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //
			}else if (Channel == 25){
				xil_printf("System Re-enabled//Ch = 15(Phrstr)\r\n");
				RawData = XSysMon_GetAdcData(&Xadc, Channel);//get data
				xil_printf("Xadc_RawData = %u\r\n", RawData); //print data
				//top line
				MYLCD_mWriteReg(BaseAddress, 32, 0x5354453a); //STE:
				MYLCD_mWriteReg(BaseAddress, 4, 0x456e2020); //En
				MYLCD_mWriteReg(BaseAddress, 8, 0x4453543a); //DST:
				MYLCD_mWriteReg(BaseAddress, 12, 0x3f3f6674); // ??ft

				//bottom line
				MYLCD_mWriteReg(BaseAddress, 16, 0x5352433a); //SRC:
				MYLCD_mWriteReg(BaseAddress, 20, 0x50687472); //Phtr
				MYLCD_mWriteReg(BaseAddress, 24, 0x73747220); //str
				MYLCD_mWriteReg(BaseAddress, 28, 0x20202020); //

			}else{}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	else if (data == btn3_value){
		xil_printf("-----------BTN3 Press-----------\r\n");
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	else{}

	/* Clear the Interrupt */
	XGpio_InterruptClear(GpioPtr, GlobalIntrMask);

}

/******************************************************************************/
/**
*
* This function disables the interrupts for the GPIO
*
* @param	IntcInstancePtr is a pointer to the Interrupt Controller
*		driver Instance
* @param	InstancePtr is a pointer to the GPIO driver Instance
* @param	IntrId is XPAR_<INTC_instance>_<GPIO_instance>_VEC
*		value from xparameters.h
* @param	IntrMask is the GPIO channel mask
*
* @return	None
*
* @note		None.
*
******************************************************************************/
void GpioDisableIntr(INTC *IntcInstancePtr, XGpio *InstancePtr,
			u16 IntrId, u16 IntrMask)
{
	XGpio_InterruptDisable(InstancePtr, IntrMask);
#ifdef XPAR_INTC_0_DEVICE_ID
	XIntc_Disable(IntcInstancePtr, IntrId);
#else
	/* Disconnect the interrupt */
	XScuGic_Disable(IntcInstancePtr, IntrId);
	XScuGic_Disconnect(IntcInstancePtr, IntrId);
#endif
	return;
}