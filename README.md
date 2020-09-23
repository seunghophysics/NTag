# NTag

NTag is a C++-based program to search for neutron capture candidates in SK data/MC events and tag with multivariate analysis (MVA) tool provided by CERN's ROOT. Any SK data/MC file can be fed to NTag to get an output root file that comes with the `ntvar` tree filled with relevant variables to the neutron tagging algorithm.

## Structure of the program

All neutron tagging in one event is steered by a mainframe class named `NTagEventInfo`.

It searches for the neutron capture candidates from the PMT hit times subtracted by the ToF to each PMT from the assumed neutron creation vertex (which can be either fit with APFit or BONSAI, or given with custom vertex options). The found candidates are fed to CERN ROOT's TMVA to get a classifier output. The higher the candidate's classifier value is, the more likely it is coming from a true neutron capture.

## Getting Started

### Dependencies

> SKOFL: `skofl_19b`
>
> ATMPD: `ATMPD_19b`
>
> ROOT: `v5.28.00h`
>
> TMVA: `v.4.2.0`

### How to install
```
git clone https://github.com/seunghophysics/NTag.git
```
```
cd NTag; make
```

### How to run
```
./bin/NTag -in (input filename) -out (output filename)
```

### Command line options

* Argument options

| Option |           Argument          |                      Example usage                   |   Use   |
|:-------|-----------------------------|------------------------------------------------------|---------|
|-in     |(input filename)             |`./bin/NTag -in in.dat`                               |mandatory|
|-out    |(output filename)            |`./bin/NTag -in in.dat -out out.root`                 |optional |
|-weight |(weight filename)            |`./bin/NTag -in in.dat -weight weight.xml`            |optional |
|-method |(MVA training method name)   |`./bin/NTag -in in.dat -method MLP -weight weight.xml`|optional |
|-vx(y,z)|(custom vertex position) [cm]|`./bin/NTag -in in.dat -vx -400 -vy 0 -vz -1200`      |optional |

* Run options

|Option|                      Example usage                                | Description |
|:-----|-------------------------------------------------------------------|-------------|
|-apply|`./bin/NTag -in NTagOut.root -apply -method MLP -weight weight.xml`|Apply specific MVA weight/method to an existing NTag output (with ntvar & truth trees) to replace the existing TMVAoutput with given weight/method. |
|-train|`./bin/NTag -in NTagOut.root -train`                               |Train with NTag output from MC (with ntvar & truth trees) to generate weight files.|
|-debug|`./bin/NTag (...) -debug`                                          |Show debug messages on output stream.|

## Output structure

* TTree `ntvar`

