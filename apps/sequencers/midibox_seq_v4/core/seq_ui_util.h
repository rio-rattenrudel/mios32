//#################################################
//# RIO: Little Remote Functions
//#################################################

extern s32 COPY_Ext_Track(u8 track, u8 begin, u8 end);
extern s32 PASTE_Ext_Track(u8 track);
extern s32 MOVE_Ext_StoreStep(u8 track, u16 step, u8 buffer, u8 clr_triggers);
extern s32 MOVE_Ext_RestoreStep(u8 track, u16 step, u8 buffer);
extern s32 SCROLL_Ext_Track(u8 track, u16 first_step, s32 incrementer);

//#################################################
//# RIO: END MODIFICATION
//#################################################