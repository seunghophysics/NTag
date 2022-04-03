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

### Contact

Seungho Han (ICRR) <han@icrr.u-tokyo.ac.jp>