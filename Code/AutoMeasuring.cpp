//************************************************************************/
// Integrated Autonomous Tester - PROJ325 - AutoMeasuring.cpp
// 
// Initialize and define the variables used by the analogue circuit
// Samples the voltage across the current sense resistor
// Samples the voltage drop across the load (Unit Under Test)
// Display the test results to the serial terminal (Putty)
// Transmit the test results to the inf-Display Module
//
//************************************************************************/

#include "AutoMeasuring.hpp"

//Functions
void Read_ADC();
void Take_Sample();
void LowRangeMeasurement ();
void HighRangeMeasurement ();
void Continuity();
void BespokeContinuity();      
    
//Global variable
int TP_Counter=0 ;
int i=0;

float TP_Resistance[Test_Point];
//Low Range Resistance variables
float voltage_lowrange=0.0;
float resistance_lowrange=0.0;
//High Range Resistance variables
float voltage_highrange=0.0;
float resistance_highrange=0.0;
//Current Measurment variables
float current=0.0;
float Vin0[N];
float Vin0_sum=0.0;
float Vin0_average=0.0;
//Low Range Voltage Measurment Variables
float Vin1[N];
float Vin1_sum=0.0;
float Vin1_average=0.0;
//High Range Voltage Measurment Variables
float Vin2[N];
float Vin2_sum=0.0;
float Vin2_average=0.0;

//Initialize Flags
bool SampleSignal_lowrange=0;
bool SampleSignal_highrange=0;
bool SampleSignal_Continuity=0;
bool SampleSignal_Bespoke=0;

//DigitalOut 
DigitalOut redLED(D15);
DigitalOut yellowLED(D14);
DigitalOut greenLED(D11);

//Analogue Inputs
AnalogIn AIN0(A0);
AnalogIn AIN1(A1);
AnalogIn AIN2(A2);

//Timer 
Ticker Sample_Ticker;

//Threads
Thread  sample_thread(osPriorityNormal, 2048, NULL);
Thread lowrange_thread(osPriorityNormal, 2048, NULL);
Thread highrange_thread(osPriorityNormal, 2048, NULL);
Thread Continuity_thread(osPriorityNormal, 2048, NULL);
Thread BespokeContinuity_thread(osPriorityNormal, 2048, NULL);

Mutex samplelock;   

//************************************************************************
// Read_ADC(void)
// Function is called by Sample_Ticker every sample rate (0.001) and 
// sets a Signal (Start_Sampling) 
// Parameters - void
// Return - void
//************************************************************************
void Read_ADC()
{
    sample_thread.signal_set(Start_Sampling);                         
}//end function

//************************************************************************
// pot() - Power On Test 
// Get the current date and time
// Flashs the Red, Green and Yellow Leds
// prints to the user the current date ,time and all user commands
// Parameters - void
// Return - void
//************************************************************************
void pot() 
{   
    //Update Date and Time
    seconds = time(NULL);                       
    timeData = localtime(&seconds); 
    set_time(mktime(timeData));     
    
    printf("**********STARTING POWER ON SELF TEST (POST)**********\n\r");

    //Toggle LEDs
    printf("ALL LEDs should be blinking\n\n\r");
    for (unsigned int n=0; n<10; n++) 
        {
            redLED    = 1;
            yellowLED = 1;
            greenLED  = 1;
            wait(0.05);
            redLED    = 0;
            yellowLED = 0;
            greenLED  = 0;     
            wait(0.05);                     
        }
    
    //Display Commands
    printf("READY FOR COMMANDS\n\r");
    printf("START               : Start Testing\r\n");
    printf("LOW                 : Set Ohmmeter range (0.1%s to 1%s)\n\r",OMEGA,OMEGA);
    printf("HIGH                : Set Ohmmeter range (1%s to 20%s)\n\r",OMEGA,OMEGA);
    printf("Continuity          : Set Continuity Test\n\r");
    printf("Bespoke Continuity  : Set Bespoke Continuity Test\n\r");
    printf("STOP                : Stop Testing\r\n");
    printf("AUTOCALI            : Set Automatic Calibration\r\n");
    printf("AUTOCALIOFF         : Disable Automatic Calibration\r\n");
    printf("SETDATE             : Set Date\r\n");
    printf("SETTIME             : Set Time\r\n");
    puts("\nPLEASE UPDATE DATE AND TIME IF REQUIRED\r\n");
    
    //Display current Time and Date
    printf("CURRENT DATE AND TIME : %s\r\n", ctime(&seconds));   
   
    //Display on LCD             
    lcd.cls();
    lcd.printf("Integrated Auto\nTester");

    printf("**********POST END**********\r\n\n");
    
}//end POT


