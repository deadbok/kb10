/* 
 * File:   processio.h
 * Author: oblivion
 *
 * Created on 28 April 2013, 14:08
 */

#ifndef USERIO_H
#define	USERIO_H

#include "usb.h"

#define TXLED   PORTDbits.RD0
#define RXLED   PORTDbits.RD1

extern void ProcessIO(void);
extern void USB_send_MIDI(USB_AUDIO_MIDI_EVENT_PACKET packet);

#endif	/* USERIO_H */

