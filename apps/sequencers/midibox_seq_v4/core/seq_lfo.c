// $Id$
/*
 * LFO Functions
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>

#include "seq_lfo.h"
#include "seq_core.h"
#include "seq_cc.h"


/////////////////////////////////////////////////////////////////////////////
// Local types
/////////////////////////////////////////////////////////////////////////////

//###########################################################################
//# RIO: Added Spezial Behaviour LFO (rstcount for increment spirals, delay)
//###########################################################################

typedef struct {
  u16 step_ctr;
  u16 pos;
  u16 rstcount;     // RIO: added rstcount
  u8  delay;        // RIO: added delay
  u8  fadeoffset;   // RIO: added fadeoffset
  u8  lasttrkccval; // RIO: added lasttrkccval
} seq_lfo_t;

// globals


//###########################################################################
//# RIO: END MODIFICATION
//###########################################################################

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

static seq_lfo_t seq_lfo[SEQ_CORE_NUM_TRACKS];


/////////////////////////////////////////////////////////////////////////////
// Local Prototypes
/////////////////////////////////////////////////////////////////////////////
static s32 SEQ_LFO_ValueGet(seq_cc_trk_t *tcc, seq_lfo_t *lfo);


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_LFO_Init(u32 mode)
{
  u8 track;

  for(track=0; track<SEQ_CORE_NUM_TRACKS; ++track)
    SEQ_LFO_ResetTrk(track);

  return 0; // no error
}

//###########################################################################
//# RIO: Added Spezial Behaviour LFO (rstcount for increment spirals, delay)
//###########################################################################

/////////////////////////////////////////////////////////////////////////////
// Initializes the LFO of a given track
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_LFO_ResetTrk(u8 track)
{
  seq_lfo_t *lfo = &seq_lfo[track];

  lfo->step_ctr = 0;
  lfo->pos = 0;
  lfo->rstcount = 0;      // RIO: added rstcount
  lfo->delay = 0;         // RIO: added delay
  lfo->fadeoffset = 0;    // RIO: added fadeoffset
  lfo->lasttrkccval = 0;  // RIO: added lasttrkccval

  seq_cc_trk_t *tcc = &seq_cc_trk[track];
  if (tcc->lfo_phase > 100 && tcc->lfo_phase <= 200) lfo->delay = tcc->lfo_phase-100;

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Handles the LFO of a given track
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_LFO_HandleTrk(u8 track, u32 bpm_tick)
{
  seq_cc_trk_t *tcc = &seq_cc_trk[track];
  seq_lfo_t *lfo = &seq_lfo[track];

  // Note: we always have to process this part for each track, otherwise the LFO
  // could get out-of-sync if temporary disabled/enabled

  // increment step counter on each step
  if( (bpm_tick % 96) == 0 && lfo->step_ctr != 65535) {// @384 ppqn (reference bpm_tick resolution)

    // RIO: added delay
    if (lfo->delay) lfo->delay--; else ++lfo->step_ctr;
  }

  // RIO: added delay
  if (lfo->delay) return 0;

  // increment waveform position
  if( lfo->step_ctr > tcc->lfo_steps_rst) {
    if( tcc->lfo_enable_flags.ONE_SHOT ) {
      // oneshot mode: halt LFO counter
      lfo->step_ctr = 65535;

      //lfo->pos = 65535;                                                           // ORIGINAL
      if (tcc->lfo_phase <= 100) lfo->pos = tcc->lfo_phase * 655;                   // RIO: setup phase in oneshot
      else                       lfo->pos = 65535;

    } else {

      // reset step counter and LFO position
      lfo->step_ctr = 0;

      lfo->rstcount++;                                                              // RIO: increment rstcount
      //lfo->pos = tcc->lfo_phase * 655; // possible phase offset: 0%..99%          // ORIGINAL
      if (tcc->lfo_phase <= 100) lfo->pos = tcc->lfo_phase * 655 * lfo->rstcount;   // RIO: added rstcount
      else                       lfo->pos = 0;

    }

  } else {

    // increment waveform pointer
    u32 lfo_ticks = (u32)(tcc->lfo_steps+1) * 96; // @384 ppqn (reference bpm_tick resolution)
    u32 inc = 65536 / lfo_ticks;
    lfo->pos += inc;

  }

  return 0; // no error
}

//###########################################################################
//# RIO: END MODIFICATION
//###########################################################################



/////////////////////////////////////////////////////////////////////////////
// modifies a MIDI event depending on LFO settings
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_LFO_Event(u8 track, seq_layer_evnt_t *e)
{
  seq_cc_trk_t *tcc = &seq_cc_trk[track];

  if( !tcc->lfo_waveform )
    return 0; // LFO disabled

  seq_lfo_t *lfo = &seq_lfo[track];
  s32 lfo_value = SEQ_LFO_ValueGet(tcc, lfo);

  // apply
  if( e->midi_package.type == NoteOn ) {
    if( tcc->lfo_enable_flags.NOTE ) {
      s16 value = e->midi_package.note + lfo_value;

      // ensure that note is in the 0..127 range
      value = SEQ_CORE_TrimNote(value, 0, 127);

      e->midi_package.note = value;
    }

    if( tcc->lfo_enable_flags.VELOCITY && e->midi_package.velocity ) {
      s16 value = e->midi_package.velocity + lfo_value;
      if( value < 1 )
	value = 1;
      else if( value > 127 )
	value = 127;
      e->midi_package.velocity = value;
    }

    if( tcc->lfo_enable_flags.LENGTH ) {
      s16 value = e->len + lfo_value;
      if( value < 1 )
	value = 1;
      else if( value > 95 )
	value = 95;
      e->len = value;
    }
  } else if( e->midi_package.type == CC ) {
    if( tcc->lfo_enable_flags.CC ) {

      //##################################################################
      //# RIO: Use only the activated CC values ​​of the tracks for the LFO
      //##################################################################
      u8 mvalue = e->midi_package.value;
      if (mvalue < 128) lfo->lasttrkccval = mvalue;
      else mvalue = lfo->lasttrkccval;
      s16 value = mvalue + lfo_value;
      //##################################################################
      //# RIO: END MODIFICATION
      //##################################################################

      if( value < 0 )
	value = 0;
      else if( value > 127 )
	value = 127;
      e->midi_package.value = value;
    }
  }

  return 0; // no error
}



/////////////////////////////////////////////////////////////////////////////
// Returns a fast modulated CC event if return value >= 1
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_LFO_FastCC_Event(u8 track, u32 bpm_tick, mios32_midi_package_t *p, u8 ignore_waveform)
{
  seq_cc_trk_t *tcc = &seq_cc_trk[track];

  if( !ignore_waveform && !tcc->lfo_waveform )
    return 0; // LFO disabled

  if( !tcc->lfo_cc )
    return 0; // CC disabled

//#################################################
//# RIO: Remove EXTRA CC
//#################################################

//  if( tcc->lfo_enable_flags.EXTRA_CC_OFF )
//    return 0; // Extra CC disabled

//#################################################
//# RIO: END MODIFICATION
//#################################################

  if( tcc->lfo_cc_ppqn < 8 ) { // not 384 ppqn
    if( tcc->lfo_cc_ppqn == 0 ) { // 1 ppqn
      if( (bpm_tick % 384) != 0 )
	return 0; // skipped due to resolution
    } else {
      int resolution = (3 << (7-tcc->lfo_cc_ppqn));
      if( (bpm_tick % resolution) != 0 )
	return 0; // skipped due to resolution
    }
  }

  seq_lfo_t *lfo = &seq_lfo[track];
  s32 lfo_value = SEQ_LFO_ValueGet(tcc, lfo) + tcc->lfo_cc_offset;

  if( lfo_value < 0 )
    lfo_value = 0;
  else if( lfo_value > 127 )
    lfo_value = 127;

  //####################################
  //# RIO: POLYPHONIC PRESSURE
  //####################################
  p->type      = (tcc->lfo_enable_flags.POLYPRESSURE) ? PolyPressure : CC;
  p->cable     = track;
  p->event     = (tcc->lfo_enable_flags.POLYPRESSURE) ? PolyPressure : CC;
  p->chn       = tcc->midi_chn;
  p->cc_number = tcc->lfo_cc;
  p->value     = lfo_value;
  //####################################
  //# RIO: END MODIFICATION
  //####################################

  return 1; // returned 1 event
}


/////////////////////////////////////////////////////////////////////////////
// This help function returns the LFO value depending on selected waveform
/////////////////////////////////////////////////////////////////////////////
static s32 SEQ_LFO_ValueGet(seq_cc_trk_t *tcc, seq_lfo_t *lfo)
{
  s32 lfo_value = (int)lfo->pos;
  switch( tcc->lfo_waveform ) {
    case SEQ_LFO_WAVEFORM_Off:
      return 0;

    case SEQ_LFO_WAVEFORM_Sine: { // currently no real sine!
      s32 tmp = 4*(lfo_value % 32768);
      if( tmp >= 65536 )
	tmp = 65536 - (tmp % 65536);
      lfo_value = (lfo_value >= 32768) ? -tmp : tmp;
    } break;

    case SEQ_LFO_WAVEFORM_Triangle: {
      s32 tmp = 2*(lfo_value % 32768);
      lfo_value = (lfo_value >= 32768) ? (65535-tmp) : tmp;
    } break;

    case SEQ_LFO_WAVEFORM_Saw:
      // no modification required
      break;

    //#################################################
    //# RIO: Added new waveforms 
    //#################################################

    case SEQ_LFO_WAVEFORM_Rec05:
    case SEQ_LFO_WAVEFORM_Rec10:
    case SEQ_LFO_WAVEFORM_Rec15:
    case SEQ_LFO_WAVEFORM_Rec20:
    case SEQ_LFO_WAVEFORM_Rec25:
    case SEQ_LFO_WAVEFORM_Rec30:
    case SEQ_LFO_WAVEFORM_Rec35:
    case SEQ_LFO_WAVEFORM_Rec40:
    case SEQ_LFO_WAVEFORM_Rec45:
    case SEQ_LFO_WAVEFORM_Rec50:
    case SEQ_LFO_WAVEFORM_Rec55:
    case SEQ_LFO_WAVEFORM_Rec60:
    case SEQ_LFO_WAVEFORM_Rec65:
    case SEQ_LFO_WAVEFORM_Rec70:
    case SEQ_LFO_WAVEFORM_Rec75:
    case SEQ_LFO_WAVEFORM_Rec80:
    case SEQ_LFO_WAVEFORM_Rec85:
    case SEQ_LFO_WAVEFORM_Rec90:
    case SEQ_LFO_WAVEFORM_Rec95: {

      // SEQ_LFO_WAVEFORM_Rec05..SEQ_LFO_WAVEFORM_Rec95
      u16 trans = 5 * 655 * (tcc->lfo_waveform-SEQ_LFO_WAVEFORM_Rec05+1);
      lfo_value = (lfo_value >= trans) ? 0 : 65535;
    } break;

    // RIO: default break for all other waveforms
    default: break;

    //#################################################
    //# RIO: END MODIFICATION
    //#################################################
  }

  // scale down to +/- 7bit
  lfo_value = (lfo_value * (tcc->lfo_amplitude-128)) / 65536;

//#################################################
//# RIO: Added new waveforms
//#################################################

  switch( tcc->lfo_waveform ) {
    case SEQ_LFO_WAVEFORM_Saw2:
    case SEQ_LFO_WAVEFORM_Saw3:
    case SEQ_LFO_WAVEFORM_Saw4:
    case SEQ_LFO_WAVEFORM_Saw5:
    case SEQ_LFO_WAVEFORM_Saw6:
    case SEQ_LFO_WAVEFORM_Saw7:
    case SEQ_LFO_WAVEFORM_Saw8:
    case SEQ_LFO_WAVEFORM_Saw9:
    case SEQ_LFO_WAVEFORM_Saw10:
    case SEQ_LFO_WAVEFORM_Saw11:
    case SEQ_LFO_WAVEFORM_Saw12:
    case SEQ_LFO_WAVEFORM_Saw13:
    case SEQ_LFO_WAVEFORM_Saw14:
    case SEQ_LFO_WAVEFORM_Saw15:
    case SEQ_LFO_WAVEFORM_Saw16:
    case SEQ_LFO_WAVEFORM_Saw17:
    case SEQ_LFO_WAVEFORM_Saw18:
    case SEQ_LFO_WAVEFORM_Saw19:
    case SEQ_LFO_WAVEFORM_Saw20:

            lfo_value -= (lfo_value % (tcc->lfo_waveform - 21)); break;

    default: {

        if ( tcc->lfo_waveform >= SEQ_LFO_WAVEFORM___V2 &&
             tcc->lfo_waveform <= SEQ_LFO_WAVEFORM_A4A4) {

            s32 tmp;

            // Referenzpunkte: 1/4 scaled down

            s32 lfoV = (16384 * (tcc->lfo_amplitude-128)) / 65536;
            s32 lfoH = (32768 * (tcc->lfo_amplitude-128)) / 65536;
            s32 lfoD = (49152 * (tcc->lfo_amplitude-128)) / 65536;
            s32 lfoA =           tcc->lfo_amplitude-128;

            s32 ref;
            u8  amp;

            u8 qV = 0; // 1viertel          quantize for this area
            u8 qH = 0; // 2viertel (Half)   quantize for this area
            u8 qD = 0; // 3viertel          quantize for this area
            u8 qA = 0; // 4viertel (ALL)    quantize for this area

            switch( tcc->lfo_waveform ) {

                case SEQ_LFO_WAVEFORM___V2: ref=lfoA;amp=1; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM___H2: ref=lfoA;amp=1; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM___D2: ref=lfoA;amp=1; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM___A2: ref=lfoA;amp=1; qV=2;qH=2;qD=2;qA=2; break;
                case SEQ_LFO_WAVEFORM_V2__: ref=lfoV;amp=2; qV=0;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H2__: ref=lfoH;amp=2; qV=0;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D2__: ref=lfoD;amp=2; qV=0;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A2__: ref=lfoA;amp=2; qV=0;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM___V4: ref=lfoA;amp=1; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM___H4: ref=lfoA;amp=1; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM___D4: ref=lfoA;amp=1; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM___A4: ref=lfoA;amp=1; qV=4;qH=4;qD=4;qA=4; break;
                case SEQ_LFO_WAVEFORM_V4__: ref=lfoV;amp=4; qV=0;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H4__: ref=lfoH;amp=4; qV=0;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D4__: ref=lfoD;amp=4; qV=0;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A4__: ref=lfoA;amp=4; qV=0;qH=0;qD=0;qA=0; break;

                case SEQ_LFO_WAVEFORM_V2V2: ref=lfoV;amp=2; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_V2H2: ref=lfoV;amp=2; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_V2D2: ref=lfoV;amp=2; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM_V2A2: ref=lfoV;amp=2; qV=2;qH=2;qD=2;qA=2; break;
                case SEQ_LFO_WAVEFORM_H2V2: ref=lfoH;amp=2; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H2H2: ref=lfoH;amp=2; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H2D2: ref=lfoH;amp=2; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM_H2A2: ref=lfoH;amp=2; qV=2;qH=2;qD=2;qA=2; break;
                case SEQ_LFO_WAVEFORM_D2V2: ref=lfoD;amp=2; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D2H2: ref=lfoD;amp=2; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D2D2: ref=lfoD;amp=2; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM_D2A2: ref=lfoD;amp=2; qV=2;qH=2;qD=2;qA=2; break;
                case SEQ_LFO_WAVEFORM_A2V2: ref=lfoA;amp=2; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A2H2: ref=lfoA;amp=2; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A2D2: ref=lfoA;amp=2; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM_A2A2: ref=lfoA;amp=2; qV=2;qH=2;qD=2;qA=2; break;

                case SEQ_LFO_WAVEFORM_V4V2: ref=lfoV;amp=4; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_V4H2: ref=lfoV;amp=4; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_V4D2: ref=lfoV;amp=4; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM_V4A2: ref=lfoV;amp=4; qV=2;qH=2;qD=2;qA=2; break;
                case SEQ_LFO_WAVEFORM_H4V2: ref=lfoH;amp=4; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H4H2: ref=lfoH;amp=4; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H4D2: ref=lfoH;amp=4; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM_H4A2: ref=lfoH;amp=4; qV=2;qH=2;qD=2;qA=2; break;
                case SEQ_LFO_WAVEFORM_D4V2: ref=lfoD;amp=4; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D4H2: ref=lfoD;amp=4; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D4D2: ref=lfoD;amp=4; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM_D4A2: ref=lfoD;amp=4; qV=2;qH=2;qD=2;qA=2; break;
                case SEQ_LFO_WAVEFORM_A4V2: ref=lfoA;amp=4; qV=2;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A4H2: ref=lfoA;amp=4; qV=2;qH=2;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A4D2: ref=lfoA;amp=4; qV=2;qH=2;qD=2;qA=0; break;
                case SEQ_LFO_WAVEFORM_A4A2: ref=lfoA;amp=4; qV=2;qH=2;qD=2;qA=2; break;

                case SEQ_LFO_WAVEFORM_V2V4: ref=lfoV;amp=2; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_V2H4: ref=lfoV;amp=2; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_V2D4: ref=lfoV;amp=2; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM_V2A4: ref=lfoV;amp=2; qV=4;qH=4;qD=4;qA=4; break;
                case SEQ_LFO_WAVEFORM_H2V4: ref=lfoH;amp=2; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H2H4: ref=lfoH;amp=2; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H2D4: ref=lfoH;amp=2; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM_H2A4: ref=lfoH;amp=2; qV=4;qH=4;qD=4;qA=4; break;
                case SEQ_LFO_WAVEFORM_D2V4: ref=lfoD;amp=2; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D2H4: ref=lfoD;amp=2; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D2D4: ref=lfoD;amp=2; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM_D2A4: ref=lfoD;amp=2; qV=4;qH=4;qD=4;qA=4; break;
                case SEQ_LFO_WAVEFORM_A2V4: ref=lfoA;amp=2; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A2H4: ref=lfoA;amp=2; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A2D4: ref=lfoA;amp=2; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM_A2A4: ref=lfoA;amp=2; qV=4;qH=4;qD=4;qA=4; break;

                case SEQ_LFO_WAVEFORM_V4V4: ref=lfoV;amp=4; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_V4H4: ref=lfoV;amp=4; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_V4D4: ref=lfoV;amp=4; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM_V4A4: ref=lfoV;amp=4; qV=4;qH=4;qD=4;qA=4; break;
                case SEQ_LFO_WAVEFORM_H4V4: ref=lfoH;amp=4; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H4H4: ref=lfoH;amp=4; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_H4D4: ref=lfoH;amp=4; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM_H4A4: ref=lfoH;amp=4; qV=4;qH=4;qD=4;qA=4; break;
                case SEQ_LFO_WAVEFORM_D4V4: ref=lfoD;amp=4; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D4H4: ref=lfoD;amp=4; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_D4D4: ref=lfoD;amp=4; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM_D4A4: ref=lfoD;amp=4; qV=4;qH=4;qD=4;qA=4; break;
                case SEQ_LFO_WAVEFORM_A4V4: ref=lfoA;amp=4; qV=4;qH=0;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A4H4: ref=lfoA;amp=4; qV=4;qH=4;qD=0;qA=0; break;
                case SEQ_LFO_WAVEFORM_A4D4: ref=lfoA;amp=4; qV=4;qH=4;qD=4;qA=0; break;
                case SEQ_LFO_WAVEFORM_A4A4: ref=lfoA;amp=4; qV=4;qH=4;qD=4;qA=4; break;

            }

            // -------------------------------------------------- Initialize

            if (!amp) amp = 1;
            tmp = lfo_value;

            if (!tcc->lfo_enable_flags.REVERSE) {

                // ---------------------------------------------- Start init

                tmp *= amp;

                // ---------------------------------------------- 1Viertel - V

                if (lfo_value < lfoV) {

                    // set quantize
                    if (qV) {
                        qV *= amp;
                        tmp -= tmp % qV;
                    }
                }

                // ---------------------------------------------- 2Viertel - H

                else if (lfo_value < lfoH) {

                    //Recalc amplitude
                    if (ref < lfoH) tmp -= lfo_value*(amp-1) - ref*(amp-1); else qH *= amp;

                    // set quantize
                    if (qH) tmp -= tmp % qH;
                }

                // ---------------------------------------------- 3Viertel - D

                else if (lfo_value < lfoD) {

                    //Recalc amplitude
                    if (ref < lfoD) tmp -= lfo_value*(amp-1) - ref*(amp-1); else qD *= amp;

                    // set quantize
                    if (qD) tmp -= tmp % qD;
                }

                // ---------------------------------------------- 3Viertel - A

                else if (lfo_value) {

                    //Recalc amplitude
                    if (ref < lfoA) tmp -= lfo_value*(amp-1) - ref*(amp-1); else qA *= amp;

                    // set quantize
                    if (qA) tmp -= tmp % qA;
                }

            } else {

                // - REVERSE ------------------------------------ Start init

                if (qV) {
                    u8 qR = qV;
                    if (qA) qA = 0; else qA = qR;
                    if (qD) qD = 0; else qD = qR;
                    if (qH) qH = 0; else qH = qR;
                    if (qV) qV = 0; else qV = qR;
                }

                // ---------------------------------------------- 1Viertel - V

                if (lfo_value < lfoV) ;// nothing to do

                // ---------------------------------------------- 2Viertel - H

                else if (lfo_value < lfoH) {

                    //Recalc amplitude
                    if (ref < lfoH) {
                        tmp *= amp;
                        tmp -= ref*(amp-1);
                        qH *= amp;
                    }

                    // set quantize
                    if (qH) tmp -= tmp % qH;
                }

                // ---------------------------------------------- 3Viertel - D

                else if (lfo_value < lfoD) {

                    //Recalc amplitude
                    if (ref < lfoD) {
                        tmp *= amp;
                        tmp -= ref*(amp-1);
                        qD *= amp;
                    }

                    // set quantize
                    if (qD) tmp -= tmp % qD;
                }

                // ---------------------------------------------- 3Viertel - A

                else if (lfo_value) {

                    //Recalc amplitude
                    if (ref < lfoA) {
                        tmp *= amp;
                        tmp -= ref*(amp-1);
                        qA *= amp;
                    }

                    // set quantize
                    if (qA) tmp -= tmp % qA;
                }
            }

            lfo_value = tmp;
        }

    } break;
  }

  // FADE UP/DOWN
  
  if (tcc->lfo_phase > 200) {

    u8 fadestart  = 0;
    u8 fadevalue  = tcc->lfo_phase - 200;

    if (fadevalue < tcc->lfo_steps_rst) 
        fadestart = tcc->lfo_steps_rst-fadevalue+1;

    if( lfo->step_ctr > fadestart) {

        // store last lfo before fade out
        if (!lfo->fadeoffset) lfo->fadeoffset = lfo_value;

        float m = (lfo->step_ctr - fadestart) / (float)fadevalue;
        if (tcc->lfo_enable_flags.FADEUP)   lfo_value = lfo->fadeoffset + (int)((127-tcc->lfo_cc_offset-lfo->fadeoffset) * m);
        else                                lfo_value = lfo->fadeoffset - (int)(lfo->fadeoffset * m);

        if      (lfo_value < 0)     lfo_value = 0;
        else if (lfo_value > 127)   lfo_value = 127;

        //DEBUG_MSG("step: %d rst: %d off: %d start: %d value: %d lfo: %d stp-start+1: %d m: %d.%03d\n", lfo->step_ctr,tcc->lfo_steps_rst,lfo->fadeoffset,fadestart,fadevalue,lfo_value,lfo->step_ctr-fadestart+1,(int)m,(int)(m*1000.0)%1000);
        return lfo_value;

    } else lfo->fadeoffset = 0;
  }

//#################################################
//# RIO: END MODIFICATION
//#################################################

  return lfo_value;
}

