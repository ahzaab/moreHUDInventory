Param(
    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    $Version
)

try{


$pluginExtesion = '.esl' 

$executingPath = split-path $SCRIPT:MyInvocation.MyCommand.Path -parent

$sourceDir = "$($Env:Skyrim64Path)"
$archiveToolDir = "$sourceDir\Tools\Archive"
$sourceDataDir = "$sourceDir\Data"
$releaseDir = "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUDInventory\Release"
$versionDir = "$releaseDir\$Version"
$tempDir = "$versionDir\.tmp"
$destDataDir = "$versionDir\Data"
$destSksePlugin = "$destDataDir\SKSE\Plugins\AHZmoreHUDInventory.dll"
$sourceSksePlugin = "$sourceDataDir\SKSE\Plugins\AHZmoreHUDInventory.dll"
$destSksePluginIni = "$destDataDir\SKSE\Plugins\AHZmoreHUDInventory.ini"
$sourceSksePluginIni = "$sourceDataDir\SKSE\Plugins\AHZmoreHUDInventory.ini"

$requiredDataDirs = @("Interface","Source\Scripts", "SKSE\Plugins", "Interface\exported\moreHUDIE", "Scripts")

$requiredDataDirs | ForEach-Object{
    if (!$(Test-Path "$destDataDir\$_"))
    {
        New-Item -ItemType Directory "$destDataDir\$_"
    }
}

if ($destSksePlugin -and $sourceSksePlugin){
    Copy-Item $sourceSksePlugin $destSksePlugin
}

if ($destSksePluginIni -and $sourceSksePluginIni){
    Copy-Item $sourceSksePluginIni $destSksePluginIni
}

$items = @(Get-ChildItem "$sourceDataDir\Source\Scripts" -Filter AhzMoreHudIE.psc)
$items += @(Get-ChildItem "$sourceDataDir\Scripts" -Include @('AhzMoreHudIE.pex') -Recurse)
$items += @(Get-ChildItem "$sourceDataDir\Interface\exported\moreHUDIE")
$items += @(Get-ChildItem "$sourceDataDir" -Filter "AHZmoreHUDInventory$pluginExtesion")
$items += @(Get-ChildItem "$sourceDataDir\Interface" -Include @('AHZmoreHUDInventory.swf') -Recurse)

$filesToCopy = $items | Select-ObjecT -ExpandProperty FullName

$filesToCopy | ForEach-Object {
    $dest = $_.Replace($sourceDataDir,$destDataDir); 
    Copy-Item $_ $dest
}

# Get te list of files to include in the bsa (Do not include the esp)
$bsaFileList = $filesToCopy | Where-object {$(Get-Item $_).Extension -ne $pluginExtesion -and $(Get-Item $_).Extension -ne '.dll'} | ForEach-Object { $_.Replace($sourceDataDir,"") } | ForEach-Object { $_.TrimStart("\") }

#Get the esp file name and create the name of the bsa file to match the esp file
$pluginFile = $filesToCopy | Where-object {$(Get-Item $_).Extension -eq $pluginExtesion} | Select-Object -First 1
$pluginFile = $pluginFile.Replace("$sourceDataDir\","");
$bsaFileName = $pluginFile.Replace($pluginExtesion, ".bsa")

#Update the bsa file list
Set-Content -Path "$executingPath\bsafilelist.txt" -Value $($bsaFileList -join "`r`n")

# Update bas script with the name of the bsa file
$bsaScript = Get-Content -Path "$executingPath\bsascript.txt"
$bsaScript = $bsaScript.Replace("_generated", "$bsaFileName")

#Create a staging folder because with using the Archive tool all the files need to be in just the right place or it will not work

New-Item -ItemType Directory "$tempDir\Data"
Copy-Item "$destDataDir\*" "$tempDir\Data" -Recurse
Copy-Item "$executingPath\bsafilelist.txt" $tempDir
Set-Content -Path "$tempDir\bsascript.txt" -Value $bsaScript
Copy-Item "$archiveToolDir\Archive.exe" $tempDir

Push-Location $tempDir
Start-Process "$tempDir\Archive" -ArgumentList "bsascript.txt" -wait -NoNewWindow -PassThru
Pop-Location

#Copy back the archive
Copy-Item "$tempDir\$bsaFileName" $versionDir
Copy-Item "$destDataDir\$pluginFile" $versionDir

# Give time for files to not be in use
Start-Sleep -Milliseconds 20

#Prepair for 7z archive
Remove-Item $tempDir\* -Force -Recurse
New-Item -ItemType Directory "$tempDir\Data"
New-Item -ItemType Directory "$tempDir\Data\SKSE\Plugins"
Copy-Item "$versionDir\$bsaFileName" "$tempDir\Data"
Copy-Item "$versionDir\$pluginFile" "$tempDir\Data"
Copy-Item $destSksePlugin "$tempDir\Data\SKSE\Plugins"
Copy-Item $destSksePluginIni "$tempDir\Data\SKSE\Plugins"

$fileVersionNane = $Version.Replace('.', '_')
$zipFileName = $pluginFile.Replace($pluginExtesion, "$fileVersionNane.7z")

Start-Process "C:\Program Files\7-Zip\7z" -ArgumentList "a `"$versionDir\$zipFileName`" `"$tempDir\Data`" -mx5 -t7z" -wait -NoNewWindow -PassThru
Start-Sleep -Milliseconds 20

#Prepare for 7z loose archive
Remove-Item $tempDir\* -Force -Recurse
<#$requiredDataDirs | ForEach-Object{
    if (!$(Test-Path "$tempDir\Data\$_"))
    {
        New-Item -ItemType Directory "$tempDir\Data\$_"
    }
}#>
#Get-ChildItem "$destDataDir\*" -Recurse | Foreach-Object {Copy-Item $_.FullName "$tempDir\Data" }

$Source = "$destDataDir"
$Files = '*'
$Dest = "$tempDir\Data"
Get-ChildItem $Source -Filter $Files -Recurse | ForEach{
    $Path = ($_.DirectoryName + "\") -Replace [Regex]::Escape($Source), $Dest
    If(!(Test-Path $Path)){New-Item -ItemType Directory -Path $Path -Force | Out-Null}
    Copy-Item $_.FullName -Destination $Path -Force
}

Remove-Item "$tempDir\Data\AHZmoreHUDInventory$pluginExtesion" -Force

$zipFileName = $pluginFile.Replace($pluginExtesion, "$($fileVersionNane)Loose.7z")
Start-Process "C:\Program Files\7-Zip\7z" -ArgumentList "a `"$versionDir\$zipFileName`" `"$tempDir\Data`" -mx5 -t7z" -wait -NoNewWindow -PassThru

}
finally
{   
    if ($(Test-Path "$tempDir"))
    {
        Start-Sleep -Milliseconds 20
        # Cleaup the temp directory
        Remove-Item $tempDir -Force -Recurse
    }
}