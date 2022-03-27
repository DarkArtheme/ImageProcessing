Get-ChildItem ".\bin.*" -Recurse | Remove-Item -Force
Get-ChildItem ".\build.vs.2022" -Recurse | Remove-Item -Force
cmake.exe -P ".\build.vs.2022.cmake"