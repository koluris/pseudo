<h1>PSeudo</h1>

[![GitHub stars](https://img.shields.io/github/stars/dkoluris/pseudo.svg?style=flat-square)](https://github.com/dkoluris/pseudo/stargazers) [![GitHub license](https://img.shields.io/github/license/dkoluris/pseudo.svg?style=flat-square)](https://github.com/dkoluris/pseudo/blob/master/LICENSE) [![Twitter](https://img.shields.io/twitter/url/https/github.com/dkoluris/pseudo.svg?style=social)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Fdkoluris%2Fpseudo)

**PSeudo aims to be the world's simplest PSX emulator** both in terms of user experience and software implementation. It is coded in **C/C++** and is utilizing modern APIs such as **OpenGL** for visuals and **OpenAL** for audio. I have been developing this emulator on a Mac, so for the time being it works on **macOS**. Some experiments are complete on **Windows** platform as well, but still needs work on the **OpenGL** implementation. In time, more platforms will be supported, as I am aiming for a cross-platform solution.

<a href="https://github.com/dkoluris/pseudo/releases/tag/version-0.87">>> Get the latest PSeudo 0.87 release for Mac <<</a>

<h2>Showcase</h2>

<img alt="Gran Turismo 2" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/turismo.jpg" width="48.5%"/><img alt="WipEout 3" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/wipeout-3.jpg" width="48.5%" align="right"/>

<img alt="Suikoden II" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/suikoden.jpg" width="48.5%"/><img alt="Crash Bandicoot" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/crash-bandicoot.jpg" width="48.5%" align="right"/>

<h2>Completion</h2>

Here's a list with the overall progress of the emulator, broken down in distinct parts.
* `95% -> CPU Mips R3000A`
* `90% -> DMA`
* `85% -> Mem IO`
* `85% -> Movie Decoder`
* `80% -> Interrupts`
* `75% -> CD Decoder`
* `70% -> GPU Primitives & Commands`
* `65% -> GPU Textures`
* `60% -> Audio`
* `60% -> GTE`
* `55% -> Rootcounters`
* `25% -> Serial IO`
* `10% -> XA Audio`

**PSeudo** can load some commercial games, but it needs work on performance and overall timing.

<img alt="Tekken 3" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/tekken.jpg" width="48.5%"/><img alt="Ridge Racer Type-4" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/racer.jpg" width="48.5%" align="right"/>

<h2>Compile / Build</h2>

In order to build the application on **macOS**, the prerequisite is Xcode 10+. For **Windows** install **Microsoft Visual Studio 2019** and on top the additional **Clang** compiler, we will use that instead of MSVC's default. This is important in order to keep the source code identical between different platforms.

<img alt="Gran Turismo 2" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/turismo-2.jpg" width="48.5%"/><img alt="WipEout XL" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/wipeout-xl.jpg" width="48.5%" align="right"/>

<h2>License</h2>

Open-source under [Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0).
