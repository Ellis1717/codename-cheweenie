param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot)
$ErrorActionPreference='Stop'
if(-not $OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
foreach($phase in @('patrol','resume')) {
    $info=[Diagnostics.ProcessStartInfo]::new()
    $info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
    $backend=if($Render -and $phase -eq 'patrol'){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase -eq 'resume'){' -CheweenieResumeCheck'}else{''}
    $projectArg=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $info.Arguments=$projectArg+$backend+' -CheweenieAdventureSmoke'+$resume+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosplash -nosound -NoVSync -AbsLog="'+(Join-Path $OutputRoot ('Adventure-'+$phase+'.log'))+'"'
    $info.UseShellExecute=$false
    $info.CreateNoWindow=$true
    $info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    $startedAt=[DateTime]::UtcNow
    $process=[Diagnostics.Process]::Start($info)
    while(-not $process.WaitForExit(1000)) {
        if(([DateTime]::UtcNow-$startedAt).TotalMinutes -gt 10){$process.Kill();throw ('Adventure acceptance timed out: '+$phase)}
    }
    if($process.ExitCode -ne 0){throw ('Adventure acceptance failed: '+$phase+' '+$process.ExitCode)}
    $report=Join-Path $OutputRoot $(if($phase -eq 'patrol'){'adventure-smoke.json'}else{'adventure-resume.json'})
    if(-not(Test-Path -LiteralPath $report)){throw ('Missing report: '+$phase)}
    if((Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $startedAt){throw ('Stale report: '+$phase)}
    $result=Get-Content -LiteralPath $report -Raw | ConvertFrom-Json
    $minimum=if($phase -eq 'patrol'){20}else{4}
    if($result.failed.Count -or $result.passed.Count -lt $minimum){throw ('Adventure checks failed: '+$phase+'. Inspect Saved/Adventure-'+$phase+'.log')}
    if($Render -and $phase -eq 'patrol') {
        foreach($name in @('AdventureTitle.png','AdventurePlay.png','AdventureComplete.png')) {
            $capture=Join-Path $OutputRoot $name
            if(-not(Test-Path -LiteralPath $capture)){throw ('Capture missing: '+$name)}
            if((Get-Item -LiteralPath $capture).LastWriteTimeUtc -lt $startedAt){throw ('Capture stale: '+$name)}
        }
    }
    $phase+': '+$result.passed.Count+' passes'
}
