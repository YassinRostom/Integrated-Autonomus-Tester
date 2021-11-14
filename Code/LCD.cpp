//************************************************************************/
// Integrated Autonomous Tester - PROJ325 - LCD.cpp
// 
//  Responsible for displaying and updating the testing system status
//
//************************************************************************/

#include "LCD.hpp"

//LCD
//LCD Driver (provided via mbed repository)
//RS D9
//E  D8
//D7,6,4,2 are the 4 bit for d4-7
TextLCD lcd(D9, D8, D7, D6, D4, D2); // rs, e, d4-d7 

//Thread
Thread LCD_thread;
EventQueue LCDqueue(32*EVENTS_EVENT_SIZE);
Mutex LCDLock;

//************************************************************************
// LCD() 
// Displays and updates the testing system status
// Parameters - void
// Return - void
//************************************************************************
void LCD()
{
    //lock the resource 
    LCDLock.lock();

    //Clear screen
    lcd.cls();
    
    //Testing system status
    if(lowrangeflag && startflag)                lcd.printf("Low Range Test");
    else if(highrangeflag && startflag)          lcd.printf("High Range Test");
    else if(continuityflag && startflag)         lcd.printf("Continuity Test");
    else if(Bespokecontinuityflag && startflag)  lcd.printf("Bespoke Test");
    else if (stoptestflag)                       lcd.printf("Test Stopped");          
    else                                         lcd.printf("Setting up Test");    
   
    //Release Lock    
    LCDLock.unlock();
}

//************************************************************************
// SDCard_LCD(string display)
// Displays SDCard status to the LCD screen
// string display - text to display to the LCD screen
// Return - void
//************************************************************************

void SDCard_LCD(string display)           
{
    //Acquire Lock
    LCDLock.lock();
    
    lcd.cls();
    lcd.printf("%s",display);       

    //Release Lock
    LCDLock.unlock();
}
