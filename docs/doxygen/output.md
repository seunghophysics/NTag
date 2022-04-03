# Output tree structure {#output-tree-structure}

## ntvar

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

## truth

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

## restq

This tree is generated if `-saveTQ` option is passed to the main function.

| Branch name   | Size              | Description                                                   |
|---------------|-------------------|---------------------------------------------------------------|
| T             | # of in-gate hits | ToF-subtracted PMT hit times (ns) sorted in ascending order   |
| Q             | # of in-gate hits | Q (p.e.) of each hit in T                                     |
| I             | # of in-gate hits | PMT cable ID of each hit in T                                 |