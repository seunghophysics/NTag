# Output tree structure {#output-tree-structure}

## settings

This tree contains all options applied when running the program.

See the list of command line options for details.

## event

* Event ID

| Branch name      | Description                                             |
|------------------|---------------------------------------------------------|
| RunNo            | Run number                                              |
| SubrunNo         | Subrun number                                           |
| EventNo          | Event number                                            |

* Event variables

| Branch name      | Description                                             |
|------------------|---------------------------------------------------------|
| QISMSK           | Total charge deposit in ID PMT within trigger (p.e.)    |
| NHITAC           | Total number of OD PMT hits within trigger              |
| TrgType          | Trigger type: SHE+AFT: 2, SHE-only: 1, Else: 0          |
| TDiff            | Time from previous event (msec)                         |
| NCandidates      | Number of signal candidates (same as `ntag` tree)       |

* Number of hits within neutron search range

| Branch name      | Description                                             |
|------------------|---------------------------------------------------------|
| NAllHits         | # of all good ID hits                                   |
| NAllODHits       | # of all good OD hits                                   |
| NBadHits         | # of all masked bad ID hits                             |
| NNegativeHits    | # of all masked ID hits with negative charge            |
| NLargeQHits      | # of all masked ID hits with charge larger than QMAX    |
| NDeadHitsByNoise | # of all dead ID hits due to noise hits                 |
| NDeadHitsBySignal| # of all dead ID hits due to signal hits                |

Deadtime is determined by PMTDEADTIME.

* Prompt vertex

Following the Super-K coordinate system (tank center being the origin):

| Branch name      | Description                                             |
|------------------|---------------------------------------------------------|
| pvx              | Prompt vertex X coordinate (cm)                         |
| pvy              | Prompt vertex Y coordinate (cm)                         |
| pvz              | Prompt vertex Z coordinate (cm)                         |
| DWall            | Distance from prompt vertex to nearest tank wall (cm)   |

* Tagged multiplicities

| Branch name      | Description                                             |
|------------------|---------------------------------------------------------|
| NTaggedE         | Total number of candidates tagged as decay electrons    |
| NTaggedN         | Total number of candidates tagged as neutron captures   |

* MC simulation

| Branch name      | Description                                             |
|------------------|---------------------------------------------------------|
| vecvx            | MC true event generation vertex X coordinate (cm)       |
| vecvy            | MC true event generation vertex Y coordinate (cm)       |
| vecvz            | MC true event generation vertex Z coordinate (cm)       |
| VtxRes           | Distance from MC true vertex to prompt vertex  (cm)     |
| MCT0             | MC simulation start time relative to trigger (ns)       |
| NTrueE           | Total number of generated true decay electrons in MC    |
| NTrueN           | Total number of generated true neutron captures in MC   |

* NEUT neutrino interaction simulation

| Branch name      | Description                                             |
|------------------|---------------------------------------------------------|
| NEUTMode         | Neutrino interaction mode (see NEUT/nemodsel.F)         |
| NuType           | Neutrino PDG type (12: e, 14: µ, +/-: nu/nu-bar)        |
| NuMom            | Neutrino momentum (GeV/c)                               |
| nu_dirx          | Neutrino X direction                                    |
| nu_diry          | Neutrino Y direction                                    |
| nu_dirz          | Neutrino Z direction                                    |

* Added noise

If run with `-add_noise true`, the following branches will be added to track where the added (random trigger) noise hits come from.

| Branch name      | Description                                             |
|------------------|---------------------------------------------------------|
| NoiseRunNo       | Run number of the added noise hits                      |
| NoiseSubrunNo    | Subrun number of the added noise hits                   |
| NoiseEventNo     | Event number of the added noise hits                    |
| NoiseEventTStart | t_min within the original noise event (ns)              |
| NoiseEventTEnd   | t_max within the original noise event (ns)              |

* Error flag

| Branch name      | Description |
|------------------|-------------|
| HitAppendError   | Error flag for SHE+AFT hit merging (0: No error, 1: OD coincidence missing, 2: ID coincidence missing, 3: ID/OD coincidence missing) |

## hit

