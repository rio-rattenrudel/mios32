// $Id$
/*
 * Header file for MIDI Input routines
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _SEQ_MIDI_IN_H
#define _SEQ_MIDI_IN_H


/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// number of transport/arpeggiator busses
#define SEQ_MIDI_IN_NUM_BUSSES 4

//#################################################
//# RIO: Little Remote Assignments
//#################################################
// External controller assignments
// They have to be aligned with the strings returned by the SEQ_MIDI_IN_ExtCtrlStr() function!
#define SEQ_MIDI_IN_EXT_CTRL_MORPH          0
#define SEQ_MIDI_IN_EXT_CTRL_SCALE          1
#define SEQ_MIDI_IN_EXT_CTRL_SONG           2
#define SEQ_MIDI_IN_EXT_CTRL_PHRASE         3
#define SEQ_MIDI_IN_EXT_CTRL_MIXER_MAP      4
#define SEQ_MIDI_IN_EXT_CTRL_MIXER_DUMP     5
#define SEQ_MIDI_IN_EXT_CTRL_PATTERN_G1     6
#define SEQ_MIDI_IN_EXT_CTRL_PATTERN_G2     7
#define SEQ_MIDI_IN_EXT_CTRL_PATTERN_G3     8
#define SEQ_MIDI_IN_EXT_CTRL_PATTERN_G4     9
#define SEQ_MIDI_IN_EXT_CTRL_BANK_G1       10
#define SEQ_MIDI_IN_EXT_CTRL_BANK_G2       11
#define SEQ_MIDI_IN_EXT_CTRL_BANK_G3       12
#define SEQ_MIDI_IN_EXT_CTRL_BANK_G4       13
#define SEQ_MIDI_IN_EXT_CTRL_ALL_NOTES_OFF 14
#define SEQ_MIDI_IN_EXT_CTRL_PLAY          15
#define SEQ_MIDI_IN_EXT_CTRL_RECORD        16
// RIO: Little Remote
#define SEQ_MIDI_IN_EXT_CTRL_LITTLE_REMOTE 17
#define SEQ_MIDI_IN_EXT_CTRL_LSTEP_REMOTE  18
#define SEQ_MIDI_IN_EXT_CTRL_LLIVE_REMOTE  19
#define SEQ_MIDI_IN_EXT_CTRL_LTEST_REMOTE  20
#define SEQ_MIDI_IN_EXT_CTRL_LNOFF_REMOTE  21
#define SEQ_MIDI_IN_EXT_CTRL_PATTERN_MUTE  22
#define SEQ_MIDI_IN_EXT_CTRL_LSVEL_REMOTE  23
#define SEQ_MIDI_IN_EXT_CTRL_LSLEN_REMOTE  24
#define SEQ_MIDI_IN_EXT_CTRL_CLOCK_TEMPO   25

// RIO: new numbers (original:16)
// number of external controllers which listen to CC (at the beginning of the list)
#define SEQ_MIDI_IN_EXT_CTRL_NUM_IX_CC     26

// not listen to CC
#define SEQ_MIDI_IN_EXT_CTRL_NRPN_ENABLED  26
#define SEQ_MIDI_IN_EXT_CTRL_PC_MODE       27

// extra CC handling (we listen to 16 CCs)
#define SEQ_MIDI_IN_EXT_CTRL_MUTES         28

// extra CC handling (we listen to 16 CCs)
#define SEQ_MIDI_IN_EXT_CTRL_STEPS         29

// total number of external controllers
#define SEQ_MIDI_IN_EXT_CTRL_NUM           30

//#################################################
//# RIO: END MODIFICATION
//#################################################


// program change modes
// They have to be aligned with the strings returned by the SEQ_MIDI_IN_ExtCtrlPcModeStr() function!
#define SEQ_MIDI_IN_EXT_CTRL_PC_MODE_OFF      0
#define SEQ_MIDI_IN_EXT_CTRL_PC_MODE_PATTERNS 1
#define SEQ_MIDI_IN_EXT_CTRL_PC_MODE_SONG     2
#define SEQ_MIDI_IN_EXT_CTRL_PC_MODE_PHRASE   3

// number of program change modes
#define SEQ_MIDI_IN_EXT_CTRL_PC_MODE_NUM      4


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

typedef union {
  u8 ALL;
  struct {
    u8 MODE_PLAY:1;
  };
} seq_midi_in_options_t;

typedef union {
  u8 ALL;
  struct {
    u8 value:7;     // key or CC value
    u8 cc_or_key:1; // 0: remote key, 1: remote CC
  };
} seq_midi_in_remote_t;


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 SEQ_MIDI_IN_Init(u32 mode);

extern const char *SEQ_MIDI_IN_ExtCtrlStr(u8 ext_ctrl);
extern const char *SEQ_MIDI_IN_ExtCtrlPcModeStr(u8 pc_mode);

// since there is currently no better place (somebody could expect this function in SEQ_MIDI_OUT...)
extern s32 SEQ_MIDI_IN_ExtCtrlSend(u8 ext_ctrl, u8 value, u8 cc_offset);

extern s32 SEQ_MIDI_IN_ResetTransArpStacks(void);
extern s32 SEQ_MIDI_IN_ResetChangerStacks(void);
extern s32 SEQ_MIDI_IN_ResetAllStacks(void);

extern s32 SEQ_MIDI_IN_Receive(mios32_midi_port_t port, mios32_midi_package_t midi_package);

extern s32 SEQ_MIDI_IN_ResetSingleTransArpStacks(u8 bus);
extern s32 SEQ_MIDI_IN_BusReceive(u8 bus, mios32_midi_package_t midi_package, u8 from_loopback_port);

extern s32 SEQ_MIDI_IN_TransposerNoteGet(u8 bus, u8 hold, u8 first_note);
extern s32 SEQ_MIDI_IN_ArpNoteGet(u8 bus, u8 hold, u8 sorted, u8 key_num);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern u8 seq_midi_in_channel[SEQ_MIDI_IN_NUM_BUSSES];
extern mios32_midi_port_t seq_midi_in_port[SEQ_MIDI_IN_NUM_BUSSES];
extern u8 seq_midi_in_lower[SEQ_MIDI_IN_NUM_BUSSES];
extern u8 seq_midi_in_upper[SEQ_MIDI_IN_NUM_BUSSES];
extern seq_midi_in_options_t seq_midi_in_options[SEQ_MIDI_IN_NUM_BUSSES];

extern u8 seq_midi_in_ext_ctrl_channel;
extern mios32_midi_port_t seq_midi_in_ext_ctrl_port;
extern mios32_midi_port_t seq_midi_in_ext_ctrl_out_port;
extern u8 seq_midi_in_ext_ctrl_asg[SEQ_MIDI_IN_EXT_CTRL_NUM];

extern seq_midi_in_remote_t seq_midi_in_remote;

extern u8 seq_midi_in_sect_channel;
extern mios32_midi_port_t seq_midi_in_sect_port;
extern mios32_midi_port_t seq_midi_in_sect_fwd_port;
extern u8 seq_midi_in_sect_note[4];


#endif /* _SEQ_MIDI_IN_H */
