SET GFXCONV=..\..\ToolsForFramework\GfxConv\gfxconvd.exe
SET MERGE=..\..\ToolsForFramework\MergeBinaryFiles\Release\MergeBinaryFiles.exe
SET CONVPNGBOB=..\..\ToolsForFramework\ConvertPNGBob\ConvertBob.exe

%GFXCONV% datas_source/IntroBack.png datas_source/IntroBack.ami imagepal
%GFXCONV% datas_source/IntroTitle.png datas_source/IntroTitle.ami imagepal
%GFXCONV% datas_source/IntroFlags.png datas_source/IntroFlags.ami imagepal
%GFXCONV% datas_source/IntroLogo.png datas_source/IntroLogo.ami imagepal

%GFXCONV% datas_source/IconLoad.png datas_source/IconLoad.ami imagepal


%MERGE% datas_source/IntroTitle.ami datas_source/IntroLogo.ami datas_source/1.tmp
%MERGE% datas_source/1.tmp datas_source/Resistance.raw datas_source/2.tmp
%MERGE% datas_source/2.tmp datas_source/NinjaCarnage.raw datas/INTRO.BIN 

copy datas_source\IntroFlags.ami datas\FLAGS.BIN
echo Done!
pause
