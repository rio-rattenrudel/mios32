MIOS32 - MBSEQ v4 - MOD
=======================
## Preamble

This fork contains heavy modifications to the original MBSEQ v4 Master Branch. The customizations are largely compatible with the original version, but include the following changes. The modifications are compatible with the LPC1769 core. The original master branch on which the modifications are based lies in the branch 'original'. All changes in the source are uniformly marked with beginning and ending comment and described in detail in an explicite info file (in German).

## Modifications

1a. [Added: rio/MBSEQ_HW.V4](https://github.com/rio-rattenrudel/mios32/commit/b33e896afac3e7300462678385642d790a279765)

1b. [Updated: rio/MBSEQ_HW.V4](https://github.com/rio-rattenrudel/mios32/commit/6928bb396463b6587c4067984cba9d36c8f43985)
  * A new hardware-config file has been added: rio/MBSEQ_HW.V4`, which contains a few own adjustments of my hardware.
  
2. [Set Default Mixer Map CCs (Bank #32 on CC4)](https://github.com/rio-rattenrudel/mios32/commit/f819b62f34d96abf60fec227359852559109abd5)

3. [Using the BPM DataWheel in MUTE and PATTERN SCREEN](https://github.com/rio-rattenrudel/mios32/commit/2c5c8c7cf2d13bd586ba9d1d8777219e64fb6549)

4. [Disable Board LEDs](https://github.com/rio-rattenrudel/mios32/commit/86219c6bfb30c0cdc701e08f40ed9ab4c7230f5b)

5. [Fixed: Selected Pattern Group - Display Update](https://github.com/rio-rattenrudel/mios32/commit/b4bb6aa1a2f24036bcb2a531e550694ea790fd64)

6. [Changed: MIXER - Separated Load & Dump CC](https://github.com/rio-rattenrudel/mios32/commit/458a5f7cd9f34d44d8ce48d0c27219b6607d275e)

7. [Changed: PATTERN - Bank Change should be set, but not executed](https://github.com/rio-rattenrudel/mios32/commit/2d8ea3485af48c657afc8e980958978c032d15ca)

8a. [Feature: Little Remote Functions (+Mixer Dump +BPM)](https://github.com/rio-rattenrudel/mios32/commit/0fccefbcb7c1db8b09b6930d4b34577eadc78395)

8b. [Fixed: Little Remote Functions (BPM Assignments)](https://github.com/rio-rattenrudel/mios32/commit/8e949d27a419daaf9ac75bddbe35d7a8a1c28f91)

9. [Fixed: PATTERNS - Visual Updates for playing pattern](https://github.com/rio-rattenrudel/mios32/commit/8557f20868a7f596aa51279c8c776041be89ebb9)

10a. [Feature: Expanded LFO with special waveforms](https://github.com/rio-rattenrudel/mios32/commit/36a6266d499074261de17ed17fbb855e2cf63c79)

10b. [Feature: Expanded LFO II (Reset, Delayed, Display)](https://github.com/rio-rattenrudel/mios32/commit/a3b7baff0a7a6a9e20dac2870e94182e08a8f5c2)

10c. [Fixed: Expanded LFO - Fade out](https://github.com/rio-rattenrudel/mios32/commit/51b5e481f5b528cd8d80142f7e39f4d4a72d4546)

11. [Feature: Proteus 2000 Integration](https://github.com/rio-rattenrudel/mios32/commit/b444395fcd240f7cd091f3c7e82c5dac74b18755)

12. [Changed: Ignore Multi-Selection for tracks](https://github.com/rio-rattenrudel/mios32/commit/703445c0ea78d9cb6e468c1eadbb31fb4eb4b528)

13. [Feature: Chord3 - Replacement (root, inv I, inv II)](https://github.com/rio-rattenrudel/mios32/commit/a784a9f851fdbadba8240522bb1e1aeac09b7373)

14. [Feature: Mutes Layer](https://github.com/rio-rattenrudel/mios32/commit/a42e063ed5b61e31ffaeedf5ccecf2914adc6a37)

15. [Changed: Use IIC-HACK firmware to allow 8 IIC port](https://github.com/rio-rattenrudel/mios32/commit/d485d7a08f962c78dbadc1680ed094ff28c01145)

16. [Fixed: Start playback once - fixed bouncing button](https://github.com/rio-rattenrudel/mios32/commit/6e905d72e1d44befa386438e57925079c7e68041)

17a. [Feature: Poly Pressure (Poly Aftertouch)](https://github.com/rio-rattenrudel/mios32/commit/4a8d6e5bdfd01b8758e158940f10daeec616a497)

17b. [Feature: Poly Pressure II (Mixer)](https://github.com/rio-rattenrudel/mios32/commit/712ecbb0d744e5b73b63051a25390812f2d1bc78)

## Recompile sizes

```
Recompiling 4.096:
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
 406964     936   62536  470436   72da4 project_build/project.elf
10000000 B __ram_start
10007d48 B __ram_end
2007c000 D __ram_start_ahb
20083aa8 B __ram_end_ahb

modified size (with IIC Hack):
-----------------------------
arm-none-eabi-size project_build/project.elf
   text    data     bss     dec     hex filename
 415828     936   62896  479660   751ac project_build/project.elf
10000000 B __ram_start
10007eb0 B __ram_end
2007c000 D __ram_start_ahb
20083aa8 B __ram_end_ahb
-----------------------------------------------------------------
```
