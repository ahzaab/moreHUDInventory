[CmdletBinding()]
param(
    [string]$CMakeExe,
    [string]$VsDevCmd,
    [string]$NinjaExe,
    [string]$DeployTarget
)

# Retain the historical entry point while sharing the safe, process-scoped
# MSVC environment and preset selection implemented by build-debug.ps1.
$buildArguments = @{}

foreach ($parameterName in @('CMakeExe', 'VsDevCmd', 'NinjaExe', 'DeployTarget'))
{
    $parameterValue = Get-Variable -Name $parameterName -ValueOnly
    if ($parameterValue)
    {
        $buildArguments[$parameterName] = $parameterValue
    }
}

& (Join-Path $PSScriptRoot 'build-debug.ps1') @buildArguments
exit $LASTEXITCODE
