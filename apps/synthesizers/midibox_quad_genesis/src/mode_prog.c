/*
 * MIDIbox Quad Genesis: Program Mode
 *
 * ==========================================================================
 *
 *  Copyright (C) 2016 Sauraen (sauraen@gmail.com)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#include <mios32.h>
#include "mode_prog.h"

#include "frontpanel.h"
#include "interface.h"
#include "syeng.h"
#include "mode_vgm.h"
#include "filebrowser.h"
#include "nameeditor.h"

synprogram_t* selprogram;

static u8 submode;
static u8 cursor;

static VgmSource** SelSource(synprogram_t* prog, u8 num){
    switch(num){
        case 0: return &prog->initsource;
        case 1: return &prog->noteonsource;
        case 2: return &prog->noteoffsource;
        default:
            DBG("SelSource error!");
            return &prog->noteonsource;
    }
}

static void DrawMenu(){
    MIOS32_LCD_Clear();
    MIOS32_LCD_CursorSet(0,0);
    if(selprogram == NULL){
        MIOS32_LCD_PrintString("No program selected to edit");
        MIOS32_LCD_CursorSet(0,1);
        MIOS32_LCD_PrintString("(Create or select program in Chan mode)");
        return;
    }
    switch(submode){
        case 0:
            MIOS32_LCD_PrintString(selprogram->name);
            MIOS32_LCD_CursorSet(15,0);
            MIOS32_LCD_PrintString("Root VGMs: Init KOn KOff ");
            MIOS32_LCD_CursorSet(0,1);
            MIOS32_LCD_PrintString("Name");
            MIOS32_LCD_CursorSet(16,1);
            MIOS32_LCD_PrintFormattedString("%d", selprogram->rootnote);
            MIOS32_LCD_CursorSet(25,1);
            MIOS32_LCD_PrintString(selprogram->initsource != NULL ? " [#] " : " --- ");
            MIOS32_LCD_CursorSet(30,1);
            MIOS32_LCD_PrintString(selprogram->noteonsource != NULL ? " [#] " : " --- ");
            MIOS32_LCD_CursorSet(35,1);
            MIOS32_LCD_PrintString(selprogram->noteoffsource != NULL ? " [#] " : " --- ");
            MIOS32_LCD_CursorSet(5*cursor,1);
            MIOS32_LCD_PrintChar('~');
            break;
    }
}

static void FilebrowserDone(char* filename){
    FrontPanel_LEDSet(FP_LED_LOAD, 0);
    if(filename == NULL || cursor < 5 || cursor > 7){
        DrawMenu();
        return;
    }
    VgmSource** ss = SelSource(selprogram, cursor-5);
    if(*ss != NULL){
        DrawMenu();
        return;
    }
    *ss = VGM_SourceStream_Create();
    VGM_SourceStream_Start(*ss, filename);
    Mode_Vgm_SelectVgm(*ss);
    Interface_ChangeToMode(MODE_VGM);
}

void Mode_Prog_Init(){
    submode = 0;
    cursor = 5;
}
void Mode_Prog_GotFocus(){
    DrawMenu();
}

void Mode_Prog_Tick(){

}
void Mode_Prog_Background(){

}

void Mode_Prog_BtnGVoice(u8 gvoice, u8 state){

}
void Mode_Prog_BtnSoftkey(u8 softkey, u8 state){
    if(selprogram == NULL) return;
    if(!state) return;
    switch(submode){
        case 0:
            if(softkey >= 5 || softkey == 3){
                cursor = softkey;
                DrawMenu();
            }else if(softkey == 0){
                NameEditor_Start(selprogram->name, 12, "Program", &DrawMenu);
            }
            break;
    }
}
void Mode_Prog_BtnSelOp(u8 op, u8 state){

}
void Mode_Prog_BtnOpMute(u8 op, u8 state){

}
void Mode_Prog_BtnSystem(u8 button, u8 state){
    if(selprogram == NULL) return;
    if(!state) return;
    if(button == FP_B_MENU){
        submode = 0;
        cursor = 5;
        DrawMenu();
        return;
    }
    switch(submode){
        case 0:
            switch(button){
                case FP_B_ENTER:
                    switch(cursor){
                        case 5:
                            Mode_Vgm_SelectVgm(selprogram->initsource);
                            break;
                        case 6:
                            Mode_Vgm_SelectVgm(selprogram->noteonsource);
                            break;
                        case 7:
                            Mode_Vgm_SelectVgm(selprogram->noteoffsource);
                            break;
                        default:
                            return;
                    }
                    Interface_ChangeToMode(MODE_VGM);
                    break;
                case FP_B_LOAD:
                    if(cursor >= 5 && cursor <= 7){
                        VgmSource** ss = SelSource(selprogram, cursor-5);
                        if(*ss != NULL){
                            MIOS32_LCD_CursorSet(0,0);
                            MIOS32_LCD_PrintString("Delete VGM before loading a new one!");
                        }else{
                            FrontPanel_LEDSet(FP_LED_LOAD, 1);
                            Filebrowser_Start("/VGM", "VGM", 1, &FilebrowserDone);
                        }
                    }
                    break;
                case FP_B_DELETE:
                    if(cursor >= 5 && cursor <= 7){
                        VgmSource** ss = SelSource(selprogram, cursor-5);
                        if(*ss != NULL){
                            Mode_Vgm_InvalidateVgm(*ss);
                            VGM_Source_Delete(*ss);
                            *ss = NULL;
                        }
                        DrawMenu();
                    }
                    break;
            }
            break;
    }
}
void Mode_Prog_BtnEdit(u8 button, u8 state){

}

void Mode_Prog_EncDatawheel(s32 incrementer){
    if(selprogram == NULL) return;
    switch(submode){
        case 0:
            if(cursor == 3){
                if((incrementer < 0 && (s32)selprogram->rootnote + incrementer >= 0) || (incrementer > 0 && (s32)selprogram->rootnote + incrementer <= 0x7F)){
                    selprogram->rootnote = (u8)((s32)selprogram->rootnote + incrementer);
                    MIOS32_LCD_CursorSet(16,1);
                    MIOS32_LCD_PrintFormattedString("%d  ", selprogram->rootnote);
                }
            }
            break;
    }
}
void Mode_Prog_EncEdit(u8 encoder, s32 incrementer){

}
