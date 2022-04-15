# Command line options {#command-line-options}

Default option values are defined in <a href=https://github.com/seunghophysics/NTag/blob/library/NTagConfig#L1>NTagConfig</a>.

## File I/O

| Option          |                               Argument                                 | Default |
|---------------- |------------------------------------------------------------------------|:-------:|
|`-in`            | Input SK data/MC                                                       | -       |
|`-out`           | Output NTag ROOT                                                       | -       |
|`-outdata`       | Output SK data/MC with NTAG bank filled                                | -       |

Use of `-outdata` option automatically invokes option `-write_bank true`. See [output](#cl-output).

## Vertex

| Option          |                               Argument                                 | Default  |
|-----------------|------------------------------------------------------------------------|:--------:|
|`-prompt_vertex` | One of `none`, `apfit`, `bonsai`, `custom`, `true`, `stmu`, `fitqun`   | `custom` |
|`-vx`            | SK x coordinate (cm) (for `custom` mode only)                          | 0        |
|`-vy`            | SK y coordinate (cm) (for `custom` mode only)                          | 0        |
|`-vz`            | SK z coordinate (cm) (for `custom` mode only)                          | 0        |
|`-PVXRES`        | Prompt vertex resolution (cm) (for `true` mode only)                   | 0        |
|`-delayed_vertex`| One of `trms`, `bonsai`, `prompt`                                      | `bonsai` |

Prompt vertex options:
- `none`: 
- `apfit`:
- `bonsai`:
- `custom`:
- `true`:
- `stmu`:
- `fitqun`:

Delayed vertex options:
- `bonsai`:
- `trms`:
- `prompt`:

## Signal search parameters

| Option          |                               Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-TMIN`          | Search start time relative to event trigger (µs)                       | 3       |
|`-TMAX`          | Search end time relative to event trigger (µs)                         | 535     |
|`-TWIDTH`        | Time window width of signal trigger (ns)                               | 14      |
|`-NHITSTH`       | Threshold number of hits within trigger time window                    | 7       |
|`-NHITSMX`       | Maximum number of hits within trigger time window                      | 400     |
|`-N200MX`        | Maximum number of hits within 200 ns                                   | 1000    |
|`-TCANWIDTH`     | Time window width to calculate features                                | 14      |
|`-TMINPEAKSEP`   | Minimum time separation between two signal triggers (ns)               | 200     |

## Tagging conditions {#tag-cond-option}

The following are also valid options for [NTagApply](#ntagapply-exe).

| Option          |                               Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-tag_e`         | `true` or `false`                                                      | `true`  |
|`-E_NHITSCUT`    | Threshold number of hits to classify candidate as decay-e              | 50      |
|`-E_TIMECUT`     | Threshold time to classify candidate as decay-e (µs)                   | 20      |
|`-TAGOUTCUT`     | Threshold TMVA signal likelihood to classify candidate as signal       | 0.7     |
|`-TMATCHWINDOW`  | Maximum time window to match candidate with true taggable (ns)         | 50      |

 ## Dark noise {#dark-noise-option}

The following are also valid options for [AddNoise](#addnoise-exe), where `-add_noise true` option is automatically turned on.

| Option          |Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-add_noise`     | `true` or `false`                                                      | `false` |
|`-noise_type`    | One of `sk5`, `sk6`, `ambe`                                            | `sk6`   |
|`-TNOISESTART`   | Noise addition start time from event trigger (µs)                      | 2       |
|`-TNOISEEND`     | Noise addition end time from event trigger (µs)                        | 536     |
|`-NOISESEED`     | Random seed                                                            | 0       |

When `-add_noise true` option is used, dark noise hits randomly extracted from dummy trigger data files stored in `/disk02/calib3/usr/han/dummy` are appended to the input SK MC before signal search starts. Note that `-NOISESEED 0` (which is default) will set a seed used in the random number generator according to the current UNIX time.

## PMT noise reduction

| Option          |                               Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-TRBNWIDTH`     | PMT deadtime (µs)                                                      | 0       |

## Event cut

| Option          |                               Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-NHITSMX`       | Maximum # of OD hits to allow                                          | 100000  |

## SK I/O

| Option          |                               Argument                                 |    Default    |
|-----------------|------------------------------------------------------------------------|:-------------:|
|`-SKOPTN`        | List of SK read options                                                | `31,30,26,25` |
|`-SKBADOPT`      | SK bad channel option                                                  | 0             |
|`-REFRUNNO`      | SK reference run number                                                | 62428         |

## Output {#cl-output}

| Option          | Argument          |                   Description                      | Default |
|-----------------|-------------------|----------------------------------------------------|:-------:|
|`-force_flat`    | `true` or `false` |                                                     | `false` |
|`-write_bank`    | `true` or `false` |                                                     | `false` |
|`-save_hits`     | `true` or `false` |                                                     | `false` |
|`-muechk`        | `true` or `false` |                                                     | `false` |
|`-neut`          | `true` or `false` |                                                     | `false` |

## TRMS-fit

| Option          |                               Argument                                 | Default |
|-----------------|------------------------------------------------------------------------|:-------:|
|`-TRMSTWIDTH`    | Time window width to apply TRMS-fit (ns)                               | 30      |
|`-INITGRIDWIDTH` | Initial vertex search grid width (cm)                                  | 800     |
|`-MINGRIDWIDTH`  | Minimum vertex search grid width (cm)                                  | 50      |
|`-GRIDSHRINKRATE`| Grid shrink rate per full grid search loop                             | 0.5     |
|`-VTXMAXRADIUS`  | Maximum radius of fit vertex from tank center (cm)                     | 5000    |

## TMVA

| Option          |                               Argument                                 | Default  |
|-----------------|------------------------------------------------------------------------|:--------:|
|`-tmva`          | `true` or `false`                                                      | `true`   |
|`-weight`        | TMVA weight file (.xml)                                                | `default`|

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
-usetruevertex
```

A macro can be used in conjunction with command line arguments, for example: `NTag -in input.zbs -macro macro.txt`
The priority order is command line arguments > macro > <a href=https://github.com/seunghophysics/NTag/blob/library/NTagConfig#L1>NTagConfig</a>.