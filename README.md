# NTag

NTag is a C++ library that helps users search for neutron capture signals from all kinds of Super-Kamiokande data and simulation. Users can directly run the main executable NTag on any Super-K data/MC file to get an output ROOT file with the [ntag tree](#output-tree-structure) that has all the features extracted by the signal candidates, or utilize the NTag <a href=annotated.html>classes</a> and <a href=include_2Calculator_8hh.html>calculator functions</a> in their own program by linking to the library.

[TOC]

## Getting Started

The following instruction assumes you are working on sukap.

### Dependencies

> CERNLIB 2005
>
> SKOFL: `skofl_trunk` or newer
>
> ATMPD: `ATMPD_21a` or newer
>
> ROOT: `v5.38.34`

### How to install

```bash
git clone https://github.com/seunghophysics/NTag.git
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

#### NTag {#ntag-exe}

NTag is the main program of this library. It can read any Super-K data/MC file and output a ROOT file with the [neutron search results](#output-tree-structure). 

```
NTag -in <input SK data/MC> -out <output NTag ROOT> <command line options>
```

#### AddNoise {#addnoise-exe}

AddNoise reads in an Super-K MC file with signal simulation only (i.e., `DS-DARK 0` in <a href=https://kmcvs.icrr.u-tokyo.ac.jp/websvn/listing.php?repname=kmcvs.icrr.u-tokyo.ac.jp&path=%2Fskdetsim%2Ftrunk%2F&#a4ecf9b1e0f9901f57b968ad20b655046>SKDETSIM</a> card or `/SKG4/DarkNoise/Mode 0` in <a href=https://github.com/SKG4/SKG4>SKG4</a> macro) and attach dark noise hits extracted from data files on sukap disks to produce an output MC file. This program is based on <a href=https://kmcvs.icrr.u-tokyo.ac.jp/websvn/listing.php?repname=kmcvs.icrr.u-tokyo.ac.jp&path=%2Fatmpd%2Ftrunk%2Fsrc%2Fanalysis%2Fneutron%2Fmccomb%2F&#abf8532438ad2c1a0a613dbac4e652f79>mccomb</a> on the ATMPD library.

For available options in AddNoise, see [options for Dark Noise](#dark-noise-option).

```
AddNoise -in <input SK signal MC> -out <output SK MC> <command line options>
```

#### NTagApply {#ntagapply-exe}

NTagApply can apply a different neutron tagging conditions to an NTag ROOT file. 
It can re-generate NTag ROOT file much faster than re-running NTag with different options.

For available options in NTagApply, see [options for Tagging Conditions](#tag-cond-option).

```
NTagApply -in <input NTag ROOT> -out <output NTag ROOT> <command line options>
```

#### NTagTrain {#ntagtrain-exe}

NTagTrain trains neural network weights on NTag ROOT file(s) using CERN ROOT's TMVA.

```
NTagTrain -in <input NTag ROOT> -out <output TMVA result> <command line options>
```

### Contact

Seungho Han (ICRR) <han@icrr.u-tokyo.ac.jp>