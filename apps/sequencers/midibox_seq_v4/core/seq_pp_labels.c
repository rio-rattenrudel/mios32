// $Id$
/*
 * PP Label array
 *
 * ==========================================================================
 *
 *  Access Virus Page B - PolyPressure Assignments
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>

#include "seq_pp_labels.h"


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

static const char pp_labels[128][9] = {
  // 0x00..0x0f
  "        ",
  "Arp Mode",
  "Arp PSel",
  "Arp Oct ",
  "Arp Hold",
  "Arp NLen",
  "Arp Swng",
  "LF3 Rate",
  "LF3 Shpe",
  "LF3 Mode",
  "LF3 KeyF",
  "LF3 Dest",
  "LF3 Amnt",
  "LF3 Fade",
  "        ",
  "        ",
  
  // 0x10..0x1f
  "Clk Tmpo",
  "Arp Clk ",
  "LF1 Clk ",
  "LF2 Clk ",
  "Dly Clk ",
  "LF3 Clk ",
  "        ",
  "        ",
  "        ",
  "Ctr Smoo",
  "Bnd Up  ",
  "Bnd Down",
  "Bnd Scle",
  "        ",
  "FL1 EPol",
  "FL2 EPol",
  
  // 0x20..0x2f
  "FL2 Link",
  "FLt Base",
  "OSC FM M",
  "OSC Phas",
  "PunchInt",
  "        ",
  "InFollow",
  "Voc Mode",
  "        ",
  "OS3 Mode",
  "OS3 Vol ",
  "OS3 Semi",
  "OS3 Detu",
  "        ",
  "        ",
  "OS1 SVel",
  
  // 0x30..0x3f
  "OS2 SVel",
  "PW  Velo",
  "FM  AVel",
  "        ",
  "        ",
  "        ",
  "FL1 EAVe",
  "FL2 EAVe",
  "RE1 Velo",
  "RE2 Velo",
  "Out2 Bal",
  "        ",
  "Amp Velo",
  "Pan Velo",
  "Def1 Sng",
  "Def2 Sng",
  
  // 0x40..0x4f
  "As1 Src ",
  "As1 Dest",
  "As1 Amt ",
  "As2 Src ",
  "As2 Des1",
  "As2 Amt1",
  "As2 Des2",
  "As2 Amt2",
  "As3 Src ",
  "As3 Des1",
  "As3 Amt1",
  "As3 Des2",
  "As3 Amt2",
  "As3 Des3",
  "As3 Amt3",
  "LF1 AsDs",

  // 0x50..0x5f
  "LF1 AsAm",
  "LF2 AsDs",
  "LF2 AsAm",
  "        ",
  "Phs Mode",
  "Phs Mix ",
  "Phs Rate",
  "Phs Deph",
  "Phs Freq",
  "Phs Feed",
  "Phs Sprd",
  "        ",
  "        ",
  "        ",
  "        ",
  "        ",
  
  // 0x60..0x6f
  "        ",
  "BassInt ",
  "BassTune",
  "InRingMo",
  "DistCurv",
  "DistInt ",
  "        ",
  "        ",
  "        ",
  "        ",
  "        ",
  "        ",
  "        ",
  "        ",
  "        ",
  "        ",
  
  // 0x70..0x7f
  "SNameCh1",
  "SNameCh2",
  "SNameCh3",
  "SNameCh4",
  "SNameCh5",
  "SNameCh6",
  "SNameCh7",
  "SNameCh8",
  "SNameCh9",
  "SNameC10",
  "FLT Sel ",
  "Categor1",
  "Categor2",
  "        ",
  "        ",
  "        ",
};



/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_PP_LABELS_Init(u32 mode)
{
  // here we could also generate the labels in RAM...

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Returns PP label
/////////////////////////////////////////////////////////////////////////////
const char *SEQ_PP_LABELS_Get(u8 pp)
{
  if( pp >= 128 )
    pp = 0; // just to avoid buffer overruns if invalid PP number selected

  return pp_labels[pp];
}
