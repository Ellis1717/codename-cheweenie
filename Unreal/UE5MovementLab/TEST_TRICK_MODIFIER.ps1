param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$GameExe,[string]$OutputRoot,[switch]$Render)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/TrickModifier'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
$info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){$GameExe}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
$rhi=if($Render){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1280 -ResY=720'}else{'-nullrhi'}
$info.Arguments=$project+$rhi+' -CheweenieTricksSmoke -CheweenieTrickModifierSmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -AbsLog="'+$OutputRoot+'/Modifier.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 10){$process.Kill();throw 'Modifier acceptance timed out.'}}
$path=Join-Path $OutputRoot 'trick-modifier.json'
if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Modifier report missing or stale.'}
$report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
if($process.ExitCode -or $report.failed.Count -or $report.passed.Count-lt 32){throw 'Modifier acceptance failed; inspect the report and log.'}
if($Render){foreach($file in @('TrickModifierGuide.png','TrickModifierLie.png','TrickModifierRoll.png','TrickModifierShake.png')){if(!(Test-Path -LiteralPath (Join-Path $OutputRoot $file)) -or (Get-Item -LiteralPath (Join-Path $OutputRoot $file)).LastWriteTimeUtc-lt $started){throw ('Modifier capture missing or stale: '+$file)}}}
'Direct trick modifier: '+$report.passed.Count+' passes'