This tree is generated if `-save_hits true` option is passed to [NTag](#ntag-exe).

Each entry in each branch is an array sorted in time in ascending order, 
with a size of the number of PMT hits in each event.

| Branch name   | Description                                                     |
|---------------|-----------------------------------------------------------------|
| t             | Event PMT hit times (ns) (event trigger = 1000 ns)              |
| q             | Event PMT hit charge (p.e.)                                     |
| i             | Event PMT hit cable number                                      |
| s             | Event PMT hit signal flag (signal hits: 1 if `-add_noise true`) |
| tof           | Event PMT hit time of flight from prompt vertex (ns)            |
| dt            | Event PMT hit time from preceding hit (ns)                      |
| b             | Event PMT hit burst noise flag (1 if dt < TRBNWIDTH)            |
| n             | Event PMT hit tagged flag (1 if tagged as signal by NTag)       |

## particle

This tree contains information of simulated particles in MC.

Each entry in each branch is an array with a size of the number of simulated particles in each event.

| Branch name   | Description                                          |
|---------------|------------------------------------------------------|
| PID           | PDG code                                             |
| ParentPID     | PDG code of the parent particle                      |
| ParentIndex   | Index of the parent particle in `particle` tree      |
| IntID         | Geant3 code of the particle generating interaction   |
| t             | Time of production relative to trigger (µs)          |
| x             | X coordinate of production vertex (cm)               |
| y             | Y coordinate of production vertex (cm)               |
| z             | Z coordinate of production vertex (cm)               |
| px            | Momentum in X direction (MeV/c)                      |
| py            | Momentum in Y direction (MeV/c)                      |
| pz            | Momentum in Z direction (MeV/c)                      |
| ParentVX      | X coordinate of parent production vertex (cm)        |
| ParentVY      | Y coordinate of parent production vertex (cm)        |
| ParentVZ      | Z coordinate of parent production vertex (cm)        |
| ParentPX      | Parent momentum in X direction (MeV/c)               |
| ParentPY      | Parent momentum in Y direction (MeV/c)               |
| ParentPZ      | Parent momentum in Z direction (MeV/c)               |
| KE            | Kinetic energy (MeV)                                 |

Below is the list of Geant3 interaction code used in IntID branch:
\snippet src/cluster/ParticleCluster/Particle/ParticleTable.cc Geant3 interaction code

## taggable

This tree contains the MC true information of the two taggable signals in NTag: decay electrons and neutron captures.

Each entry in each branch is an array with a size of the number of taggable signals in each event.

| Branch name   | Description                                                    |
|---------------|----------------------------------------------------------------|
| Type          | Type (1: decay-e, 2: n-capture)                                |
| TaggedType    | Tagged type (0: missed, 1: decay-e, 2: n-capture)              |
| t             | Time relative to trigger (µs)                                  |
| E             | Total energy (MeV)                                             |
| tagvx         | X coordinate (cm)                                              |
| tagvy         | Y coordinate (cm)                                              |
| tagvz         | Z coordinate (cm)                                              |
| DistFromPV    | Distane from prompt vertex  (cm)                               |
| DWall         | Distance to nearest tank wall (cm)                             |
| EarlyIndex    | Array index of corresponding signal candidate in `mue` tree    |
| DelayedIndex  | Array index of corresponding signal candidate in `ntag` tree   |
| parvx         | X coordinate of parent particle (cm)                           |
| parvy         | Y coordinate of parent particle (cm)                           |
| parvz         | Z coordinate of parent particle (cm)                           |
| ParentE       | Total energy of parent particle (MeV)                          |
| ParentT       | Parent particle creation time relative to trigger (µs)         |
| ParentIntID   | Parent particle creation interaction ID                        |
| ParentIndex   | Parent particle index in `particle` tree                       |

## mue

This tree contains the information of signal candidates retrieved from the <a href=https://kmcvs.icrr.u-tokyo.ac.jp/websvn/listing.php?repname=kmcvs.icrr.u-tokyo.ac.jp&path=%2Fskdetsim%2Ftrunk%2F&#a4ecf9b1e0f9901f57b968ad20b655046>MUE</a> bank. This tree will be filled only if the input file (ZEBRA file) has a MUE bank, which can be created if the ATMPD decay-e search routine `muechk` (or `APFit`) is applied before feeding it to NTag. MUE candidates with time (FitT) overlapping with the neutron search time interval specified by `-TMIN` and `-TMAX` commands (see [here](#signal-search-parameters) for details) will be pruned automatically to prevent double-counting a single signal by MUE and NTag. 

| Branch name   | Description                                                              |
|---------------|--------------------------------------------------------------------------|
| NCandidates   | Total number of MUE candidates                                           |

Each entry in the following branches is an array with a size of the number of MUE candidates in each event. Vertex fit is skipped for MUE candidates that are too close in time.

| Branch name   | Description                                                                   |
|---------------|-------------------------------------------------------------------------------|
| DWall         | Distance to nearest wall (cm)                                                 |
| FitT          | Fitted time relative to trigger (µs)                                          |
| GateType      | Gate type (1: sub-event 2: gate 3: bye-bye)                                   |
| Goodness      | Muechk fit goodness                                                           |
| Label         | True class label (0: noise, 1: decay-e, 2: p(n,γ), 3: Gd(n,γ))                |
| NHits         | Number of hits as recorded by `muechk`: (N50 for GateType==1, N30 otherwise)  |
| TagClass      | Tag result (0: noise, 1: decay-e, 2: n-capture)                               |
| TagIndex      | Array index of corresponding taggable signal in `taggable` tree               |
| dirx          | Fitted X direction                                                            |
| diry          | Fitted Y direction                                                            |
| dirz          | Fitted Z direction                                                            |
| x             | X coordinate of fitted vertex (cm)                                            |
| y             | Y coordinate of fitted vertex (cm)                                            |
| z             | Z coordinate of fitted vertex (cm)                                            |

## ntag

| Branch name   | Description                                                              |
|---------------|--------------------------------------------------------------------------|
| NCandidates   | Total number of NTag candidates                                          |

Each entry in the following branches is an array with a size of the number of NTag candidates in each event.
NN label K stands for Keras, and T stands for TMVA.

| Branch name       | NN    | Description                                                                         |
|-------------------|:-----:|-------------------------------------------------------------------------------------|
| BSdirks           |       | BONSAI dirKS                                                                        |
| BSenergy          |       | BONSAI reconstructed energy (MeV)                                                   |
| BSovaq            |       | BONSAI ovaQ                                                                         |
| Beta_l            |  K/T  | \f$\beta_l=\frac{2}{N_{Hits}(N_{Hits}-1)}\sum_{i\neq j}P_l(\cos{\theta_ij})\f$      |
| BurstRatio        |   K   | Ratio of burst (dt < TRBNWIDTH) PMTs within candidate                               |
| DPrompt           |   -   | Distance from prompt vertex to fitted vertex (cm)                                   |
| DTaggable         |   -   | Distance from true taggable vertex to fitted vertex (cm)                            |
| DWall             |  K/T  | Distance from fitted vertex to nearest wall (cm)                                    |
| DWallMeanDir      |  K/T  | Distance from fitted vertex to wall in mean hit direction (cm)                      |
| FitGoodness       |   K   | Ad-hoc vertex fit goodness (see VertexFitManager::GetGoodness)                      |
| FitT              |   -   | Fitted time relative to trigger (µs)                                                |
| Label             |   -   | True class label (0: noise, 1: decay-e, 2: p(n,γ), 3: Gd(n,γ))                      |
| MeanDirAngleMean  |   T   | Mean of angles between each hit direction and mean hit direction                    |
| MeanDirAngleRMS   |   T   | RMS of angles between each hit direction and mean hit direction                     |
| N1300             |   T   | Number of hits within (-520, +780) ns from FitT                                     |
| N200              |   T   | Number of hits within ±100 ns from FitT                                             |
| N30               |   -   | Number of hits within ±15 ns from FitT                                              |
| N3000             |   -   | Number of hits within ±15 ns from FitT                                              |
| N50               |   -   | Number of hits within (-520, +2480) ns from FitT                                    |
| NHits             |  K/T  | Number of hits within [TCANWIDTH](#signal-search-parameters)                        |
| NNoisyPMT         |   -   | Number of hits with dark rate larger than average                                   |
| NResHits          |   K   | N200 - NHits                                                                        |
| NoisyPMTRatio     |   -   | Ratio of hits with dark rate larger than average                                    |
| OpeningAngleMean  |   T   | Mean of all possible 3-hit opening angles                                           |
| OpeningAngleSkew  |   T   | Skewness of all possible 3-hit opening angles                                       |
| OpeningAngleStdev |   T   | Standard deviation of all possible 3-hit opening angles                             |
| QSum              |   -   | Sum of deposit charge (p.e.)                                                        |
| SignalRatio       |   -   | Ratio of signal hits (non-zero only if `-add_noise true`)                           |
| TRMS              |  K/T  | RMS of ToF-subtracted hit times                                                     |
| TagClass          |   -   | Tag result (0: noise, 1: decay-e, 2: n-capture)                                     |
| TagIndex          |   -   | Array index of corresponding taggable signal in `taggable` tree                     |
| TagOut            |   -   | Neural-network output signal likelihood in range (0, 1)                             |
| fvx               |   -   | X coordinate of fitted vertex (cm)                                                  |
| fvy               |   -   | Y coordinate of fitted vertex (cm)                                                  |
| fvz               |   -   | Z coordinate of fitted vertex (cm)                                                  |