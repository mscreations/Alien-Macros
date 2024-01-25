# Alien Macros depends on libconfig for configuration file support.
# I haven't found prebuilt binaries for the library which means I have
# to compile it myself. The repo includes the vcxproj file, but the versions
# are too old to compile without complaint by Visual Studio. This script
# rewrites the settings
$vcxprojfile = './libconfig/lib/libconfig++.vcxproj'
$hppfile = './libconfig/lib/libconfig.h++'

$file = (Get-Content -Path $vcxprojfile -Raw)
$modified = $false

if ((Select-String -Pattern '<PlatformToolset>v143</PlatformToolset>' -Path $vcxprojfile).Matches.Length -eq 0)
{
    # Update PlatformToolset in vcxproj
    $file = $file -replace '(\s*<PlatformToolset>)v\d{3}(.*)', '${1}v143${2}'
    $modified = $true
}
if ((Select-String -Pattern '<WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>' -Path $vcxprojfile).Matches.Length -eq 0)
{
    # Update WindowsTargetPlatformVersion
    $file = $file -replace '(\s*<WindowsTargetPlatformVersion>)10\.0[0-9\.]*(<.*)', '${1}10.0${2}'
    $modified = $true
}
if ((Select-String -Pattern '<OutDir>\$\(SolutionDir\)build\\\$\(Platform\)\\\$\(Configuration\)\\</OutDir>' -Path $vcxprojfile).Matches.Length -eq 0)
{
    # Update build paths
    $file = $file -replace '(\s*<OutDir>\$\(SolutionDir\)build\\\$\(Platform\)\\).*(<.*)', '${1}$(Configuration)\${2}'
    $modified = $true
}
if ($modified)
{
    # Save back to original file
    Set-Content -Path $vcxprojfile -Value $file
}
else {
    Write-Host "Nothing done"
}

$file = (Get-Content -Path $hppfile -Raw)
# disable warning about non dll-interface class used as base for dll-interface class. We are not concerned with errors from libconfig.
if (($file | Select-String -Pattern '#pragma warning\(disable:4275\)').Matches.Length -eq 0)
{
    $file = "#pragma warning(disable:4275)`n" + $file
    Set-Content -Path $hppfile -Value $file
}

