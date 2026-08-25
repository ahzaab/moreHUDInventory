[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$Source,

    [Parameter(Mandatory)]
    [string]$GameRoot,

    [Parameter(Mandatory)]
    [string]$OutputDirectory,

    [string[]]$AdditionalImports = @(),

    [switch]$Optimize
)

$ErrorActionPreference = 'Stop'

$sourceFile = Get-Item -LiteralPath $Source
$compiler = Join-Path $GameRoot 'Papyrus Compiler\PapyrusCompiler.exe'
$gameSources = Join-Path $GameRoot 'Data\Source\Scripts'
$flags = Join-Path $gameSources 'TESV_Papyrus_Flags.flg'

foreach ($requiredPath in @($compiler, $gameSources, $flags)) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Required Papyrus path does not exist: $requiredPath"
    }
}

New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$imports = @($sourceFile.DirectoryName) + $AdditionalImports + @($gameSources)
$arguments = @(
    $sourceFile.FullName
    "-import=$($imports -join ';')"
    "-output=$OutputDirectory"
    "-flags=$flags"
)
if ($Optimize) {
    $arguments += '-optimize'
}

& $compiler @arguments
if ($LASTEXITCODE -ne 0) {
    throw "Papyrus compiler failed with exit code $LASTEXITCODE"
}

$expectedPex = Join-Path $OutputDirectory ($sourceFile.BaseName + '.pex')
if (-not (Test-Path -LiteralPath $expectedPex)) {
    throw "Compiler reported success but did not produce $expectedPex"
}

Get-Item -LiteralPath $expectedPex
