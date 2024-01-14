$gitver = $(git rev-parse --short HEAD)
$file = "./include/version.h"
(Get-Content $file) -replace "^#define VERSION_BUILD\w*.*$", "#define VERSION_BUILD       $gitver" | Set-Content $file
