param([ValidateSet('Host','Join')][string]$Mode='Host',[string]$Address,[ValidateRange(1024,65535)][int]$Port=7777)
$ErrorActionPreference='Stop'
$package=if(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Windows')){$PSScriptRoot}else{[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\builds\UnrealCoop-0.1.0'))}
$game=Join-Path $package 'Windows\CheweenieUE5Lab.exe'
$marker=Join-Path $package 'validated-coop.json'
if(!(Test-Path -LiteralPath $game) -or !(Test-Path -LiteralPath $marker)){throw 'Validated LAN build missing. Keep using UNREAL_PLAY.cmd for the existing solo game; see docs/UNREAL_COOP.md to build/test LAN.'}
$validation=Get-Content -LiteralPath $marker -Raw|ConvertFrom-Json
if($validation.version -ne 'LAN-0.1.0' -or $validation.checks -lt 124 -or !$validation.rendered -or @($validation.files).Count -lt 1){throw 'LAN validation marker is incomplete. Rerun FINALIZE_COOP.ps1 after the packaged checks pass.'}
foreach($file in $validation.files){$target=Join-Path $package ('Windows/'+$file.path);if(!(Test-Path -LiteralPath $target) -or (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $file.sha256){throw 'LAN payload changed after verification. Rebuild and rerun TEST_COOP.ps1.'}}
if($Mode -eq 'Join'){
    if(!$Address){$Address=Read-Host 'Host PC IPv4 address (shown by HOST_COOP)'}
    $parsed=$null
    if(![Net.IPAddress]::TryParse($Address,[ref]$parsed) -or $parsed.AddressFamily -ne [Net.Sockets.AddressFamily]::InterNetwork){throw 'Enter the host IPv4 address, for example 192.168.1.20. The port is supplied separately.'}
    $url=$parsed.ToString()+':'+$Port
}else{
    $url='/Game/VisualLab/Comparison?game=/Script/CheweenieUE5Lab.CozyCoopMode?listen'
    Write-Host 'Host LAN addresses (choose the Wi-Fi/Ethernet network shared with the other PC):'
    Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue|Where-Object {$_.IPAddress -ne '127.0.0.1' -and $_.IPAddress -notlike '169.254.*'}|Select-Object InterfaceAlias,IPAddress|Format-Table
    Write-Host ('Friends join your address on UDP '+$Port+'. Each PC needs this same build. The first guest starts as Lakitu; 3 selects Rio.')
}
Start-Process -FilePath $game -ArgumentList @($url,('-port='+$Port),'-dx12','-windowed','-ResX=1280','-ResY=720')
if($Mode -eq 'Host'){Read-Host 'Leave the game running. Press Enter to close this address window'|Out-Null}
