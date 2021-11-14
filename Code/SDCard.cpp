//************************************************************************
// Integrated Autonomous Tester - PROJ325 - SDCard.cpp
// 
//  Store Test results onto the SD Card.
//  All information are logged to a FAT format file
//
//************************************************************************

#include "SDCard.hpp"

//SD Card Initialization
SDBlockDevice sd(PB_5, D12, D13, D10);      //mosi, miso, sclk, cs        
FATFileSystem* fs;           
FILE* fp;                            

bool SDCard_Ready;

//Thread
Thread SDCard_thread;
EventQueue SDCardqueue(512*EVENTS_EVENT_SIZE);

//************************************************************************
// SDCard(void) 
// Responsible for Initializing SD Card and opening a FAT format file 
// to write the Test results.
// Parameters - void
// Return - void
//************************************************************************
void SDCard(void)
{
    //Initialize SD Card
    if ( sd.init() != 0) 
    {
        //Initialization failed
        puts("WARNING:SD CARD INITIALISATION FAILED\r");
        SDCard_Ready = false;     
    }
    else
    {
        //Initialization succeed
        //Acquire lock
        dateLock.lock();
        
        seconds = time(NULL);
        timeData = localtime(&seconds);              
        
        //Release Lock
        dateLock.unlock();
        
        puts("SD CARD INITIALISED\r");
    
        //Create a new filing system for SD Card     
        fs = new FATFileSystem("sd", &sd);
    
        //Open File to append
        FILE* fp = fopen("/sd/IAT_Test_Results.csv","a");
        
        if (fp == NULL)         error("WARNING: COULD NOT OPEN FILE FOR WRITE\r\n");
        else
        {
            puts("FILE OPEN FOR WRITING\r\n");
            SDCard_Ready = true;
        }
        
        //Close File
        fclose(fp);
    }
}//end SDCard

