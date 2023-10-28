param (
    [string]$BuildType = "Debug"
)

$ORIG_DIR = Get-Location
$BUILD_DIR = "build/$BuildType"

if (-not (Test-Path -Path $BUILD_DIR -PathType Container)) {
    New-Item -Path $BUILD_DIR -ItemType Directory
}

Set-Location -Path $BUILD_DIR

$CMakeBuildType = if ($BuildType -eq "Debug") { "Debug" } else { "Release" }
$CMakeCC = "clang"
$CMakeCXX = "clang++"
$CMakeGenerator = "Ninja"
$CMakeArgs = "-DCMAKE_BUILD_TYPE=$CMakeBuildType", "-DCMAKE_C_COMPILER=$CMakeCC", "-DCMAKE_CXX_COMPILER=$CMakeCXX", "-G", $CMakeGenerator, "../.."

& cmake $CMakeArgs
& ninja

Set-Location -Path $ORIG_DIR