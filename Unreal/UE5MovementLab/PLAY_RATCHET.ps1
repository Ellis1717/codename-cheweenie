param([ValidateSet('Solo','Host','Join')][string]$Mode='Solo',[string]$Address,[ValidateRange(1024,65535)][int]$Port=7777)
$ErrorActionPreference='Stop'
$package=if(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Windows')){$PSScriptRoot}else{[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../builds/UnrealRatchet-0.10.2'))}
$marker=Join-Path $package 'validated-ratchet.json'
if(!(Test-Path -LiteralPath $marker)){throw 'Validated Ratchet package missing; see UNREAL_HALLOWEEN.md.'}
$gate=Get-Content -LiteralPath $marker -Raw|ConvertFrom-Json
if($gate.version -ne 'Ratchet-0.10.2' -or $gate.checks -lt 220 -or !$gate.rendered -or @($gate.files).Count -lt 1){throw 'Ratchet package validation incomplete'}
foreach($file in $gate.files){$target=Join-Path $package ('Windows/'+$file.path);if(!(Test-Path -LiteralPath $target) -or (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $file.sha256){throw 'Ratchet package changed after validation; rebuild and rerun the packaged checks.'}}
$argsList=@('-dx12','-windowed','-ResX=1280','-ResY=720')
if($Mode -eq 'Solo'){
    $local=Join-Path $package 'LocalGameData'
    if(Test-Path -LiteralPath (Join-Path $local 'streets.json')){$argsList+=('-CheweenieNeighbourhood="'+(Join-Path $local 'streets.json')+'"')}
    if(Test-Path -LiteralPath (Join-Path $local 'NeighbourDogs.json')){$argsList+=('-CheweenieDogs="'+(Join-Path $local 'NeighbourDogs.json')+'"')}
}else{
    if($Mode -eq 'Join'){
        if(!$Address){$Address=Read-Host 'Host PC IPv4 address'}
        $parsed=$null
        if(![Net.IPAddress]::TryParse($Address,[ref]$parsed) -or $parsed.AddressFamily -ne [Net.Sockets.AddressFamily]::InterNetwork){throw 'Enter the host IPv4 address; supply the port separately.'}
        $url=$parsed.ToString()+':'+$Port
    }else{$url='/Game/VisualLab/Comparison?game=/Script/CheweenieUE5Lab.CozyCoopMode?listen'}
    $argsList=@($url,('-port='+$Port))+$argsList
}
Start-Process -FilePath (Join-Path $package 'Windows/CheweenieUE5Lab.exe') -ArgumentList $argsList
