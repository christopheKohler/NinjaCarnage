#!/bin/bash
## cd d:/Kristof/AmstradCPC_Dev/cpctelera-1.4.2_cygwin/ninjacarnage
rm ninjacarnageA.dsk
make all
mv ninja.dsk ninjacarnageA.dsk
../cpctelera/tools/iDSK-0.13/bin/iDSK.exe ninjacarnageA.dsk -i ./TIMEUP.BIN -i ./LEVEL01.BIN -i ./LEVEL02.BIN -i ./LEVEL11.BIN -i ./LEVEL12.BIN -i ./LEVEL13.BIN -i ./LEVEL21.BIN -i ./LEVEL22.BIN -i ./LEVEL23.BIN -i ./LEVEL31.BIN -i ./LEVEL32.BIN -i ./LEVEL33.BIN -i ./LEVEL41.BIN -i ./LEVEL42.BIN -i ./LEVEL43.BIN -i ./NINJA00.BIN -i ./NINJA01.BIN -i ./NINJA02.BIN -i ./NINJA03.BIN -i ./NINJA04.BIN -i ./NINJA05.BIN -i ./NINJA0C.BIN -i ./NINJA0T.BIN ./LEVEL51.BIN -i ./LEVEL52.BIN -i ./LEVEL53.BIN -i ./LEVELD1.BIN -i ./NINJA0D.BIN
read -p "Appuyer sur une touche pour continuer ..."
## make run
## read -p "Press [Enter] key to start backup..."
