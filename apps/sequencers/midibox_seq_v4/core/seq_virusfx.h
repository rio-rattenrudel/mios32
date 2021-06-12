// $Id$
/*
 * Header file for virusfx routines
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */
//####################################
//# RIO: VIRUSFX
//####################################
#ifndef _SEQ_VIRUSFX_H
#define _SEQ_VIRUSFX_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

typedef union {
  u8 ALL;
  struct {
    u8 MIDI_PORT;
    u8 MIDI_CH;
    u8 MODE;
    u8 SHAPE_DAMPING;
    u8 CLOCK;
    u8 COLOR;
    u8 FEEDBACK;
    u8 DEPTH_TYPE;
    u8 RATE_DECTIME;
    u8 TIME_PREDLY;
  };
} seq_virusfx_last_values_t;

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 SEQ_VIRUSFX_Init(u32 mode);

extern void SEQ_VIRUSFX_Reset(u8 track);
extern s32 SEQ_VIRUSFX_Send(u8 track);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

#endif /* _SEQ_VIRUSFX_H */
//####################################
//# RIO: END MODIFICATION
//####################################