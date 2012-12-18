// $Id$
/*
 * DIN access functions for MIDIbox NG
 *
 * ==========================================================================
 *
 *  Copyright (C) 2012 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _MBNG_DIN_H
#define _MBNG_DIN_H

#include "mbng_event.h"

/////////////////////////////////////////////////////////////////////////////
// global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Type definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 MBNG_DIN_Init(u32 mode);
extern s32 MBNG_DIN_NotifyToggle(u32 pin, u32 pin_value);
extern s32 MBNG_DIN_NotifyReceivedValue(mbng_event_item_t *item, u16 value);
extern s32 MBNG_DIN_GetCurrentValueFromId(mbng_event_item_id_t id);

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////


#endif /* _MBNG_DIN_H */