param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[string]$GameExe,[string]$OutputRoot,[switch]$Render,[int]$Port=17777,[switch]$Ratchet)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Coop'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
$started=[DateTime]::UtcNow
$processes=@()
try {
    foreach($role in @('host','guest')) {
        $info=[Diagnostics.ProcessStartInfo]::new()
        $info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
        $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" '}
        $url=if($role -eq 'host'){'/Game/VisualLab/Comparison?game=/Script/CheweenieUE5Lab.CozyCoopMode?listen'}else{'127.0.0.1:'+$Port}
        $rhi=if($Render){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1280 -ResY=720'}else{'-nullrhi'}
        $info.Arguments=$project+$url+' -game '+$rhi+' -port='+$Port+' -CheweenieCoopSmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/'+$role+'.log"'
        if($Ratchet){$info.Arguments+=' -CheweenieRatchetLan'}
        $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
        $processes+= [Diagnostics.Process]::Start($info)
        if($role -eq 'host'){
            $deadline=[DateTime]::UtcNow.AddSeconds(60)
            while(!(Test-Path (Join-Path $OutputRoot 'host.log')) -or !(Select-String -LiteralPath (Join-Path $OutputRoot 'host.log') -Pattern 'IpNetDriver.*listening on port' -Quiet)) {
                if($processes[0].HasExited -or [DateTime]::UtcNow -gt $deadline){throw 'Listen server did not start. Inspect host.log.'};Start-Sleep -Milliseconds 500
            }
        }
    }
    while(@($processes|Where-Object {!$_.HasExited}).Count){if(([DateTime]::UtcNow-$started).TotalSeconds -gt 150){throw 'Two-process co-op timed out'};Start-Sleep -Milliseconds 500}
    foreach($p in $processes){if($p.ExitCode){throw 'A co-op process failed; inspect reports and logs'}}
    foreach($role in @('host','guest')){
        $report=Join-Path $OutputRoot ('coop-'+$role+'.json')
        if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Missing/stale co-op report'}
        $result=Get-Content $report -Raw|ConvertFrom-Json
        if($result.failed.Count -or $result.passed.Count -lt 20){throw ('Incomplete/failed '+$role+' acceptance')}
        $role+': '+$result.passed.Count+' passes'
        if($Render){$capture=Join-Path $OutputRoot $(if($role -eq 'host'){'CoopHost.png'}else{'CoopGuest.png'});if(!(Test-Path $capture) -or (Get-Item $capture).LastWriteTimeUtc -lt $started){throw 'Missing/stale co-op gameplay capture'}}
    }
} finally {foreach($p in $processes){if(!$p.HasExited){$p.Kill()}}}
