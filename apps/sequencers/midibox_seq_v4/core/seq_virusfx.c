// $Id$
/*
 * VirusFX Functions
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
/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>
#include "tasks.h"

#include "seq_virusfx.h"
#include "seq_midi_in.h"
#include "seq_midi_router.h"
#include "seq_core.h"
#include "seq_cc.h"


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_VIRUSFX_Init(u32 mode)
{
  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Sends a CC or PP value, considers Bus target
/////////////////////////////////////////////////////////////////////////////
static s32 SEQ_VIRUSFX_SendCCPP(mios32_midi_port_t midi_port, mios32_midi_chn_t midi_chn, u8 ccpp, u8 value, u8 isPP)
{
  mios32_midi_package_t midi_package;

  midi_package.type  = isPP ? 0xa : 0xb;
  midi_package.evnt0 = isPP ? 0xa0 | midi_chn : 0xb0 | midi_chn;
  midi_package.evnt1 = ccpp;
  midi_package.evnt2 = value;

  if( (midi_port & 0xf0) == 0xf0 ) { // send to bus?
    // forward to router
    SEQ_MIDI_ROUTER_Receive(midi_port, midi_package);

    // forward to transposer/arpeggiator/CC parser/etc...
    SEQ_MIDI_IN_Receive(midi_port, midi_package);

    return 0; // no error
  }

  return MIOS32_MIDI_SendPackage(midi_port, midi_package);
}

void SEQ_VIRUSFX_Reset(u8 track)
{
  seq_cc_trk_t *tcc = &seq_cc_trk[track];

  // init undefined
  tcc->virusfx_last_values.MODE = 128;
  tcc->virusfx_last_values.SHAPE_DAMPING = 128;
  tcc->virusfx_last_values.CLOCK = 128;
  tcc->virusfx_last_values.COLOR = 128;
  tcc->virusfx_last_values.FEEDBACK = 128;
  tcc->virusfx_last_values.DEPTH_TYPE = 128;
  tcc->virusfx_last_values.RATE_DECTIME = 128;
  tcc->virusfx_last_values.TIME_PREDLY = 128;
}

/////////////////////////////////////////////////////////////////////////////
// sends MIDI data depending on selected virusfx settings
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_VIRUSFX_Send(u8 track)
{
  seq_cc_trk_t *tcc = &seq_cc_trk[track];

  if (!tcc->virusfx_active) return 0; // nothing to do

  mios32_midi_port_t midi_port  = SEQ_CC_Get(track, SEQ_CC_MIDI_PORT);
  u8 midi_chn                   = SEQ_CC_Get(track, SEQ_CC_MIDI_CHANNEL);

  if (tcc->virusfx_last_values.MIDI_PORT != midi_port) {
    tcc->virusfx_last_values.MIDI_PORT = midi_port;
    SEQ_VIRUSFX_Reset(track);
  }

  if (tcc->virusfx_last_values.MIDI_CH != midi_chn) {
    tcc->virusfx_last_values.MIDI_CH = midi_chn;
    SEQ_VIRUSFX_Reset(track);
  }

  MUTEX_MIDIOUT_TAKE;

  if (tcc->virusfx_last_values.MODE != tcc->virusfx_mode) {
    tcc->virusfx_last_values.MODE = tcc->virusfx_mode;
    SEQ_VIRUSFX_SendCCPP(midi_port, midi_chn, 0x70, tcc->virusfx_mode, 0);
  }

  if (tcc->virusfx_last_values.SHAPE_DAMPING != tcc->virusfx_shape_damping) {
    tcc->virusfx_last_values.SHAPE_DAMPING = tcc->virusfx_shape_damping;
    SEQ_VIRUSFX_SendCCPP(midi_port, midi_chn, 0x76, tcc->virusfx_shape_damping, 0);
  }

  if (tcc->virusfx_last_values.CLOCK != tcc->virusfx_clock) {
    tcc->virusfx_last_values.CLOCK = tcc->virusfx_clock;
    SEQ_VIRUSFX_SendCCPP(midi_port, midi_chn, 0x14, tcc->virusfx_clock, 1);
  }

  if (tcc->virusfx_last_values.COLOR != tcc->virusfx_color) {
    tcc->virusfx_last_values.COLOR = tcc->virusfx_color;
    SEQ_VIRUSFX_SendCCPP(midi_port, midi_chn, 0x77, tcc->virusfx_color, 0);
  }

  if (tcc->virusfx_last_values.FEEDBACK != tcc->virusfx_feedback) {
    tcc->virusfx_last_values.FEEDBACK = tcc->virusfx_feedback;
    SEQ_VIRUSFX_SendCCPP(midi_port, midi_chn, 0x73, tcc->virusfx_feedback, 0);
  }

  if (tcc->virusfx_last_values.DEPTH_TYPE != tcc->virusfx_depth_type) {
    tcc->virusfx_last_values.DEPTH_TYPE = tcc->virusfx_depth_type;
    SEQ_VIRUSFX_SendCCPP(midi_port, midi_chn, 0x75, tcc->virusfx_depth_type, 0);
  }

  if (tcc->virusfx_last_values.RATE_DECTIME != tcc->virusfx_rate_dectime) {
    tcc->virusfx_last_values.RATE_DECTIME = tcc->virusfx_rate_dectime;
    SEQ_VIRUSFX_SendCCPP(midi_port, midi_chn, 0x74, tcc->virusfx_rate_dectime, 0);
  }

  if (tcc->virusfx_last_values.TIME_PREDLY != tcc->virusfx_time_predly) {
    tcc->virusfx_last_values.TIME_PREDLY = tcc->virusfx_time_predly;
    SEQ_VIRUSFX_SendCCPP(midi_port, midi_chn, 0x72, tcc->virusfx_time_predly, 0);
  }

  MUTEX_MIDIOUT_GIVE;

  //DEBUG_MSG("track: %d", track);

  return 0; // no error
}
//####################################
//# RIO: END MODIFICATION
//####################################