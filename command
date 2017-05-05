#!/bin/sh
./mcf_base.amd64-m64-gcc42-nn inp.in &
./cactusADM_base.amd64-m64-gcc42-nn benchADM.par &
./namd_base.amd64-m64-gcc42-nn  --input namd.input --iterations 38  --output namd.out &
./sjeng_base.amd64-m64-gcc42-nn ref.txt &
./sch
echo -e '\003' 
