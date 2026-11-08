param([ValidateSet('Hockey','CrewMenu','Camera','CoreConfig')][string]$Suite,[string]$GameExe,[string]$OutputRoot,[switch]$Render,[switch]$Resume,[string]$EngineRoot='D:/Epic Games/UE_5.8')
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot ('Saved/UnifiedActivities/'+$Suite)}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Force $OutputRoot|Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess -MultiprocessSaveConfig '}
$rhi=if($Render){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
$flag=@{Hockey='CheweenieHockeySmoke';CrewMenu='CheweenieCrewMenuSmoke';Camera='CheweenieCameraSmoke';CoreConfig='CheweenieCoreConfigSmoke'}[$Suite]
$name=@{Hockey='hockey-smoke.json';CrewMenu='crew-menu-smoke.json';Camera='camera-smoke.json';CoreConfig=if($Resume){'core-config-resume.json'}else{'core-config.json'}}[$Suite]
$info.Arguments=$project+$rhi+' -'+$flag+' -CheweenieTestOutput="'+$OutputRoot+'" -UserDir="'+$OutputRoot+'/User" -NoGamepad -unattended -nosplash -nosound -NoVSync -ExecCmds="t.MaxFPS 60,DisableAllScreenMessages" -AbsLog="'+$OutputRoot+'/'+$Suite+$(if($Resume){'-resume'}else{''})+'.log"'
if($Suite-eq 'Hockey'){$info.Arguments=$info.Arguments.Replace(' -nosound',' -AudioMixer')}
if($Resume){$info.Arguments+=' -CheweenieCoreConfigResume'}
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache';$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 15){$process.Kill();throw ($Suite+' timed out')}}
if($process.ExitCode){throw ($Suite+' failed; inspect its log')}
$dest=Join-Path $OutputRoot $name
if(!(Test-Path $dest)-or(Get-Item $dest).LastWriteTimeUtc-lt $started){$from=Join-Path $OutputRoot ('User/Saved/'+$name);if(!(Test-Path $from)-or(Get-Item $from).LastWriteTimeUtc-lt $started){throw ('Missing/stale '+$name)};Copy-Item $from $dest}
$j=Get-Content $dest -Raw|ConvertFrom-Json
$min=@{Hockey=15;CrewMenu=18;Camera=15;CoreConfig=if($Resume){4}else{10}}[$Suite]
if($j.failed.Count-or $j.passed.Count-lt $min){throw ($Suite+' assertions failed')}
if($Render){
 $names=@{Hockey=@('HockeyTruck.png','HockeyDressingRoom.png');Camera=@('CameraFollow.png','CameraOverview.png','CameraDogLevel.png');CrewMenu=@('CrewMainMenu.png','CrewMarty.png','CrewLakitu.png','CrewRio.png');CoreConfig=@('CoreConfiguration.png')}[$Suite]
 foreach($capture in $names){$dest=Join-Path $OutputRoot $capture;if(!(Test-Path $dest)-or(Get-Item $dest).LastWriteTimeUtc-lt $started){$from=Join-Path $OutputRoot ('User/Saved/'+$capture);if(!(Test-Path $from)-or(Get-Item $from).LastWriteTimeUtc-lt $started){throw ('Missing/stale capture '+$capture)};Copy-Item $from $dest}}
}
$Suite+': '+$j.passed.Count+' assertions passed'
if($Suite-eq 'CoreConfig'-and !$Resume){& $PSCommandPath -Suite CoreConfig -GameExe $GameExe -OutputRoot $OutputRoot -Resume -EngineRoot $EngineRoot}