//*************************************************************************************
// SDCard_GetData(void)
// Gets the time at which the test was preformed
// Adds the test results on the SDqueue to the SD card
// Parameters - void
// Return - void
//*************************************************************************************
void SDCard_GetData(void)
{
    if(SDCard_Ready)
    {
        //Acquire lock
        dateLock.lock();
        
        //Determine the current date and time
        seconds = time(NULL);
        timeData = localtime(&seconds);              

        //Release Lock
        dateLock.unlock();
        
        //open the file
        fp= fopen("/sd/IAT_Test_Results.csv","a");
        
        
        if (fp == NULL)
        {
            //SD Card Initialization failed
            printf("WARNING: FILE COULD NOT BE OPENED\r\n");
            SDCard_Ready = false;        
        }
        else
        {   
            //SD Card Initialization succeed                         
            if (lowrangeflag && startflag && TP_Counter != 0)              
            {
                //Low Range Test  
                //Acquire Lock
                samplelock.lock();
                  
                if( TP_Counter == 1 )
                {
                    fprintf(fp,"%s %s\r", ctime(&seconds), "Low Range Test");
                    fprintf(fp,"%s,%s,%s,%s\r", "Test Point ", "Resistance(ohm)","Voltage(V)","Current(A)");
                }// end if 
        
                if(voltage_lowrange > 0.150f && voltage_lowrange < 0.280f)     fprintf(fp,"%d,%s\r", TP_Counter, "Out Of Range");   
                else if (voltage_lowrange  >= 0.280f)                          fprintf(fp,"%d,%s\r", TP_Counter, "Open Circuit");                
                else if (voltage_lowrange < ShortC_Condition)                  fprintf(fp,"%d,%s\r", TP_Counter, "Short Circuit");
                else                                                           fprintf(fp,"%d,%5.3f,%5.3f,%5.3f\r", TP_Counter, resistance_lowrange,voltage_lowrange,current);

                //Release Lock
                samplelock.unlock();
                
            }//end Low Range Test
         
        else if (highrangeflag && startflag  && TP_Counter != 0)
        {
            //High Range Test
            
            //Acquire Lock
            samplelock.lock();
            
            if( TP_Counter == 1 )
            {
                fprintf(fp,"%s %s\r", ctime(&seconds), "High Range Test");
                fprintf(fp,"%s,%s,%s,%s\r", "Test Point ", "Resistance(ohm)","Voltage(V)","Current(A)");
            }
        
            if (voltage_highrange  > 2.3f && voltage_highrange < 3.0f)      fprintf(fp,"%d,%s\r", TP_Counter, "Out Of Range");
            else if (voltage_highrange  >= OpenC_Condition)                 fprintf(fp,"%d,%s\r", TP_Counter, "Open Circuit");
            else if (voltage_lowrange < ShortC_Condition)                   fprintf(fp,"%d,%s\r", TP_Counter, "Short Circuit");                   
            else                                                            fprintf(fp,"%d,%5.3f,%5.3f,%5.3f\r", TP_Counter, resistance_highrange,voltage_highrange,current); 

            //Release Lock
            samplelock.unlock();
                             
        }//end High Range Test
        
        else if (continuityflag && startflag&& TP_Counter != 0)
        {
            //Continuity Test
            
            //Acquire Lock
            samplelock.lock();
             
            if( TP_Counter == 1 )
            {
                fprintf(fp,"%s %s\r", ctime(&seconds), "Continuity Test");
                fprintf(fp,"%s,%s,%s,%s\r", "Test Point ", "Resistance(ohm)","Voltage(V)","Current(A)");
            }                
            
            if (voltage_highrange  >= OpenC_Condition)            fprintf(fp,"%d,%s\r", TP_Counter, "Open Circuit");
            else if (voltage_lowrange < ShortC_Condition)         fprintf(fp,"%d,%s\r", TP_Counter, "Pass");
            else                                                  fprintf(fp,"%d,%s\r", TP_Counter, "Fail");
            
            //Release Lock
            samplelock.unlock();
            
        }//End Continuity Test
        
        else if (Bespokecontinuityflag && startflag && TP_Counter != 0 && Bespoke_Measure==0)
        {
            //Bespoke Continuity Test
                    
            //Acquire Lock
            samplelock.lock();
                
            if( TP_Counter == 1 )
            {
                fprintf(fp,"%s %s\r", ctime(&seconds), "Bespoke Continuity Test");
                fprintf(fp,"%s,%s,%s,%s\r", "Test Point ", "Resistance(ohm)","Voltage(V)","Current(A)");
            }  
            
            if ( (resistance_lowrange < (TP_Resistance[TP_Counter-1]+0.5f) && resistance_lowrange > (TP_Resistance[TP_Counter-1]-0.5f)) || (resistance_highrange < (TP_Resistance[TP_Counter-1]+0.5f) && resistance_highrange > (TP_Resistance[TP_Counter-1]-0.5f)))
            {
                fprintf(fp,"%d,%s\r", TP_Counter, "Pass");
            }
            else if (resistance_highrange > 25)             fprintf(fp,"%d,%s\r", TP_Counter, "Open Circuit");
            else                                            fprintf(fp,"%d,%s\r", TP_Counter, "Fail");  
            
            //Release Lock
            samplelock.unlock();
            
        }//end Bespoke Continuity Test
        
        //Close File
        fclose(fp);
        
        }//end if fp
    }//end SDCard_Ready
}//end SDCard_GetData

//*************************************************************************************
// SwitchHandler(void)
// Responsible for calling the SDCard queue
// Parameters - void
// Return - void
//*************************************************************************************
void SwitchHandler(void)
{
    SDCardqueue.call(SDCard_Mount);
}

//****************************************************************************
// SDCard_Mount(void) 
// Function is called when the SDCard_sw is pressed
// The function either mount or unmount the SD Card
// Informs the user whether the SD Card is initialised successfully or not
// Parameters - void
// Return - void
//****************************************************************************
void SDCard_Mount(void)
{    
    if (SDCard_Ready)
    {
        //If the SD card is currently initialised, uninitialise it
        fclose(fp);
        sd.deinit();
        SDCard_Ready = false;
                    
        //Inform the user
        printf("SD CARD UNMOUNTED\r\n");
        LCDqueue.call(SDCard_LCD,"SD UNMOUNTED..");             
    }
    else
    {
        //initialise SDCard
        SDCard();
        
        //Inform the user
        if (SDCard_Ready)
        {
            printf("SD CARD MOUNTED\r\n");
            LCDqueue.call(SDCard_LCD,"SD MOUNTED..");
        }
        else   LCDqueue.call(SDCard_LCD,"SD FAILED..");         
    }//end if SDCard_Ready
}//end SDCard_Mount
