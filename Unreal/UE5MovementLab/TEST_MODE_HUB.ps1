param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$GameExe,[string]$OutputRoot,[switch]$Render,[switch]$Lan,[int]$Port=18888,[string]$ArchiveIndex)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/ModeHub'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
if(!$ArchiveIndex -and $GameExe){$candidate=Join-Path (Split-Path -Parent (Split-Path -Parent $GameExe)) 'LocalGameData/ArchiveIndex.json';if(Test-Path -LiteralPath $candidate){$ArchiveIndex=$candidate}}
$roles=if($Lan){@('host','guest')}else{@('solo')};$processes=@();$started=[DateTime]::UtcNow
try {
    foreach($role in $roles){
        $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){$GameExe}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
        $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
        $rhi=if($Render){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
        $info.Arguments=$project+$rhi+' -CheweenieModeHub -CheweenieHubSmoke -CheweenieHubPort='+$Port+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/'+$role+'.log"'
        if($ArchiveIndex){$info.Arguments+=' -CheweenieArchiveIndex="'+$ArchiveIndex+'"'}
        if($Lan){$info.Arguments+=' -CheweenieHubLan'};if($role-eq 'guest'){$info.Arguments+=' -CheweenieHubGuest'}
        $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache'
        $processes+=$( $info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'; [Diagnostics.Process]::Start($info) )
        if($role-eq 'host'){
            $deadline=[DateTime]::UtcNow.AddMinutes(6)
            while(!(Test-Path -LiteralPath (Join-Path $OutputRoot 'host.log')) -or !(Select-String -LiteralPath (Join-Path $OutputRoot 'host.log') -Pattern ('IpNetDriver.*listening on port '+$Port) -Quiet)){
                if($processes[0].HasExited -or [DateTime]::UtcNow-gt $deadline){throw 'Hub host failed to start.'};Start-Sleep -Milliseconds 500
            }
        }
    }
    while(@($processes|Where-Object {!$_.HasExited}).Count){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 10){throw 'Mode hub test timed out.'};Start-Sleep -Milliseconds 500}
    foreach($process in $processes){if($process.ExitCode){throw 'Mode hub process failed; inspect its report and log.'}}
    foreach($role in $roles){
        $name=if($role-eq 'solo'){'hub-smoke.json'}else{'hub-'+$role+'.json'};$path=Join-Path $OutputRoot $name
        if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Hub report missing or stale.'}
        $report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
        if($report.failed.Count -or $report.passed.Count-lt $(if($Lan){7}else{29})){throw ('Incomplete/failed report '+$name)}
        $role+': '+$report.passed.Count+' passes'
    }
}finally{foreach($process in $processes){if(!$process.HasExited){$process.Kill()}}}
