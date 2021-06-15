MIOS32 - MBSEQ v4 - MOD
=======================
## Preamble

This fork contains heavy modifications to the MBSEQ v4 Master Branch. The customizations are largely compatible with the original version (especially for the LPC1769 core), but include the following changes. The master branch version on which the modifications are based lies in the branch 'original'. All changes in the source are uniformly marked with beginning and ending comment and described in detail in an explicite info file (in German).

## Modifications

1. [Hardware Config - rio/MBSEQ_HW.V4](https://github.com/rio-rattenrudel/mios32/blob/master/apps/sequencers/midibox_seq_v4/hwcfg/rio/MBSEQ_HW.V4)

* A new hardware-config file has been added: rio/MBSEQ_HW.V4, which contains a few own adjustments of my hardware.
  * Tap Tempo - Beat Controller Config
  * Clock Divider Config
  * Clock Divider Up/Down Buttons
  * Proteus / XLTurbo Buttons
  * Prev/Next Group Buttons
  * Prev/Next Track Buttons

  
2. [Set Default Mixer Map CCs (Bank #32 on CC4)](https://github.com/rio-rattenrudel/mios32/commit/f819b62f34d96abf60fec227359852559109abd5)

* The default CCs have been changed, also with the background of CC #32 (Bankchange) always ready to have.
  
3. [Using the BPM DataWheel in MUTE and PATTERN SCREEN](https://github.com/rio-rattenrudel/mios32/commit/2c5c8c7cf2d13bd586ba9d1d8777219e64fb6549)

* This is comfortable in these screens.

4. [Disable Board LEDs](https://github.com/rio-rattenrudel/mios32/commit/86219c6bfb30c0cdc701e08f40ed9ab4c7230f5b)

* The LEDs actually shimmered through the case and are irrelevant.

5. [Fixed: Selected Pattern Group - Display Update](https://github.com/rio-rattenrudel/mios32/commit/b4bb6aa1a2f24036bcb2a531e550694ea790fd64)

* This fix shows the patterns (you want to change) correctly.

6. [Changed: MIXER - Separated Load & Dump CC](https://github.com/rio-rattenrudel/mios32/commit/458a5f7cd9f34d44d8ce48d0c27219b6607d275e)

* Mixer load and mixer dump are available here as two independent CCs as they work completely independently of each other. "Load" - loads the map time-consuming from the SD. This can happen before a pending change. The "dump", however, sends the map directly out.

7. [Changed: Bank Change should be set, but not executed](https://github.com/rio-rattenrudel/mios32/commit/2d8ea3485af48c657afc8e980958978c032d15ca)

* Incoming bank changes should be registered, but not immediately cause a change. This is done by the pattern change.

8. Little Remote Functions

* Little Remote CCs were introduced to allow fast and direct working on the sequencer externally. Some functions allow fast movement within the views, other functions are for copying, pasting or editing steps. In addition, patterns can be muted or the tempo can be adjusted or, last but not least, a mixer dump can be triggered.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;a. [Feature: Little Remote Functions (+Mixer Dump +BPM)](https://github.com/rio-rattenrudel/mios32/commit/0fccefbcb7c1db8b09b6930d4b34577eadc78395)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;b. [Fixed: Little Remote Functions (BPM Assignments)](https://github.com/rio-rattenrudel/mios32/commit/8e949d27a419daaf9ac75bddbe35d7a8a1c28f91)

9. [Fixed: Visual Updates for playing pattern](https://github.com/rio-rattenrudel/mios32/commit/8557f20868a7f596aa51279c8c776041be89ebb9)

10. Expanded LFO

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;a. [Feature: Expanded LFO with special waveforms](https://github.com/rio-rattenrudel/mios32/commit/36a6266d499074261de17ed17fbb855e2cf63c79)

* The LFO has been enhanced with special waveforms: quantized saws and saws with different rising course, where in addition to the progression (quarter, half, 3 quarter, everything) a rate-multiplier and a quantizer can be set. Also, the progression can be reversed. In addition, the phase endpoint was corrected.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;b. [Feature: Expanded LFO II (Reset, Delayed, Display)](https://github.com/rio-rattenrudel/mios32/commit/a3b7baff0a7a6a9e20dac2870e94182e08a8f5c2)

* Furthermore, a reset count was introduced for the effect of rising spirals and a delay time was introduced before the LFO starts. The first version of a fade-out (fade high/low) was implemented (here still faulty).

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;c. [Fixed: Expanded LFO - Fade out](https://github.com/rio-rattenrudel/mios32/commit/51b5e481f5b528cd8d80142f7e39f4d4a72d4546)

* The fade-out feature has been fixed here.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;d. [Fixed: Use only the activated CC values of the tracks for the LFO + Refactoring](https://github.com/rio-rattenrudel/mios32/commit/8b3f09a02e9457242f621ea81ca903b38fc3da1b)

* _Note: affects data storage_

11. [Feature: Proteus 2000 Integration](https://github.com/rio-rattenrudel/mios32/commit/b444395fcd240f7cd091f3c7e82c5dac74b18755)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[[Updated Version]](https://github.com/rio-rattenrudel/mios32/commit/ebb8326ac450a5283028aa07efa46a3965ab6a7b)

* It is possible to directly select the channel of the Proteus devices - even if you have a proteus device. the associated buttons can be found in the hardware configuration file.

12. [Changed: Ignore Multi-Selection for tracks](https://github.com/rio-rattenrudel/mios32/commit/703445c0ea78d9cb6e468c1eadbb31fb4eb4b528)

* I find it difficult to work with multiple selection, and especially since it is possible to select the tracks much faster without the multiple selection.

13. [Feature: Chord3 - Replacement (root, inv I, inv II)](https://github.com/rio-rattenrudel/mios32/commit/a784a9f851fdbadba8240522bb1e1aeac09b7373)

* A complete and very own chord replacement with a lot of inversions

* _Note: affects data storage_

14. [Feature: Mutes Layer](https://github.com/rio-rattenrudel/mios32/commit/a42e063ed5b61e31ffaeedf5ccecf2914adc6a37)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[[Updated Version]](https://github.com/rio-rattenrudel/mios32/commit/619b7c046dd0e69f9d7798d0c96d919ff6d9d100)

* A mute-layer parameter is the fastest and most direct way (without latences) to automatically mute or unmute tracks.

* _Note: affects data storage_

15. [Changed: Use IIC-HACK firmware to allow 8 IIC port](https://github.com/rio-rattenrudel/mios32/commit/d485d7a08f962c78dbadc1680ed094ff28c01145)

* This hack allows you to use 2x Quad IIC modules in a row. The requirement is that the firmware of the IIC Pics get its own (and therefore higher) ID.

16. [Fixed: Start playback once - fixed bouncing button](https://github.com/rio-rattenrudel/mios32/commit/6e905d72e1d44befa386438e57925079c7e68041)

* Simple fix to prevent the bouncing of a more used button.

17. Poly Pressure Feature

* Special feature to enable sending of Poly Pressure events (equivalent to CC). It was less about the pressure (multi aftertouch) event itself, but rather to be able to control all the parameters of the Access Virus A,B,C... etc. Polypressure can be sent by Layer, Mixer or LFO.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;a. [Feature: Poly Pressure (Poly Aftertouch)](https://github.com/rio-rattenrudel/mios32/commit/4a8d6e5bdfd01b8758e158940f10daeec616a497)

* The Extra-CC assignment can be changed by the 13th button to PP (PolyPressure). Then the LFO will be sent out as a PolyPressure Event. In the track layer, the PP event can now also be selected equivalently to the CC.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;b. [Feature: Poly Pressure II (Mixer)](https://github.com/rio-rattenrudel/mios32/commit/712ecbb0d744e5b73b63051a25390812f2d1bc78)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[[Updated Version]](https://github.com/rio-rattenrudel/mios32/commit/619b7c046dd0e69f9d7798d0c96d919ff6d9d100)

* In the Mixer Map, the CC assignments (128..255) are considered PP assignments (PolyPressure). These PP assignments are represented by the number 1..127 and a '+' character.

* _Note: affects data storage_

18. [Feature: Tap Tempo - Beat Controller](https://github.com/rio-rattenrudel/mios32/commit/b7834298602f0dbf8bf8d3313261790ad375a609)

* This feature allows the (repeated) sending of tap tempo information to a selected port. The tap tempo is triggered again by "Start" and "BPM" updates.

19. Feature: Prev/Next Button Assignments

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;a. [Added: Prev/Next Group](https://github.com/rio-rattenrudel/mios32/commit/ce58c7ad02447bab5a2d71de3d0e329394826e37)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;b. [Added: Prev/Next Track](https://github.com/rio-rattenrudel/mios32/commit/457c1cc05d5c79efcd8d38a110f5012839b00c39)

* This feature allows the button assignment to select previous or next groups or tracks.

20. [Feature: Clock Shifter](https://github.com/rio-rattenrudel/mios32/commit/924bd56061c24017291fae6e4347e30b93476cf0)

* This feature adds the clock shifter algorithm to a selected port. A port and a standard offset can be defined in the hardware configuration. The offset ranges from 0..95 (= 1bar [4/4 beat], values> 48 representing the negative values). With a user adjustment (via buttons), the offset can be moved forward or backward indefinitely.

21. [Added: Flashing GATE Cursor](https://github.com/rio-rattenrudel/mios32/commit/e6dfaa13f1b68d5be0e43eac3a3bc2deac2cd3e9)

* With the feature, the step cursor for the GATE flashes in Edit-View.

22. [Feature: Auto Fast Mode 2](https://github.com/rio-rattenrudel/mios32/commit/afe1d2fefd1be49681b4ee5f6ab3899f36ad64f9)

* Now this hw config param (ENC_AUTO_FAST 2) activates the fast mode for all encoder/pages/par initially, what the encoder doesn't wear out so quickly.

23. [Datawheel Assignment / Secondary Button-Function (special behaviour)](https://github.com/rio-rattenrudel/mios32/commit/8507d87c8a25839ed837384db333334b7ac75c10)

* The mixer name is limited to 16 characters in order to store addition values for 1 mixer channel, 2 parameters and 1 mod to the Mixer Map. These 4 additional pieces of information are relevant for the data wheel assignment. In the standard case, the BPM value is changed in the mute and pattern page by the datawheel. Otherwise the defined parameters of the mixer channel are used. The mod value signals whether parameters should be used reverse (127..0 instead of 0..127), which can be selected using the buttons below the Par1 and Par2 info.

* The parameters can be selected during runtime in the mute or pattern page using DATAWHEEL ACTION and SECONDARY ACTION. There are also several modes behind it:

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;a. BPM can be halfed or doubled

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;b. PAR1 and PAR2 can be used single or combined with (+/-) operation

* The BUTTON_EXIT (normaly bound to options page) is used here (mute and pattern page) for toggling between fast / normal behavior. There is now also a BUTTON_FINETUNE_ENCODER config option to set the encoder movement to slow while it is pressed. In the edit page, the stepviews for the held DATAWHEEL ACTION Button can be selected with the datawheel. The SECONDARY ACTION button toggles between the Proteus Pages (P2000, XLTurbo).

* _Note: affects data storage_

24. [Peavey Spectrum Analog Filter CC To SysEx](https://github.com/rio-rattenrudel/mios32/commit/1e624004235df2f6309a85a010e0befec76cf97e)

* This feature enables the support of Peavey Spectrum Analog Filter, which only accepts SysEx for all parameters of EditBuffer. That's why the CC to SysEx conversion came into play. There is a new SysEx routine, which has been adapted in the mixer and seq_midi_out sources. Port, Channel, CC-Offset and the Vol (1..3) number for the velocity can be configured in the hardware file.

* _Note: affects data storage_

25. [Trigger Stepview Playback (instead pause button)](https://github.com/rio-rattenrudel/mios32/commit/82481f7fe9e3d4658851b3e73ac7b84427000da0)

* Alternatively, you can use the pause button to move the playing cursor to the visible stepview within the next synchronized measure event. It can be activated through the config file.

26. [Track FX: "Access Virus" / Mixer: FX Send](https://github.com/rio-rattenrudel/mios32/commit/a71b3d3c39eeaf02eab2c75e6af8e8bb97e7f2c1)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;a. [Fixed: Color Range -64..63](https://github.com/rio-rattenrudel/mios32/commit/f5a391b8c997a43dc8df606998456e4d7432b3ce)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;b. [Fixed: Initial Values](https://github.com/rio-rattenrudel/mios32/commit/6650bf8f1e48173ff3635721ebd4300d7195359b)

* This feature allows the configuration of the Access Virus Fx parameters in the Track FX section and is then sent to the selected Ch / Port. This is necessary in order to be able to individually control the Multi-FX parameters in the Virus’s Multi Single mode. The "Fx Send" can then be individually defined in the mixer map. Alternatively you can use the "FX Send 113" and "Expression 11" as standard parameters in the mixer map (via SEQ_MIXER_USE_ALTERNATIVE_PARAM Flag).

* _Note: affects data storage_

## Recompile sizes

```
Recompiling 4.097:
--------------------------------------------------------------------
Application successfully built for:
Processor: LPC1769
Family:    LPC17xx
Board:     MBHP_CORE_LPC17
LCD:       universal
--------------------------------------------------------------------

original size:
------------------
arm-none-eabi-size project_build/project.elf
   text    data     bss     dec     hex filename
 409684     936   62552  473172   73854 project_build/project.elf
10000000 B __ram_start
10007d58 B __ram_end
2007c000 D __ram_start_ahb
20083aa8 B __ram_end_ahb

modified size (with IIC Hack):
-----------------------------
arm-none-eabi-size project_build/project.elf
   text    data     bss     dec     hex filename
 423044     960   62944  486948   76e24 project_build/project.elf
10000000 B __ram_start
10007ee0 B __ram_end
2007c000 D __ram_start_ahb
20083ac0 B __ram_end_ahb
-----------------------------------------------------------------
```
