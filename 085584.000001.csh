#!/bin/csh -f
cd /disk02/usr6/mharada/AmBe/apr22/data/swtrg_lib/
#source /home/sklowe/skofl/r29965/env.csh
source /usr/local/sklib_gcc8/skofl-trunk/env.csh

source ./path/csh set
hostname
./bin/NTag -out /disk02/calib3/usr/mharada/ambe/apr22/data//swtrg_lib/085584/ambe_swtrg.085584.000001.root -in /disk02/calib3/usr/mharada/ambe/apr22/data//shereduc/085584/ambe_shereduc.085584.000001.root -prompt_vertex none -delayed_vertex lowfit -vx -1237 -vy -70.7 -vz -1200 -REFRUNNO 85576 -SKBADOPT 55 -SKOPTN 31,30,25 -tagger cut tag_e false -add_noise false -TMIN 4. -TWIDTH 200 -TCANWIDTH 30 -TMATCHWINDOW 200 -NHITSTH 25 -NHITSMX 200 -TMINPEAKSEP 1300 -print 0
