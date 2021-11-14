//*************************************************************************
// Integrated Autonomous Tester - PROJ325 - main.cpp
// 
// Configure the Nucleo F429ZI board, Run Power On Test function
// Attach sample ISR and start Threads
//
//*************************************************************************
#include "mbed.h"
#include <string>
#include "AutoMeasuring.hpp"
#include "SerialTerminal.h"
#include "LCD.hpp"        
#include "SwitchingMatrix.hpp"
#include "SDCard.hpp"   
#include "switches.h"
#include "Bluetooth.h"

Serial pc(USBTX,USBRX);
Serial HC05_Master(PE_8,PE_7);

//Main 
int main() 
{
    pc.baud(38400);
    HC05_Master.baud(38400);
    
    //Power on test
    pot();
    
    //Attach the sample ISR       
    Sample_Ticker.attach(&Read_ADC,sample_rate); 
    
    //Start Threads
    sample_thread.start(Take_Sample);
    SwitchingMatrix_thread.start(Switch_Relays);
    lowrange_thread.start(LowRangeMeasurement );      
    highrange_thread.start(HighRangeMeasurement );     
    Continuity_thread.start(Continuity);                       
    BespokeContinuity_thread.start(BespokeContinuity);
    serialterminal_thread.start(Serial_Terminal);
    LCD_thread.start(callback(&LCDqueue, &EventQueue::dispatch_forever));
    SDCard_thread.start(callback(&SDCardqueue, &EventQueue::dispatch_forever));
    Bluetooth_thread.start(Bluetooth_Tx_Data);
    
    //Queues                
    SDCardqueue.call(SDCard);
    //Update the LCD Display
    LCDqueue.call_every(Update_Display, LCD);
    
    while(1) 
    {   
        }//end while
} //end main
