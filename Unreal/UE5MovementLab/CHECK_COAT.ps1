param([Parameter(Mandatory=$true)][string]$OutputRoot,[int]$Width=1920,[int]$Height=1080)
$ErrorActionPreference='Stop';Add-Type -AssemblyName System.Drawing
$probe=Get-Content (Join-Path $OutputRoot 'CoatProbe.json') -Raw|ConvertFrom-Json
$bmp=[Drawing.Bitmap]::new((Join-Path $OutputRoot 'MartyPortrait.png'))
try{
    if($bmp.Width -ne $Width -or $bmp.Height -ne $Height){throw 'Portrait did not render at the requested resolution'}
    $x=[int][math]::Round($probe.x);$y=[int][math]::Round($probe.y)
    if($x -lt 3 -or $y -lt 3 -or $x -ge $bmp.Width-3 -or $y -ge $bmp.Height-3){throw 'Coat probe is outside the portrait'}
    $r=0.;$g=0.;$b=0.
    for($dy=-2;$dy -le 2;$dy++){for($dx=-2;$dx -le 2;$dx++){$c=$bmp.GetPixel($x+$dx,$y+$dy);$r+=$c.R/25.;$g+=$c.G/25.;$b+=$c.B/25.}}
    if($r -lt 35 -or $r -lt $g*1.08 -or $g -lt $b*1.05){throw ('Marty coat is black or lacks its tan palette: '+$r+','+$g+','+$b)}
    [ordered]@{width=$bmp.Width;height=$bmp.Height;probeX=$x;probeY=$y;meanRGB=@($r,$g,$b);tanCoatVisible=$true;scope='Actual rendered pixels at projected torso; likeness still requires visual review'}|ConvertTo-Json|Set-Content (Join-Path $OutputRoot 'coat-render.json')
}finally{$bmp.Dispose()}
