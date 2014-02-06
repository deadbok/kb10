#include "midi.h"
#include <p18cxxx.h>
#include <usart.h>
#include "USB/usb_function_midi.h"
#include "userio.h"

//Recieve buffer
unsigned char   RX_buffer_write_pos;
unsigned char   RX_buffer_read_pos;
unsigned char   RX_buffer[32];

//Read a byte from the uart
unsigned char MIDI_get_byte(void)
{
    unsigned char data = 0;

    while(RX_buffer_write_pos == 0)
    {}

    data = RX_buffer[RX_buffer_read_pos];
    //Advance read position
    RX_buffer_read_pos++;

    //If we have emptied the buffer, reset the index
    if (RX_buffer_read_pos == RX_buffer_write_pos)
    {
        RX_buffer_read_pos = 0;
        RX_buffer_write_pos = 0;
    }

    return(data);
}

//Init the UART for use with the MIDI-protocol
void MIDI_init(void)
{
    RX_buffer_read_pos = 0;
    RX_buffer_write_pos = 0;
    //Serial RX/MIDI pin is input
    TRISCbits.RC7 = 1;
    /* spbrg=fosc/(64*baud)-1=9 for 20MHz crystal, 31.25kbaud MIDI */
    //SPBRG = 9;
    //Clock may be 48MHz from the PLL
    //SPBRG = 23;
    SPBRG = 95;
    SPBRGH = 0;
    BAUDCONbits.BRG16 = 1;

    TXSTA = 0;           // Reset USART registers to POR state
    RCSTA = 0;
        
    //inverted signal on serial port
    BAUDCONbits.RXDTP = 1;
    //non inverted signal
    //BAUDCONbits.RXDTP = 0;


    //Enable recieve interrupt
    PIE1bits.RCIE = 1;
    //High priority
    IPR1bits.RCIP = 1;

    RCSTAbits.CREN = 1;
    RCSTAbits.SPEN = 1;
}

void MIDI_active_sense(void)
{
    USB_AUDIO_MIDI_EVENT_PACKET MIDI_packet;

    MIDI_packet.CableNumber = 0;
    MIDI_packet.CodeIndexNumber = MIDI_CIN_1_BYTE_MESSAGE;

    MIDI_packet.MIDI_0 = 0xFE; //Active sense
    MIDI_packet.MIDI_1 = 0;
    MIDI_packet.MIDI_2 = 0;

    USB_send_MIDI(MIDI_packet);
}

void MIDI_process_input(void)
{
    unsigned char               data = 0;
    USB_AUDIO_MIDI_EVENT_PACKET MIDI_packet;

    //If there is data
    if (RX_buffer_write_pos > 0)
    {
        //Get the first byte of the message
        data = MIDI_get_byte();

        //Ignore Active sense
        //if (data == 0b11111110)
        //{
        //    return;
        //}

        MIDI_packet.CableNumber = 0;
        MIDI_packet.CodeIndexNumber = MIDI_CIN_SINGLE_BYTE;

        MIDI_packet.MIDI_0 = data;
        MIDI_packet.MIDI_1 = 0;
        MIDI_packet.MIDI_2 = 0;

        USB_send_MIDI(MIDI_packet);
    }
}

void MIDI_task(void)
{
    //Buffer serial data
    if (PIR1bits.RCIF)
    {
        RXLED = 1;

        //Get the data if there is no overflow
        if (RX_buffer_write_pos < 32)
        {
            RX_buffer[RX_buffer_write_pos] = RCREG;
            RX_buffer_write_pos++;
        }

        //If there is an error condition
        if (RCSTAbits.FERR || RCSTAbits.OERR)
        {
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
        PIR1bits.RCIF = 0;
    }
}