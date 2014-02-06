/* 
 * File:   usb.h
 * Author: oblivion
 *
 * Created on 28 April 2013, 14:19
 */

#ifndef USB_H
#define	USB_H

#include "./USB/usb.h"
#include "./USB/usb_function_midi.h"

extern unsigned char ReceivedDataBuffer[64];
extern unsigned char ToSendDataBuffer[64];
extern USB_AUDIO_MIDI_EVENT_PACKET midiData;

extern USB_HANDLE USBTxHandle;
extern USB_HANDLE USBRxHandle;

//extern USB_VOLATILE BYTE msCounter;
extern USB_VOLATILE unsigned int msCounter;

#endif	/* USB_H */

