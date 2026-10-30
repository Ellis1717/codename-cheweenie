param([string]$EngineRoot='D:\Epic Games\UE_5.8', [string]$CacheRoot='D:\CheweenieValidation\UE5Cache', [switch]$Play)
$ErrorActionPreference='Stop'
$editor=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
if(-not(Test-Path -LiteralPath $editor)){throw 'UE5 editor missing; supply -EngineRoot.'}
$info=[System.Diagnostics.ProcessStartInfo]::new()
$info.FileName=$editor
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -dx12 -nosplash -NoVSync'
if($Play){$info.Arguments+=' -game -windowed -ResX=1280 -ResY=720'}
$info.UseShellExecute=$false
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
[System.Diagnostics.Process]::Start($info) | Select-Object Id
