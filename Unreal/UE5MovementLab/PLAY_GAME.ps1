param([string]$PackageRoot=$PSScriptRoot,[switch]$ArchiveAcceptance,[string]$TestOutput,[switch]$VerifyArchives,[string]$TestArchiveId)
$ErrorActionPreference='Stop'
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot)
$local=Join-Path $PackageRoot 'LocalGameData'
$saved=Join-Path $PackageRoot 'HubSaved'
New-Item -ItemType Directory -Path $saved -Force|Out-Null
$request=Join-Path $saved 'ArchiveRequest.json'
$token=[guid]::NewGuid().ToString('N')

function Assert-Payload($base,$files){
    $base=[IO.Path]::GetFullPath($base).TrimEnd('\','/')
    if(!$files.Count){throw 'Payload integrity manifest is empty.'}
    foreach($file in $files){
        if([IO.Path]::IsPathRooted($file.path)){throw 'Absolute payload path rejected.'}
        $target=[IO.Path]::GetFullPath((Join-Path $base $file.path))
        if(!$target.StartsWith($base+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'Payload path outside its build rejected.'}
        if(!(Test-Path -LiteralPath $target -PathType Leaf) -or (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $file.sha256){throw ('Build integrity check failed: '+$file.path)}
    }
}
function Run-Game([string]$exe,[string[]]$arguments){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=$exe;$info.WorkingDirectory=Split-Path -Parent $exe
    $info.UseShellExecute=$false
    $info.Arguments=$arguments -join ' '
    $child=[Diagnostics.Process]::Start($info)
    while(!$child.WaitForExit(1000)){}
    if($child.ExitCode){throw ('Game exited with code '+$child.ExitCode)}
}
function Get-VerifiedArchiveExe($archive){
    $base=[IO.Path]::GetFullPath($archive.root).TrimEnd('\','/')
    $exe=[IO.Path]::GetFullPath($archive.exe)
    if(!$exe.StartsWith($base+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase) -or [IO.Path]::GetFileName($exe)-notin @('Cheweenie.exe','CheweenieLab.exe','CheweenieUE5Lab.exe')){throw 'Archive executable is outside its recorded build.'}
    Assert-Payload $base @($archive.files)
    return $exe
}

$marker=Join-Path $PackageRoot 'validated-unified.json'
if(!(Test-Path -LiteralPath $marker)){throw 'Validated unified package missing; run the release checks first.'}
$gate=Get-Content -LiteralPath $marker -Raw|ConvertFrom-Json
$minimumChecks=if($gate.version-in @('Unified-0.14.0','Unified-0.14.1','Unified-0.14.2','Unified-0.15.0','Unified-0.15.1','Unified-0.16.0')){if(!$gate.combinedGameVerified){throw 'Combined-game checks missing'};824}elseif($gate.version-eq 'Unified-0.13.1'){if($gate.overlaysVerified){689}else{607}}else{605}
if($gate.version-eq 'Unified-0.14.1'){if(!$gate.controllerButtonsVerified){throw 'Controller-button checks missing'};$minimumChecks=874}
if($gate.version-in @('Unified-0.14.2','Unified-0.15.0','Unified-0.15.1','Unified-0.16.0')){if(!$gate.controllerButtonsVerified-or !$gate.controllerComfortVerified){throw 'Controller comfort checks missing'};$minimumChecks=950}
if($gate.version -in @('Unified-0.15.0','Unified-0.15.1','Unified-0.16.0')){if(!$gate.movementStancesVerified){throw 'Movement stance checks missing'};$minimumChecks=1030}
if($gate.version-notin @('Unified-0.13.0','Unified-0.13.1','Unified-0.14.0','Unified-0.14.1','Unified-0.14.2','Unified-0.15.0','Unified-0.15.1','Unified-0.16.0') -or $gate.checks-lt $minimumChecks -or !$gate.rendered){throw 'Unified build validation incomplete.'}
if($gate.version-in @('Unified-0.15.1','Unified-0.16.0')){if(!$gate.sportsParkVerified-or $gate.checks-lt 1081){throw 'Sports park validation incomplete'}}
if($gate.version-eq 'Unified-0.16.0'-and (!$gate.coreIntegrationVerified-or !$gate.coreConfigVerified-or $gate.checks-lt 1150)){throw 'Cumulative feature/configuration validation missing'}
Assert-Payload (Join-Path $PackageRoot 'Windows') @($gate.files)
$catalog=Join-Path $local 'ArchiveBuilds.json'
$returned=$false;$errorFlag=$false;$archives=@()
try{if(Test-Path -LiteralPath $catalog){$archives=@((Get-Content -LiteralPath $catalog -Raw|ConvertFrom-Json).entries)}}catch{$errorFlag=$true;$_|Out-String|Set-Content -LiteralPath (Join-Path $saved 'ArchiveError.txt')}
if($TestArchiveId-and !$ArchiveAcceptance){throw 'TestArchiveId requires unattended archive acceptance'}
if($ArchiveAcceptance-and $TestArchiveId){
    if(!$TestOutput){throw 'Archive fixture requires TestOutput'}
    $fixture=@($archives|Where-Object id -eq $TestArchiveId)
    if($fixture.Count-ne 1-or $fixture[0].engine-ne 'Unreal'){throw 'Exactly one retained Unreal fixture is required'}
    New-Item -ItemType Directory -Path $TestOutput -Force|Out-Null
    @{entries=@(@{id=$fixture[0].id;label=$fixture[0].label})}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $TestOutput 'ArchiveFixtureIndex.json')
}
if($VerifyArchives){
    if($errorFlag -or !$archives.Count){throw 'Archive catalog is missing or invalid.'}
    $results=@(foreach($archive in $archives){
        try{$null=Get-VerifiedArchiveExe $archive;@{id=$archive.id;label=$archive.label;verified=$true;files=$archive.files.Count}}
        catch{@{id=$archive.id;label=$archive.label;verified=$false;error=$_.Exception.Message}}
    })
    if($TestOutput){New-Item -ItemType Directory -Path $TestOutput -Force|Out-Null;@{scope='Payload integrity only; not a new gameplay test of every historical build';archives=$results}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath (Join-Path $TestOutput 'archive-integrity.json')}
    if(@($results|Where-Object {!$_.verified}).Count){throw 'Some archives failed integrity verification.'}
    Write-Output ('Verified retained archive payloads: '+$results.Count)
    return
}
do {
    if(Test-Path -LiteralPath $request){Remove-Item -LiteralPath $request}
    $argsList=@('-dx12','-windowed','-ResX=1280','-ResY=720','-CheweenieModeHub')
    foreach($pair in @(@('streets.json','CheweenieNeighbourhood'),@('NeighbourDogs.json','CheweenieDogs'))){$path=Join-Path $local $pair[0];if(Test-Path -LiteralPath $path){$argsList+=('-'+$pair[1]+'="'+$path+'"')}}
    $argsList+=@(('-CheweenieArchiveIndex="'+(Join-Path $local 'ArchiveIndex.json')+'"'),('-CheweenieArchiveRequest="'+$request+'"'),('-CheweenieArchiveToken='+$token))
    if($errorFlag){$argsList+='-CheweenieArchiveError';$errorFlag=$false}
    if($ArchiveAcceptance){
        if(!$TestOutput){throw 'Archive acceptance needs -TestOutput.'}
        New-Item -ItemType Directory -Path $TestOutput -Force|Out-Null
        $argsList=@($argsList|Where-Object {$_-notmatch '^-dx12$'})+@('-nullrhi','-nosound','-unattended','-CheweenieHubArchiveSmoke',('-UserDir="'+(Join-Path $TestOutput 'CurrentUser')+'"'),('-CheweenieTestOutput="'+$TestOutput+'"'),('-AbsLog="'+$TestOutput+'/latest-'+[int]$returned+'.log"'))
        if($returned){$argsList+='-CheweenieArchiveReturned'}
        if($TestArchiveId){$argsList=@($argsList|Where-Object {$_-notmatch '^-CheweenieArchiveIndex='})+('-CheweenieArchiveIndex="'+(Join-Path $TestOutput 'ArchiveFixtureIndex.json')+'"')}
    }
    $latestStarted=[DateTime]::UtcNow
    Run-Game (Join-Path $PackageRoot 'Windows/CheweenieUE5Lab.exe') $argsList
    if(!(Test-Path -LiteralPath $request)){
        if($ArchiveAcceptance){
            $report=Join-Path $TestOutput 'hub-archive-return.json'
            if(!$returned -or !(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc-lt $latestStarted){throw 'Current-game archive-return report missing or stale.'}
            $checks=Get-Content -LiteralPath $report -Raw|ConvertFrom-Json
            if($checks.failed.Count -or $checks.passed.Count-lt 2){throw 'Current game failed the archive return checks.'}
        }
        break
    }
    try {
        $choice=Get-Content -LiteralPath $request -Raw|ConvertFrom-Json
        if($choice.token-ne $token){throw 'Archive request token is invalid.'}
        $matches=@($archives|Where-Object {$_.id-eq $choice.id})
        if($matches.Count-ne 1){throw 'Archive choice is not in this launcher catalog.'}
        $archive=$matches[0];$base=[IO.Path]::GetFullPath($archive.root).TrimEnd('\','/')
        $exe=Get-VerifiedArchiveExe $archive
        $oldArgs=@('-windowed','-ResX=1280','-ResY=720')
        $oldLocal=Join-Path $(if($archive.engine-eq 'Unreal'){Split-Path -Parent $base}else{$base}) 'LocalGameData'
        foreach($pair in @(@('streets.json','CheweenieNeighbourhood'),@('NeighbourDogs.json','CheweenieDogs'))){$path=Join-Path $oldLocal $pair[0];if(Test-Path -LiteralPath $path){$oldArgs+=('-'+$pair[1]+'="'+$path+'"')}}
        if($ArchiveAcceptance){
            if($archive.engine-ne 'Unreal'){throw 'Use an Unreal fixture for unattended round-trip acceptance.'}
            # Isolate synthetic input from hardware polling. The retained one-frame
            # recenter probe needs 120 Hz to keep its pending look below one degree.
            # Ordinary main/archive play keeps the owner's frame and input settings.
            $oldArgs+=@('-nullrhi','-NoGamepad','-ExecCmds="t.MaxFPS 120"','-unattended','-nosound','-CheweenieSmoke',('-UserDir="'+(Join-Path $TestOutput 'ArchivedUser')+'"'),('-CheweenieTestOutput="'+$TestOutput+'"'),('-AbsLog="'+$TestOutput+'/archive.log"'))
        }
        $archiveStarted=[DateTime]::UtcNow
        Run-Game $exe $oldArgs
        if($ArchiveAcceptance){
            $report=Join-Path $TestOutput 'ArchivedUser/Saved/movement-smoke.json'
            if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc-lt $archiveStarted){throw 'Archived executable report missing or stale.'}
            $checks=Get-Content -LiteralPath $report -Raw|ConvertFrom-Json
            if($checks.failed.Count -or $checks.passed.Count-lt 16){throw 'Archived movement acceptance failed.'}
            Copy-Item -LiteralPath $report -Destination (Join-Path $TestOutput 'archive-movement.json')
        }
        $returned=$true
    } catch {
        if($ArchiveAcceptance){throw}
        $_|Out-String|Set-Content -LiteralPath (Join-Path $saved 'ArchiveError.txt')
        $errorFlag=$true
    }
} while($true)
