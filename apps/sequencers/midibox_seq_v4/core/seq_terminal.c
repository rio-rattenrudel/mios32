// $Id$
/*
 * MIDIbox SEQ MIDI Terminal
 *
 * ==========================================================================
 *
 *  Copyright (C) 2010 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>
#include <string.h>

#include <seq_midi_out.h>
#include <ff.h>

#include "tasks.h"

#include "app.h"
#include "seq_terminal.h"


#include "seq_core.h"
#include "seq_song.h"
#include "seq_cc.h"
#include "seq_layer.h"
#include "seq_par.h"
#include "seq_trg.h"
#include "seq_mixer.h"
#include "seq_midi_port.h"

#include "seq_file.h"
#include "seq_file_b.h"
#include "seq_file_s.h"
#include "seq_file_m.h"
#include "seq_file_g.h"
#include "seq_file_c.h"
#include "seq_file_gc.h"
#include "seq_file_hw.h"

#include "seq_statistics.h"

#if !defined(MIOS32_FAMILY_EMULATION)
#include "umm_malloc.h"
#include "uip.h"
#include "uip_task.h"
#include "osc_server.h"
#endif


/////////////////////////////////////////////////////////////////////////////
// Local defines
/////////////////////////////////////////////////////////////////////////////

#define STRING_MAX 80


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

static u8 line_buffer[STRING_MAX];
static u16 line_ix;


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_TERMINAL_Init(u32 mode)
{
  // install the callback function which is called on incoming characters
  // from MIOS Terminal
  MIOS32_MIDI_DebugCommandCallback_Init(SEQ_TERMINAL_Parse);

  // clear line buffer
  line_buffer[0] = 0;
  line_ix = 0;

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// help function which parses a decimal or hex value
// returns >= 0 if value is valid
// returns -1 if value is invalid
/////////////////////////////////////////////////////////////////////////////
static s32 get_dec(char *word)
{
  if( word == NULL )
    return -1;

  char *next;
  long l = strtol(word, &next, 0);

  if( word == next )
    return -1;

  return l; // value is valid
}


/////////////////////////////////////////////////////////////////////////////
// Parser
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_TERMINAL_Parse(mios32_midi_port_t port, u8 byte)
{
  // temporary change debug port (will be restored at the end of this function)
  mios32_midi_port_t prev_debug_port = MIOS32_MIDI_DebugPortGet();
  MIOS32_MIDI_DebugPortSet(port);

  if( byte == '\r' ) {
    // ignore
  } else if( byte == '\n' ) {
    // example for parsing the command:
    char *separators = " \t";
    char *brkt;
    char *parameter;

    if( parameter = strtok_r(line_buffer, separators, &brkt) ) {
      if( strcmp(parameter, "help") == 0 ) {
	SEQ_TERMINAL_PrintHelp(DEBUG_MSG);
      } else if( strcmp(parameter, "system") == 0 ) {
	SEQ_TERMINAL_PrintSystem(DEBUG_MSG);
      } else if( strcmp(parameter, "global") == 0 ) {
	SEQ_TERMINAL_PrintGlobalConfig(DEBUG_MSG);
      } else if( strcmp(parameter, "config") == 0 ) {
	SEQ_TERMINAL_PrintSessionConfig(DEBUG_MSG);
      } else if( strcmp(parameter, "tracks") == 0 ) {
	SEQ_TERMINAL_PrintTracks(DEBUG_MSG);
      } else if( strcmp(parameter, "track") == 0 ) {
	  char *arg;
	  if( (arg = strtok_r(NULL, separators, &brkt)) ) {
	    int track = get_dec(arg);
	    if( track < 1 || track > SEQ_CORE_NUM_TRACKS ) {
	      MUTEX_MIDIOUT_TAKE;
	      DEBUG_MSG("Wrong track number %d - expected track 1..%d\n", track, SEQ_CORE_NUM_TRACKS);
	      MUTEX_MIDIOUT_GIVE;
	    } else {
	      SEQ_TERMINAL_PrintTrack(DEBUG_MSG, track-1);
	    }
	  } else {
	    MUTEX_MIDIOUT_TAKE;
	    DEBUG_MSG("Please specify track, e.g. \"track 1\"\n");
	    MUTEX_MIDIOUT_GIVE;
	  }
      } else if( strcmp(parameter, "mixer") == 0 ) {
	SEQ_TERMINAL_PrintCurrentMixerMap(DEBUG_MSG);
      } else if( strcmp(parameter, "song") == 0 ) {
	SEQ_TERMINAL_PrintCurrentSong(DEBUG_MSG);
      } else if( strcmp(parameter, "grooves") == 0 ) {
	SEQ_TERMINAL_PrintGrooveTemplates(DEBUG_MSG);
      } else if( strcmp(parameter, "memory") == 0 ) {
	SEQ_TERMINAL_PrintMemoryInfo(DEBUG_MSG);
      } else if( strcmp(parameter, "network") == 0 ) {
	SEQ_TERMINAL_PrintNetworkInfo(DEBUG_MSG);
      } else if( strcmp(parameter, "sdcard") == 0 ) {
	SEQ_TERMINAL_PrintSdCardInfo(DEBUG_MSG);
      } else if( strcmp(parameter, "reset") == 0 ) {
	MIOS32_SYS_Reset();
      } else {
	MUTEX_MIDIOUT_TAKE;
	DEBUG_MSG("Unknown command - type 'help' to list available commands!\n");
	MUTEX_MIDIOUT_GIVE;
      }
    }

    line_ix = 0;

  } else if( line_ix < (STRING_MAX-1) ) {
    line_buffer[line_ix++] = byte;
    line_buffer[line_ix] = 0;
  }

  // restore debug port
  MIOS32_MIDI_DebugPortSet(prev_debug_port);

  return 0; // no error
}


s32 SEQ_TERMINAL_PrintHelp(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;

  MUTEX_MIDIOUT_TAKE;
  out("Welcome to " MIOS32_LCD_BOOT_MSG_LINE1 "!");
  out("Following commands are available:");
  out("  system:         print system info\n");
  out("  global:         print global configuration\n");
  out("  config:         print local session configuration\n");
  out("  tracks:         print overview of all tracks\n");
  out("  track <track>:  print info about a specific track\n");
  out("  mixer:          print current mixer map\n");
  out("  song:           print current song info\n");
  out("  grooves:        print groove templates\n");
  out("  memory:         print memory allocation info\n");
  out("  sdcard:         print SD Card info\n");
  out("  network:        print ethernet network info\n");
  out("  reset:          resets the MIDIbox SEQ (!)\n");
  out("  help:           this page\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}


s32 SEQ_TERMINAL_PrintSystem(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;
  char str_buffer[128];

  MUTEX_MIDIOUT_TAKE;

  out("System Informations:\n");
  out("====================\n");
  out(MIOS32_LCD_BOOT_MSG_LINE1 " " MIOS32_LCD_BOOT_MSG_LINE2 "\n");

  mios32_sys_time_t t = MIOS32_SYS_TimeGet();
  int hours = (t.seconds / 3600) % 24;
  int minutes = (t.seconds % 3600) / 60;
  int seconds = (t.seconds % 3600) % 60;

  out("Operating System: MIOS32\n");
  out("Board: " MIOS32_BOARD_STR "\n");
  out("Chip Family: " MIOS32_FAMILY_STR "\n");
  if( MIOS32_SYS_SerialNumberGet((char *)str_buffer) >= 0 )
    out("Serial Number: %s\n", str_buffer);
  else
    out("Serial Number: ?\n");
  out("Flash Memory Size: %d bytes\n", MIOS32_SYS_FlashSizeGet());
  out("RAM Size: %d bytes\n", MIOS32_SYS_RAMSizeGet());

  out("Systime: %02d:%02d:%02d\n", hours, minutes, seconds);
  out("CPU Load: %02d%%\n", SEQ_STATISTICS_CurrentCPULoad());
  out("MIDI Scheduler: Alloc %3d/%3d Drops: %3d",
	    seq_midi_out_allocated, seq_midi_out_max_allocated, seq_midi_out_dropouts);

  u32 stopwatch_value_max = SEQ_STATISTICS_StopwatchGetValueMax();
  u32 stopwatch_value = SEQ_STATISTICS_StopwatchGetValue();
  if( stopwatch_value_max == 0xffffffff ) {
    out("Stopwatch: Overrun!\n");
  } else if( !stopwatch_value_max ) {
    out("Stopwatch: no result yet\n");
  } else {
    out("Stopwatch: %d/%d uS\n", stopwatch_value, stopwatch_value_max);
  }

#if !defined(MIOS32_FAMILY_EMULATION) && configGENERATE_RUN_TIME_STATS
  // send Run Time Stats to MIOS terminal
  out("FreeRTOS Task RunTime Stats:\n");
  FREERTOS_UTILS_RunTimeStats();
#endif

  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}


s32 SEQ_TERMINAL_PrintGlobalConfig(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;

  MUTEX_MIDIOUT_TAKE;
  out("Global Configuration:\n");
  out("=====================\n");
  SEQ_FILE_GC_Debug();

  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}

s32 SEQ_TERMINAL_PrintSessionConfig(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;

  MUTEX_MIDIOUT_TAKE;
  out("Session Configuration:\n");
  out("======================\n");
  SEQ_FILE_C_Debug();

  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}

s32 SEQ_TERMINAL_PrintTracks(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;
  char str_buffer[128];

  MUTEX_MIDIOUT_TAKE;
  out("Track Overview:\n");
  out("===============\n");

  out("| Track | Mode  | Layer P/T/I | Steps P/T | Length | Port  | Chn. | Muted |\n");
  out("+-------+-------+-------------+-----------+--------+-------+------+-------+\n");

  u8 track;
  for(track=0; track<SEQ_CORE_NUM_TRACKS; ++track) {
    seq_event_mode_t event_mode = SEQ_CC_Get(track, SEQ_CC_MIDI_EVENT_MODE);
    u16 num_instruments = SEQ_TRG_NumInstrumentsGet(track);
    u16 num_par_layers = SEQ_PAR_NumLayersGet(track);
    u16 num_par_steps = SEQ_PAR_NumStepsGet(track);
    u16 num_trg_layers = SEQ_TRG_NumLayersGet(track);
    u16 num_trg_steps = SEQ_TRG_NumStepsGet(track);
    u16 length = (u16)SEQ_CC_Get(track, SEQ_CC_LENGTH) + 1;
    mios32_midi_port_t midi_port = SEQ_CC_Get(track, SEQ_CC_MIDI_PORT);
    u8 midi_chn = SEQ_CC_Get(track, SEQ_CC_MIDI_CHANNEL) + 1;

    sprintf(str_buffer, "| G%dT%d  | %s |",
	    (track/4)+1, (track%4)+1,
	    SEQ_LAYER_GetEvntModeName(event_mode));

    sprintf((char *)(str_buffer + strlen(str_buffer)), "   %2d/%2d/%2d  |  %3d/%3d  |   %3d  | %s%c |  %2d  |",
	    num_par_layers, num_trg_layers, num_instruments, 
	    num_par_steps, num_trg_steps,
	    length,
	    SEQ_MIDI_PORT_OutNameGet(SEQ_MIDI_PORT_OutIxGet(midi_port)),
	    SEQ_MIDI_PORT_OutCheckAvailable(midi_port) ? ' ' : '*',
	    midi_chn);

    if( seq_core_trk_muted & (1 << track) )
      sprintf((char *)(str_buffer + strlen(str_buffer)), "  yes  |\n");
    else if( seq_core_trk[track].layer_muted )
      sprintf((char *)(str_buffer + strlen(str_buffer)), " layer |\n");
    else
      sprintf((char *)(str_buffer + strlen(str_buffer)), "  no   |\n");

    out(str_buffer);
  }

  out("+-------+-------+-------------+-----------+--------+-------+------+-------+\n");

  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}

s32 SEQ_TERMINAL_PrintTrack(void *_output_function, u8 track)
{
  void (*out)(char *format, ...) = _output_function;

  MUTEX_MIDIOUT_TAKE;

  out("Track Parameters of G%dT%d", (track/4)+1, (track%4)+1);
  out("========================\n");

  SEQ_FILE_T_Debug(track);

  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}

s32 SEQ_TERMINAL_PrintCurrentMixerMap(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;
  char str_buffer[128];

  MUTEX_MIDIOUT_TAKE;
  u8 map = SEQ_MIXER_NumGet();
  int i;

  out("Mixer Map #%3d\n", map+1);
  out("==============\n");

  out("|Num|Port|Chn|Prg|Vol|Pan|Rev|Cho|Mod|CC1|CC2|CC3|CC4|C1A|C2A|C3A|C4A|\n");
  out("+---+----+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+\n");

  for(i=0; i<16; ++i) {
    sprintf(str_buffer, "|%3d|%s|", i, SEQ_MIDI_PORT_OutNameGet(SEQ_MIDI_PORT_OutIxGet(SEQ_MIXER_Get(i, SEQ_MIXER_PAR_PORT))));

    int par;

    for(par=1; par<2; ++par)
      sprintf((char *)(str_buffer + strlen(str_buffer)), "%3d|", SEQ_MIXER_Get(i, par)+1);

    for(par=2; par<12; ++par) {
      u8 value = SEQ_MIXER_Get(i, par);
      if( value )
	sprintf((char *)(str_buffer + strlen(str_buffer)), "%3d|", value-1);
      else
	sprintf((char *)(str_buffer + strlen(str_buffer)), " - |");
    }

    for(par=12; par<16; ++par)
      sprintf((char *)(str_buffer + strlen(str_buffer)), "%3d|", SEQ_MIXER_Get(i, par));

    out("%s\n", str_buffer);
  }

  out("+---+----+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+\n");
  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}


s32 SEQ_TERMINAL_PrintCurrentSong(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;
  u8 song = SEQ_SONG_NumGet();

  MUTEX_MIDIOUT_TAKE;
  int i;

  out("Song #%2d\n", song+1);
  out("========\n");

  out("Name: '%s'\n", seq_song_name);
  MIOS32_MIDI_SendDebugHexDump((u8 *)&seq_song_steps[0], SEQ_SONG_NUM_STEPS*sizeof(seq_song_step_t));

  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}

s32 SEQ_TERMINAL_PrintGrooveTemplates(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;

  MUTEX_MIDIOUT_TAKE;
  out("Groove Templates:\n");
  out("=================\n");
  SEQ_FILE_G_Debug();

  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}

extern void * ptr_array[256];
s32 SEQ_TERMINAL_PrintMemoryInfo(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;
  // TODO: umm_info doesn't allow to define output function

#if !defined(MIOS32_FAMILY_EMULATION)
  umm_info( NULL, 1 );
#endif

  return 0; // no error
}


///////////////////////////////////////////////////////////////////
// These time and date functions and other bits of following code were adapted from 
// Rickey's world of Microelectronics under the creative commons 2.5 license.
// http://www.8051projects.net/mmc-sd-interface-fat16/final-code.php
static void ShowFatTime(u32 ThisTime, char* msg)
{
   u8 AM = 1;

   int Hour, Minute, Second;

   Hour = ThisTime >> 11;        // bits 15 through 11 hold Hour...
   Minute = ThisTime & 0x07E0;   // bits 10 through 5 hold Minute... 0000 0111 1110 0000
   Minute = Minute >> 5;
   Second = ThisTime & 0x001F;   //bits 4 through 0 hold Second...   0000 0000 0001 1111
   
   if( Hour > 11 )
   {
      AM = 0;
      if( Hour > 12 )
         Hour -= 12;
   }
     
   sprintf( msg, "%02d:%02d:%02d %s", Hour, Minute, Second*2,
         (AM)?"AM":"PM");
   return;
}

static void ShowFatDate(u32 ThisDate, char* msg)
{

   int Year, Month, Day;

   Year = ThisDate >> 9;         // bits 15 through 9 hold year...
   Month = ThisDate & 0x01E0;    // bits 8 through 5 hold month... 0000 0001 1110 0000
   Month = Month >> 5;
   Day = ThisDate & 0x001F;      //bits 4 through 0 hold day...    0000 0000 0001 1111
   sprintf( msg, "%02d/%02d/%02d", Month, Day, Year-20);
   return;
}


s32 SEQ_TERMINAL_PrintSdCardInfo(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;

  FRESULT res;
  FILINFO fno;
  DIR dir;
  int i;
  char *fn;
  char str_buffer[128];

  MUTEX_MIDIOUT_TAKE;

  out("SD Card Informations\n");
  out("====================\n");

#if !defined(MIOS32_FAMILY_EMULATION)
  // this yield ensures, that Debug Messages are sent before we continue the execution
  // Since MIOS Studio displays the time at which the messages arrived, this allows
  // us to measure the delay of following operations
  taskYIELD();

  MUTEX_SDCARD_TAKE;
  SEQ_FILE_PrintSDCardInfos();
  MUTEX_SDCARD_GIVE;
#endif

  out("\n");
  out("Reading Root Directory\n");
  out("======================\n");

  taskYIELD();

  if( !SEQ_FILE_SDCardAvailable() ) {
    sprintf(str_buffer, "not connected");
  } else if( !SEQ_FILE_VolumeAvailable() ) {
    sprintf(str_buffer, "Invalid FAT");
  } else {
    out("Retrieving SD Card informations - please wait!\n");
    MUTEX_MIDIOUT_GIVE;
    MUTEX_SDCARD_TAKE;
    SEQ_FILE_UpdateFreeBytes();
    MUTEX_SDCARD_GIVE;
    MUTEX_MIDIOUT_TAKE;

    sprintf(str_buffer, "'%s': %u of %u MB free", 
	    SEQ_FILE_VolumeLabel(),
	    (unsigned int)(SEQ_FILE_VolumeBytesFree()/1000000),
	    (unsigned int)(SEQ_FILE_VolumeBytesTotal()/1000000));
  }
  out("SD Card: %s\n", str_buffer);

  taskYIELD();

#if _USE_LFN
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

  MUTEX_SDCARD_TAKE;
  if( (res=f_opendir(&dir, "/")) != FR_OK ) {
    out("Failed to open root directory - error status: %d\n", res);
  } else {
    while( (f_readdir(&dir, &fno) == FR_OK) && fno.fname[0] ) {
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
      char date[10];
      ShowFatDate(fno.fdate,(char*)&date);
      char time[12];
      ShowFatTime(fno.ftime,(char*)&time);
      out("[%s%s%s%s%s%s%s] %s  %s   %s %u %s\n",
		(fno.fattrib & AM_RDO ) ? "r" : ".",
		(fno.fattrib & AM_HID ) ? "h" : ".",
		(fno.fattrib & AM_SYS ) ? "s" : ".",
		(fno.fattrib & AM_VOL ) ? "v" : ".",
		(fno.fattrib & AM_LFN ) ? "l" : ".",
		(fno.fattrib & AM_DIR ) ? "d" : ".",
		(fno.fattrib & AM_ARC ) ? "a" : ".",
		date,time,
		(fno.fattrib & AM_DIR) ? "<DIR>" : " ",
		fno.fsize,fn);
    }
  }
  MUTEX_SDCARD_GIVE;

  taskYIELD();

  out("\n");
  out("Checking SD Card at application layer\n");
  out("=====================================\n");


  {
    u8 bank;
    for(bank=0; bank<SEQ_FILE_B_NUM_BANKS; ++bank) {
      int num_patterns = SEQ_FILE_B_NumPatterns(bank);
      if( num_patterns )
	out("File MBSEQ_B%d.V4: valid (%d patterns)\n", bank+1, num_patterns);
      else
	out("File MBSEQ_B%d.V4: doesn't exist\n", bank+1, num_patterns);
    }

    int num_maps = SEQ_FILE_M_NumMaps();
    if( num_maps )
      out("File MBSEQ_M.V4: valid (%d mixer maps)\n", num_maps);
    else
      out("File MBSEQ_M.V4: doesn't exist\n");
    
    int num_songs = SEQ_FILE_S_NumSongs();
    if( num_songs )
      out("File MBSEQ_S.V4: valid (%d songs)\n", num_songs);
    else
      out("File MBSEQ_S.V4: doesn't exist\n");

    if( SEQ_FILE_G_Valid() )
      out("File MBSEQ_G.V4: valid\n");
    else
      out("File MBSEQ_G.V4: doesn't exist\n");
    
    if( SEQ_FILE_C_Valid() )
      out("File MBSEQ_C.V4: valid\n");
    else
      out("File MBSEQ_C.V4: doesn't exist\n");
    
    if( SEQ_FILE_HW_Valid() )
      out("File MBSEQ_HW.V4: valid\n");
    else
      out("File MBSEQ_HW.V4: doesn't exist or hasn't been re-loaded\n");
  }

  out("done.\n");
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}


s32 SEQ_TERMINAL_PrintNetworkInfo(void *_output_function)
{
  void (*out)(char *format, ...) = _output_function;

  MUTEX_MIDIOUT_TAKE;

#if defined(MIOS32_FAMILY_EMULATION)
  out("No network informations available in emulation!");
#else

  out("MBHP_ETH module connected: %s", UIP_TASK_NetworkDeviceAvailable() ? "yes" : "no");
  if( !UIP_TASK_NetworkDeviceAvailable() ) {
    out("Please reboot your MIDIbox SEQ to restart module detection!");
  } else {
    out("Ethernet services running: %s", UIP_TASK_ServicesRunning() ? "yes" : "no");
    out("DHCP: %s", UIP_TASK_DHCP_EnableGet() ? "enabled" : "disabled");

    if( UIP_TASK_DHCP_EnableGet() && !UIP_TASK_ServicesRunning() ) {
      out("IP address: not available yet");
      out("Netmask: not available yet");
      out("Default Router (Gateway): not available yet");
    } else {
      uip_ipaddr_t ipaddr;
      uip_gethostaddr(&ipaddr);
      out("IP address: %d.%d.%d.%d",
	  uip_ipaddr1(ipaddr), uip_ipaddr2(ipaddr),
	  uip_ipaddr3(ipaddr), uip_ipaddr4(ipaddr));

      uip_ipaddr_t netmask;
      uip_getnetmask(&netmask);
      out("Netmask: %d.%d.%d.%d",
	  uip_ipaddr1(netmask), uip_ipaddr2(netmask),
	  uip_ipaddr3(netmask), uip_ipaddr4(netmask));

      uip_ipaddr_t draddr;
      uip_getdraddr(&draddr);
      out("Default Router (Gateway): %d.%d.%d.%d",
	  uip_ipaddr1(draddr), uip_ipaddr2(draddr),
	  uip_ipaddr3(draddr), uip_ipaddr4(draddr));
    }

    u32 osc_remote_ip = OSC_SERVER_RemoteIP_Get();
    out("OSC Remote address: %d.%d.%d.%d",
	(osc_remote_ip>>24)&0xff, (osc_remote_ip>>16)&0xff,
	(osc_remote_ip>>8)&0xff, (osc_remote_ip>>0)&0xff);
    out("OSC Remote port: %d", OSC_SERVER_RemotePortGet());
    out("OSC Local port: %d", OSC_SERVER_LocalPortGet());
  }
#endif
  MUTEX_MIDIOUT_GIVE;

  return 0; // no error
}