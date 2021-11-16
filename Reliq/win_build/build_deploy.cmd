cmake -G"Visual Studio 16 2019" ../
msbuild Reliq.sln /p:Configuration=Release
copy /y VST3\Release\Phaser.vst3\Contents\x86_64-win\Reliq.vst3 D:\VST3
