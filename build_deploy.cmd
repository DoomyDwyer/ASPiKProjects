set target=%1
IF %1.==. set target=Release

for %%I in (..) do set project=%%~nxI
echo %%~nxI

set projectFile=%project%.sln
IF NOT EXIST %projectFile% cmake -G"Visual Studio 16 2019" ../

msbuild %projectFile% /p:Configuration=%target%
copy /y VST3\%target%\%project%.vst3\Contents\x86_64-win\%project%.vst3 D:\VST3
