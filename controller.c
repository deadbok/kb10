#include "controller.h"
#include <p18cxxx.h>
#include <delays.h>
#include "USB/usb_function_midi.h"
#include "userio.h"

unsigned char   ctrl_vals[10];
unsigned char   ctrl_old_vals[10];

void ctrl_init(void)
{
    unsigned char   i;

    //Set all pins as inputs
    //AN0-AN4
    TRISA = 0b00101111;
    //AN5-AN7
    TRISE = 0b00000111;
    //AN7-AN9
    TRISB = 0b00001100;
    //Init the ADC for 10 channels of input
    //Negative reference from ground
    ADCON1bits.VCFG0 = 0;
    //Positive reference to 5V+ supply
    ADCON1bits.VCFG1 = 0;
    //AN0-AN9 are analog inputs
    ADCON1bits.PCFG = 0b0101;
    
    //ADC conversion clock
    ADCON2bits.ADCS =0b110;
    //Left adjusted
    ADCON2bits.ADFM = 0;
    //2 TAD
    ADCON2bits.ACQT = 0b001;

    //Enable ADC
    ADCON0bits.ADON = 1;

    //Zero out all controller values
    for (i = 0; i < 10; i++)
    {
        ctrl_old_vals[i] = 0;
    }
}

void ctrl_sample(unsigned char input)
{
    //Set input
    ADCON0bits.CHS = input;

    //Wait
    Delay10TCYx(150);

    //Start ADC
    ADCON0bits.GO = 1;

    //Wait for conversion
    while (ADCON0bits.NOT_DONE)
    {
    }

    //Convert to 7 bits and save
    ctrl_vals[input] = ADRESH >> 1;
}

int abs(int val)
 {      if (val < 0)
                return(-val);
        else
                return(val);
 }

void ctrl_process(unsigned char ctrl_nr)
{
    unsigned char               i;
    int                tmp;
    USB_AUDIO_MIDI_EVENT_PACKET MIDI_packet;

    ctrl_sample(ctrl_nr);

    //if the value has changed, send it
    tmp = abs((int)ctrl_old_vals[ctrl_nr] - (int)ctrl_vals[ctrl_nr]);
    if (tmp > 1)
    {
        //Prepare the MIDI packet
        MIDI_packet.CableNumber = 0;
        MIDI_packet.CodeIndexNumber = MIDI_CIN_CONTROL_CHANGE;

        //Control change on channel 2
        MIDI_packet.DATA_0 = 0b10110010;
        //Set the control number
        MIDI_packet.DATA_1 = ctrl_nr;
        //Set the value
        MIDI_packet.DATA_2 = ctrl_vals[ctrl_nr];

        //Save the new value
        ctrl_old_vals[ctrl_nr] = ctrl_vals[ctrl_nr];

        //Send the MIDI package
        USB_send_MIDI(MIDI_packet);
    }
}