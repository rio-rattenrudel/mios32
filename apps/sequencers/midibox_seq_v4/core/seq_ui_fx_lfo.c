// $Id$
//########################################################################################################
//# RIO: LFO expanded with special waveforms (Saw Quantized and Amp Multiplikator + Quantizer), Reverse, 
//       Phase set to 250 + Bugfix LED Offset
//       adapted source from original version 4.080 (without ITEM_DISABLE_EXTRA_CC)
//########################################################################################################

/*
 * LFO Fx page
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
#include <tasks.h>

#include "seq_lcd.h"
#include "seq_ui.h"
#include "seq_cc.h"
#include "seq_cc_labels.h"
#include "seq_lfo.h"

//#################################################
//# RIO: Added Running?
//#################################################

#include "seq_bpm.h"

//#################################################
//# RIO: END MODIFICATION
//#################################################


/////////////////////////////////////////////////////////////////////////////
// Local definitions
/////////////////////////////////////////////////////////////////////////////

//####################################
//# RIO: POLYPHONIC PRESSURE
//####################################
#define NUM_OF_ITEMS       15
#define ITEM_GXTY          0
#define ITEM_WAVEFORM      1
#define ITEM_AMPLITUDE     2
#define ITEM_PHASE         3
#define ITEM_STEPS         4
#define ITEM_STEPS_RST     5
#define ITEM_ENABLE_ONE_SHOT  6
#define ITEM_ENABLE_NOTE   7
#define ITEM_ENABLE_VELOCITY 8
#define ITEM_ENABLE_LENGTH 9
#define ITEM_ENABLE_CC     10
#define ITEM_CC            11
#define ITEM_CC_OFFSET     12
#define ITEM_ENABLE_PP     13
#define ITEM_CC_PPQN       14
//####################################
//# RIO: END MODIFICATION
//####################################

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

static u8 edit_cc_number;


/////////////////////////////////////////////////////////////////////////////
// Local LED handler function
/////////////////////////////////////////////////////////////////////////////
static s32 LED_Handler(u16 *gp_leds)
{
  if( ui_cursor_flash ) // if flashing flag active: no LED flag set
    return 0;

  switch( ui_selected_item ) {
    case ITEM_GXTY: *gp_leds = 0x0001; break;
    case ITEM_WAVEFORM: *gp_leds = 0x0002; break;
    case ITEM_AMPLITUDE: *gp_leds = 0x0004; break;
    case ITEM_PHASE: *gp_leds = 0x0008; break;
    case ITEM_STEPS: *gp_leds = 0x0010; break;
    case ITEM_STEPS_RST: *gp_leds = 0x0020; break;
    case ITEM_ENABLE_ONE_SHOT: *gp_leds = 0x0040; break;
    case ITEM_ENABLE_NOTE: *gp_leds = 0x0100; break;
    case ITEM_ENABLE_VELOCITY: *gp_leds = 0x0200; break;
    case ITEM_ENABLE_LENGTH: *gp_leds = 0x0400; break;
    case ITEM_ENABLE_CC: *gp_leds = 0x0800; break;
    //####################################
    //# RIO: POLYPHONIC PRESSURE
    //####################################
    case ITEM_ENABLE_PP: *gp_leds = 0x1000; break;
    //####################################
    //# RIO: END MODIFICATION
    //####################################
    case ITEM_CC: *gp_leds = 0x2000; break;
    case ITEM_CC_OFFSET: *gp_leds = 0x4000; break;
    case ITEM_CC_PPQN: *gp_leds = 0x8000; break;
  }

#if 0
  // inconsistent and doesn't look so nice
  u8 visible_track = SEQ_UI_VisibleTrackGet();
  u8 flags = SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS);
  *gp_leds |= (((u16)(flags & 0x1e)) << 7) | (((u16)(flags & 0x01)) << 6);
#endif

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Local encoder callback function
// Should return:
//   1 if value has been changed
//   0 if value hasn't been changed
//  -1 if invalid or unsupported encoder
/////////////////////////////////////////////////////////////////////////////
static s32 Encoder_Handler(seq_ui_encoder_t encoder, s32 incrementer)
{
  u8 visible_track = SEQ_UI_VisibleTrackGet();

  switch( encoder ) {
    case SEQ_UI_ENCODER_GP1:
      ui_selected_item = ITEM_GXTY;
      break;

    case SEQ_UI_ENCODER_GP2:
      ui_selected_item = ITEM_WAVEFORM;
      break;

    case SEQ_UI_ENCODER_GP3:
      ui_selected_item = ITEM_AMPLITUDE;
      break;

    case SEQ_UI_ENCODER_GP4:
      ui_selected_item = ITEM_PHASE;
      break;

    case SEQ_UI_ENCODER_GP5:
      ui_selected_item = ITEM_STEPS;
      break;

    case SEQ_UI_ENCODER_GP6:
      ui_selected_item = ITEM_STEPS_RST;
      break;

    case SEQ_UI_ENCODER_GP7:
      ui_selected_item = ITEM_ENABLE_ONE_SHOT;
      break;

    case SEQ_UI_ENCODER_GP8:
      return -1; // not mapped

    case SEQ_UI_ENCODER_GP9:
      ui_selected_item = ITEM_ENABLE_NOTE;
      break;

    case SEQ_UI_ENCODER_GP10:
      ui_selected_item = ITEM_ENABLE_VELOCITY;
      break;

    case SEQ_UI_ENCODER_GP11:
      ui_selected_item = ITEM_ENABLE_LENGTH;
      break;

    case SEQ_UI_ENCODER_GP12:
      ui_selected_item = ITEM_ENABLE_CC;
      break;

    //####################################
    //# RIO: POLYPHONIC PRESSURE
    //####################################
    case SEQ_UI_ENCODER_GP13:
      ui_selected_item = ITEM_ENABLE_PP;
      break;
    //####################################
    //# RIO: END MODIFICATION
    //####################################

    case SEQ_UI_ENCODER_GP14:
      // CC number selection now has to be confirmed with GP button
      if( ui_selected_item != ITEM_CC ) {
	edit_cc_number = SEQ_CC_Get(visible_track, SEQ_CC_LFO_CC);
	ui_selected_item = ITEM_CC;
	SEQ_UI_Msg(SEQ_UI_MSG_USER, 2000, "Please confirm CC", "with GP button!");
      } else if( incrementer == 0 ) {
	if( edit_cc_number != SEQ_CC_Get(visible_track, SEQ_CC_LFO_CC) ) {
	  SEQ_UI_CC_Set(SEQ_CC_LFO_CC, edit_cc_number);
	  SEQ_UI_Msg(SEQ_UI_MSG_USER, 2000, "CC number", "has been changed.");
	}

	// send event
	mios32_midi_package_t p;
	if( SEQ_LFO_FastCC_Event(visible_track, 0, &p, 1) >= 1 ) {
	  MUTEX_MIDIOUT_TAKE;
	  MIOS32_MIDI_SendPackage(SEQ_CC_Get(visible_track, SEQ_CC_MIDI_PORT), p);
	  MUTEX_MIDIOUT_GIVE;
	}
      }
      break;

    case SEQ_UI_ENCODER_GP15:
      ui_selected_item = ITEM_CC_OFFSET;
      break;

    case SEQ_UI_ENCODER_GP16:
      ui_selected_item = ITEM_CC_PPQN;
      break;
  }

  // for GP encoders and Datawheel
  switch( ui_selected_item ) {
    case ITEM_GXTY:          return SEQ_UI_GxTyInc(incrementer);
    case ITEM_WAVEFORM:      {
      //#################################################
      //# RIO: TOGGLE REVERSE
      //#################################################

      u8 wave = SEQ_CC_Get(visible_track, SEQ_CC_LFO_WAVEFORM);
      if ( wave >= SEQ_LFO_WAVEFORM___V2 && wave <=SEQ_LFO_WAVEFORM_A4A4) {
          u8 mask = 1 << 5;
          u8 value = SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS);
          if( incrementer == 0 ) SEQ_UI_CC_SetFlags(SEQ_CC_LFO_ENABLE_FLAGS, mask, value ^ mask);
      }
      return SEQ_UI_CC_Inc(SEQ_CC_LFO_WAVEFORM, 0, 121, incrementer);    // RIO: ORIGINAL: 0, 22, ..

      //#################################################
      //# RIO: END MODIFICATION
      //#################################################
                             }
    case ITEM_AMPLITUDE:     return SEQ_UI_CC_Inc(SEQ_CC_LFO_AMPLITUDE, 0, 255, incrementer);
    case ITEM_PHASE:         {
      //#################################################
      //# RIO: TOGGLE FADE UP/DOWN
      //#################################################

      if (SEQ_CC_Get(visible_track, SEQ_CC_LFO_PHASE) > 200) {
          u8 mask = 1 << 6;
          u8 value = SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS);
          if( incrementer == 0 ) SEQ_UI_CC_SetFlags(SEQ_CC_LFO_ENABLE_FLAGS, mask, value ^ mask);
      }
      return SEQ_UI_CC_Inc(SEQ_CC_LFO_PHASE, 0, 250, incrementer);       // RIO: ORIGINAL: 0, 99, ..

      //#################################################
      //# RIO: END MODIFICATION
      //#################################################
                             }
    case ITEM_STEPS:         return SEQ_UI_CC_Inc(SEQ_CC_LFO_STEPS, 0, 255, incrementer);
    case ITEM_STEPS_RST:     return SEQ_UI_CC_Inc(SEQ_CC_LFO_STEPS_RST, 0, 255, incrementer);

    //####################################
    //# RIO: POLYPHONIC PRESSURE
    //####################################
    case ITEM_ENABLE_ONE_SHOT:
    case ITEM_ENABLE_NOTE:
    case ITEM_ENABLE_VELOCITY:
    case ITEM_ENABLE_LENGTH:
    case ITEM_ENABLE_CC:
    case ITEM_ENABLE_PP: {
      u8 flag = ui_selected_item - ITEM_ENABLE_ONE_SHOT;
      u8 mask = 1 << flag;
      u8 value = SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS);
      if( incrementer == 0 ) // toggle
	SEQ_UI_CC_SetFlags(SEQ_CC_LFO_ENABLE_FLAGS, mask, value ^ mask);
      else if( incrementer > 0 )
	SEQ_UI_CC_SetFlags(SEQ_CC_LFO_ENABLE_FLAGS, mask, mask);
      else
	SEQ_UI_CC_SetFlags(SEQ_CC_LFO_ENABLE_FLAGS, mask, 0);
    } break;
    //####################################
    //# RIO: END MODIFICATION
    //####################################

    case ITEM_CC: {
      // CC number selection now has to be confirmed with GP button
      s32 status = SEQ_UI_Var8_Inc(&edit_cc_number, 0, 127, incrementer);
      mios32_midi_port_t port = SEQ_CC_Get(visible_track, SEQ_CC_MIDI_PORT);
      u8 loopback = (port & 0xf0) == 0xf0;
      if( !edit_cc_number ) {
	SEQ_UI_Msg(SEQ_UI_MSG_USER_R, 1000, "LFO CC", "disabled");
      } else {
	SEQ_UI_Msg(SEQ_UI_MSG_USER_R, 1000, loopback ? "Loopback CC" : "Controller:", (char *)SEQ_CC_LABELS_Get(port, edit_cc_number, 0));
      }
      return status;
    } break;

    case ITEM_CC_OFFSET:     return SEQ_UI_CC_Inc(SEQ_CC_LFO_CC_OFFSET, 0, 127, incrementer);
    case ITEM_CC_PPQN:       return SEQ_UI_CC_Inc(SEQ_CC_LFO_CC_PPQN, 0, 8, incrementer);
  }

  return -1; // invalid or unsupported encoder
}


/////////////////////////////////////////////////////////////////////////////
// Local button callback function
// Should return:
//   1 if value has been changed
//   0 if value hasn't been changed
//  -1 if invalid or unsupported button
/////////////////////////////////////////////////////////////////////////////
static s32 Button_Handler(seq_ui_button_t button, s32 depressed)
{
  if( depressed ) return 0; // ignore when button depressed

#if 0
  // leads to: comparison is always true due to limited range of data type
  if( button >= SEQ_UI_BUTTON_GP1 && button <= SEQ_UI_BUTTON_GP16 ) {
#else
  if( button <= SEQ_UI_BUTTON_GP16 ) {
#endif
    // re-use encoder handler - only select UI item, don't increment
    return Encoder_Handler((int)button, 0);
  }

  // remaining buttons:
  switch( button ) {
    case SEQ_UI_BUTTON_Select:
    case SEQ_UI_BUTTON_Right:
      if( ++ui_selected_item >= NUM_OF_ITEMS )
	ui_selected_item = 0;
      return 1; // value always changed

    case SEQ_UI_BUTTON_Left:
      if( ui_selected_item == 0 )
	ui_selected_item = NUM_OF_ITEMS-1;
      else
	--ui_selected_item;
      return 1; // value always changed

    case SEQ_UI_BUTTON_Up:
      return Encoder_Handler(SEQ_UI_ENCODER_Datawheel, 1);

    case SEQ_UI_BUTTON_Down:
      return Encoder_Handler(SEQ_UI_ENCODER_Datawheel, -1);
  }

  return -1; // invalid or unsupported button
}


/////////////////////////////////////////////////////////////////////////////
// Local Display Handler function
// IN: <high_prio>: if set, a high-priority LCD update is requested
/////////////////////////////////////////////////////////////////////////////
static s32 LCD_Handler(u8 high_prio)
{
  //#################################################
  //# RIO: Added new waveforms (LCD update)
  //#################################################

  if( high_prio ) {

      if (SEQ_BPM_IsRunning()) {
          u8 visible_track = SEQ_UI_VisibleTrackGet();
          u8 value = SEQ_CC_Get(visible_track, SEQ_CC_LFO_WAVEFORM);

          if ( value ) {

              mios32_midi_package_t p;

              if( SEQ_LFO_FastCC_Event(visible_track, 0, &p, 1) >= 1 ) {
                char buffer[7];
                sprintf(buffer, "V: %3d", p.value);
                SEQ_UI_Msg(SEQ_UI_MSG_USER_R, 1000, buffer, 0);
              }
          }
      }

      return 0;
  }
  // return 0; // there are no high-priority updates //RIO: changed

  //#################################################
  //# RIO: END MODIFICATION
  //#################################################


  // layout:
  // 00000000001111111111222222222233333333330000000000111111111122222222223333333333
  // 01234567890123456789012345678901234567890123456789012345678901234567890123456789
  // <--------------------------------------><-------------------------------------->
  // Trk. Wave Amp. Phs. Steps Rst OneShot   Note Vel. Len.  CC   ExtraCC# Offs. PPQN
  // GxTy Sine   64   0%   16   16  on        off  off  off  off        001   64  384

  u8 visible_track = SEQ_UI_VisibleTrackGet();

  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_CursorSet(0, 0);

  //##############################
  //# RIO: Display Header adapted
  //##############################

  // Original:
  //SEQ_LCD_PrintString("Trk. Wave Amp. Phs. Steps Rst OneShot   Note Vel. Len.  CC   ExtraCC# Offs. PPQN");

  char buffer1[5];
  char buffer2[5];
  char ch = ' ';

  u8 value = SEQ_CC_Get(visible_track, SEQ_CC_LFO_WAVEFORM);
  if ( value >= SEQ_LFO_WAVEFORM___V2 && value <=SEQ_LFO_WAVEFORM_A4A4) sprintf(buffer1, "R+Q+");
  else                                                                  sprintf(buffer1, "Wave");

  if (SEQ_CC_Get(visible_track, SEQ_CC_LFO_PHASE) > 200) {
    if (SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS) & (1 << 6))  sprintf(buffer2, "FdUp");
    else                                                                sprintf(buffer2, "FdDn");
  }
  else if (SEQ_CC_Get(visible_track, SEQ_CC_LFO_PHASE) > 100)           sprintf(buffer2, "Dly.");
  else                                                                  sprintf(buffer2, "Phs.");

  SEQ_LCD_PrintFormattedString("Trk. %s%cAmp. %s Steps Rst OneShot   Note Vel. Len.  CC   Extra%s# Offs. PPQN",buffer1,ch,buffer2, (SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS) & (1 << 7)) ? "PP" : "CC");

  //##############################
  //# RIO: END MODIFICATION
  //##############################


  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_CursorSet(0, 1);

  if( ui_selected_item == ITEM_GXTY && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(4);
  } else {
    SEQ_LCD_PrintGxTy(ui_selected_group, ui_selected_tracks);
  }
  SEQ_LCD_PrintSpaces(1);

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_WAVEFORM && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    u8 value = SEQ_CC_Get(visible_track, SEQ_CC_LFO_WAVEFORM);

    if( value <= 3 ) {
      const char waveform_str[4][6] = {
	" off ",
	"Sine ",
	"Tri. ",
	"Saw. "
      };

      SEQ_LCD_PrintString((char *)waveform_str[value]);

  //#################################################
  //# RIO: Added new waveforms (LCD update)
  //#################################################

    // Original:
    //} else {
    //  SEQ_LCD_PrintFormattedString(" R%02d ", (value-4+1)*5);
    //}

    } else if (value <= 22) SEQ_LCD_PrintFormattedString(" R%02d ", (value-4+1)*5);
      else if (value <= 41) SEQ_LCD_PrintFormattedString(" S%02d ",  value-21);

      else if (value == SEQ_LFO_WAVEFORM___V2) SEQ_LCD_PrintString("--V2 ");
      else if (value == SEQ_LFO_WAVEFORM___H2) SEQ_LCD_PrintString("--H2 ");
      else if (value == SEQ_LFO_WAVEFORM___D2) SEQ_LCD_PrintString("--D2 ");
      else if (value == SEQ_LFO_WAVEFORM___A2) SEQ_LCD_PrintString("--A2 ");
      else if (value == SEQ_LFO_WAVEFORM_V2__) SEQ_LCD_PrintString("V2-- ");
      else if (value == SEQ_LFO_WAVEFORM_H2__) SEQ_LCD_PrintString("H2-- ");
      else if (value == SEQ_LFO_WAVEFORM_D2__) SEQ_LCD_PrintString("D2-- ");
      else if (value == SEQ_LFO_WAVEFORM_A2__) SEQ_LCD_PrintString("A2-- ");
      else if (value == SEQ_LFO_WAVEFORM___V4) SEQ_LCD_PrintString("--V4 ");
      else if (value == SEQ_LFO_WAVEFORM___H4) SEQ_LCD_PrintString("--H4 ");
      else if (value == SEQ_LFO_WAVEFORM___D4) SEQ_LCD_PrintString("--D4 ");
      else if (value == SEQ_LFO_WAVEFORM___A4) SEQ_LCD_PrintString("--A4 ");
      else if (value == SEQ_LFO_WAVEFORM_V4__) SEQ_LCD_PrintString("V4-- ");
      else if (value == SEQ_LFO_WAVEFORM_H4__) SEQ_LCD_PrintString("H4-- ");
      else if (value == SEQ_LFO_WAVEFORM_D4__) SEQ_LCD_PrintString("D4-- ");
      else if (value == SEQ_LFO_WAVEFORM_A4__) SEQ_LCD_PrintString("A4-- ");

      else if (value == SEQ_LFO_WAVEFORM_V2V2) SEQ_LCD_PrintString("V2V2 ");
      else if (value == SEQ_LFO_WAVEFORM_V2H2) SEQ_LCD_PrintString("V2H2 ");
      else if (value == SEQ_LFO_WAVEFORM_V2D2) SEQ_LCD_PrintString("V2D2 ");
      else if (value == SEQ_LFO_WAVEFORM_V2A2) SEQ_LCD_PrintString("V2A2 ");
      else if (value == SEQ_LFO_WAVEFORM_H2V2) SEQ_LCD_PrintString("H2V2 ");
      else if (value == SEQ_LFO_WAVEFORM_H2H2) SEQ_LCD_PrintString("H2H2 ");
      else if (value == SEQ_LFO_WAVEFORM_H2D2) SEQ_LCD_PrintString("H2D2 ");
      else if (value == SEQ_LFO_WAVEFORM_H2A2) SEQ_LCD_PrintString("H2A2 ");
      else if (value == SEQ_LFO_WAVEFORM_D2V2) SEQ_LCD_PrintString("D2V2 ");
      else if (value == SEQ_LFO_WAVEFORM_D2H2) SEQ_LCD_PrintString("D2H2 ");
      else if (value == SEQ_LFO_WAVEFORM_D2D2) SEQ_LCD_PrintString("D2D2 ");
      else if (value == SEQ_LFO_WAVEFORM_D2A2) SEQ_LCD_PrintString("D2A2 ");
      else if (value == SEQ_LFO_WAVEFORM_A2V2) SEQ_LCD_PrintString("A2V2 ");
      else if (value == SEQ_LFO_WAVEFORM_A2H2) SEQ_LCD_PrintString("A2H2 ");
      else if (value == SEQ_LFO_WAVEFORM_A2D2) SEQ_LCD_PrintString("A2D2 ");
      else if (value == SEQ_LFO_WAVEFORM_A2A2) SEQ_LCD_PrintString("A2A2 ");

      else if (value == SEQ_LFO_WAVEFORM_V2V4) SEQ_LCD_PrintString("V2V4 ");
      else if (value == SEQ_LFO_WAVEFORM_V2H4) SEQ_LCD_PrintString("V2H4 ");
      else if (value == SEQ_LFO_WAVEFORM_V2D4) SEQ_LCD_PrintString("V2D4 ");
      else if (value == SEQ_LFO_WAVEFORM_V2A4) SEQ_LCD_PrintString("V2A4 ");
      else if (value == SEQ_LFO_WAVEFORM_H2V4) SEQ_LCD_PrintString("H2V4 ");
      else if (value == SEQ_LFO_WAVEFORM_H2H4) SEQ_LCD_PrintString("H2H4 ");
      else if (value == SEQ_LFO_WAVEFORM_H2D4) SEQ_LCD_PrintString("H2D4 ");
      else if (value == SEQ_LFO_WAVEFORM_H2A4) SEQ_LCD_PrintString("H2A4 ");
      else if (value == SEQ_LFO_WAVEFORM_D2V4) SEQ_LCD_PrintString("D2V4 ");
      else if (value == SEQ_LFO_WAVEFORM_D2H4) SEQ_LCD_PrintString("D2H4 ");
      else if (value == SEQ_LFO_WAVEFORM_D2D4) SEQ_LCD_PrintString("D2D4 ");
      else if (value == SEQ_LFO_WAVEFORM_D2A4) SEQ_LCD_PrintString("D2A4 ");
      else if (value == SEQ_LFO_WAVEFORM_A2V4) SEQ_LCD_PrintString("A2V4 ");
      else if (value == SEQ_LFO_WAVEFORM_A2H4) SEQ_LCD_PrintString("A2H4 ");
      else if (value == SEQ_LFO_WAVEFORM_A2D4) SEQ_LCD_PrintString("A2D4 ");
      else if (value == SEQ_LFO_WAVEFORM_A2A4) SEQ_LCD_PrintString("A2A4 ");

      else if (value == SEQ_LFO_WAVEFORM_V4V2) SEQ_LCD_PrintString("V4V2 ");
      else if (value == SEQ_LFO_WAVEFORM_V4H2) SEQ_LCD_PrintString("V4H2 ");
      else if (value == SEQ_LFO_WAVEFORM_V4D2) SEQ_LCD_PrintString("V4D2 ");
      else if (value == SEQ_LFO_WAVEFORM_V4A2) SEQ_LCD_PrintString("V4A2 ");
      else if (value == SEQ_LFO_WAVEFORM_H4V2) SEQ_LCD_PrintString("H4V2 ");
      else if (value == SEQ_LFO_WAVEFORM_H4H2) SEQ_LCD_PrintString("H4H2 ");
      else if (value == SEQ_LFO_WAVEFORM_H4D2) SEQ_LCD_PrintString("H4D2 ");
      else if (value == SEQ_LFO_WAVEFORM_H4A2) SEQ_LCD_PrintString("H4A2 ");
      else if (value == SEQ_LFO_WAVEFORM_D4V2) SEQ_LCD_PrintString("D4V2 ");
      else if (value == SEQ_LFO_WAVEFORM_D4H2) SEQ_LCD_PrintString("D4H2 ");
      else if (value == SEQ_LFO_WAVEFORM_D4D2) SEQ_LCD_PrintString("D4D2 ");
      else if (value == SEQ_LFO_WAVEFORM_D4A2) SEQ_LCD_PrintString("D4A2 ");
      else if (value == SEQ_LFO_WAVEFORM_A4V2) SEQ_LCD_PrintString("A4V2 ");
      else if (value == SEQ_LFO_WAVEFORM_A4H2) SEQ_LCD_PrintString("A4H2 ");
      else if (value == SEQ_LFO_WAVEFORM_A4D2) SEQ_LCD_PrintString("A4D2 ");
      else if (value == SEQ_LFO_WAVEFORM_A4A2) SEQ_LCD_PrintString("A4A2 ");

      else if (value == SEQ_LFO_WAVEFORM_V4V4) SEQ_LCD_PrintString("V4V4 ");
      else if (value == SEQ_LFO_WAVEFORM_V4H4) SEQ_LCD_PrintString("V4H4 ");
      else if (value == SEQ_LFO_WAVEFORM_V4D4) SEQ_LCD_PrintString("V4D4 ");
      else if (value == SEQ_LFO_WAVEFORM_V4A4) SEQ_LCD_PrintString("V4A4 ");
      else if (value == SEQ_LFO_WAVEFORM_H4V4) SEQ_LCD_PrintString("H4V4 ");
      else if (value == SEQ_LFO_WAVEFORM_H4H4) SEQ_LCD_PrintString("H4H4 ");
      else if (value == SEQ_LFO_WAVEFORM_H4D4) SEQ_LCD_PrintString("H4D4 ");
      else if (value == SEQ_LFO_WAVEFORM_H4A4) SEQ_LCD_PrintString("H4A4 ");
      else if (value == SEQ_LFO_WAVEFORM_D4V4) SEQ_LCD_PrintString("D4V4 ");
      else if (value == SEQ_LFO_WAVEFORM_D4H4) SEQ_LCD_PrintString("D4H4 ");
      else if (value == SEQ_LFO_WAVEFORM_D4D4) SEQ_LCD_PrintString("D4D4 ");
      else if (value == SEQ_LFO_WAVEFORM_D4A4) SEQ_LCD_PrintString("D4A4 ");
      else if (value == SEQ_LFO_WAVEFORM_A4V4) SEQ_LCD_PrintString("A4V4 ");
      else if (value == SEQ_LFO_WAVEFORM_A4H4) SEQ_LCD_PrintString("A4H4 ");
      else if (value == SEQ_LFO_WAVEFORM_A4D4) SEQ_LCD_PrintString("A4D4 ");
      else if (value == SEQ_LFO_WAVEFORM_A4A4) SEQ_LCD_PrintString("A4A4 ");
  }

  //#################################################
  //# RIO: END MODIFICATION
  //#################################################

  //#################################################
  //# RIO: Added REVERSE
  //#################################################

  if (value >= SEQ_LFO_WAVEFORM___V2 && value <= SEQ_LFO_WAVEFORM_A4A4) {
       SEQ_LCD_CursorSet(9, 1);
       SEQ_LCD_PrintChar((SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS) & (1 << 5)) ? '>' : '<');
  }
  
  //#################################################
  //# RIO: END MODIFICATION
  //#################################################

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_AMPLITUDE && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    int value = (int)SEQ_CC_Get(visible_track, SEQ_CC_LFO_AMPLITUDE) - 128;
    SEQ_LCD_PrintFormattedString("%4d ", value);
  }


  //###########################
  //# RIO: Delay and FadeOut
  //###########################

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_PHASE && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(6);
  } else {
  	// Original:
    // SEQ_LCD_PrintFormattedString("%3d%%  ", SEQ_CC_Get(visible_track, SEQ_CC_LFO_PHASE));

    u8 tmp = SEQ_CC_Get(visible_track, SEQ_CC_LFO_PHASE);   // RIO: added Delay and FadeOut
    if      (tmp > 200) SEQ_LCD_PrintFormattedString("%3d   ", tmp-200);
    else if (tmp > 100) SEQ_LCD_PrintFormattedString("%3d   ", (tmp-100) << 1);
    else                SEQ_LCD_PrintFormattedString("%3d%%  ", tmp);
  }

  //###########################
  //# RIO: END MODIFICATION
  //###########################
  
  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_STEPS && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    SEQ_LCD_PrintFormattedString("%3d  ", (int)SEQ_CC_Get(visible_track, SEQ_CC_LFO_STEPS)+1);
  }

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_STEPS_RST && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(4);
  } else {
    SEQ_LCD_PrintFormattedString("%3d ", (int)SEQ_CC_Get(visible_track, SEQ_CC_LFO_STEPS_RST)+1);
  }

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_ENABLE_ONE_SHOT && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    SEQ_LCD_PrintString((SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS) & (1 << 0)) ? "  on " : " off ");
  }

  SEQ_LCD_PrintSpaces(5);

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_ENABLE_NOTE && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    SEQ_LCD_PrintString((SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS) & (1 << 1)) ? "  on " : " off ");
  }

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_ENABLE_VELOCITY && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    SEQ_LCD_PrintString((SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS) & (1 << 2)) ? "  on " : " off ");
  }

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_ENABLE_LENGTH && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    SEQ_LCD_PrintString((SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS) & (1 << 3)) ? "  on " : " off ");
  }

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_ENABLE_CC && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    SEQ_LCD_PrintString((SEQ_CC_Get(visible_track, SEQ_CC_LFO_ENABLE_FLAGS) & (1 << 4)) ? "  on " : " off ");
  }

  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_PrintSpaces(5);
  if( ui_selected_item == ITEM_CC && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    u8 current_value = SEQ_CC_Get(visible_track, SEQ_CC_LFO_CC);
    u8 edit_value = (ui_selected_item == ITEM_CC) ? edit_cc_number : current_value;

    if( edit_value )
      SEQ_LCD_PrintFormattedString(" %03d", edit_value);
    else
      SEQ_LCD_PrintString(" ---");
    SEQ_LCD_PrintChar((current_value != edit_value) ? '!' : ' ');
  }

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_CC_OFFSET && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    SEQ_LCD_PrintFormattedString(" %3d ", SEQ_CC_Get(visible_track, SEQ_CC_LFO_CC_OFFSET));
  }

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_CC_PPQN && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    u8 value = SEQ_CC_Get(visible_track, SEQ_CC_LFO_CC_PPQN);
    int ppqn = 1;
    if( value )
      ppqn = 3 << (value-1);

    SEQ_LCD_PrintFormattedString(" %3d ", ppqn);
  }

  ///////////////////////////////////////////////////////////////////////////

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_UI_FX_LFO_Init(u32 mode)
{
  // install callback routines
  SEQ_UI_InstallButtonCallback(Button_Handler);
  SEQ_UI_InstallEncoderCallback(Encoder_Handler);
  SEQ_UI_InstallLEDCallback(LED_Handler);
  SEQ_UI_InstallLCDCallback(LCD_Handler);

  return 0; // no error
}

//########################################################################################################
//# RIO: END MODIFICATION
//########################################################################################################