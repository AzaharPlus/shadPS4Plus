@echo off
setlocal EnableDelayedExpansion

set par=%2%
set gamesDir="PATH_TO_GAMES_DIR"
set addonsDir="PATH_TO_ADDONS_DIR"

IF %gamesDir%=="PATH_TO_GAMES_DIR" (
	echo You need to update gamesDir with the games path used by ShadPs4
	IF NOT "%par%"=="--batch" pause
	exit 0
)

IF %addonsDir%=="PATH_TO_ADDONS_DIR" (
	echo You need to update addonsDir with the addons path used by ShadPs4
	IF NOT "%par%"=="--batch" pause
	exit 0
)

%0\..\pkg_extractor.exe %1% --check-type

IF %errorlevel%==0 (
	echo An error has occurred
) ELSE (
	IF %errorlevel%==101 (
		echo The file is a base game, installing to %gamesDir%
	)

	IF %errorlevel%==102 (
		echo The file is a game update, installing to %gamesDir%
	)

	IF %errorlevel%==103 (
		echo The file is a dlc, installing to %addonsDir%
		set "gamesDir=%addonsDir%"
	)
	
	%0\..\pkg_extractor.exe %1 !gamesDir!
)

IF NOT "%par%"=="--batch" pause
