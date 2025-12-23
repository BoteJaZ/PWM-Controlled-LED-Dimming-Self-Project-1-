//#############################################################################
//
// To Dim an LED using ePWM1
// CPU1 controls ePWM1 
// ePWM1 - sawtooth waveform of 10 kHz
// Used for loop to update CMPA in which duty ratio is varied and by extension brightness of the LED
//
//#############################################################################

//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

//
// Main
//

volatile uint16_t x = 2500; 

void main(void)
{
    // Initialize the system
    InitSysCtrl();

    // Set the frequency clock for ePWM module to less than or equal to 100 MHz
    EALLOW;
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 1;

    // Assign ePWM1 to CPU1 
    DevCfgRegs.CPUSEL0.bit.EPWM1 = 0;

    // Enable clock access to ePWM1 via CPU1
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    EDIS;

    // Stop timer clock for ePWM1
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    // Configure the ePWM1 module
    // Time base sub-module
    // Clock fed to ePWM module is 100 MHz. TBCLK = 100 MHz/ (HSPCLKDIV * CLKDIV) = 100 MHz/ (2 * 1) = 50 MHz
    // Configure ePWM1
    EPwm1Regs.TBCTL.bit.FREE_SOFT = 2;
    EPwm1Regs.TBCTL.bit.CLKDIV = 0;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 1;
    EPwm1Regs.TBCTL.bit.PRDLD = 0;
    EPwm1Regs.TBCTL.bit.CTRMODE = 0;        // up count mode

    // Load value in Period Register
    EPwm1Regs.TBPRD = 5000 - 1;             // 10kHz sawtooth waveform

    // Counter-compare sub-module 
    EPwm1Regs.CMPCTL.bit.LOADAMODE = 0;
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0;
    

    // Action Qualifier sub-module
    // TBCTR = 0, A will be high (bit ZRO - AQCTLA/B)
    EPwm1Regs.AQCTLA.bit.ZRO = 2;
    EPwm1Regs.AQCTLA.bit.CAU = 1;

    // Configure GPIO0 as ePWM functionality
    // GPIO0 Group 00b
    EALLOW;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;

    // GPIO0 as ePWM pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;
    EDIS;

    // Start the timer counter for ePWM 1
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
     
    // Infinite loop
    while (1) 
    {
        for (x = 0; x < 5000; x+= 50)
        {
            EPwm1Regs.CMPA.bit.CMPA = x;
            DELAY_US(10000);
        }

        for (x = 5000; x > 0; x-= 50)
        {
            EPwm1Regs.CMPA.bit.CMPA = x;
            DELAY_US(10000);
        }

        // EPwm1Regs.CMPA.bit.CMPA = x;
    }
}

//
// End of File
//
