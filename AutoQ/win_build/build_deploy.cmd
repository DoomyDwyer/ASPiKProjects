cmake -G"Visual Studio 16 2019" ../
msbuild AutoQ.sln
copy /y VST3\Debug\AutoQ.vst3\Contents\x86_64-win\AutoQ.vst3 D:\VST3
