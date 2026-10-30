param([string]$MapFile,[string]$DogRoster)
$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\LocalGameData'))
New-Item -ItemType Directory -Path $taskRoot -Force | Out-Null
if($MapFile){
    $map=Get-Content -LiteralPath $MapFile -Raw | ConvertFrom-Json
    if($map.version -ne 1 -or !$map.roads -or !$map.stops -or !$map.school){throw 'Expected the retained local streets v1 map including school'}
    Copy-Item -LiteralPath $MapFile -Destination (Join-Path $taskRoot 'streets.json')
}
$rosterPath=Join-Path $taskRoot 'NeighbourDogs.json'
if($DogRoster){
    $dogs=Get-Content -LiteralPath $DogRoster -Raw | ConvertFrom-Json
    if($dogs.version -ne 1 -or $dogs.dogs.Count -gt 8){throw 'Expected a v1 roster with up to eight slots'}
    Copy-Item -LiteralPath $DogRoster -Destination $rosterPath
}elseif(!(Test-Path -LiteralPath $rosterPath)){Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'Config\NeighbourDogs.example.json') -Destination $rosterPath}
'Local map/roster stay outside Content and Git. UNREAL_PLAY.cmd passes them to the 0.5 package.'
$taskRoot
