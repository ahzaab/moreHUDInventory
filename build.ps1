rm -R .\build
mkdir build | OUT-NULL
Push-Location .\build
try
{
    cmake ..
    cmake --build . --config Release -v
}
finally
{
    Pop-Location
}