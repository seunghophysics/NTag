# NTag

NTag is a C++ library that helps users search for neutron capture signals from all kinds of Super-Kamiokande data and simulation. Users can directly run the main executable NTag on any Super-K data/MC file to get an output ROOT file with the [ntag tree](#output-tree-structure) that has all the features extracted by the signal candidates, or utilize the NTag <a href=annotated.html>classes</a> and <a href=include_2Calculator_8hh.html>calculator functions</a> in their own program by linking to the library.

[TOC]

## Getting Started

The following instruction assumes you're working on sukap.

### Dependencies

> SKOFL: `skofl_21a` or newer
>
> ATMPD: `ATMPD_21a` or newer
>
> ROOT: `v5.38.34`

### How to install

```bash
git clone --branch library https://github.com/seunghophysics/NTag.git
cd NTag && make
```
Set the environment variables $PATH and $NTAGLIBPATH automatically by typing in the following commands.

| Shell type | Install command       | Uninstall command       |
|------------|-----------------------|-------------------------|
| bash       | `. path/bash set`     | `. path/bash unset`     |
| csh/tcsh   | `source path/csh set` | `source path/csh unset` |

### How to run executables

 Once you install NTag, there will be executables created in a directory named `bin`. 
 
 All executables can be run with a command of the form:

 ```
 <executable> -in <input file path> -out <output file path> <command line options>
 ```

 For available command line options, see [here](#command-line-options).

#### NTag

NTag is the main program of this library. It can read any Super-K data/MC file and output a ROOT file with the [neutron search results](#output-tree-structure). 

```
NTag -in <input SK data/MC> -out <output NTag ROOT> <command line options>
```

#### AddNoise

AddNoise reads in an Super-K MC file with signal simulation only (i.e., `DS-DARK 0` in <a href=http://kmcvs.icrr.u-tokyo.ac.jp/viewvc/svnrepos/skdetsim>SKDETSIM</a> card or `/SKG4/DarkNoise/Mode 0` in <a href=https://github.com/SKG4/SKG4>SKG4</a> macro) and produce an output MC file with dark noise hits extracted from data files on sukap disks attached. This program is based on <a href=http://kmcvs.icrr.u-tokyo.ac.jp/viewvc/svnrepos/atmpd/trunk/src/analysis/neutron/mccomb>mccomb</a> on the ATMPD library.

```
AddNoise -in <input SK MC signal> -out <output SK MC> <command line options>
```

#### NTagApply

NTagApply can apply a different neutrong tagging conditions to an NTag ROOT file. 
It can re-generate NTag ROOT file much faster than re-running NTag with different options.

```
NTagApply -in <input NTag ROOT> -out <output NTag ROOT> <command line options>
```

#### NTagTrain

NTagTrain trains neural network weights on NTag ROOT file(s) using CERN ROOT's TMVA.

```
NTagTrain -in <input NTag ROOT> -out <output TMVA result> <command line options>
```

### Command line options {#command-line-options}

* File I/O
| Option          |                               Argument                                 |
|:--------------- |------------------------------------------------------------------------|
|`-in`            |`<input SK data/MC>`                                                    |
|`-out`           |`<output NTag ROOT>`                                                    |
|`-outdata`       |`<output SK data/MC with NTAG bank filled>`                             |

* Vertex
| Option          |                               Argument                                 |
|:----------------|------------------------------------------------------------------------|
|`-prompt_vertex` |One of `none`, `apfit`, `bonsai`, `custom`, `true`, `stmu`, `fitqun`    |
|`-vx`            |`<SK x coordinate [cm]>` (`custom` mode only)                           |
|`-vy`            |`<SK y coordinate [cm]>` (`custom` mode only)                           |
|`-vz`            |`<SK z coordinate [cm]>` (`custom` mode only)                           |
|`-PVXRES`        |`<prompt vertex resolution [cm]>` (`true` mode only)                    |
|`-delayed_vertex`|One of `trms`, `bonsai`, `prompt`                                       |

* SK I/O
| Option          |                               Argument                                 |
|:----------------|------------------------------------------------------------------------|
|`-SKOPTN`        |`<SK read option>`                                                      |
|`-SKBADOPT`      |`<SK bad channel option>`                                               |
|`-REFRUNNO`      |`<SK reference run number>`                                             |

* Write options
| Option          |                               Argument                                 |
|:----------------|------------------------------------------------------------------------|
|`-force_flat`    |`true` or `false`                                                       |
|`-write_bank`    |`true` or `false`                                                       |
|`-save_hits`     |`true` or `false`                                                       |

* Event cut


"force_flat", , "write_bank", "noise_type", "save_hits",
"muechk", "neut", "add_noise", "weight", "debug",
, "vx", "vy", "vz", "tag_e", "tmva", 
"SKOPTN", "SKBADOPT", "REFRUNNO",
"TMIN", "TMAX", "TRBNWIDTH", "PVXRES", "NODHITMX",
"TNOISESTART", "TNOISEEND", "NOISESEED",
"TWIDTH", "NHITSTH", "NHITSMX", "N200MX", "TCANWIDTH", 
"TMINPEAKSEP", "TMATCHWINDOW",
"TRMSTWIDTH", "INITGRIDWIDTH", "MINGRIDWIDTH", "GRIDSHRINKRATE", "VTXSRCRANGE",
"E_NHITSCUT", "E_TIMECUT", "N_OUTCUT",
"print"

| Option  |           Argument            |          
|:--------|-------------------------------|
|-in      | (input file path)             |
|-out     | (output file path)            |
|-prompt_vertex  | (weight filename)             |
|-delayed_vertex  | (MVA training method name)    |
|-method  | (MVA training method name)    |
|-macro   | (macro filename)              |
|-vx(y,z) | (custom vertex position) [cm] |
|-NHITSTH/MX| (NHits threshold / maximum) |
|-TWIDTH  | (Sliding T window width) [ns] |
|-T0TH/MX | (T0 threshold / maximum) [ns] |
|-TRBNWIDTH | (PMT deadtime width) [&mus] |
|-PVXRES | (Prompt vertex resolution) [cm] |
|-VTXSRCRANGE | (Neut-fit search range) [cm] |
|-MINGRIDWIDTH | (Neut-fit minimum grid width) [cm] |
|-sigTQpath | (output from `-readTQ` option) |
|-addSKOPTN | (SK option to add)          |
|-removeSKOPTN | (SK option to remove)    |
|-SKOPTN  | (SK option to set)            |
|-SKBADOPT | (SK bad channel option to set)     |
|-REFRUNNO | (Reference run for dark-noise, etc.) |


| Option  |           Argument            |                      Example usage              |   Use     |
|:--------|-------------------------------|-------------------------------------------------|-----------|
|-in      | (input filename)              | `NTag -in in.dat`                               | mandatory |
|-out     | (output filename)             | `NTag -in in.dat -out out.root`                 | optional  |
|-weight  | (weight filename)             | `NTag -in in.dat -weight weight.xml`            | optional  |
|-method  | (MVA training method name)    | `NTag -in in.dat -method MLP -weight weight.xml`| optional  |
|-method  | (MVA training method name)    | `NTag -in in*.root -method MLP,GradBDT -train`  | optional  |
|-macro   | (macro filename)              | `NTag -macro macro.xyz`                         | optional  |
|-vx(y,z) | (custom vertex position) [cm] | `NTag -in in.dat -vx -400 -vy 0 -vz -1200`      | optional  |
|-NHITSTH/MX| (NHits threshold / maximum) | `NTag -in in.dat -NHITSTH 5 -NHITSMX 70`        | optional  |
|-TWIDTH  | (Sliding T window width) [ns] | `NTag -in in.dat -TWIDTH 13`                    | optional  |
|-T0TH/MX | (T0 threshold / maximum) [ns] | `NTag -in in.dat -T0TH 18 -NHITSMX 835`         | optional  |
|-TRBNWIDTH | (PMT deadtime width) [&mus] | `NTag -in in.dat -TRBNWIDTH 6`                  | optional  |
|-PVXRES | (Prompt vertex resolution) [cm] | `NTag -in in.dat -PVXRES 10`                   | optional  |
|-VTXSRCRANGE | (Neut-fit search range) [cm] | `NTag -in in.dat -VTXSRCRANGE 1000`          | optional  |
|-MINGRIDWIDTH | (Neut-fit minimum grid width) [cm] | `NTag -in in.dat -MINGRIDWIDTH 10`    | optional  |
|-sigTQpath | (output from `-readTQ` option) | `NTag -in in.dat -sigTQpath sigtq.root`      | optional  |
|-addSKOPTN | (SK option to add)          | `NTag -in in.dat -addSKOPTN 23,24`              | optional  |
|-removeSKOPTN | (SK option to remove)    | `NTag -in in.dat -removeSKOPTN 25,26`           | optional  |
|-SKOPTN  | (SK option to set)            | `NTag -in in.dat -SKOPTN 25,26,16`              | optional  |
|-SKBADOPT | (SK bad channel option to set)     | `NTag -in in.dat  -SKBADOPT 55`                 | optional  |
|-REFRUNNO | (Reference run for dark-noise, etc.) | `NTag -in in.dat -REFRUNNO 85619`   | optional  |

* Run options

|Option|                      Example usage                                | Description |
|:-----|-------------------------------------------------------------------|-------------|
|-apply|`NTag -in NTagOut.root -apply -method MLP -weight weight.xml`|Apply specific MVA weight/method to an existing NTag output (with ntvar & truth trees) to replace the existing TMVAoutput with given weight/method. |
|-train|`NTag -in NTagOut00\*.root -train` |Train with NTag output from MC (with ntvar & truth trees) to generate weight files. Wildcard `\*` usable. |
|-multiclass|`NTag (...) -train -multiclass`  |Start multiclass (Gd/H/Noise) classification instead of binary.|
|-debug|`NTag (...) -debug` |Show debug messages on output stream.|
|-noMVA|`NTag (...) -noMVA` |Only search for candidates, without applying TMVA to get classifer output. The branch `TMVAOutput` is not generated. |
|-noFit|`NTag (...) -noFit` |Neut-fit is not used and no related variables are saved to save time. `-noMVA` is automatically called. |
|-noBONSAI|`NTag (...) -noBONSAI` |Skip extracting BONSAI-related candidate variables. |
|-noTOF|`NTag (...) -noTOF` |Disable subtracting ToF from raw hit times. This option removes prompt vertex dependency. |
|-readTQ|`NTag (...) -readTQ`  |Extract raw TQ from input file and save to a flat ROOT tree `rawtq`. Applicable to ZBS only. |
|-saveTQ|`NTag (...) -saveTQ`  |Save ToF-subtracted TQ hit vectors used in capture candidate search in a tree `restq`.|
|-forceFlat|`NTag (...) -forceFlat`  |Force flat (MC-like flat event) mode for data files. |
|-usetruevertex|`NTag (...) -usetruevertex` |Use true vector vertex from common `skvect` as a prompt vertex. |
|-usestmuvertex|`NTag (...) -usestmuvertex`  |Use muon stopping position as a prompt vertex. |

* Macro rules:

Use `#` as the first character in a line to make the entire line a comment.

Arguments must be space-delimited, for example: `-(option) (argument)`

A macro can be used with command line arguments, for example: `NTag -in input.zbs -macro macro.txt`

Example:
```
# input file
-in somepath/input.zbs

# ouptut file
-out otherpath/output.root

# options
-usetruevertex
```

## Output tree structure {#output-tree-structure}

* TTree `ntvar`

| Branch name      | Size        | NN | Description                                             |
|------------------|-------------|----|---------------------------------------------------------|
| RunNo            | 1           | X  | Run number                                              |
| SubrunNo         | 1           | X  | Subrun number                                           |
| EventNo          | 1           | X  | Event number                                            |
| APNDecays        | 1           | X  | # of decay electrons of sub event type                  |
| APNMuE	       | 1           | X  | # of decay electrons                                    |
| APNRings		   | APNRings    | X  | # of Cherenkov rings                                    |
| APRingPID	       | APNRings    | X  | PID of ring                                             |
| APMom	           | APNRings    | X  | Reconstructed ring momentum                             |
| APMomE	       | APNRings    | X  | Reconstructed ring momentum as e-like                   |
| APMomMu	       | APNRings    | X  | Reconstructed ring momentum as mu-like                  |
| pvx              | 1           | X  | X coordinate of APFit neutrino vertex                   |
| pvy              | 1           | X  | Y coordinate of APFit neutrino vertex                   |
| pvz              | 1           | X  | Z coordinate of APFit neutrino vertex                   |
| EVis	           | 1           | X  | Visible energy (MeV/c)                                  |
| TrgType	       | 1           | X  | Trigger type (0: MC, 1: SHE, 2: SHE+AFT, 3: Non-SHE)    |
| NHITAC	       | 1           | X  | Number of OD hits                                       |
| MaxN200		   | 1           | X  | Max N200 in event                                       |
| MaxN200Time	   | 1           | X  | Time of MaxN200 peak                                    |
| NCandidates	   | 1           | X  | # of capture candidates                                 |
| FirstHitTime_ToF | 1           | X  | Index of first hit in T_ToF                             |
| nvx              | NCandidates | X  | X coordinate of Neut-fit vertex                         |
| nvy              | NCandidates | X  | Y coordinate of Neut-fit vertex                         |
| nvz              | NCandidates | X  | Z coordinate of Neut-fit vertex                         |
| N50              | NCandidates | X  | # of PMT hits in 50 ns                                  |
| ReconCT	       | NCandidates | X  | Reconstructed capture time (ns)                         |
| QSum             | NCandidates | X  | Sum of Q in 10 ns (p.e.)                                |
| TRMS50	       | NCandidates | X  | RMS of PMT hit time in 50 ns with Neut-fit vertex       |
| TSpread          | NCandidates | X  | Spread of PMT hit time (max-min) in TWIDTH              |
| NHits	           | NCandidates | O  | # of PMT hits in NTagEventInfo::TWIDTH (ns)             |
| N200             | NCandidates | O  | # of PMT hits in 200 ns                                 |
| TRMS             | NCandidates | O  | RMS of PMT hit time in TWIDTH                           |
| DWall	           | NCandidates | O  | Distance from prompt vertex to wall (cm)                |
| DWallMeanDir	   | NCandidates | O  | Distance from vertex to wall in mean hit direction (cm) |
| AngleMean 	   | NCandidates | O  | Mean of all possible opening angles (deg)               |
| AngleStdev	   | NCandidates | O  | Standard deviation of all possible opening angles (deg) |
| AngleSkew 	   | NCandidates | O  | Skewness of all possibile opening angles                |
| Beta1            | NCandidates | O  | Beta1 calculated in TWIDTH                              |
| Beta2            | NCandidates | O  | Beta2 calculated in TWIDTH                              |
| Beta3            | NCandidates | O  | Beta3 calculated in TWIDTH                              |
| Beta4            | NCandidates | O  | Beta4 calculated in TWIDTH                              |
| Beta5            | NCandidates | O  | Beta5 calculated in TWIDTH                              |
| prompt_nfit	   | NCandidates | O  | Distance to Neut-fit vertex from prompt vertex          |
| BSenergy	       | NCandidates | X  | BONSAI energy in 50 ns                                  |
| bsvx	           | NCandidates | X  | X coordinate of BONSAI vertex                           |
| bxvy             | NCandidates | X  | Y coordinate of BONSAI vertex                           |
| bsvz             | NCandidates | X  | Z coordinate of BONSAI vertex                           |
| BSwall	       | NCandidates | X  | Distance to wall from BONSAI vertex                     |
| BSgood	       | NCandidates | X  | BONSAI fit goodness paramters                           |
| BSdirks          | NCandidates | X  | BONSAI dir_KS                                           |
| BSpatlik         | NCandidates | X  | BONSAI pattern likelihood                               |
| BSovaq           | NCandidates | X  | BONSAI ovaq                                             |
| prompt_bonsai	   | NCandidates | X  | Distance to BONSAI vertex from prompt vertex            |
| bonsai_nfit	   | NCandidates | X  | Distance to Neut-fit vertex from BONSAI vertex          |
| DoubleCount      | NCandidates | X  | (MC-only) 0: Not double count 1: Double count           |
| CTDiff	       | NCandidates | X  | (MC-only) Diff. between true/recon capture times (ns)   |
| capvx 	       | NCandidates | X  | (MC-only) X of related true capture vertex (cm)         |
| capvy            | NCandidates | X  | (MC-only) Y of related true capture vertex (cm)         |
| capvz            | NCandidates | X  | (MC-only) Z of related true capture vertex (cm)         |
| CaptureType	   | NCandidates | X  | (MC-only) 0: Noise 1: H-capture 2: Gd-capture           |
| TMVAOutput       | NCandidates | X  | TMVA classifier output value                            |

All variables with suffix `_n` are variables calculated using the Neut-fit vertex instead of the prompt vertex.

* TTree `truth`

| Branch name   | Size          | Description                                                   |
|---------------|---------------|---------------------------------------------------------------|
| TrgOffset		| 1             | Trigger offset                                                |
| NTrueCaptures | 1             | # of true neutron captures                                    |
| TrueCT        | NTrueCaptures | True capture time (ns)                                        |
| capvx 	    | NTrueCaptures | True capture vertex (cm)                                      |
| capvy 	    | NTrueCaptures | True capture vertex (cm)                                      |
| capvz 	    | NTrueCaptures | True capture vertex (cm)                                      |
| NGamma        | NTrueCaptures | Gamma multiplicity                                            |
| TotGammaE	    | NTrueCaptures | Total gamma energy emitted (MeV)                              |
| NSavedSec     | 1             | # of secondary particles (neutron & capture related)          |
| secvx         | NSavedSec     | X coordinate of the secondary creation vertex                 |
| secvy         | NSavedSec     | Y coordinate of the secondary creation vertex                 |
| secvz         | NSavedSec     | Z coordinate of the secondary creation vertex                 |
| secpx         | NSavedSec     | X-direction momentum of the secondary creation vertex         |
| secpy         | NSavedSec     | Y-direction momentum of the secondary creation vertex         |
| secpz         | NSavedSec     | Z-direction momentum of the secondary creation vertex         |
| SecMom        | NSavedSec     | Momentum of the secondary particle                            |
| SecPID        | NSavedSec     | PDG code of the secondary particle                       |
| SecT          | NSavedSec     | Generated time of the secondary particle                      |
| ParentPID     | NSavedSec     | Parent particle code of the secondary particle                |
| SecIntID      | NSavedSec     | Interaction code (GEANT3) that produced the secondary         |
| SecDWall	    | NSavedSec     | Distance to capture vertex from wall (cm)                     |
| SecCaptureID  | NSavedSec     | Related true capture ID                                       |
| NVecInNeut    | 1             | # of particle at neutrino interaction                         |
| NeutIntMode   | 1             | Interaction mode of neutrino, see NEUT/nemodsel.F             |
| NeutIntMom    | 1             | Momentum at neutrino interaction (GeV/c)                      |
| NnInNeutVec	| 1             | # of neutron in input vector                                  |
| NeutVecPID    | NVecInNeut    | PDG code at neutrino interaction (PDG code)              |
| NVec          | 1             | # of particle of primary vectors in MC                        |
| vecx          | 1             | X coordinate of primary vector vertex                         |
| vecy          | 1             | X coordinate of primary vector vertex                         |
| vecz          | 1             | X coordinate of primary vector vertex                         |
| VecPID        | NVec          | Primary vector particle code (Geant3 code)                    |
| VecMom        | NVec          | Primary vector momentum                                       |
| vecpx         | NVec          | X-direction momentum of primary vector                        |
| vecpy         | NVec          | Y-direction momentum of primary vector                        |
| vecpz         | NVec          | Z-direction momentum of primary vector                        |

* TTree `restq`

This tree is generated if `-saveTQ` option is passed to the main function.

| Branch name   | Size              | Description                                                   |
|---------------|-------------------|---------------------------------------------------------------|
| T             | # of in-gate hits | ToF-subtracted PMT hit times (ns) sorted in ascending order   |
| Q             | # of in-gate hits | Q (p.e.) of each hit in T                                     |
| I             | # of in-gate hits | PMT cable ID of each hit in T                                 |

## Contact

Seungho Han (ICRR) <han@icrr.u-tokyo.ac.jp>