// $Id$
/*
 * Header file for LFO routines
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _SEQ_LFO_H
#define _SEQ_LFO_H

#include "seq_layer.h"

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

//#################################################
//# RIO: Added new waveforms
//#################################################

typedef enum {
  SEQ_LFO_WAVEFORM_Off,
  SEQ_LFO_WAVEFORM_Sine,
  SEQ_LFO_WAVEFORM_Triangle,
  SEQ_LFO_WAVEFORM_Saw,
  SEQ_LFO_WAVEFORM_Rec05,
  SEQ_LFO_WAVEFORM_Rec10,
  SEQ_LFO_WAVEFORM_Rec15,
  SEQ_LFO_WAVEFORM_Rec20,
  SEQ_LFO_WAVEFORM_Rec25,
  SEQ_LFO_WAVEFORM_Rec30,
  SEQ_LFO_WAVEFORM_Rec35,
  SEQ_LFO_WAVEFORM_Rec40,
  SEQ_LFO_WAVEFORM_Rec45,
  SEQ_LFO_WAVEFORM_Rec50,
  SEQ_LFO_WAVEFORM_Rec55,
  SEQ_LFO_WAVEFORM_Rec60,
  SEQ_LFO_WAVEFORM_Rec65,
  SEQ_LFO_WAVEFORM_Rec70,
  SEQ_LFO_WAVEFORM_Rec75,
  SEQ_LFO_WAVEFORM_Rec80,
  SEQ_LFO_WAVEFORM_Rec85,
  SEQ_LFO_WAVEFORM_Rec90,
  SEQ_LFO_WAVEFORM_Rec95,
  // RIO: added new waveforms
  SEQ_LFO_WAVEFORM_Saw2,
  SEQ_LFO_WAVEFORM_Saw3,
  SEQ_LFO_WAVEFORM_Saw4,
  SEQ_LFO_WAVEFORM_Saw5,
  SEQ_LFO_WAVEFORM_Saw6,
  SEQ_LFO_WAVEFORM_Saw7,
  SEQ_LFO_WAVEFORM_Saw8,
  SEQ_LFO_WAVEFORM_Saw9,
  SEQ_LFO_WAVEFORM_Saw10,
  SEQ_LFO_WAVEFORM_Saw11,
  SEQ_LFO_WAVEFORM_Saw12,
  SEQ_LFO_WAVEFORM_Saw13,
  SEQ_LFO_WAVEFORM_Saw14,
  SEQ_LFO_WAVEFORM_Saw15,
  SEQ_LFO_WAVEFORM_Saw16,
  SEQ_LFO_WAVEFORM_Saw17,
  SEQ_LFO_WAVEFORM_Saw18,
  SEQ_LFO_WAVEFORM_Saw19,
  SEQ_LFO_WAVEFORM_Saw20,

  SEQ_LFO_WAVEFORM___V2,
  SEQ_LFO_WAVEFORM___H2,
  SEQ_LFO_WAVEFORM___D2,
  SEQ_LFO_WAVEFORM___A2,
  SEQ_LFO_WAVEFORM_V2__,
  SEQ_LFO_WAVEFORM_H2__,
  SEQ_LFO_WAVEFORM_D2__,
  SEQ_LFO_WAVEFORM_A2__,
  SEQ_LFO_WAVEFORM___V4,
  SEQ_LFO_WAVEFORM___H4,
  SEQ_LFO_WAVEFORM___D4,
  SEQ_LFO_WAVEFORM___A4,
  SEQ_LFO_WAVEFORM_V4__,
  SEQ_LFO_WAVEFORM_H4__,
  SEQ_LFO_WAVEFORM_D4__,
  SEQ_LFO_WAVEFORM_A4__,

  SEQ_LFO_WAVEFORM_V2V2,
  SEQ_LFO_WAVEFORM_V2H2,
  SEQ_LFO_WAVEFORM_V2D2,
  SEQ_LFO_WAVEFORM_V2A2,
  SEQ_LFO_WAVEFORM_H2V2,
  SEQ_LFO_WAVEFORM_H2H2,
  SEQ_LFO_WAVEFORM_H2D2,
  SEQ_LFO_WAVEFORM_H2A2,
  SEQ_LFO_WAVEFORM_D2V2,
  SEQ_LFO_WAVEFORM_D2H2,
  SEQ_LFO_WAVEFORM_D2D2,
  SEQ_LFO_WAVEFORM_D2A2,
  SEQ_LFO_WAVEFORM_A2V2,
  SEQ_LFO_WAVEFORM_A2H2,
  SEQ_LFO_WAVEFORM_A2D2,
  SEQ_LFO_WAVEFORM_A2A2,

  SEQ_LFO_WAVEFORM_V4V2,
  SEQ_LFO_WAVEFORM_V4H2,
  SEQ_LFO_WAVEFORM_V4D2,
  SEQ_LFO_WAVEFORM_V4A2,
  SEQ_LFO_WAVEFORM_H4V2,
  SEQ_LFO_WAVEFORM_H4H2,
  SEQ_LFO_WAVEFORM_H4D2,
  SEQ_LFO_WAVEFORM_H4A2,
  SEQ_LFO_WAVEFORM_D4V2,
  SEQ_LFO_WAVEFORM_D4H2,
  SEQ_LFO_WAVEFORM_D4D2,
  SEQ_LFO_WAVEFORM_D4A2,
  SEQ_LFO_WAVEFORM_A4V2,
  SEQ_LFO_WAVEFORM_A4H2,
  SEQ_LFO_WAVEFORM_A4D2,
  SEQ_LFO_WAVEFORM_A4A2,

  SEQ_LFO_WAVEFORM_V2V4,
  SEQ_LFO_WAVEFORM_V2H4,
  SEQ_LFO_WAVEFORM_V2D4,
  SEQ_LFO_WAVEFORM_V2A4,
  SEQ_LFO_WAVEFORM_H2V4,
  SEQ_LFO_WAVEFORM_H2H4,
  SEQ_LFO_WAVEFORM_H2D4,
  SEQ_LFO_WAVEFORM_H2A4,
  SEQ_LFO_WAVEFORM_D2V4,
  SEQ_LFO_WAVEFORM_D2H4,
  SEQ_LFO_WAVEFORM_D2D4,
  SEQ_LFO_WAVEFORM_D2A4,
  SEQ_LFO_WAVEFORM_A2V4,
  SEQ_LFO_WAVEFORM_A2H4,
  SEQ_LFO_WAVEFORM_A2D4,
  SEQ_LFO_WAVEFORM_A2A4,

  SEQ_LFO_WAVEFORM_V4V4,
  SEQ_LFO_WAVEFORM_V4H4,
  SEQ_LFO_WAVEFORM_V4D4,
  SEQ_LFO_WAVEFORM_V4A4,
  SEQ_LFO_WAVEFORM_H4V4,
  SEQ_LFO_WAVEFORM_H4H4,
  SEQ_LFO_WAVEFORM_H4D4,
  SEQ_LFO_WAVEFORM_H4A4,
  SEQ_LFO_WAVEFORM_D4V4,
  SEQ_LFO_WAVEFORM_D4H4,
  SEQ_LFO_WAVEFORM_D4D4,
  SEQ_LFO_WAVEFORM_D4A4,
  SEQ_LFO_WAVEFORM_A4V4,
  SEQ_LFO_WAVEFORM_A4H4,
  SEQ_LFO_WAVEFORM_A4D4,
  SEQ_LFO_WAVEFORM_A4A4,

} seq_lfo_waveform_t;

//#################################################
//# RIO: END MODIFICATION
//#################################################

//#################################################
//# RIO: Added new commands
//#################################################

typedef union {
  u8 ALL;
  struct {
    u8 ONE_SHOT:1;
    u8 NOTE:1;
    u8 VELOCITY:1;
    u8 LENGTH:1;
    u8 CC:1;
    u8 REVERSE:1;   // RIO: added Reverse
    u8 FADEUP:1;    // RIO: added Fade OUT
    u8 FADEABS:1;   // RIO: added Fade OUT
  };
} seq_lfo_enable_flags_t;

//#################################################
//# RIO: END MODIFICATION
//#################################################

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 SEQ_LFO_Init(u32 mode);

extern s32 SEQ_LFO_ResetTrk(u8 track);
extern s32 SEQ_LFO_HandleTrk(u8 track, u32 bpm_tick);
extern s32 SEQ_LFO_Event(u8 track, seq_layer_evnt_t *e);
extern s32 SEQ_LFO_FastCC_Event(u8 track, u32 bpm_tick, mios32_midi_package_t *p, u8 ignore_waveform);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

#endif /* _SEQ_LFO_H */