//************************************************************************
// Take_Sample(void)
// Executed by Switch_Relays and Read_ADC
// Samples the voltage across the current sense resistor
// Samples the voltage drop across the load (Unit Under Test)
// Signals the Bluetooth thread to transmit data
// Signals LowRangeMeasurement , HighRangeMeasurement , Continuity and
// BespokeContinuity
// Add data to the SD Card queue
// Parameters - void
// Return - void
//************************************************************************
void Take_Sample()
{
    while(1)
    {
        while(Relay_Settled)
        {
            //Wait for Signal from Read_ADC
            Thread::signal_wait(Start_Sampling);
                                  
            //Disable Interrupts
            __disable_irq();
            
            //Acquire lock
            samplelock.lock();
            
            //Read ADC(A0)
            Vin0[i] = AIN0.read();
            Vin0_sum=Vin0_sum+Vin0[i];        
            
            //Read ADC(A1)
            Vin1[i] = AIN1.read();
            Vin1_sum=Vin1_sum+Vin1[i];
            
            //Read ADC(A2)
            Vin2[i] = AIN2.read();
            Vin2_sum=Vin2_sum+Vin2[i];    
            
            //Increment Counter
            ++i;
                                                                 
            if( i > (N-1) ) 
            {
                //Reset Counter
                i=0;                                 
                   
               //Reset the Test point counter 
               if (TP_Counter == Test_Point) TP_Counter=0;                                                                                                                                    
        
               //Increment Test Point Counter
               TP_Counter++;
               
               //Calculate and Reset ADC     
               Vin0_average= Vin0_sum/N;            
               Vin0_sum=0;                                         
               Vin1_average= Vin1_sum/N;            
               Vin1_sum=0;                          
               Vin2_average= Vin2_sum/N;            
               Vin2_sum=0;                              
               
               //Clear ADC Arrays  
               for ( int c=0 ; c<N ; ++c)
               {
                    Vin0[c]=0;                              
                    Vin1[c]=0;                              
                    Vin2[c]=0;                              
                }//end for
                
                //Current Calculation
                current=(((Vin0_average*2.0f*3.3f)/10.16f)/0.999134f);         
                    
                //Low and High Range Voltage Calculations
                voltage_lowrange=((Vin1_average*3.3f)/(10.13f));
                voltage_highrange=(Vin2_average*3.3f);
                    
                //Calibration Routine
                if (AutoCalibrate_Measurments)
                {                           
                    //Calibrate Low Range Measuremnt 
                    voltage_lowrange=voltage_lowrange-0.010f;
                    if (voltage_lowrange < 0) voltage_lowrange=0; 
                    voltage_highrange=voltage_highrange-0.010f;   
                    if (voltage_highrange < 0) voltage_highrange=0; 
                                              
                }//end AutoCalibration 
                        
                //Low and High Range Resistance Calculations
                resistance_lowrange=(voltage_lowrange/current);                                          
                resistance_highrange=(voltage_highrange/current);
                
                //Signal threads 
                SampleSignal_lowrange=1;
                SampleSignal_highrange=1;
                SampleSignal_Continuity=1;
                SampleSignal_Bespoke=1;
                
                SwitchingMatrix_thread.signal_set(SampleSignal_SwitchingM);
                
                BleutoothTx=1;
                
                if (SDCard_Ready)  SDCardqueue.call(SDCard_GetData);
                
                //Release Mutex
                samplelock.unlock();
            
            }//end if      
        
            //Enable Interrupts
            __enable_irq();
            
            //Release Mutex
            samplelock.unlock(); 
               
        }//end while Relay_Settled     
    }//end while(1)
}//end sample


