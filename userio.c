#include "userio.h"
#include <p18cxxx.h>
#include "midi.h"
#include "controller.h"
//#include <usart.h>

unsigned int   led_delay = 0;
unsigned char  ctrl_nr = 0;


void ProcessIO(void)
{
    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;

    //Delay to make the RX/TX blinking visible
    if (led_delay == 50)
    {
        TXLED = 0;
        RXLED = 0;
        led_delay = 0;
    }
    led_delay++;

    //Handle MIDI input from the MIDI port
    MIDI_process_input();

    //Handle knobs
    ctrl_process(ctrl_nr);
    //next knob
    ctrl_nr++;
    if (ctrl_nr > 0)
    {
        ctrl_nr = 0;
    }

    //If 300ms seconds have passed without a message, send active sense
    if (msCounter == 0)
    {
        MIDI_active_sense();
        //Reset the counter
        msCounter = 300;
    }
    
    if(!USBHandleBusy(USBRxHandle))
    {
        //We have received a MIDI packet from the host, process it and then
        //  prepare to receive the next packet

        //INSERT MIDI PROCESSING CODE HERE

        //Get ready for next packet (this will overwrite the old data)
        USBRxHandle = USBRxOnePacket(MIDI_EP,(BYTE*)&ReceivedDataBuffer,64);
    }
}//end ProcessIO

void USB_send_MIDI(USB_AUDIO_MIDI_EVENT_PACKET packet)
{
    unsigned char   i;

    TXLED = 1;

    for (i = 0; i < 4; i++)
    {
        midiData.v[i] = packet.v[i];
    }
    if(!USBHandleBusy(USBTxHandle))
    {
        USBTxHandle = USBTxOnePacket(MIDI_EP,(BYTE*)&midiData,4);

        //New message sent reset ms counter to 300 ms for active sense
        msCounter = 300;
    }
}