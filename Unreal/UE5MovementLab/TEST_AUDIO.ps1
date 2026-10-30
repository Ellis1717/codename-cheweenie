param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[string]$GameExe,[string]$OutputRoot)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Audio'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
# Keep silent submixes rendering during acceptance so mute exports real zero samples.
# Normal play retains Unreal's silence optimization.
$info.Arguments=$project+'-dx12 -RenderOffscreen -windowed -ResX=1280 -ResY=720 -AudioMixer -ini:Engine:[Audio]:UnfocusedVolumeMultiplier=1.0 -ExecCmds="au.NeverDisableSubmixes 1" -CheweenieAudioSmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Audio.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Audio test timed out'}}
if($process.ExitCode){throw 'Audio runtime test failed; inspect Audio.log'}
$path=Join-Path $OutputRoot 'audio-smoke.json'
if(!(Test-Path $path) -or (Get-Item $path).LastWriteTimeUtc -lt $started){throw 'Audio report missing/stale'}
$report=Get-Content $path -Raw|ConvertFrom-Json
if($report.failed.Count -or $report.passed.Count -lt 10){throw 'Audio assertions failed'}
foreach($name in @('Barks','Music','Muted')){ $p=Join-Path $OutputRoot ($name+'.wav');if(!(Test-Path $p) -or (Get-Item $p).LastWriteTimeUtc -lt $started){throw ('Missing/stale audio capture: '+$name)} }
& (Join-Path $EngineRoot 'Engine\Binaries\ThirdParty\Python3\Win64\python.exe') (Join-Path $PSScriptRoot 'Scripts\verify_audio_capture.py') $OutputRoot
if($LASTEXITCODE){throw 'Recorded master output failed waveform checks'}
'Audio: '+$report.passed.Count+' assertions plus bark/music/mute waveform checks'
