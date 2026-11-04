param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$ZipPath)
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot)
$ZipPath=[IO.Path]::GetFullPath($ZipPath)
$partial=$ZipPath+'.partial'
if((Test-Path -LiteralPath $ZipPath) -or (Test-Path -LiteralPath $partial)){throw 'Archive destination already exists; choose a new name.'}
$gate=Get-Content -LiteralPath (Join-Path $PackageRoot 'validated-unified.json') -Raw|ConvertFrom-Json
$minimum=if($gate.version -in @('Unified-0.14.0','Unified-0.14.1')){824}elseif($gate.overlaysVerified){689}else{607}
if($gate.version -eq 'Unified-0.14.1'){if(!$gate.controllerButtonsVerified){throw 'Controller-button checks missing'};$minimum=874}
if($gate.version -notin @('Unified-0.13.1','Unified-0.14.0','Unified-0.14.1') -or !$gate.rendered -or $gate.checks -lt $minimum -or !$gate.files.Count){throw 'A validated cumulative distribution is required.'}
$windows=[IO.Path]::GetFullPath((Join-Path $PackageRoot 'Windows')).TrimEnd('\','/')
$entries=@()
foreach($file in $gate.files){
    if([IO.Path]::IsPathRooted($file.path) -or $file.path -match '(^|[\\/])(Saved|LocalGameData|Logs)([\\/]|$)|\.log$'){throw 'Runtime/private payload path rejected.'}
    $source=[IO.Path]::GetFullPath((Join-Path $windows $file.path))
    if(!$source.StartsWith($windows+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'Payload path escapes the distribution.'}
    if(!(Test-Path -LiteralPath $source -PathType Leaf) -or (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ne $file.sha256){throw ('Payload changed: '+$file.path)}
    $entries+=@{source=$source;name=('Windows/'+$file.path.Replace('\','/'));sha256=$file.sha256}
}
foreach($name in @('validated-unified.json','PLAY_GAME.ps1','PLAY_GAME.cmd','README.txt')){
    $source=Join-Path $PackageRoot $name
    if(!(Test-Path -LiteralPath $source -PathType Leaf)){throw ('Portable launcher file missing: '+$name)}
    $entries+=@{source=$source;name=$name;sha256=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash}
}
$stream=[IO.File]::Open($partial,[IO.FileMode]::CreateNew)
try{
    $zip=[IO.Compression.ZipArchive]::new($stream,[IO.Compression.ZipArchiveMode]::Create,$true)
    try{foreach($entry in $entries){[void][IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip,$entry.source,$entry.name,[IO.Compression.CompressionLevel]::Optimal)}}finally{$zip.Dispose()}
}finally{$stream.Dispose()}
$zip=[IO.Compression.ZipFile]::OpenRead($partial)
try{
    if($zip.Entries.Count -ne $entries.Count){throw 'Archive entry count differs from the validated file list.'}
    foreach($entry in $entries){
        $item=$zip.GetEntry($entry.name)
        if(!$item){throw ('Archive entry missing: '+$entry.name)}
        $inputStream=$item.Open();$hasher=[Security.Cryptography.SHA256]::Create()
        try{$hash=([BitConverter]::ToString($hasher.ComputeHash($inputStream))).Replace('-','');if($hash -ne $entry.sha256){throw ('Archive round-trip hash mismatch: '+$entry.name)}}finally{$hasher.Dispose();$inputStream.Dispose()}
    }
}finally{$zip.Dispose()}
Move-Item -LiteralPath $partial -Destination $ZipPath
@{version=$gate.version;payloadFiles=$gate.files.Count;totalEntries=$entries.Count;roundTripHashesVerified=$true;privateDataExcluded=$true;sha256=(Get-FileHash -LiteralPath $ZipPath -Algorithm SHA256).Hash;bytes=(Get-Item -LiteralPath $ZipPath).Length}|ConvertTo-Json|Set-Content -LiteralPath ($ZipPath+'.validation.json')
Get-Content -LiteralPath ($ZipPath+'.validation.json')
