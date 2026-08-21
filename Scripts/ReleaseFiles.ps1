[CmdletBinding()]
param(
    [string]$Version,
    [string]$CMakeExe,
    [string]$VsDevCmd,
    [string]$NinjaExe,
    [string]$ArchiveExe,
    [string]$SevenZipExe,
    [string]$OutputDirectory,
    [switch]$BuildScaleform,
    [string]$FlashExe,
    [string]$NpmExe,
    [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path

# Use the CMake version by default so package names and DLL metadata stay aligned.
if (-not $Version)
{
    $cmakeText = Get-Content -LiteralPath (Join-Path $repositoryRoot 'CMakeLists.txt') -Raw
    $versionMatch = [regex]::Match($cmakeText, '(?ms)project\s*\(\s*AHZmoreHUDInventory\s+VERSION\s+(\d+\.\d+\.\d+\.\d+)')
    if (-not $versionMatch.Success)
    {
        throw 'Could not read the four-part project version from CMakeLists.txt.'
    }

    $Version = $versionMatch.Groups[1].Value
}

if ($Version -notmatch '^\d+\.\d+\.\d+\.\d+$')
{
    throw "Invalid four-part version: $Version"
}

if ($BuildScaleform)
{
    $scaleformArguments = @{}
    if ($FlashExe)
    {
        $scaleformArguments.FlashExe = $FlashExe
    }
    if ($NpmExe)
    {
        $scaleformArguments.NpmExe = $NpmExe
    }

    & (Join-Path $PSScriptRoot 'BuildScaleform.ps1') @scaleformArguments
}

if (-not $SkipBuild)
{
    $buildArguments = @{ Configuration = 'Release' }
    foreach ($parameterName in @('CMakeExe', 'VsDevCmd', 'NinjaExe'))
    {
        $parameterValue = Get-Variable -Name $parameterName -ValueOnly
        if ($parameterValue)
        {
            $buildArguments[$parameterName] = $parameterValue
        }
    }

    & (Join-Path $repositoryRoot 'build.ps1') @buildArguments
    if ($LASTEXITCODE -ne 0)
    {
        throw "Release build failed with exit code $LASTEXITCODE."
    }
}

# Resolve 7-Zip from parameters, environment, or PATH without hard-coded locations.
if (-not $SevenZipExe)
{
    $SevenZipExe = $env:SEVENZIP_EXE
}

if (-not $SevenZipExe)
{
    $sevenZipCommand = Get-Command 7z.exe, 7zz.exe -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($sevenZipCommand)
    {
        $SevenZipExe = $sevenZipCommand.Source
    }
}

if (-not (Test-Path -LiteralPath $SevenZipExe -PathType Leaf))
{
    throw '7-Zip was not found. Supply -SevenZipExe, set SEVENZIP_EXE, or add 7z to PATH.'
}

$sourceDataDirectory = Join-Path $repositoryRoot 'Data'
$buildDirectory = Join-Path $repositoryRoot 'build\release-msvc'
$pluginPath = Join-Path $buildDirectory 'AHZmoreHUDInventory.dll'
$symbolsPath = Join-Path $buildDirectory 'AHZmoreHUDInventory.pdb'
$iniPath = Join-Path $sourceDataDirectory 'SKSE\Plugins\AHZmoreHUDInventory.ini'

$requiredFiles = @(
    $pluginPath
    $symbolsPath
    $iniPath
    (Join-Path $sourceDataDirectory 'AHZmoreHUDInventory.esl')
    (Join-Path $sourceDataDirectory 'Interface\AHZmoreHUDInventory.swf')
    (Join-Path $sourceDataDirectory 'Interface\exported\moreHUDIE\baseIcons.swf')
    (Join-Path $sourceDataDirectory 'Interface\exported\moreHUDIE\baseLargeItemCard.swf')
)

foreach ($requiredFile in $requiredFiles)
{
    if (-not (Test-Path -LiteralPath $requiredFile -PathType Leaf))
    {
        throw "Required release file is missing: $requiredFile"
    }
}

if (-not $OutputDirectory)
{
    $OutputDirectory = Join-Path $repositoryRoot 'release'
}

$releaseRoot = [IO.Path]::GetFullPath($OutputDirectory)
$versionDirectory = [IO.Path]::GetFullPath((Join-Path $releaseRoot $Version))
if (-not $versionDirectory.StartsWith($releaseRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase))
{
    throw 'The version output directory resolved outside the selected release directory.'
}

# Recreate only this version's generated output, never the repository or release root.
if (Test-Path -LiteralPath $versionDirectory)
{
    Remove-Item -LiteralPath $versionDirectory -Recurse -Force
}

$stagingDirectory = Join-Path $versionDirectory '.staging'
$stagingDataDirectory = Join-Path $stagingDirectory 'Data'
$standardRoot = Join-Path $versionDirectory 'standard'
$looseRoot = Join-Path $versionDirectory 'loose'
New-Item -ItemType Directory -Path $stagingDataDirectory, $standardRoot, $looseRoot -Force | Out-Null

try
{
    Copy-Item -Path (Join-Path $sourceDataDirectory '*') -Destination $stagingDataDirectory -Recurse -Force

    # Install the main movie in both locations for vanilla UI and SkyUI compatibility.
    $exportedInterface = Join-Path $stagingDataDirectory 'Interface\exported'
    New-Item -ItemType Directory -Path $exportedInterface -Force | Out-Null
    Copy-Item -LiteralPath (Join-Path $stagingDataDirectory 'Interface\AHZmoreHUDInventory.swf') -Destination (Join-Path $exportedInterface 'AHZmoreHUDInventory.swf') -Force

    $stagingPluginDirectory = Join-Path $stagingDataDirectory 'SKSE\Plugins'
    New-Item -ItemType Directory -Path $stagingPluginDirectory -Force | Out-Null
    Copy-Item -LiteralPath $pluginPath, $symbolsPath -Destination $stagingPluginDirectory -Force

    $bsaName = 'AHZmoreHUDInventory.bsa'
    $archiveArguments = @{
        StagingDirectory = $stagingDirectory
        BsaName = $bsaName
    }
    if ($ArchiveExe)
    {
        $archiveArguments.ArchiveExe = $ArchiveExe
    }

    & (Join-Path $PSScriptRoot 'ArchiveFiles.ps1') @archiveArguments | ForEach-Object { Write-Verbose $_ }
    $bsaPath = Join-Path $stagingDirectory $bsaName

    # The primary Nexus package contains the ESL, BSA, release DLL, PDB, and default INI.
    $standardData = Join-Path $standardRoot 'Data'
    $standardPluginDirectory = Join-Path $standardData 'SKSE\Plugins'
    New-Item -ItemType Directory -Path $standardPluginDirectory -Force | Out-Null
    Copy-Item -LiteralPath $bsaPath -Destination $standardData -Force
    Copy-Item -LiteralPath (Join-Path $stagingDataDirectory 'AHZmoreHUDInventory.esl') -Destination $standardData -Force
    Copy-Item -Path (Join-Path $stagingPluginDirectory '*') -Destination $standardPluginDirectory -Force

    # Preserve the historical optional loose-assets package. It intentionally omits
    # the ESL and is installed over the primary package for troubleshooting/overrides.
    $looseData = Join-Path $looseRoot 'Data'
    Copy-Item -LiteralPath $stagingDataDirectory -Destination $looseRoot -Recurse -Force
    Remove-Item -LiteralPath (Join-Path $looseData 'AHZmoreHUDInventory.esl') -Force

    # Match the human-facing Nexus upload names. Nexus adds its mod and upload IDs
    # to downloaded filenames, so those generated numeric suffixes do not belong here.
    $fileVersion = $Version.Replace('.', '-')
    $standardArchive = Join-Path $versionDirectory "moreHUD Inventory Edition - AE-$fileVersion.7z"
    $looseArchive = Join-Path $versionDirectory "moreHUD Inventory Edition Loose Version - AE-$fileVersion.7z"

    & $SevenZipExe a $standardArchive (Join-Path $standardRoot 'Data') -mx5 -t7z
    if ($LASTEXITCODE -ne 0)
    {
        throw "7-Zip failed to create the primary package with exit code $LASTEXITCODE."
    }

    & $SevenZipExe a $looseArchive (Join-Path $looseRoot 'Data') -mx5 -t7z
    if ($LASTEXITCODE -ne 0)
    {
        throw "7-Zip failed to create the loose package with exit code $LASTEXITCODE."
    }

    Write-Host "Created $standardArchive"
    Write-Host "Created $looseArchive"
}
finally
{
    if (Test-Path -LiteralPath $stagingDirectory)
    {
        Remove-Item -LiteralPath $stagingDirectory -Recurse -Force
    }
    if (Test-Path -LiteralPath $standardRoot)
    {
        Remove-Item -LiteralPath $standardRoot -Recurse -Force
    }
    if (Test-Path -LiteralPath $looseRoot)
    {
        Remove-Item -LiteralPath $looseRoot -Recurse -Force
    }
}
