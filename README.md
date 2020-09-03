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