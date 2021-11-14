//*************************************************************************
// Integrated Autonomous Tester - PROJ325 - Bluetooth.h
// 
// Responsible for transmitting the test results to the 
// inf-Display Module (Slave) via Bluetooth serial port protocol module
//
//*************************************************************************
#include "Bluetooth.h"

//Function
void Bluetooth_Tx_Data();

//Variables
int    State=0;
int    Bespoke_State=0;
char str[100];
bool BleutoothTx = 0;

//Thread
Thread Bluetooth_thread;
Mutex BluetoothLock;

//************************************************************************
// Bluetooth_Tx_Data() 
// Transmit the test results to the Inf-Display Module (Slave Module)
// Parameters - void
// Return - void
//************************************************************************
void Bluetooth_Tx_Data()
{
    while(1)
    {
        while(BleutoothTx)
        {   
            //Set the State variable to inform the Slave Module which test is running
            if (lowrangeflag && startflag && TP_Counter != 0)                   State=1;
            else if (highrangeflag && startflag && TP_Counter != 0)             State=2;
            else if (continuityflag && startflag  && TP_Counter != 0)           State=3;
            else if (Bespokecontinuityflag && startflag && TP_Counter != 0)
            {
                State=4;
                Bespoke_State=6;
                if (Bespoke_Measure==0)
                {
                    State=4;                    
                    if ( (resistance_lowrange < (TP_Resistance[TP_Counter-1]+0.5f) && resistance_lowrange > (TP_Resistance[TP_Counter-1]-0.5f)) || (resistance_highrange < (TP_Resistance[TP_Counter-1]+0.5f) && resistance_highrange > (TP_Resistance[TP_Counter-1]-0.5f)))
                    Bespoke_State=7;            
                    else if (resistance_highrange > 25)     Bespoke_State=9;
                    else                                    Bespoke_State=8; 
                }
            }
            else if (stoptestflag)    State=5;
            
            //Acquire lock
            BluetoothLock.lock();
            
            wait_ms(200);
            HC05_Master.printf(" ");
            HC05_Master.printf(" ");
            wait_ms(200);
            HC05_Master.printf(" ");
            
            //Acquire Lock
            samplelock.lock();
            
            //Store Test Results in a string in the buffer
            sprintf(str,"ST%d %d %d %2.2f %1.3f %1.3f %3.2f %1.3fE",TP_Counter,State ,Bespoke_State ,resistance_lowrange,voltage_lowrange,current,resistance_highrange,voltage_highrange);
            
            //Start Transmitting Data
            for (int t=0; t<TX_MAX+1; t++)
            {
                wait_ms(5);
                
                //Transmit Data
                HC05_Master.printf("%c",str[t]);
                
                //Clear Tx Buffer
                if (t == TX_MAX)   memset(str, 0, sizeof(str));   
                              
                wait_ms(5);
                
            }///end for
            
            //Reset Flag
            BleutoothTx=0;
            
            //Release Lock
            samplelock.unlock();
            
            //Release Lock           
            BluetoothLock.unlock();
            
        }//end BleutoothTx
    }//end while(1)
}//end Bluetooth_TXData
