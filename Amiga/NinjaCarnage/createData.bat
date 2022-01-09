SET GFXCONV=..\..\Tools\GfxConv\gfxconvd.exe
SET PALSTRIP=..\..\Tools\StripImagePalette\Release\StripImagePalette.exe

%GFXCONV% data/Banner320_8colors.png data/Banner320_8colors.ami imagepal
echo Done!
pause
