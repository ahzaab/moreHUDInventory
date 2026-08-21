[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$StagingDirectory,
    [string]$ArchiveExe,
    [string]$BsaName = 'AHZmoreHUDInventory.bsa'
)

$ErrorActionPreference = 'Stop'

# Resolve the Creation Kit archive tool without storing a workstation path.
if (-not $ArchiveExe)
{
    $ArchiveExe = $env:ARCHIVE_EXE
}

if (-not $ArchiveExe)
{
    $gameRoot = $env:SKYRIM_AE_ROOT
    if (-not $gameRoot)
    {
        $gameRoot = $env:Skyrim64AEPath
    }

    if ($gameRoot)
    {
        $candidate = Join-Path $gameRoot 'Tools\Archive\Archive.exe'
        if (Test-Path -LiteralPath $candidate -PathType Leaf)
        {
            $ArchiveExe = $candidate
        }
    }
}

if (-not $ArchiveExe)
{
    $archiveCommand = Get-Command Archive.exe -ErrorAction SilentlyContinue
    if ($archiveCommand)
    {
        $ArchiveExe = $archiveCommand.Source
    }
}

if (-not (Test-Path -LiteralPath $ArchiveExe -PathType Leaf))
{
    throw 'Archive.exe was not found. Supply -ArchiveExe, set ARCHIVE_EXE, or set SKYRIM_AE_ROOT.'
}

$dataDirectory = Join-Path $StagingDirectory 'Data'
if (-not (Test-Path -LiteralPath $dataDirectory -PathType Container))
{
    throw "The staging Data directory does not exist: $dataDirectory"
}

# Archive only game assets. The plugin, symbols, configuration, and ESL remain loose.
$excludedExtensions = @('.dll', '.pdb', '.ini', '.esl', '.esp')
$archiveFiles = Get-ChildItem -LiteralPath $dataDirectory -Recurse -File |
    Where-Object { $excludedExtensions -notcontains $_.Extension.ToLowerInvariant() } |
    ForEach-Object { $_.FullName.Substring($dataDirectory.Length).TrimStart('\') } |
    Sort-Object

if (-not $archiveFiles)
{
    throw 'No files were found for the BSA.'
}

# Bethesda's tool expects Windows paths and CRLF-delimited command files.
$fileListPath = Join-Path $StagingDirectory 'bsafilelist.txt'
$scriptPath = Join-Path $StagingDirectory 'bsascript.txt'
$ascii = [Text.ASCIIEncoding]::new()
[IO.File]::WriteAllText($fileListPath, ($archiveFiles -join "`r`n") + "`r`n", $ascii)

$archiveScript = @(
    'Log: Archive.log'
    'New Archive'
    'Check: Menus'
    'Check: Misc'
    'Check: Retain Directory Names'
    'Check: Retain File Names'
    'Set File Group Root: Data\'
    'Add File Group: bsafilelist.txt'
    "Save Archive: $BsaName"
)
[IO.File]::WriteAllText($scriptPath, ($archiveScript -join "`r`n") + "`r`n", $ascii)

Push-Location $StagingDirectory
try
{
    $archiveProcess = Start-Process -FilePath $ArchiveExe -ArgumentList 'bsascript.txt' -WorkingDirectory $StagingDirectory -Wait -PassThru -NoNewWindow
    if ($archiveProcess.ExitCode -ne 0)
    {
        throw "Archive.exe failed with exit code $($archiveProcess.ExitCode)."
    }
}
finally
{
    Pop-Location
}

$bsaPath = Join-Path $StagingDirectory $BsaName
if (-not (Test-Path -LiteralPath $bsaPath -PathType Leaf))
{
    throw "Archive.exe did not produce $bsaPath."
}

Write-Output $bsaPath
