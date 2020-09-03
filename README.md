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
./bin/main -in (input filename) -out (output filename)
```

### Run options

| Option | Argument |Example usage  | Description  |
|:---|---|------------------------|----------------|
| -in  | (input filename) |`./bin/NTag -in in.dat` |  (mandatory) |
| -out  |(output filename) |`./bin/NTag -in in.dat -out out.root` |  (optional) |
| -weight  | (weight filename) | `./bin/NTag -in in.dat -weight weight.xml` | (optional) |
| -vx | (custom vertex position [cm]) |`./bin/NTag -in in.dat -vx -400 -vy 0 -vz -1200`  |(optional) |
| -apply  | |`./bin/NTag ` |   |
| -train  |  | |   |
| -debug  |  | |   |



* "-in" : 
* "-out" :
* "-weight" :
* "-method" :
* "-vx / -vy / -vz" :
* "-apply" :
* "-train" :
* "-debug" : 