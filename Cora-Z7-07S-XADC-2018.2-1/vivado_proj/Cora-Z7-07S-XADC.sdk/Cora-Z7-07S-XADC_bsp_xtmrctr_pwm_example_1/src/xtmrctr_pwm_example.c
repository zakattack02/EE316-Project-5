///////PROF CODE////////////////////PROF CODE////////////////////PROF CODE////////////////////PROF CODE//


	/***************************** Include Files *********************************/
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

	/**************************** Type Definitions *******************************/

	/***************** Macros (Inline Functions) Definitions *********************/

	/************************** Function Prototypes ******************************/
	int TmrCtrPwmExample(XTmrCtr *InstancePtr, u16 DeviceId);
	static void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);
	/************************** Variable Definitions *****************************/
	INTC InterruptController;  /* The instance of the Interrupt Controller */
	XTmrCtr TimerCounterInst;  /* The instance of the Timer Counter */

	/*
	 * The following variables are shared between non-interrupt processing and
	 * interrupt processing such that they must be global.
	 */
	static int   PeriodTimerHit = FALSE;
	static int   HighTimerHit = FALSE;

	int main(void)
	{
		int Status;
		/* Run the Timer Counter PWM example */
		Status = TmrCtrPwmExample(&TimerCounterInst, XPAR_TMRCTR_2_DEVICE_ID);
		if (Status != XST_SUCCESS) {
			xil_printf("Tmrctr PWM Example Failed\r\n");
			return XST_FAILURE;
		}

		xil_printf("Successfully ran Tmrctr PWM Example\r\n");
		xil_printf("end\r\n");
		return XST_SUCCESS;
	}

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

		u32 RawData = 33000;
		Div = 66000/(float)RawData;

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
