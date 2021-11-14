//************************************************************************/
// Integrated Autonomous Tester - PROJ325 - SwitchingMatrix.hpp
//
// Switches the relays to create connections between sections of the 
// electrical harness and the testing system
//
//************************************************************************/

#include "SwitchingMatrix.hpp"

//Function
void Switch_Relays();

//Variables
int on = 1,off = 0;     
bool Relay_Settled = 0;

//Thread
Thread SwitchingMatrix_thread;  

//Relay initialization 
DigitalOut Relay1(PE_14);
DigitalOut Relay2(PE_15);
DigitalOut Relay3(PB_10);
DigitalOut Relay4(PB_11);

//************************************************************************
// Switch_Relays() - 
// Executed by Serial Terminal commands
// Turn the Relay On, signals the sample thread to start sampling 
// wait for the samples to be ready then turn the relay off
// Parameters - void
// Return - void
//************************************************************************


void Switch_Relays()
{
    while(1)
    {
        while(startflag && (lowrangeflag  || highrangeflag || continuityflag || Bespokecontinuityflag))
        {                   
            //Test Point 1
            //Turn Relay On 
            Relay1= on;
        
            //Relay and Circuit Settle time
            wait(1.0);
            
            //Set flag
            Relay_Settled = 1;        
            
            //Wait for signal
            Thread::signal_wait(SampleSignal_SwitchingM);
            
            //Re-set flag
            Relay_Settled = 0;        
            
            //Turn Relay off                       
            Relay1= off;                        

            //Wait for the relay to Settle                     
            wait(0.5);                              
            
            //Acquire lock
            samplelock.lock();
            
            //Reset ADC (sampling) arrays
            i=0;
            for ( int g=0 ; g<N ; ++g)
            {
                Vin0[g]=0;                              //Reset ADC(A0) Measurement Array
                Vin1[g]=0;                              //Reset ADC(A1) Measurement Array
                Vin2[g]=0;                              //Reset ADC(A2) Measurement Array
            }//end for
            Vin0_sum=0;                                 //Reset ADC(A0) Sum
            Vin1_sum=0;                                 //Reset ADC(A0) Sum
            Vin2_sum=0;                                 //Reset ADC(A0) Sum

            //Release lock
            samplelock.unlock(); 
            
           /****************************************************************/ 
           //Test Point 2              

            //Turn Relay On 
            Relay2= on;
            
            //Relay and Circuit Settle time
            wait(1.0);                      
            
            //Set flag
            Relay_Settled = 1;        
            
            //wait for signal
            Thread::signal_wait(SampleSignal_SwitchingM);
            
            //Reset flag
            Relay_Settled = 0;        
            
            //Turn Relay off                        
            Relay2= off;                        
            
            //Wait for the relay to Settle                        
            wait(0.5); 
            
            //Acquire lock
            samplelock.lock();
            
            //Reset ADC (sampling) arrays
            i=0;
            for ( int g=0 ; g<N ; ++g)
            {
                Vin0[g]=0;                              //Reset ADC(A0) Measurement Array
                Vin1[g]=0;                              //Reset ADC(A1) Measurement Array
                Vin2[g]=0;                              //Reset ADC(A2) Measurement Array
                }//end for
            Vin0_sum=0;                                 //Reset ADC(A0) Sum
            Vin1_sum=0;                                 //Reset ADC(A0) Sum
            Vin2_sum=0;                                 //Reset ADC(A0) Sum
            
            //Release lock
            samplelock.unlock();
            
            /****************************************************************/
            //Test Point 3
            
            //Turn Relay On               
            Relay3= on;
            
            //Relay and Circuit Settle time
            wait(1.0);                      
            
            //Set flag
            Relay_Settled = 1;        
            
            //wait for signal
            Thread::signal_wait(SampleSignal_SwitchingM);
            
            //Re-set flag
            Relay_Settled = 0;        
            
            //Turn Relay off                        
            Relay3= off;                        
            
            //Wait for the relay to turn off                        
            wait(0.5); 
            
            //Acquire lock
            samplelock.lock();
            
            //Reset ADC (sampling) arrays
            i=0;
            for ( int g=0 ; g<N ; ++g)
            {
                Vin0[g]=0;                              //Reset ADC(A0) Measurement Array
                Vin1[g]=0;                              //Reset ADC(A1) Measurement Array
                Vin2[g]=0;                              //Reset ADC(A2) Measurement Array
                }//end for
            Vin0_sum=0;                                 //Reset ADC(A0) Sum
            Vin1_sum=0;                                 //Reset ADC(A0) Sum
            Vin2_sum=0;                                 //Reset ADC(A0) Sum
            
             //Release lock
            samplelock.unlock();

            /****************************************************************/
            //Test Point 4
            
            //Turn Relay On                 
            Relay4= on;
            
            //Relay and Circuit Settle time
            wait(1.0);                      
            
            //Set flag to start sampling
            Relay_Settled = 1;        
            
            //wait for signal from the sample function
            Thread::signal_wait(SampleSignal_SwitchingM);
            
            //Re-set the flag
            Relay_Settled = 0;        //Reset flag
            
            //Turn relay off                       
            Relay4= off;                        
            
            //Wait for the relay to Settle                       
            wait(0.5); 
            
            //Acquire lock
            samplelock.lock();
            
            //Reset ADC (sampling) arrays
            i=0;
            for ( int g=0 ; g<N ; ++g)
            {
                Vin0[g]=0;                              //Reset ADC(A0) Measurement Array
                Vin1[g]=0;                              //Reset ADC(A1) Measurement Array
                Vin2[g]=0;                              //Reset ADC(A2) Measurement Array
            }//end for
            Vin0_sum=0;                                 //Reset ADC(A0) Sum
            Vin1_sum=0;                                 //Reset ADC(A0) Sum
            Vin2_sum=0;                                 //Reset ADC(A0) Sum                     
            
            //Release lock
            samplelock.unlock();
            
            //Reset Bespoke Flag
            if (Bespokecontinuityflag == 1)     Bespoke_Measure=0;
                
        }//end while
    }//end while
}//end function
