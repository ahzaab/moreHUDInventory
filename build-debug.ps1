[CmdletBinding()]
param(
    [string]$CMakeExe,
    [string]$VsDevCmd,
    [string]$NinjaExe,
    [string]$DeployTarget
)

# Keep the debug entry point convenient for VS Code while sharing all environment setup with build.ps1.
$buildArguments = @{
    Configuration = 'Debug'
}

if ($CMakeExe)
{
    $buildArguments.CMakeExe = $CMakeExe
}

if ($VsDevCmd)
{
    $buildArguments.VsDevCmd = $VsDevCmd
}

if ($NinjaExe)
{
    $buildArguments.NinjaExe = $NinjaExe
}

if ($DeployTarget)
{
    $buildArguments.DeployTarget = $DeployTarget
}


& (Join-Path $PSScriptRoot 'build.ps1') @buildArguments
exit $LASTEXITCODE