| Branch name      | Size        | NN | Description                                             |
|------------------|-------------|----|---------------------------------------------------------|
| RunNo            | 1           | X  | Run number                                              |
| SubrunNo         | 1           | X  | Subrun number                                           |
| EventNo          | 1           | X  | Event number                                            |
| APNDecays        | 1           | X  | # of decay electrons of sub event type                  |
| APNMuE	         | 1           | X  | # of decay electrons                                    |
| APNRings		     | APNRings    | X  | # of Cherenkov rings                                    |
| APRingPID	       | APNRings    | X  | PID of ring                                             |
| APMom	           | APNRings    | X  | Reconstructed ring momentum                             |
| APMomE	         | APNRings    | X  | Reconstructed ring momentum as e-like                   |
| APMomMu	         | APNRings    | X  | Reconstructed ring momentum as mu-like                  |
| pvx              | 1           | X  | X coordinate of APFit neutrino vertex                   |
| pvy              | 1           | X  | Y coordinate of APFit neutrino vertex                   |
| pvz              | 1           | X  | Z coordinate of APFit neutrino vertex                   |
| DWall	           | 1           | X  | Distance to neutrino vertex from wall (cm)              |
| EVis	           | 1           | X  | Visible energy (MeV/c)                                  |
| TrgType	         | 1           | X  | Trigger type (0: MC, 1: SHE, 2: SHE+AFT)                |
| NHITAC	         | 1           | X  | Number of OD hits                                       |
| MaxN200		       | 1           | X  | Max N200 in event                                       |
| MaxN200Time	     | 1           | X  | Time of MaxN200 peak                                    |
| NCandidates	     | 1           | X  | # of capture candidates                                 |
| FirstHitTime_ToF | 1           | X  | Index of first hit in T_ToF                             |
| nvx              | NCandidates | X  | X coordinate of Neut-fit vertex                         |
| nvy              | NCandidates | X  | Y coordinate of Neut-fit vertex                         |
| nvz              | NCandidates | X  | Z coordinate of Neut-fit vertex                         |
| N10n             | NCandidates | X  | N10 calculated with Neut-fit vertex                     |
| N10	             | NCandidates | O  | # of PMT hits in 10 ns                                  |
| N50              | NCandidates | O  | # of PMT hits in 50 ns                                  |
| N200             | NCandidates | O  | # of PMT hits in 200 ns                                 |
| ReconCT	         | NCandidates | O  | Reconstructed capture time (ns)                         |
| QSum10           | NCandidates | O  | Sum of Q in 10 ns (p.e.)                                |
| TRMS10           | NCandidates | O  | RMS of PMT hit time in 10 ns                            |
| TRMS50	         | NCandidates | O  | RMS of PMT hit time in 50 ns with Neut-fit vertex       |
| TSpread          | NCandidates | O  | Spread of PMT hit time (max-min) in 10 ns               |
| Beta14_10	       | NCandidates | O  | Beta14 calculated in 10 ns                              |
| Beta14_50	       | NCandidates | O  | Beta14 calculated in 50 ns                              |
| Beta1            | NCandidates | O  | Beta1 calculated in 10 ns                               |
| Beta2            | NCandidates | O  | Beta2 calculated in 10 ns                               |
| Beta3            | NCandidates | O  | Beta3 calculated in 10 ns                               |
| Beta4            | NCandidates | O  | Beta4 calculated in 10 ns                               |
| Beta5            | NCandidates | O  | Beta5 calculated in 10 ns                               |
| BSenergy	       | NCandidates | O  | BONSAI energy in 50 ns                                  |
| bsvx	           | NCandidates | O  | X coordinate of BONSAI vertex                           |
| bxvy             | NCandidates | O  | Y coordinate of BONSAI vertex                           |
| bsvz             | NCandidates | O  | Z coordinate of BONSAI vertex                           |
| BSwall	         | NCandidates | O  | Distance to wall from BONSAI vertex                     |
| BSgood	         | NCandidates | O  | BONSAI fit goodness paramters                           |
| BSdirks          | NCandidates | O  | BONSAI dir_KS                                           |
| BSpatlik         | NCandidates | O  | BONSAI pattern likelihood                               |
| BSovaq           | NCandidates | O  | BONSAI ovaq                                             |
| NWall	           | NCandidates | O  | Distance to wall from Neut-fit vertex                   |
| prompt_bonsai	   | NCandidates | O  | Distance to BONSAI vertex from prompt vertex            |
| prompt_nfit	     | NCandidates | O  | Distance to Neut-fit vertex from prompt vertex          |
| bonsai_nfit	     | NCandidates | O  | Distance to Neut-fit vertex from BONSAI vertex          |
| ReconCTn		     | NCandidates | X  | (MC-only) Capture time with Neut-fit                    |
| TRMS10n	         | NCandidates | X  | (MC-only) PMT hit time RMS in 10 ns with Neut-fit       |
| IsCapture	       | NCandidates | X  | (MC-only) 0: Not true capture 1: True capture           |
| DoubleCount      | NCandidates | X  | (MC-only) 0: Not double count 1: Double count           |
| CTDiff	         | NCandidates | X  | (MC-only) Diff. between true/recon capture times (ns)   |
| truecapvx	       | NCandidates | X  | (MC-only) X of related true capture vertex (cm)         |
| truecapvy        | NCandidates | X  | (MC-only) Y of related true capture vertex (cm)         |
| truecapvz        | NCandidates | X  | (MC-only) Z of related true capture vertex (cm)         |
| IsGdCapture		   | NCandidates | X  | (MC-only) 0: Not Gd-capture 1: Gd-capture               |
| TMVAOutput    	 | NCandidates | X  | TMVA classifier output value                            |

* TTree `truth`

| Branch name   | Size          | Description                                                   |
|---------------|---------------|---------------------------------------------------------------|
| TrgType		    | 1             | Trigger offset                                                |
| NTrueCaptures | 1             | # of true neutron captures                                    |
| TrueCT        | NTrueCaptures | True capture time (ns)                                        |
| capvx 	      | NTrueCaptures | True capture vertex (cm)                                      |
| capvy 	      | NTrueCaptures | True capture vertex (cm)                                      |
| capvz 	      | NTrueCaptures | True capture vertex (cm)                                      |
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
| SecPID        | NSavedSec     | Particle code of the secondary particle                       |
| SecT          | NSavedSec     | Generated time of the secondary particle                      |
| ParentPID     | NSavedSec     | Parent particle code of the secondary particle                |
| SecIntID      | NSavedSec     | Interaction code (GEANT3) that produced the secondary         |
| SecDWall	    | NSavedSec     | Distance to capture vertex from wall (cm)                     |
| SecCaptureID  | NSavedSec     | Related true capture ID                                       |
| NVecInNeut    | 1             | # of particle at neutrino interaction                         |
| NeutIntMode   | 1             | Interaction mode of neutrino, see NEUT/nemodsel.F             |
| NeutIntMom    | 1             | Momentum at neutrino interaction (GeV/c)                      |
| NnInNeutVec	  | 1             | # of neutron in input vector                                  |
| NeutVecPID    | NVecInNeut    | Particle code at neutrino interaction (PDG code)              |
| NVec          | 1             | # of particle of primary vectors in MC                        |
| vecx          | 1             | X coordinate of primary vector vertex                         |
| vecy          | 1             | X coordinate of primary vector vertex                         |
| vecz          | 1             | X coordinate of primary vector vertex                         |
| VecPID        | NVec          | Primary vector particle code (Geant3 code)                    |
| VecMom        | NVec          | Primary vector momentum                                       |
| vecpx         | NVec          | X-direction momentum of primary vector                        |
| vecpy         | NVec          | Y-direction momentum of primary vector                        |
| vecpz         | NVec          | Z-direction momentum of primary vector                        |