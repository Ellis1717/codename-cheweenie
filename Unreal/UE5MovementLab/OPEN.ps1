param([string]$EngineRoot='D:\Epic Games\UE_5.8', [string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$module=Join-Path $PSScriptRoot 'Binaries\Win64\UnrealEditor-CheweenieUE5Lab.dll'
if(-not(Test-Path -LiteralPath $module)){throw 'Movement module is not built. Run BUILD.ps1 first; see README.md for the compiler requirement.'}
$editor=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
if(-not(Test-Path -LiteralPath $editor)){throw 'Unreal 5.8 editor missing; supply -EngineRoot.'}
$info=[System.Diagnostics.ProcessStartInfo]::new()
$info.FileName=$editor
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -dx12 -nosplash -windowed -ResX=1280 -ResY=720'
$info.UseShellExecute=$false
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
[System.Diagnostics.Process]::Start($info) | Select-Object Id
