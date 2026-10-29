param([string]$UnityEditorPath = 'C:\Program Files\Unity\Hub\Editor\6000.3.10f1\Editor')
$ErrorActionPreference = 'Stop'
$manifest = Get-Content Packages/manifest.json -Raw | ConvertFrom-Json
$count = 0
foreach ($dependency in $manifest.dependencies.PSObject.Properties) {
    $package = Join-Path $UnityEditorPath ('Data/Resources/PackageManager/BuiltInPackages/' + $dependency.Name + '/package.json')
    if (!(Test-Path -LiteralPath $package)) { throw "Dependency unavailable in this editor: $($dependency.Name)" }
    $installed = Get-Content -LiteralPath $package -Raw | ConvertFrom-Json
    if ($installed.version -ne $dependency.Value) { throw "Version mismatch for $($dependency.Name): $($installed.version)" }
    $count++
}
$guids = @{}
foreach ($asset in Get-ChildItem Assets -Recurse | Where-Object { $_.Extension -ne '.meta' -and $_.FullName -notmatch '\\Generated(\\|$)' }) {
    $meta = $asset.FullName + '.meta'
    if (!(Test-Path -LiteralPath $meta)) { throw "Missing meta: $($asset.FullName)" }
    $line = Get-Content -LiteralPath $meta | Where-Object { $_ -match '^guid: ' }
    if (!$line -or $guids.ContainsKey($line)) { throw "Missing or duplicate GUID: $meta" }
    $guids[$line] = $true
    if ($asset.Extension -eq '.asmdef') { $null = Get-Content -LiteralPath $asset.FullName -Raw | ConvertFrom-Json }
}
"PASS: $count manifest versions match bundled packages; $($guids.Count) assets/folders have unique metadata; assembly definition JSON parses. This audit does not run package resolution or Unity import."
