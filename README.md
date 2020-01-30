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

* It is possible to directly select the channel of the Proteus devices - even if you have a proteus device. The metronome and scrubbing function have been overridden for this.

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
 407860     936   62528  471324   7311c project_build/project.elf
10000000 B __ram_start
10007d40 B __ram_end
2007c000 D __ram_start_ahb
20083aa8 B __ram_end_ahb

modified size (with IIC Hack):
-----------------------------
arm-none-eabi-size project_build/project.elf
   text    data     bss     dec     hex filename
 418044     952   62864  481860   75a44 project_build/project.elf
10000000 B __ram_start
10007e90 B __ram_end
2007c000 D __ram_start_ahb
20083ab8 B __ram_end_ahb
-----------------------------------------------------------------
```
