
set arg1=%1
set Release=Release
set Debug=Debug
	
xcopy			/y			.\Engine\Bin\Engine.lib			.\Reference\Librarys\
xcopy			/y			.\Engine\ThirdPartyLib\*.lib	.\Reference\Librarys\
xcopy			/y			.\Engine\ThirdPartyLib\*.dll	.\Client\Bin\
xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\
xcopy			/y/s		.\Engine\public\*.*				.\Reference\Headers\

if %Release% == %arg1% xcopy			/y			.\Engine\ThirdPartyLib\physx\release\*.dll	.\Client\Bin\
if %Debug% == %arg1% xcopy			/y			.\Engine\ThirdPartyLib\physx\debug\*.dll	.\Client\Bin\