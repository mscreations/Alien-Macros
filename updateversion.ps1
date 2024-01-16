$gitver = $(git rev-parse --short HEAD)
$file = "./include/version-git.h"
if ((Select-String -Path $file -Pattern $gitver).Matches.Length -ne 1)
{
    (Get-Content $file) -replace "^#define VERSION_BUILD\w*.*$", "#define VERSION_BUILD       $gitver" | Set-Content $file
}

