param([string]$EngineRoot='D:\Epic Games\UE_5.8', [string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$info=[System.Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$project=Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject'
$info.Arguments='"'+$project+'" -dx12 -RenderOffscreen -unattended -nosplash -nosound -NoVSync -windowed -ResX=1280 -ResY=720 -AbsLog="'+(Join-Path $PSScriptRoot 'Saved\CapturePreview.log')+'"'
$info.UseShellExecute=$false
$info.CreateNoWindow=$true
$info.WindowStyle=[System.Diagnostics.ProcessWindowStyle]::Hidden
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$info.EnvironmentVariables['CHEWEENIE_UE5_CAPTURE']='1'
$startedAt=[DateTime]::UtcNow
$process=[System.Diagnostics.Process]::Start($info)
if(-not $process.WaitForExit(900000)){throw 'Capture exceeded 15 minutes; editor left running for diagnosis.'}
if($process.ExitCode -ne 0){throw ('Capture editor failed: '+$process.ExitCode)}
$capture=Join-Path $PSScriptRoot 'Saved\Screenshots\WindowsEditor\UE5Preview.png'
if(-not(Test-Path -LiteralPath $capture)){throw 'Unreal did not save the expected screenshot; inspect Saved/CapturePreview.log.'}
if((Get-Item -LiteralPath $capture).LastWriteTimeUtc -lt $startedAt){throw 'Capture is stale; inspect Saved/CapturePreview.log.'}
Get-Item -LiteralPath $capture | Select-Object FullName,Length
