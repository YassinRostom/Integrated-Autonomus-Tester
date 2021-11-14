//************************************************************************/
// Integrated Autonomous Tester - PROJ325 - Switches.cpp
// 
// Debouncing and handling the SDCard Mount switch 
// 
//************************************************************************/

#include "switches.h"

//Switches initialization
InterruptIn SDCard_sw(PC_6);
Debounce SDCard_switch(SDCard_sw, &SwitchHandler);        

//************************************************************************
// Debounce::Debounce(void) 
// Debounce class constructor. Initialises class variables
// Parameters - InterruptIn& reference to an InterruptIn, void(*f)() pointer
//                          to a function that is called when the switch is 
//                          pressed
// Return - void
//************************************************************************
Debounce::Debounce(InterruptIn& gpioIn, void(*f)()) : sw(gpioIn), func(f)
{
    this->SwitchState = RISE_EDGE;
    this->sw.rise(callback(this, &Debounce::swRisingEdge));
}

//************************************************************************
// Debounce::swTimeOutHandler(void) 
// Member function for handling switch presses and releases timing
// Parameters - void
// Return - void
//************************************************************************
void Debounce::Switch_TimeOutHandler(void)
{
    this->SwitchTimeOut.detach();
    switch (this->SwitchState)
    {
        case RISE_EDGE:
            this->sw.fall(callback(this, &Debounce::swFallingEdge));
            break;
        case FALL_EDGE:
            this->sw.rise(callback(this, &Debounce::swRisingEdge));
            break;
    }
}

//************************************************************************
// Debounce::swRisingEdge(void) 
// Changes the state to rise and debounce the switch
// Parameters - void
// Return - void
//************************************************************************

void Debounce::swRisingEdge(void)
{
    this->sw.fall(NULL);
    this->SwitchState = RISE_EDGE;
    this->SwitchTimeOut.attach(callback(this, &Debounce::Switch_TimeOutHandler),0.05);
}

//************************************************************************
// Switch::swFallingEdge(void) 
// Changes the state to fall and debounce the switch
// Calls the function pointer to by func
// Parameters - void
// Return - void
//************************************************************************
void Debounce::swFallingEdge(void)
{
    this->sw.fall(NULL);
    this->SwitchState = FALL_EDGE;
    this->SwitchTimeOut.attach(callback(this, &Debounce::Switch_TimeOutHandler), 0.05);
    (*func)();
}
