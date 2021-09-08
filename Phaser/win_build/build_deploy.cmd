cmake -G"Visual Studio 16 2019" ../
msbuild Phaser.sln
copy /y VST3\Debug\Phaser.vst3\Contents\x86_64-win\Phaser.vst3 D:\VST3
