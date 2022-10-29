# Command line options {#command-line-options}

Default option values are defined in <a href=NTagConfig_source.html>NTagConfig</a>.

## File I/O

| Option          |                               Argument                                 | Default |
|---------------- |------------------------------------------------------------------------|:-------:|
|`-in`            | Input SK data/MC                                                       | -       |
|`-out`           | Output NTag ROOT                                                       | -       |
|`-outdata`       | Output SK data/MC with NTAG bank filled                                | -       |

Use of `-outdata` option automatically invokes option `-write_bank true`. See [output](#cl-output).

## Run mode

| Option          |                               Argument                                 | Default |
|---------------- |------------------------------------------------------------------------|:-------:|
|`-mode`          | Run mode                                                               | -       |

The run mode option `-mode` is to provide users a standard set of options for each delayed vertex mode and neural network type.
The argument to `-mode` can be one of the following, which are in the form of `delayed_vertex`_`NN_type`:
- `prompt_keras`: assume (prompt vertex)=(delayed vertex), use Keras neural network (SK4,5,6,7)
- `prompt_tmva`: assume (prompt vertex)=(delayed vertex), use TMVA neural network (SK6,7)
- `bonsai_keras`: use BONSAI and Keras neural network (SK6,7)
- `bonsai_tmva`: use BONSAI and TMVA neural network (SK6,7)
- `lowfit_cuts`: use LOWFIT and simple cuts (no neural-network invovled), standard in the LOWE group (SK6,7)

The implementation of this mode is based on the card files in the `card` directory. 
For example, `-mode prompt_keras` works in the same way as `-macro $NTAGLIBPATH/card/prompt_keras.card`.
The options in the card can be overridden by other command line options or another card/macro file given by the `-macro` command.

## Vertex

| Option          |                               Argument                                 | Default  |
|-----------------|------------------------------------------------------------------------|:--------:|
|`-prompt_vertex` | One of `none`, `apfit`, `bonsai`, `custom`, `true`, `stmu`, `fitqun`   | `custom` |
|`-vx`            | SK x coordinate (cm) (for `custom` mode only)                          | 0        |
|`-vy`            | SK y coordinate (cm) (for `custom` mode only)                          | 0        |
|`-vz`            | SK z coordinate (cm) (for `custom` mode only)                          | 0        |
|`-PVXRES`        | Prompt vertex resolution (cm) (for `true` mode only)                   | 0        |
|`-PVXBIAS`       | Prompt vertex bias (cm) (for `true` mode only)                         | 0        |
|`-correct_tof`   | `true` if correcting ToF from prompt vertex, otherwise `false`         | `true`   |
|`-delayed_vertex`| One of `trms`, `bonsai`, `prompt`, `lowfit`                            | `bonsai` |

N.B. `-prompt_vertex none` automatically turns on `-correct_tof false`.


## Signal search parameters {#signal-search-parameters}

| Option          |                               Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-TMIN`          | Search start time relative to event trigger (µs)                       | 3       |
|`-TMAX`          | Search end time relative to event trigger (µs)                         | 535     |
|`-QMAX`          | Maximum allowed PMT hit charge (p.e.)                                  | 10      |
|`-TWIDTH`        | Time window width of signal trigger (ns)                               | 14      |
|`-NHITSTH`       | Hit trigger for candidate selection                                    | 7       |
|`-NHITSMX`       | Maximum number of hits for hit trigger                                 | 400     |
|`-N200MX`        | Maximum number of hits within 200 ns                                   | 1000    |
|`-TMINPEAKSEP`   | Minimum time separation between two signal triggers (ns)               | 200     |
|`-TCANWIDTH`     | Time window width to calculate features                                | 14      |
|`-MINNHITS`      | Minimum number of allowed hits in the output                           | 7       |
|`-MAXNHITS`      | Maximum number of allowed hits in the ouptut                           | 400     |


## Tagging conditions {#tag-cond-option}

The following are also valid options for [NTagApply](#ntagapply-exe).

| Option          |                          Argument                                |                Default                |
|-----------------|------------------------------------------------------------------|:-------------------------------------:|
|`-TMATCHWINDOW`  | Maximum time window to match candidate with true taggable (ns)   | 50                                    |
|`-NN_type`       | `keras` or `tmva`                                                | `keras`                               |
|`-weight`        | TMVA weight file (.xml)                                          | `default`                             |
|`-E_CUTS`        | Cuts for decay-e selection                                       | `(TagOut>0.7)&&(NHits>50)&&(FitT<20)` |
|`-N_CUTS`        | Cuts for neutron capture selection                               | `(TagOut>0.7)`                        |

 ## Dark noise {#dark-noise-option}

The following are also valid options for [AddNoise](#addnoise-exe), where `-add_noise true` option is automatically turned on.

| Option          | Argument                                                               |             Default            |
|-----------------|------------------------------------------------------------------------|:------------------------------:|
|`-add_noise`     | `true` or `false`                                                      | `false`                        |
|`-repeat_noise`  | `true` if allowing repetition for limited amount of noise              | `true`                         |
|`-noise_path`    | Directory path to search for noise files                               | `/disk02/calib3/usr/han/dummy` |
|`-noise_type`    | One of `sk4`, `sk5`, `sk6`, `ambe`, or `default` (auto)                | `default`                      |
|`-TNOISESTART`   | Noise addition start time from event trigger (µs)                      | 2                              |
|`-TNOISEEND`     | Noise addition end time from event trigger (µs)                        | 536                            |
|`-NOISESEED`     | Random seed                                                            | 0                              |
|`-PMTDEADTIME`   | Artificial PMT deadtime (ns)                                           | 1000                           |

When `-add_noise true` option is used, dark noise hits randomly extracted from dummy trigger data files stored in the path specified by `-noise_path` (`/disk02/calib3/usr/han/dummy` by default) are appended to the input SK MC before signal search starts. Note that `-NOISESEED 0` (which is default) will set a seed used in the random number generator according to the current UNIX time.

## Variables for output variables

| Option          |                               Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-TGATEMIN`      | Minimum hit time from trigger for QISMSK calculation (µs)              | -0.5208 |
|`-TGATEMAX`      | Maximum hit time from trigger for QISMSK calculation (µs)              |  0.7792 |
|`-TRBNWIDTH`     | Time window width for burst noise (ns)                                 | 10000   |

## Event cut

| Option          |                               Argument                                 | Default  |
|-----------------|------------------------------------------------------------------------|:--------:|
|`-NIDHITSMX`     | Maximum # of ID hits to allow                                          | 1000000  |
|`-NODHITSMX`     | Maximum # of OD hits to allow                                          | 100000   |

## SK I/O (detector parameters)

| Option          |                               Argument                                 |    Default    |
|-----------------|------------------------------------------------------------------------|:-------------:|
|`-SKOPTN`        | List of SK read options                                                | `31,30,26,25` |
|`-SKBADOPT`      | SK bad channel option                                                  | 0             |
|`-REFRUNNO`      | SK reference run number                                                | 0             |
|`-lowfit_param`  | Lowfit calib parameter type: `skdetsim` or `skg4`                      | `skg4`*       |

If `-REFRUNNO 0`, NTag looks up a reference run number that is closest to a given event.

*For SKDETSIM MC, the default value is `skdetsim`.

## Output {#cl-output}

| Option          | Argument          |                   Description                                 | Default |
|-----------------|-------------------|---------------------------------------------------------------|:-------:|
|`-force_flat`    | `true` or `false` | Turn off automatic AFT merging and force flat event structure | `false` |
|`-write_bank`    | `true` or `false` | Fill NTAG common block                                        | `false` |
|`-save_hits`     | `true` or `false` | Save raw PMT hit information to output and create `hit` tree  | `false` |

When reading data (not MC) files, NTag automatically merges PMT hits from an SHE-triggered event with those from the subsequent AFT-triggered events. This automatic AFT merging can be turned off by passing `-force_flat true` to keep each event separate as in MC.

## TRMS-fit

| Option          |                               Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-TRMSTWIDTH`    | Time window width to apply TRMS-fit (ns)                               | 30      |
|`-INITGRIDWIDTH` | Initial vertex search grid width (cm)                                  | 800     |
|`-MINGRIDWIDTH`  | Minimum vertex search grid width (cm)                                  | 50      |
|`-GRIDSHRINKRATE`| Grid shrink rate per full grid search loop                             | 0.5     |
|`-VTXMAXRADIUS`  | Maximum radius of fit vertex from tank center (cm)                     | 5000    |


## Logging

| Option          |                               Argument                                 |
|-----------------|------------------------------------------------------------------------|
|`-print`         | `true` or `false` or list of candidate features to print               |
|`-debug`         | `true` or `false`                                                      |

## Macro rules

Use `#` as the first character in a line to make the entire line a comment.

Arguments must be space-delimited, for example: `-(option) (argument)`

* Macro example:

```
# input file
-in somepath/input.zbs

# ouptut file
-out otherpath/output.root

# options
-prompt true
```

A macro can be used in conjunction with command line arguments, for example: `NTag -in input.zbs -macro macro.txt`
The priority order is command line arguments > macro > run mode > <a href=NTagConfig_source.html>NTagConfig</a>.