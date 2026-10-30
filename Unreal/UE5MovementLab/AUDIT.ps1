param([string]$EngineRoot='D:\Epic Games\UE_5.8', [string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$editor=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
if(-not(Test-Path -LiteralPath $editor)){throw 'Unreal 5.8 editor missing; supply -EngineRoot.'}
New-Item -ItemType Directory -Path (Join-Path $PSScriptRoot 'Saved') -Force | Out-Null
$project=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\UE5VisualLab\CheweenieUE5Lab.uproject'))
$script=(Join-Path $PSScriptRoot 'Scripts\audit_assets.py').Replace('\','/')
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=$editor
$info.Arguments='"'+$project+'" -ExecutePythonScript="'+$script+'" -unattended -nop4 -nosplash -nullrhi -AbsLog="'+(Join-Path $PSScriptRoot 'Saved\CollisionAudit.log')+'"'
$info.UseShellExecute=$false
$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$startedAt=[DateTime]::UtcNow
$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)) {
    if(([DateTime]::UtcNow-$startedAt).TotalMinutes -gt 10){$process.Kill(); throw 'Native asset audit timed out.'}
}
if($process.ExitCode -ne 0){throw ('Native asset audit failed: '+$process.ExitCode)}
$report=Join-Path $PSScriptRoot 'Saved\asset-audit.json'
if(-not(Test-Path -LiteralPath $report)){throw 'Native asset audit report missing.'}
if((Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $startedAt){throw 'Native asset audit report is stale. Inspect Saved/CollisionAudit.log.'}
$result=Get-Content -LiteralPath $report -Raw | ConvertFrom-Json
if(@($result.traversal_collision.PSObject.Properties).Count -lt 4){throw 'Traversal collision checks missing from report.'}
$result
