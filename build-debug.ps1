#rm -R .\build
#mkdir build | OUT-NULL
Push-Location .\build
try
{
    cmake ..
    cmake --build . -v
}
finally
{
    Pop-Location
}