#include "usb.h"
#include "midi.h"
#include "controller.h"
/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:
 *
 *****************************************************************************/
void UserInit(void)
{
    //initialize the variable holding the handle for the last
    // transmission
    USBTxHandle = NULL;
    USBRxHandle = NULL;

    //Set status led pins as output
    TRISDbits.RD0 = 0;
    TRISDbits.RD1 = 0;

    //Initialize midi in port
    MIDI_init();
    //Initialize the knobs
    ctrl_init();
}//end UserInit
