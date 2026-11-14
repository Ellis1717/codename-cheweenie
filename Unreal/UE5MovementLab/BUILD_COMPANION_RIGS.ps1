param([string]$Blender='C:\Program Files\Blender Foundation\Blender 5.0\blender.exe',[string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
& $Blender --background --python (Join-Path $PSScriptRoot 'Scripts/build_companion_rigs.py')
if($LASTEXITCODE){throw 'Companion model export failed'}
$info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -ExecutePythonScript="'+(Join-Path $PSScriptRoot 'Scripts/import_companion_rigs.py')+'" -unattended -nop4 -nosplash -nullrhi -AbsLog="'+(Join-Path $PSScriptRoot 'Saved/ImportCompanionRigs.log')+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot;$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Companion rig import timed out'}}
if($process.ExitCode){throw 'Companion rig import failed'}
$report=Join-Path $PSScriptRoot 'Saved/companion-rigs-import.json'
if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $started){throw 'Import report missing or stale'}
$data=Get-Content -LiteralPath $report -Raw | ConvertFrom-Json
foreach($name in @('Bauer','Hank','Potcake','Lakitu','Rio')){if(!$data.$name.skeletalUsage){throw ('Skeletal materials missing for '+$name)}}
'All five companion rigs imported'
