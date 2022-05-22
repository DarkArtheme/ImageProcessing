Get-ChildItem ".\bin.*" -Recurse | Remove-Item -Force -Recurse
Get-ChildItem ".\build.vs.2022" -Recurse | Remove-Item -Force -Recurse
cmake.exe -P ".\build.vs.2022.cmake"