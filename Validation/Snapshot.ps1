$ErrorActionPreference = 'Stop'
$files = @('.gitignore','AGENTS.md','README.md') + @(Get-ChildItem Assets,Packages,ProjectSettings,docs,Validation -Recurse -File | Where-Object { $_.FullName -notmatch '\\(bin|obj)\\' } | ForEach-Object { $_.FullName })
$items = foreach ($f in $files) {
    $i = Get-Item -LiteralPath $f
    @{path=[IO.Path]::GetRelativePath((Get-Location).Path,$i.FullName).Replace('\','/'); content=[IO.File]::ReadAllText($i.FullName)}
}
ConvertTo-Json -InputObject @($items) -Depth 3 -Compress