//************************************************************************
// LowRangeMeasurement(void)
// Executed by Serial Terminal commands and Sample thread
// Display the Low range measurement test results on the serial terminal
// Parameters - void
// Return - void
//************************************************************************
void LowRangeMeasurement ()
{   
    while(1)
    {
        if(lowrangeflag && startflag)
        {    
            if(SampleSignal_lowrange)
            {
                //Acquire lock
                samplelock.lock();
                  
                if (TP_Counter == 1)    puts("\n**********************************Low Range Test**********************************\r");
                 
                //Harness Test
                if (voltage_lowrange > 0.150f && voltage_lowrange < 0.280f)    printf("Test Point %d      Out of Range\n\r",TP_Counter);  //CHECK
                else if (voltage_lowrange  >= 0.280f)                           printf("Test Point %d      Open Circuit\n\r",TP_Counter);
                else if (voltage_lowrange < ShortC_Condition)                  printf("Test Point %d      Short Circuit\n\r",TP_Counter);
                else   printf(GREEN"Test Point %d      Resistance= %5.3f %s      Voltage= %5.3f V      Current= %5.3f A\n\r",TP_Counter, resistance_lowrange,OMEGA,voltage_lowrange,current);
         
                //printf("Test Point %d      Resistance= %5.3f %s      Voltage= %5.3f V      Current= %5.3f A\n\r",TP_Counter, resistance_lowrange,OMEGA,voltage_lowrange,current);
                
                //Reset flag
                SampleSignal_lowrange=0;   
            
                //Release lock
                samplelock.unlock();
                
            }//end if SampleSignal_lowrange
        }//end if 
    }//end while
}//end LowRangeMeasurement 


//************************************************************************
// HighRangeMeasurement(void)
// Executed by Serial Terminal commands and Sample thread
// Display the High range measurement test results on the serial terminal
// Parameters - void
// Return - void
//************************************************************************
void HighRangeMeasurement ()
{
    while(1)
    {
        if(highrangeflag && startflag)
        {            
            if(SampleSignal_highrange)
            {
                //Acquire lock
                samplelock.lock();  
            
                if (TP_Counter == 1)    puts("\n**********************************High Range Test**********************************\r");
                
                //Harness Test
                if (voltage_highrange  > 2.3f && voltage_highrange < 3.0f)    printf("Test Point %d      Out of Range\n\r",TP_Counter);
                else if (voltage_highrange  >= OpenC_Condition)               printf("Test Point %d      Open Circuit\n\r",TP_Counter);
                else if (voltage_lowrange < ShortC_Condition)                 printf("Test Point %d      Short Circuit\n\r",TP_Counter);  
                else printf("Test Point %d      Resistance= %5.3f %s      Voltage= %5.3f V      Current= %5.3f A\n\r",TP_Counter, resistance_highrange,OMEGA,voltage_highrange,current);
         
                //printf("Test Point %d      Resistance= %5.3f %s      Voltage= %5.3f V      Current= %5.3f A\n\r",TP_Counter, resistance_highrange,OMEGA,voltage_highrange,current);
                
                 //Reset Flag
                 SampleSignal_highrange=0;

                //Release lock
                samplelock.unlock();
                
            }//end if
        }//end if
    }// end while     
}//end HighRangeMeasurement 

