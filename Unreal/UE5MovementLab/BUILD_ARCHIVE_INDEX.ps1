param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string[]]$ArchiveRoots)
$ErrorActionPreference='Stop'
# Local inventory only: private absolute paths never enter Content or Git.
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot)
$data=Join-Path $PackageRoot 'LocalGameData'
New-Item -ItemType Directory -Path $data -Force|Out-Null
$entries=@();$seen=@{}
foreach($root in $ArchiveRoots){
    if(!(Test-Path -LiteralPath $root)){continue}
    foreach($dir in Get-ChildItem -LiteralPath $root -Directory|Sort-Object Name -Descending){
        if($dir.FullName -eq $PackageRoot){continue}
        $native=Join-Path $dir.FullName 'Windows/CheweenieUE5Lab.exe'
        $unity=Join-Path $dir.FullName 'Cheweenie.exe'
        if(!(Test-Path -LiteralPath $unity)){$unity=Join-Path $dir.FullName 'CheweenieLab.exe'}
        $engine=if(Test-Path -LiteralPath $native){'Unreal'}elseif(Test-Path -LiteralPath $unity){'Unity'}else{continue}
        $exe=if($engine-eq 'Unreal'){$native}else{$unity}
        $base=if($engine-eq 'Unreal'){Join-Path $dir.FullName 'Windows'}else{$dir.FullName}
        $marker=Get-ChildItem -LiteralPath $dir.FullName -Filter 'validated*.json' -File|Sort-Object LastWriteTimeUtc -Descending|Select-Object -First 1
        $files=@();$version=$dir.Name;$validated=$false
        if($marker){
            $gate=Get-Content -LiteralPath $marker.FullName -Raw|ConvertFrom-Json
            if($gate.files.Count){$files=@($gate.files);$validated=$true;if($gate.version){$version=$gate.version}}
        }
        if(!$files.Count){
            $files=@(Get-ChildItem -LiteralPath $base -File -Recurse|Where-Object {$_.FullName -notmatch '[\\/]Saved[\\/]|[\\/]Logs[\\/]|[\\/]LocalGameData[\\/]' -and $_.Extension -notin @('.log','.pdb')}|ForEach-Object {
                @{path=$_.FullName.Substring($base.Length+1).Replace('\','/');sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash}
            })
        }
        # Deduplicate exact distributions, not just their identical bootstrap exes.
        $identity=($files|Sort-Object path|ForEach-Object {$_.path+':'+$_.sha256}) -join '|'
        $sha=[Security.Cryptography.SHA256]::Create()
        try{$id=([BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($identity)))).Replace('-','').ToLowerInvariant()}finally{$sha.Dispose()}
        if($seen.ContainsKey($id)){continue};$seen[$id]=$true
        $label=$engine+' / '+$dir.Name+$(if($validated){' (retained release)'}else{' (legacy snapshot)'})
        $entries+=@{id=$id;label=$label;root=$base;exe=$exe;engine=$engine;files=$files}
    }
}
@{entries=$entries}|ConvertTo-Json -Depth 8|Set-Content -LiteralPath (Join-Path $data 'ArchiveBuilds.json')
@{entries=@($entries|ForEach-Object {@{id=$_.id;label=$_.label}})}|ConvertTo-Json -Depth 5|Set-Content -LiteralPath (Join-Path $data 'ArchiveIndex.json')
'Retained local archive choices: '+$entries.Count
