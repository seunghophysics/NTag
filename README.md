# NTag

NTag is a C++-based program to search for neutron capture candidates in SK data/MC events and tag with multivariate analysis (MVA) tool provided by CERN's ROOT. Any SK data/MC file can be fed to NTag to get an output root file that comes with the `ntvar` tree filled with relevant variables to the neutron tagging algorithm. 

## Structure of the program

All neutron tagging in one event is steered by a mainframe class named `NTagEventInfo`. 

It searches for the neutron capture candidates from the PMT hit times subtracted by the ToF to each PMT from the assumed neutron creation vertex (which can be either fit with APFit or BONSAI, or given with custom vertex options). The found candidates are fed to CERN ROOT's TMVA to get a classifier output. The higher the candidate's classifier value is, the more likely it is coming from a true neutron capture.

## Output structure

* TTree `ntvar`

nrun           :  run number            
nev            :  event number          
nsub           :  subrun number           
ndcy		: number of decay electron of sub event type
apnmue		: # of decay-e
vx,vy,vz	: apfit neutrino vertex
wall		: distance to neutrino vertex from wall (cm)
evis		: visible energy (MeV/c)
trgofst		: trigger offset
nhitac		: OD timecut hit in cluster 
N200M 		: max N200 in this event
T200M 		: time of N200M happens

np		: # capture candidate
dt(np)		: candidate time (ns)
N10(np)		: # PMT hits in 10ns
N200(np)	: # PMT hits in 200ns
sumQ(np)	: sum of Q in 10ns
trmsold(np)	: RMS of PMT hit time in 10ns
spread(np)	: spread of PMT hit time (max-min) in 10ns
tindex(np)	: index of first hit in raw PMT hit time
beta14(np)	: beta14 calculated in 10ns

N50(np)		: # PMT hits in 50ns
beta14_40(np),	: beta calculated in 50ns
beta1(np),
beta2(np),
beta3(np),
beta4(np)
beta5(np)

tbsenergy(np)	: BONSAI energy 			in 50ns
tbsvx(np),	: BONSAI vertex
tbsvy(np),
tbsvz(np)
tbswall(np)	: distance to wall from BONSAI vertex
tbsgood(np),	: BONSAI fit goodness paramters
tbsdirks(np),
tbspatlik(np),
tbsovaq(np)

nvx,nvy,nvz(np) : neut-fit vertex (vertex fitter by simply minimizing trms calculated from hits in 50ns)
nwall(np)	: distance to wall from neut-fit vertex
N10n(np)	: N10 calculated with neut-fit vertex
dtn(np)		: candidate time calculated with neut-fit vertex
trms(np)	: RMS of PMT hit time in 10ns calculated with neut-fit vertex
trms40(np)	: RMS of PMT hit time in 50ns calculated with neut-fit vertex

realneutron(np)	: true capture (signal) or bkg
doubleCount(np) : double-count event or not
timeRes(np)	: time diff. between true capture time and candidate time
truth_vx(np),	: related true capture vertex
truth_vy(np),
truth_vz(np)
nGd(np)		: Gd-capture or H-capture

TMVAoutput(np)	: TMVA classifier output value("BDT method" by default)

* TTree `truth`

trgofst		: trigger offset
nhitac		: OD timecut hit in cluster 
ndcy		: number of decay electron of sub event type
qismsk		: sum of Q of inner PMT
apnmue		: # of decay-e
pvx		: apfit neutrino vertex
wall		: distance to neutrino vertex from wall (cm)
evis		: visible energy (MeV/c)
                  This is the sum of amome of each rings
apnring		: # of cherenkov ring
apip(nring)	: ID of each ring's particle
apamom(nring)	: reconstructed momentum 
amome(nring)	: momentum  as e-like 
amomm(nring)	: momentum  as mu-like 

nCT		: number of true neutron capture
captureTime(nCT): capture time (ns)
capPos(nCT)	: capture vertex
nGam(nCT)	: gamma multiplicity
totGamEn(nCT)	: total gamma energy emitted (MeV) 

nscnd      	: Number                        of secondary particles (neutron & capture related)
vtxscnd       	: Vertex                        of the generated point
pscnd         	: Momentum                      of the secondary particle
pabsscnd        : Magnitude of Momentum         of the secondary particle
iprtscnd      	: Particle code                 of the secondary particle
tscnd         	: Generated time                of the secondary particle
iprntprt      	: Parent particle code          of the secondary particle
lmecscnd      	: Interaction code (GEANT) that produced the secondary particle (18: Neutron Capture)
wallscnd	: distance to capture vertex from wall (cm)
capId		: which capture (nCT) this particle is related to

numne          :  number of particle at neutrino interaction
               :  numnu=1    incoming neutrino
               :        2    target
               :        3    outgoing lepton
               :        4    outgoing target
               :      >=5    other particles
modene         :  interaction mode of neutrino, see neut/nemodsel.F
               :  +-1        CC quasi-elastic
               :  +-(11-13)  CC single pi from delta resonance
               :  +-16       CC coherent pi production
               :  +-21       CC multi pi production
               :  +-27       CC diffractive pion production
               :  +-(31-34)  NC single pi from delta resonance
               :  +-36       NC coherent pi 
               :  +-41       NC multi pi production
               :  +-47       NC diffractive pion production
               :  +-(51,52)  NC elastic
ipne(numne)    :  particle code at neutrino interaction (P.D.G. code)
               :  ipnu(1)==12 nu-e    ipnu(1)==-12 nu-e-bar
               :  ipnu(1)==14 nu-mu   ipnu(1)==-14 nu-mu-bar
               :  ipnu(1)==16 nu-tau  ipnu(1)==-16 nu-tau-bar
pnu(numnu)     :  momentum at neutrino interaction (GeV/c)
nN	       :  # of neutron in input vector

nvect          :  number of particle of Vector at detector simulation
ip(nvect)      :  Vector particle code (Geant code)
pos(3)         :  Vector vertex   
pin(nvect,3)   :  Vector momentum
pabs(nvect)    :  Vector momentum magnitude

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
