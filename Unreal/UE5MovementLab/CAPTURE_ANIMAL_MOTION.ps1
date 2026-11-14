param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$OutputRoot,[string]$FFmpegPath)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/AnimalMotionReview'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
if(!$FFmpegPath){$FFmpegPath=(Get-ChildItem -LiteralPath (Join-Path $env:USERPROFILE '.codex/video-tools/imageio_ffmpeg/binaries') -Filter '*ffmpeg*.exe' | Select-Object -First 1).FullName}
if(!(Test-Path -LiteralPath $FFmpegPath)){throw 'Supply -FFmpegPath for the optional motion preview encoder.'}
$info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess -dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720 -CheweenieTrailerCapture -CheweenieAnimalMotion -UserDir="'+$OutputRoot+'/User" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Motion.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache';$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Animal motion capture timed out'}}
if($process.ExitCode){throw 'Animal motion capture failed'}
$frames=Join-Path $OutputRoot 'User/Saved/AnimalMotion';$receipt=Join-Path $frames 'capture-complete.txt'
if(!(Test-Path -LiteralPath $receipt) -or (Get-Item -LiteralPath $receipt).LastWriteTimeUtc -lt $started){throw 'Capture receipt missing or stale'}
foreach($n in 0..119){$file=Join-Path $frames ('frame_{0:d4}.png' -f $n);if(!(Test-Path -LiteralPath $file) -or (Get-Item -LiteralPath $file).LastWriteTimeUtc -lt $started){throw 'Missing or stale animation frame'}}
$movie=Join-Path $OutputRoot 'AnimalMotion.mp4'
& $FFmpegPath -v error -y -framerate 24 -i (Join-Path $frames 'frame_%04d.png') -frames:v 120 -c:v libx264 -crf 18 -pix_fmt yuv420p -movflags +faststart $movie
if($LASTEXITCODE){throw 'Preview encoding failed'}
& $FFmpegPath -v error -i $movie -f null NUL
if($LASTEXITCODE){throw 'Preview full decode failed'}
$movie
