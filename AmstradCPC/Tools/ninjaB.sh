#!/bin/bash
## cd d:/Kristof/AmstradCPC_Dev/cpctelera-1.4.2_cygwin/ninjacarnage
rm ninjacarnageB.dsk
make all
rm title.scr
cp titleB.scr TITLE.SCR
mv ninja.dsk ninjacarnageB.dsk
../cpctelera/tools/iDSK-0.13/bin/iDSK.exe ninjacarnageB.dsk -i ./TIMEUP.BIN -i ./LEVEL61.BIN -i ./LEVEL62.BIN -i ./LEVEL63.BIN -i ./LEVEL71.BIN -i ./LEVEL72.BIN -i ./LEVEL73.BIN -i ./LEVEL81.BIN -i ./LEVEL82.BIN -i ./LEVEL83.BIN -i ./LEVEL91.BIN -i ./LEVEL92.BIN -i ./LEVEL93.BIN -i ./NINJA06.BIN -i ./NINJA07.BIN -i ./NINJA08.BIN -i ./NINJA09.BIN -i ./NINJA0C.BIN -i ./LEVELC1.BIN -i ./LEVELC2.BIN -i ./LEVELC3.BIN -i ./NINJA0T.BIN -i ./TITLE.SCR
read -p "Appuyer sur une touche pour continuer ..."
## make run
## read -p "Press [Enter] key to start backup..."
