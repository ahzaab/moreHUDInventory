[CmdletBinding()]
param(
    [string]$FlashExe,
    [string]$NpmExe,
    [string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$sourceDirectory = Join-Path $repositoryRoot 'AS2'

# Keep generated compiler output under build/ and out of source control.
if (-not $OutputDirectory)
{
    $OutputDirectory = Join-Path $repositoryRoot 'build\scaleform'
}

# Resolve Flash/Animate through parameters, environment, or PATH only.
if (-not $FlashExe)
{
    $flashCandidates = @($env:FLASH_EXE)
    if ($env:FlashPath)
    {
        $flashCandidates += Join-Path $env:FlashPath 'Flash.exe'
    }

    foreach ($candidate in $flashCandidates)
    {
        if ($candidate -and (Test-Path -LiteralPath $candidate -PathType Leaf))
        {
            $FlashExe = $candidate
            break
        }
    }

    if (-not $FlashExe)
    {
        $flashCommand = Get-Command Flash.exe, Animate.exe -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($flashCommand)
        {
            $FlashExe = $flashCommand.Source
        }
    }
}

if (-not (Test-Path -LiteralPath $FlashExe -PathType Leaf))
{
    throw 'Adobe Flash/Animate was not found. Supply -FlashExe or set FLASH_EXE.'
}

if (-not $NpmExe)
{
    $npmCommand = Get-Command npm.cmd -ErrorAction SilentlyContinue
    if ($npmCommand)
    {
        $NpmExe = $npmCommand.Source
    }
}

if (-not (Test-Path -LiteralPath $NpmExe -PathType Leaf))
{
    throw 'npm.cmd was not found. Install Node.js or supply -NpmExe.'
}

New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
Push-Location $PSScriptRoot
try
{
    # flc is pinned to the same version used by the moreHUDSE/SkywindUI workflow.
    & $NpmExe install --ignore-scripts --no-package-lock
    if ($LASTEXITCODE -ne 0)
    {
        throw "npm install failed with exit code $LASTEXITCODE."
    }

    $flc = Join-Path $PSScriptRoot 'node_modules\.bin\flc.cmd'
    & $flc --interactive-compiler $FlashExe --input-directory $sourceDirectory --output-directory $OutputDirectory --include-pattern '*.fla' --debug false
    if ($LASTEXITCODE -ne 0)
    {
        throw "flc failed with exit code $LASTEXITCODE."
    }
}
finally
{
    Pop-Location
}

# The main movie is intentionally installed twice for vanilla and SkyUI compatibility.
$destinations = @{
    'AHZmoreHUDInventory.swf' = @('Interface\AHZmoreHUDInventory.swf')
    'baseIcons.swf' = @('Interface\exported\moreHUDIE\baseIcons.swf')
    'baseLargeItemCard.swf' = @('Interface\exported\moreHUDIE\baseLargeItemCard.swf')
}

$sourceDataDirectory = Join-Path $repositoryRoot 'Data'
foreach ($publishedName in $destinations.Keys)
{
    $publishedFile = Get-ChildItem -LiteralPath $OutputDirectory -Filter $publishedName -File -Recurse | Select-Object -First 1
    if (-not $publishedFile)
    {
        throw "flc did not publish $publishedName."
    }

    foreach ($relativeDestination in $destinations[$publishedName])
    {
        $destination = Join-Path $sourceDataDirectory $relativeDestination
        New-Item -ItemType Directory -Path (Split-Path -Parent $destination) -Force | Out-Null
        Copy-Item -LiteralPath $publishedFile.FullName -Destination $destination -Force
    }
}

Write-Host "Published Inventory Scaleform files to $sourceDataDirectory"
