## Signing and Notarization on MacOS ##

All Plugins, both VST3 and Audio Units, need signing to be able to run in a DAW on MacOS. These plugins are signed with a valid Apple "Developer ID Application" certificate. If you download the Zip files from the Internet though, MacOS expects them also to have been *notarized*. I've given up trying to notarize these plugins for now, as no matter which options I tried, the process always failed.

There is however a workaround to get these plugins loading on a Mac, even after downloading them from the Internet:

- Download the plugins for your architecture, and deploy them into the correct folder, as per the instructions above.
- The first time you try to load them in your DAW, the irritating pop-up dialog will appear, that it's been downloaded from the Internet, and Apple doesn't trust you to to keep yourself safe:
![](https://github.com/DoomyDwyer/ASPiKProjects/blob/main/pics/Mac_Security_Workaround/Problem.png)
- At this point, you should open **System Preferences** and go to the **Security & Privacy** app. on the **General** tab, you should see an option to "Open Anyway". Click on this button:
![](https://github.com/DoomyDwyer/ASPiKProjects/blob/main/pics/Mac_Security_Workaround/Solution.png)
- You should then get a second pop-up, in which there's an "Open" button. Click on this:
![](https://github.com/DoomyDwyer/ASPiKProjects/blob/main/pics/Mac_Security_Workaround/Success.png)

The plugin will now load in your DAW, and you won't be bothered with this pop-up, until you update the plugins. Hopefully by then, I'll have a more permanent solution in place.

If you encounter any issues running these plugins, either on Windows or MacOS, feel free to raise an [issue](https://github.com/DoomyDwyer/ASPiKProjects/issues). The MacOS plugins should work on all versions of MacOS >= 10.9. The Windows VST3s should work on any 64-bit Windows system.