//************************************************************************
// Continuity(void)
// Executed by Serial Terminal commands and Sample thread
// Display the Continuity test results on the serial terminal
// Parameters - void
// Return - void
//************************************************************************
void Continuity()
{    
    while(1)
    {
        if(continuityflag && startflag)
        {
            if (SampleSignal_Continuity)   
            {
                //Acquire lock
                samplelock.lock();  
                
                if (TP_Counter == 1)     puts("\n**********************************Continuity Test**********************************\r");
                                          
                //Harness Test
                if (voltage_highrange  >= OpenC_Condition)        printf("Test Point %d      Open Circuit\n\r",TP_Counter);
                else if (voltage_lowrange < ShortC_Condition)     printf("Test Point %d      Pass\n\r",TP_Counter);
                else 
                {
                    if (voltage_lowrange < 0.150f)        printf("Test Point %d      Fail      Resistance= %5.3f %s\n\r",TP_Counter, resistance_lowrange,OMEGA);
                    else if (voltage_highrange < 2.3f)    printf("Test Point %d      Fail      Resistance= %5.3f %s\n\r",TP_Counter, resistance_highrange,OMEGA);
                    else                                  printf("Test Point %d      Fail\n\r",TP_Counter);

                 }//end if TP_Counter
                
                 //Reset flag
                 SampleSignal_Continuity=0;   

                //Release lock
                samplelock.unlock();
                
            }//end if SampleSignal_Continuity
        }//end if 
    }//end while       
}//end Continuity

//************************************************************************
// BespokeContinuity(void)
// Executed by Serial Terminal commands and Sample thread
// Display the Bespoke Continuity test results on the serial terminal
// Parameters - void
// Return - void
//************************************************************************
void BespokeContinuity()
{
    while(1)
    {
        if(Bespokecontinuityflag)          
        {
            if(SampleSignal_Bespoke)
            {
                //Acquire lock
                samplelock.lock(); 
               
               //Harness Test
               if (Bespoke_Measure == 1)
               {
                    if(TP_Counter == 1)         puts("\n**************************Bespoke Continuity Test**********************************\r");
                    
                    if (voltage_lowrange < 0.150f)
                    {
                         TP_Resistance[TP_Counter-1]=resistance_lowrange;
                         printf("Test Point %d      Measure      Resistance= %5.3f %s\n\r",TP_Counter, TP_Resistance[TP_Counter-1],OMEGA);
                     }
                    else if (voltage_highrange < 2.3f)
                    {
                         TP_Resistance[TP_Counter-1]=resistance_highrange;
                         printf("Test Point %d      Measure      Resistance= %5.3f %s\n\r",TP_Counter, TP_Resistance[TP_Counter-1],OMEGA);
                    }
                    else 
                    {
                        TP_Resistance[TP_Counter-1]=resistance_highrange;
                        printf("Test Point %d      Open Circuit\n\r",TP_Counter);
                    }
                    
                }
                else
                {
                    
                    if (TP_Counter == 1)    puts("\n**************************Bespoke Continuity Test**********************************\r");
                    
                    if ( (resistance_lowrange < (TP_Resistance[TP_Counter-1]+0.5f) && resistance_lowrange > (TP_Resistance[TP_Counter-1]-0.5f)) || (resistance_highrange < (TP_Resistance[TP_Counter-1]+0.5f) && resistance_highrange > (TP_Resistance[TP_Counter-1]-0.5f)))
                    {
                        printf("Test Point %d      Pass\n\r",TP_Counter);
                    }
                    else if (resistance_highrange > 25)     printf("Test Point %d      Open Circuit\n\r",TP_Counter);
                    else                                    printf("Test Point %d      Fail\n\r",TP_Counter);    
                }
                
            //Reset Flag
            SampleSignal_Bespoke =0;
                
            //Release lock
            samplelock.unlock();

            }//end if SampleSignal_Bespoke
        }//end if
    }//end while
}//end Bespoke_Continuity
