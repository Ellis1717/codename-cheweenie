param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[string]$OutputRoot,[switch]$StageOnly)
$ErrorActionPreference='Stop'
if(-not $OutputRoot){$OutputRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\builds\UnrealAdventure-0.6.1'))}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
$validationMarker=Join-Path $OutputRoot 'validated.json'
if(Test-Path -LiteralPath $validationMarker){Remove-Item -LiteralPath $validationMarker}
if(-not(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Content\Adventure\Audio\Home.uasset'))){throw 'Retained adventure audio missing; run IMPORT_AUDIO.ps1.'}
$uat=Join-Path $EngineRoot 'Engine\Build\BatchFiles\RunUAT.bat'
if(-not(Test-Path -LiteralPath $uat)){throw 'Unreal Automation Tool missing; supply -EngineRoot.'}
[Environment]::SetEnvironmentVariable('UE-LocalDataCachePath',$CacheRoot,'Process')
$project=Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject'
$prepare=if($StageOnly){@('-skipbuild','-skipcook')}else{@('-build','-cook')}
& $uat BuildCookRun "-project=$project" -nop4 -platform=Win64 -clientconfig=Development @prepare -stage -pak -archive -prereqs -nodebuginfo "-archivedirectory=$OutputRoot" -unattended -utf8output '-ubtargs=-NoXGE -NoUBA -MaxParallelActions=1 -gather'
if($LASTEXITCODE -ne 0){throw ('Windows packaging failed: '+$LASTEXITCODE)}
$game=Join-Path $OutputRoot 'Windows\CheweenieUE5Lab.exe'
if(-not(Test-Path -LiteralPath $game)){throw 'Packaged game launcher missing after archive.'}
Get-Item -LiteralPath $game | Select-Object FullName,Length
