// $Id$
/*
 * Fx VIRUSFX page
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
#include <string.h>
#include "seq_lcd.h"
#include "seq_ui.h"
#include "seq_cc.h"


/////////////////////////////////////////////////////////////////////////////
// Local definitions
/////////////////////////////////////////////////////////////////////////////

#define NUM_OF_ITEMS                10
#define ITEM_GXTY                   0
#define ITEM_VIRUSFX_ACTIVE         1
#define ITEM_VIRUSFX_MODE           2
#define ITEM_VIRUSFX_SHAPE_DAMPING  3
#define ITEM_VIRUSFX_CLOCK          4
#define ITEM_VIRUSFX_COLOR          5
#define ITEM_VIRUSFX_FEEDBACK       6
#define ITEM_VIRUSFX_DEPTH_TYPE     7
#define ITEM_VIRUSFX_RATE_DECTIME   8
#define ITEM_VIRUSFX_TIME_PREDLY    9

/////////////////////////////////////////////////////////////////////////////
// Local LED handler function
/////////////////////////////////////////////////////////////////////////////
static s32 LED_Handler(u16 *gp_leds)
{
  if( ui_cursor_flash ) // if flashing flag active: no LED flag set
    return 0;

  u8 visible_track = SEQ_UI_VisibleTrackGet();
  u8 mode = SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_MODE);
		
  switch( ui_selected_item ) {
    case ITEM_GXTY:                   *gp_leds = 0x0001; break;
    case ITEM_VIRUSFX_ACTIVE:         *gp_leds = 0x0002; break;
    case ITEM_VIRUSFX_MODE:           *gp_leds = 0x000C; break;
    case ITEM_VIRUSFX_SHAPE_DAMPING:  if (mode >= 2 && mode <= 4) *gp_leds = 0x0800; else *gp_leds = 0x0030; break;
    case ITEM_VIRUSFX_CLOCK:          *gp_leds = 0x0100; break;
    case ITEM_VIRUSFX_COLOR:          *gp_leds = 0x0200; break;
    case ITEM_VIRUSFX_FEEDBACK:       if (mode != 2) *gp_leds = 0x0400; break;
    case ITEM_VIRUSFX_DEPTH_TYPE:     if (mode >= 2 && mode <= 4) *gp_leds = 0x0030; else *gp_leds = 0x0800; break;
    case ITEM_VIRUSFX_RATE_DECTIME:   *gp_leds = 0x1000; break;
    case ITEM_VIRUSFX_TIME_PREDLY:    if (!SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_CLOCK)) *gp_leds = 0x2000; break;
  }

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
  u8 mode = SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_MODE);

  switch( encoder ) {
    case SEQ_UI_ENCODER_GP1:
      ui_selected_item = ITEM_GXTY;
      break;

    case SEQ_UI_ENCODER_GP2:
      ui_selected_item = ITEM_VIRUSFX_ACTIVE;
      break;

    case SEQ_UI_ENCODER_GP3:
    case SEQ_UI_ENCODER_GP4:
      ui_selected_item = ITEM_VIRUSFX_MODE;
      break;
    
    case SEQ_UI_ENCODER_GP5:
    case SEQ_UI_ENCODER_GP6:
      if (mode >= 2 && mode <= 4) ui_selected_item = ITEM_VIRUSFX_DEPTH_TYPE;
      else                        ui_selected_item = ITEM_VIRUSFX_SHAPE_DAMPING;
      break;

    case SEQ_UI_ENCODER_GP9:
      ui_selected_item = ITEM_VIRUSFX_CLOCK;
      break;

    case SEQ_UI_ENCODER_GP10:
      ui_selected_item = ITEM_VIRUSFX_COLOR;
      break;

    case SEQ_UI_ENCODER_GP11:
      if (mode == 2) return -1; // not mapped
      ui_selected_item = ITEM_VIRUSFX_FEEDBACK;
      break;
      
    case SEQ_UI_ENCODER_GP12:
      if (mode >= 2 && mode <= 4) ui_selected_item = ITEM_VIRUSFX_SHAPE_DAMPING;
      else                        ui_selected_item = ITEM_VIRUSFX_DEPTH_TYPE;
      break;

    case SEQ_UI_ENCODER_GP13:
      ui_selected_item = ITEM_VIRUSFX_RATE_DECTIME;
      break;
      
    case SEQ_UI_ENCODER_GP14:
      if (SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_CLOCK)) return -1; // not mapped
      ui_selected_item = ITEM_VIRUSFX_TIME_PREDLY;
      break;

    case SEQ_UI_ENCODER_GP7:
    case SEQ_UI_ENCODER_GP8:
    case SEQ_UI_ENCODER_GP16:
      return -1; // not mapped
  }

  // for GP encoders and Datawheel
  switch( ui_selected_item ) {
	case ITEM_GXTY:                   return SEQ_UI_GxTyInc(incrementer);
	case ITEM_VIRUSFX_ACTIVE:         return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_ACTIVE, 0, 1, incrementer);
	case ITEM_VIRUSFX_MODE:           return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_MODE, 0, 25, incrementer);
	case ITEM_VIRUSFX_SHAPE_DAMPING:  if (mode >= 2 && mode <= 4) mode = 127;
                                    else {
                                      if (SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_SHAPE_DAMPING) > 5) {
                                        SEQ_CC_Set(visible_track, SEQ_CC_VIRUSFX_SHAPE_DAMPING, 5);
                                        return 0;
                                      }
                                      mode = 5;
                                    }
                                    return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_SHAPE_DAMPING, 0, mode, incrementer);
	case ITEM_VIRUSFX_CLOCK:          return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_CLOCK, 0, 16, incrementer);
	case ITEM_VIRUSFX_COLOR:          return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_COLOR, 0, 127, incrementer);
	case ITEM_VIRUSFX_FEEDBACK:       return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_FEEDBACK, 0, 127, incrementer);
	case ITEM_VIRUSFX_DEPTH_TYPE:     if (mode >= 2 && mode <= 4) {
                                      if (SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_DEPTH_TYPE) > 3) {
                                        SEQ_CC_Set(visible_track, SEQ_CC_VIRUSFX_DEPTH_TYPE, 3);
                                        return 0;
                                      }
                                      mode = 3; 
                                    } else mode = 127;
                                    return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_DEPTH_TYPE, 0, mode, incrementer);
	case ITEM_VIRUSFX_RATE_DECTIME:   return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_RATE_DECTIME, 0, 127, incrementer);
	case ITEM_VIRUSFX_TIME_PREDLY:    return SEQ_UI_CC_Inc(SEQ_CC_VIRUSFX_TIME_PREDLY, 0, 127, incrementer);
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
  if( high_prio )
    return 0; // there are no high-priority updates

  // layout:
  // 00000000001111111111222222222233333333330000000000111111111122222222223333333333
  // 01234567890123456789012345678901234567890123456789012345678901234567890123456789
  // <--------------------------------------><-------------------------------------->
  // Trk.  Intensity     Note Vel/CC Length  
  // G1T1      0          off   off   off    

    // we want to show vertical bars
    SEQ_LCD_InitSpecialChars(SEQ_LCD_CHARSET_VBars);
    
  u8 visible_track = SEQ_UI_VisibleTrackGet();
  u8 mode = SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_MODE);
  u8 clock = SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_CLOCK);

  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_CursorSet(0, 0);

  if (mode >= 2 && mode <= 4) {
    SEQ_LCD_PrintString("Trk. Enab  Mode      Type               ");
    if (mode == 2)  SEQ_LCD_PrintString(" Clk  Clr       Dmp  Dcy  ");
    else            SEQ_LCD_PrintString(" Clk  Clr  Fdb  Dmp  Dcy  ");
  } else {
    SEQ_LCD_PrintString("Trk. Enab  Mode      Shape              ");
    SEQ_LCD_PrintString(" Clk  Clr  Fdb  Dpt  Rte  ");
  }
  if      (clock)     SEQ_LCD_PrintString("              ");
  else if (mode == 2) SEQ_LCD_PrintString("PreDly        ");
  else                SEQ_LCD_PrintString("Time          ");
  
  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_CursorSet(0, 1);

  if( ui_selected_item == ITEM_GXTY && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(4);
  } else {
    SEQ_LCD_PrintGxTy(ui_selected_group, ui_selected_tracks);
  }
  SEQ_LCD_PrintSpaces(1);

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_VIRUSFX_ACTIVE && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(4);
  } else {
    SEQ_LCD_PrintString( ( SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_ACTIVE) ) ? "  on" : " off" );
  }
  SEQ_LCD_PrintSpaces(2);

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_VIRUSFX_MODE && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(9);
  } else {
    switch (mode) {
      case 0: SEQ_LCD_PrintString( " off     " ); break;
      case 1: SEQ_LCD_PrintString( "Delay    " ); break;
      case 2: SEQ_LCD_PrintString( "Reverb   " ); break;
      case 3: SEQ_LCD_PrintString( "Rev+Fb1  " ); break;
      case 4: SEQ_LCD_PrintString( "Rev+Fb2  " ); break;
      case 5: SEQ_LCD_PrintString( "Delay 2:1" ); break;
      case 6: SEQ_LCD_PrintString( "Delay 4:3" ); break;
      case 7: SEQ_LCD_PrintString( "Delay 4:1" ); break;
      case 8: SEQ_LCD_PrintString( "Delay 8:7" ); break;
      case 9: SEQ_LCD_PrintString( "Patt 1+1 " ); break;
      case 10: SEQ_LCD_PrintString( "Patt 2+1 " ); break;
      case 11: SEQ_LCD_PrintString( "Patt 3+1 " ); break;
      case 12: SEQ_LCD_PrintString( "Patt 4+1 " ); break;
      case 13: SEQ_LCD_PrintString( "Patt 5+1 " ); break;
      case 14: SEQ_LCD_PrintString( "Patt 2+3 " ); break;
      case 15: SEQ_LCD_PrintString( "Patt 2+5 " ); break;
      case 16: SEQ_LCD_PrintString( "Patt 3+2 " ); break;
      case 17: SEQ_LCD_PrintString( "Patt 3+3 " ); break;
      case 18: SEQ_LCD_PrintString( "Patt 3+4 " ); break;
      case 19: SEQ_LCD_PrintString( "Patt 3+5 " ); break;
      case 20: SEQ_LCD_PrintString( "Patt 4+3 " ); break;
      case 21: SEQ_LCD_PrintString( "Patt 4+5 " ); break;
      case 22: SEQ_LCD_PrintString( "Patt 5+2 " ); break;
      case 23: SEQ_LCD_PrintString( "Patt 5+3 " ); break;
      case 24: SEQ_LCD_PrintString( "Patt 5+4 " ); break;
      case 25: SEQ_LCD_PrintString( "Patt 5+5 " ); break;
    }
  }
  SEQ_LCD_PrintSpaces(1);

  ///////////////////////////////////////////////////////////////////////////
  if (mode >= 2 && mode <= 4) {
    if( ui_selected_item == ITEM_VIRUSFX_DEPTH_TYPE && ui_cursor_flash ) {
      SEQ_LCD_PrintSpaces(9);
    } else {
      switch (SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_DEPTH_TYPE)) {
        case 0: SEQ_LCD_PrintString( "Ambience " ); break;
        case 1: SEQ_LCD_PrintString( "SmallRoom" ); break;
        case 2: SEQ_LCD_PrintString( "LargeRoom" ); break;
        case 3: SEQ_LCD_PrintString( "Hall     " ); break;
        default: SEQ_LCD_PrintString( "<  ???  >" ); break;
      }
    }
  } else {
    if( ui_selected_item == ITEM_VIRUSFX_SHAPE_DAMPING && ui_cursor_flash ) {
      SEQ_LCD_PrintSpaces(9);
    } else {
      switch (SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_SHAPE_DAMPING)) {
        case 0: SEQ_LCD_PrintString( "Sinus    " ); break;
        case 1: SEQ_LCD_PrintString( "Triangle " ); break;
        case 2: SEQ_LCD_PrintString( "Sawtooth " ); break;
        case 3: SEQ_LCD_PrintString( "Square   " ); break;
        case 4: SEQ_LCD_PrintString( "S&H      " ); break;
        case 5: SEQ_LCD_PrintString( "S&G    " ); break;
        default: SEQ_LCD_PrintString( "<  ???  >" ); break;
      }
    }
  }

  SEQ_LCD_PrintSpaces(10);

  SEQ_LCD_CursorSet(40, 1);

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_VIRUSFX_CLOCK && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(4);
  } else {
    switch (SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_CLOCK)) {
      case 0: SEQ_LCD_PrintString( " off" ); break;
      case 1: SEQ_LCD_PrintString( "1/64" ); break;
      case 2: SEQ_LCD_PrintString( "1/32" ); break;
      case 3: SEQ_LCD_PrintString( "1/16" ); break;
      case 4: SEQ_LCD_PrintString( " 1/8" ); break;
      case 5: SEQ_LCD_PrintString( " 1/4" ); break;
      case 6: SEQ_LCD_PrintString( " 1/2" ); break;
      case 7: SEQ_LCD_PrintString( "3/64" ); break;
      case 8: SEQ_LCD_PrintString( "3/32" ); break;
      case 9: SEQ_LCD_PrintString( "3/16" ); break;
      case 10: SEQ_LCD_PrintString( " 3/8" ); break;
      case 11: SEQ_LCD_PrintString( "1/24" ); break;
      case 12: SEQ_LCD_PrintString( "1/12" ); break;
      case 13: SEQ_LCD_PrintString( " 1/6" ); break;
      case 14: SEQ_LCD_PrintString( " 1/3" ); break;
      case 15: SEQ_LCD_PrintString( " 2/3" ); break;
      case 16: SEQ_LCD_PrintString( " 3/4" ); break;
    }
  }
  SEQ_LCD_PrintSpaces(1);

  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_VIRUSFX_COLOR && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(4);
  } else {
    SEQ_LCD_PrintVBar(SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_COLOR) >> 4);
    SEQ_LCD_PrintFormattedString("%3d", SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_COLOR)-64);
  }
  SEQ_LCD_PrintSpaces(1);

  ///////////////////////////////////////////////////////////////////////////
  if( (ui_selected_item == ITEM_VIRUSFX_FEEDBACK && ui_cursor_flash) || mode == 2) {
    SEQ_LCD_PrintSpaces(4);
  } else {
    SEQ_LCD_PrintVBar(SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_FEEDBACK) >> 4);
    SEQ_LCD_PrintFormattedString("%3d", SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_FEEDBACK));
  }
  SEQ_LCD_PrintSpaces(1);

  ///////////////////////////////////////////////////////////////////////////
  if (mode >= 2 && mode <= 4) {
    if( ui_selected_item == ITEM_VIRUSFX_SHAPE_DAMPING && ui_cursor_flash ) {
      SEQ_LCD_PrintSpaces(4);
    } else {
      SEQ_LCD_PrintVBar(SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_SHAPE_DAMPING) >> 4);
      SEQ_LCD_PrintFormattedString("%3d", SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_SHAPE_DAMPING));
    }
  } else {
    if( ui_selected_item == ITEM_VIRUSFX_DEPTH_TYPE && ui_cursor_flash ) {
      SEQ_LCD_PrintSpaces(4);
    } else {
      SEQ_LCD_PrintVBar(SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_DEPTH_TYPE) >> 4);
      SEQ_LCD_PrintFormattedString("%3d", SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_DEPTH_TYPE));
    }
  }
  SEQ_LCD_PrintSpaces(1);  


  ///////////////////////////////////////////////////////////////////////////
  if( ui_selected_item == ITEM_VIRUSFX_RATE_DECTIME && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(4);
  } else {
    SEQ_LCD_PrintVBar(SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_RATE_DECTIME) >> 4);
    SEQ_LCD_PrintFormattedString("%3d", SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_RATE_DECTIME));
  }
  SEQ_LCD_PrintSpaces(1);

  ///////////////////////////////////////////////////////////////////////////
  if( (ui_selected_item == ITEM_VIRUSFX_TIME_PREDLY && ui_cursor_flash) || SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_CLOCK)) {
    SEQ_LCD_PrintSpaces(8);
  } else {
    u8 val = SEQ_CC_Get(visible_track, SEQ_CC_VIRUSFX_TIME_PREDLY);
    SEQ_LCD_PrintFormattedString("  %3d ms", (int)(5.464*val));
  }

  ///////////////////////////////////////////////////////////////////////////


  SEQ_LCD_PrintSpaces(7);

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_UI_FX_VIRUSFX_Init(u32 mode)
{
  // install callback routines
  SEQ_UI_InstallButtonCallback(Button_Handler);
  SEQ_UI_InstallEncoderCallback(Encoder_Handler);
  SEQ_UI_InstallLEDCallback(LED_Handler);
  SEQ_UI_InstallLCDCallback(LCD_Handler);

  return 0; // no error
}
//####################################
//# RIO: END MODIFICATION
//####################################