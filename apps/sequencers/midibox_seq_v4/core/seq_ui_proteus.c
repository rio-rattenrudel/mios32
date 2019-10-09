//####################################
//# RIO: PROTEUS added
//####################################

/*
 * Proteus page
 *
 * ==========================================================================
 *
 *  Rio
 *
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>
#include <string.h>
#include "seq_lcd.h"
#include "seq_ui.h"
#include "seq_midi_port.h"
#include "seq_midi_sysex.h"


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

u8 proteus_typ;
u8 proteus_id1 = 1;
u8 proteus_id2 = 2;
u8 proteus_out_port1 = 50;
u8 proteus_out_port2 = 51;
u8 proteus_sel_ch1 = 0;
u8 proteus_sel_ch2 = 0;


/////////////////////////////////////////////////////////////////////////////
// Local encoder callback function
// Should return:
//   1 if value has been changed
//   0 if value hasn't been changed
//  -1 if invalid or unsupported encoder
/////////////////////////////////////////////////////////////////////////////
static s32 Encoder_Handler(seq_ui_encoder_t encoder, s32 incrementer)
{
  if( (encoder >= SEQ_UI_ENCODER_GP9) && (encoder <= SEQ_UI_ENCODER_GP11) ) {

    u8 port_ix;

    if (proteus_typ == 1) {
      port_ix = SEQ_MIDI_PORT_OutIxGet(proteus_out_port1);
      if( SEQ_UI_Var8_Inc(&port_ix, 0, SEQ_MIDI_PORT_OutNumGet()-1, incrementer) >= 0 ) {
      	proteus_out_port1 = SEQ_MIDI_PORT_OutPortGet(port_ix);
        return 1; // value changed
      }
    } else {
      port_ix = SEQ_MIDI_PORT_OutIxGet(proteus_out_port2);
      if( SEQ_UI_Var8_Inc(&port_ix, 0, SEQ_MIDI_PORT_OutNumGet()-1, incrementer) >= 0 ) {
      	proteus_out_port2 = SEQ_MIDI_PORT_OutPortGet(port_ix);
        return 1; // value changed
      }
    }
  }

  if( encoder >= SEQ_UI_ENCODER_GP12 ) {

    if (proteus_typ == 1) {
      if( SEQ_UI_Var8_Inc(&proteus_id1, 0, 126, incrementer) >= 0 ) return 1; // value changed
    } else {
      if( SEQ_UI_Var8_Inc(&proteus_id2, 0, 126, incrementer) >= 0 ) return 1; // value changed
    }
  }

  return 0; // no change
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
  switch( button ) {
    case SEQ_UI_BUTTON_GP1:
    case SEQ_UI_BUTTON_GP2:
    case SEQ_UI_BUTTON_GP3:
    case SEQ_UI_BUTTON_GP4:
    case SEQ_UI_BUTTON_GP5:
    case SEQ_UI_BUTTON_GP6:
    case SEQ_UI_BUTTON_GP7:
    case SEQ_UI_BUTTON_GP8:
    case SEQ_UI_BUTTON_GP9:
    case SEQ_UI_BUTTON_GP10:
    case SEQ_UI_BUTTON_GP11:
    case SEQ_UI_BUTTON_GP12:
    case SEQ_UI_BUTTON_GP13:
    case SEQ_UI_BUTTON_GP14:
    case SEQ_UI_BUTTON_GP15:
    case SEQ_UI_BUTTON_GP16:
        if( depressed ) return 0; // ignore when button depressed

        if (proteus_typ == 1) {
          proteus_sel_ch1 = button+1;
          SEQ_MIDI_SYSEX_PROTEUS_SendChannel(proteus_out_port1,proteus_id1,proteus_sel_ch1-1);
        } else {
          proteus_sel_ch2 = button+1;
          SEQ_MIDI_SYSEX_PROTEUS_SendChannel(proteus_out_port2,proteus_id2,proteus_sel_ch2-1);
        }

      return 1; // nothing to do for button
  }

  return -1; // invalid or unsupported button
}

/////////////////////////////////////////////////////////////////////////////
// Local LED handler function
/////////////////////////////////////////////////////////////////////////////
static s32 LED_Handler(u16 *gp_leds)
{
  if (proteus_typ == 1) {

      if (proteus_sel_ch1)  *gp_leds = 1 << (proteus_sel_ch1-1);
      else                  *gp_leds = 0x0000;

  } else {

      if (proteus_sel_ch2)  *gp_leds = 1 << (proteus_sel_ch2-1);
      else                  *gp_leds = 0x0000;

  }

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Local Display Handler function
// IN: <high_prio>: if set, a high-priority LCD update is requested
/////////////////////////////////////////////////////////////////////////////
static s32 LCD_Handler(u8 high_prio)
{
  if( high_prio )
    return 0; // there are no high-priority update

  // layout:
  // 00000000001111111111222222222233333333330000000000111111111122222222223333333333
  // 01234567890123456789012345678901234567890123456789012345678901234567890123456789
  // <--------------------------------------><-------------------------------------->
  // Trk.        Utility Functions                       Quick Menu Change
  // G1T1 Copy Paste Clr Move Scrl Rand Undo Save Rec. Live Opt. PMte Disk Mute UnMte

  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_CursorSet(0, 0);

  if (proteus_typ == 1) {

      SEQ_LCD_PrintString("Proteus 2000                            ");

      SEQ_LCD_PrintString("Port: ");
      SEQ_LCD_PrintMIDIOutPort(proteus_out_port1);
      SEQ_LCD_PrintString("      ID:");

      SEQ_LCD_PrintFormattedString("%3d", proteus_id1);
      SEQ_LCD_PrintSpaces(18);

  } else {

      SEQ_LCD_PrintString("Proteus XL-Turbo                        ");

      SEQ_LCD_PrintString("Port: ");
      SEQ_LCD_PrintMIDIOutPort(proteus_out_port2);
      SEQ_LCD_PrintString("      ID:");

      SEQ_LCD_PrintFormattedString("%3d", proteus_id2);
      SEQ_LCD_PrintSpaces(18);

  }


  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_CursorSet(0, 1);

  unsigned char i;
  for (i = 1; i < 17; i++) {


    if (proteus_typ == 1) {

      if ( i == proteus_sel_ch1) {

        if( ui_cursor_flash ) SEQ_LCD_PrintSpaces(5);
        else                  SEQ_LCD_PrintFormattedString(" #%2d ", proteus_sel_ch1);

      } else SEQ_LCD_PrintFormattedString(" #%2d ", i);

    } else {

      if ( i == proteus_sel_ch2) {

        if( ui_cursor_flash ) SEQ_LCD_PrintSpaces(5);
        else                  SEQ_LCD_PrintFormattedString(" #%2d ", proteus_sel_ch2);

      } else SEQ_LCD_PrintFormattedString(" #%2d ", i);

    }
  }

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_UI_PROTEUS_Init(u32 mode)
{
  proteus_typ       = 1;

  // install callback routines
  SEQ_UI_InstallButtonCallback(Button_Handler);
  SEQ_UI_InstallEncoderCallback(Encoder_Handler);
  SEQ_UI_InstallLEDCallback(LED_Handler);
  SEQ_UI_InstallLCDCallback(LCD_Handler);

  ui_hold_msg_ctr = 0;

  return 0; // no error
}

s32 SEQ_UI_XLTURBO_Init(u32 mode)
{
  proteus_typ       = 2;

  // install callback routines
  SEQ_UI_InstallButtonCallback(Button_Handler);
  SEQ_UI_InstallEncoderCallback(Encoder_Handler);
  SEQ_UI_InstallLEDCallback(LED_Handler);
  SEQ_UI_InstallLCDCallback(LCD_Handler);

  ui_hold_msg_ctr = 0;

  return 0; // no error
}

//####################################
//# RIO: END MODIFICATION
//####################################