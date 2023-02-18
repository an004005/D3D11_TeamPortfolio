
	
xcopy			/y			.\Engine\Bin\Engine.lib			.\Reference\Librarys\
xcopy			/y			.\Engine\ThirdPartyLib\*.lib	.\Reference\Librarys\
xcopy			/y			.\Engine\ThirdPartyLib\*.dll	.\Client\Bin\
xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\
xcopy			/y/s		.\Engine\public\*.*				.\Reference\Headers\