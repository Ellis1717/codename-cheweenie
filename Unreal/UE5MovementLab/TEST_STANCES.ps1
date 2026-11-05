param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$GameExe,[string]$OutputRoot,[switch]$Render)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/MovementStances'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
$info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '}
$backend=if($Render){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1280 -ResY=720'}else{'-nullrhi'}
$info.Arguments=$project+$backend+' -CheweenieStanceSmoke -ExecCmds="t.MaxFPS 60" -CheweenieTestOutput="'+$OutputRoot+'" -UserDir="'+(Join-Path $OutputRoot 'User')+'" -unattended -nosound -nosplash -AbsLog="'+(Join-Path $OutputRoot 'Stances.log')+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1';$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
try{while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 10){throw 'Movement stance audit timed out'}}}finally{if(!$process.HasExited){$process.Kill()}}
$path=Join-Path $OutputRoot 'movement-stances.json';if(!(Test-Path -LiteralPath $path)-or(Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Movement stance report missing/stale'}
$report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json;if($process.ExitCode-or $report.failed.Count-or $report.passed.Count-lt 50){throw ('Movement stance checks failed: '+($report.failed-join '; '))}
if($Render){foreach($name in @('MartyRunningJump.png','MartyTwoLegWalk.png','MartyCrawlingUnderBeam.png','MartyTwoLegJacket.png','MartyTwoLegFollow.png','MartyCrawlFollow.png')){if(!(Test-Path -LiteralPath (Join-Path $OutputRoot $name))){throw ('Missing movement capture: '+$name)}}}
'Movement stances: '+$report.passed.Count+' checks passed'
