<h1>PSeudo</h1>

[![GitHub stars](https://img.shields.io/github/stars/dkoluris/pseudo.svg)](https://github.com/dkoluris/pseudo/stargazers) [![GitHub license](https://img.shields.io/github/license/dkoluris/pseudo.svg)](https://github.com/dkoluris/pseudo/blob/master/LICENSE)

<a href="https://github.com/dkoluris/pseudo/releases/tag/version-0.8">>> Get the latest PSeudo 0.8 release for Mac <<</a>

**PSeudo aims to be the world's simplest PSX emulator** both in terms of user experience and software implementation. It is coded in **C++** and is utilizing modern APIs such as **OpenGL** for visuals and **OpenAL** for audio. I have been developing this emulator on a Mac, so for the time being it works on **macOS** & **iOS**. This will change in future releases; more platforms will be supported, as I am aiming for a cross-platform solution.

<br/><a href="http://www.youtube.com/watch?v=BFvLRjOE99E">Check out the video below, for a homebrew demo/cracktro showcase:<br/>

<img alt="FF9" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/FF9.jpg" width="48.5%"/><img alt="Super Pox" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/Pox.jpg" width="48.5%" align="right"/>

<img alt="Casper" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/Casper.jpg" width="48.5%"/><img alt="Paradox Deadline" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/Deadline.jpg" width="48.5%" align="right"/>

</a>

<h2>Completion</h2>

Here's a list with the overall progress of the emulator, broken down in distinct parts. Components with 0% progress are not listed.
* `95% -> PSX-EXE Loader`
* `90% -> CPU Mips R3000A`
* `90% -> Interrupts`
* `85% -> GPU Primitives & Commands`
* `80% -> Mem IO`
* `75% -> GPU Textures`
* `70% -> Audio`
* `60% -> Rootcounters`
* `60% -> DMA`
* `50% -> Serial IO`
* `20% -> GTE (Co-processor 2)`
* `15% -> CD-ROM Decoder`

**PSeudo** can load some initial game screens, but nothing more for now. Also, for quite some time I will keep working on it with the provided slow CPU Interpreter.

<h2>License</h2>

Open-source under [Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0).
