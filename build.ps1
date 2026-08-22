[CmdletBinding()]
param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [string]$CMakeExe,
    [string]$VsDevCmd,
    [string]$NinjaExe,
    [string]$DeployTarget
)

$ErrorActionPreference = 'Stop'

# Map the requested configuration to the editor-independent CMake presets.
if ($Configuration -eq 'Debug')
{
    $configurePreset = 'build-debug-msvc'
    $buildPreset = 'debug-msvc'
}
else
{
    $configurePreset = 'build-release-msvc'
    $buildPreset = 'release-msvc'
}

# A normal VS Code terminal does not automatically inherit the MSVC include and library paths.
# Import vcvars64 into this PowerShell process only when cl.exe is not already configured.
$compilerCommand = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $compilerCommand -or -not $env:INCLUDE)
{
    if (-not $VsDevCmd)
    {
        $VsDevCmd = $env:VCVARS64
        if (-not $VsDevCmd)
        {
            $vcvarsCommand = Get-Command vcvars64.bat -ErrorAction SilentlyContinue
            if ($vcvarsCommand)
            {
                $VsDevCmd = $vcvarsCommand.Source
            }
        }
    }

    if (-not (Test-Path -LiteralPath $VsDevCmd -PathType Leaf))
    {
        throw 'Visual Studio x64 environment script was not found. Supply -VsDevCmd or set VCVARS64.'
    }

    # Visual Studio may replace VCPKG_ROOT with its bundled copy, so preserve the caller's selection.
    $callerVcpkgRoot = $env:VCPKG_ROOT
    $developerEnvironment = & $env:ComSpec /d /c "call `"$VsDevCmd`" -arch=x64 -host_arch=x64 >nul && set"
    if ($LASTEXITCODE -ne 0)
    {
        throw "Visual Studio developer environment initialization failed with exit code $LASTEXITCODE."
    }

    foreach ($line in $developerEnvironment)
    {
        $separator = $line.IndexOf('=')
        if ($separator -gt 0)
        {
            $name = $line.Substring(0, $separator)
            $value = $line.Substring($separator + 1)
            [Environment]::SetEnvironmentVariable($name, $value, 'Process')
        }
    }

    if ($callerVcpkgRoot)
    {
        $env:VCPKG_ROOT = $callerVcpkgRoot
    }
}

# Prefer Visual Studio's bundled CMake over PATH so unrelated toolchains cannot take priority.
if (-not $CMakeExe)
{
    $CMakeExe = $env:CMAKE_EXE
}

if (-not $CMakeExe -and $env:VSINSTALLDIR)
{
    $visualStudioCMake = Join-Path $env:VSINSTALLDIR 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
    if (Test-Path -LiteralPath $visualStudioCMake -PathType Leaf)
    {
        $CMakeExe = $visualStudioCMake
    }
}

if (-not $CMakeExe)
{
    $cmakeCommand = Get-Command cmake.exe -ErrorAction SilentlyContinue
    if ($cmakeCommand)
    {
        $CMakeExe = $cmakeCommand.Source
    }
}

if (-not (Test-Path -LiteralPath $CMakeExe -PathType Leaf))
{
    throw 'cmake.exe was not found. Supply -CMakeExe, set CMAKE_EXE, or add CMake to PATH.'
}

# Resolve Ninja explicitly so another development toolchain cannot win through PATH priority.
if (-not $NinjaExe)
{
    $NinjaExe = $env:NINJA_EXE
}

if (-not $NinjaExe -and $env:VSINSTALLDIR)
{
    $visualStudioNinja = Join-Path $env:VSINSTALLDIR 'Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe'
    if (Test-Path -LiteralPath $visualStudioNinja -PathType Leaf)
    {
        $NinjaExe = $visualStudioNinja
    }
}

if (-not $NinjaExe)
{
    $ninjaCommand = Get-Command ninja.exe -ErrorAction SilentlyContinue
    if ($ninjaCommand)
    {
        $NinjaExe = $ninjaCommand.Source
    }
}

if (-not (Test-Path -LiteralPath $NinjaExe -PathType Leaf))
{
    throw 'ninja.exe was not found. Supply -NinjaExe, set NINJA_EXE, or add Ninja to PATH.'
}

# A command-line deployment target affects this process only and does not leak into other profiles.
$previousDeployTargets = $env:SkyrimPluginTargets
if ($DeployTarget)
{
    $env:SkyrimPluginTargets = $DeployTarget
}
else
{
    Remove-Item Env:SkyrimPluginTargets -ErrorAction SilentlyContinue
}

Push-Location $PSScriptRoot
try
{
    & $CMakeExe --preset $configurePreset -S $PSScriptRoot "-DCMAKE_MAKE_PROGRAM=$NinjaExe"
    if ($LASTEXITCODE -ne 0)
    {
        exit $LASTEXITCODE
    }

    & $CMakeExe --build --preset $buildPreset --verbose
    exit $LASTEXITCODE
}
finally
{
    Pop-Location
    $env:SkyrimPluginTargets = $previousDeployTargets
}
