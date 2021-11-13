cmake -G"Visual Studio 16 2019" ../
msbuild AutoQ.sln /p:Configuration=Release
copy /y VST3\Release\AutoQ.vst3\Contents\x86_64-win\AutoQ.vst3 D:\VST3
