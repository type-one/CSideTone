# CSideTone
Portable C voice capture and (relatively) low latency feedback

https://github.com/type-one/CSideTone

Note: C++17 implementation available at https://github.com/type-one/SideTone

# What
Small test program written in C11 to have voice feedback with reasonably short latency. 
The code is portable and use the miniaudio framework (https://miniaud.io/).

# Why
The purpose was to test a simple double buffer structure with frame locking, with a 
capture thread producing samples and a playing thread replaying what was captured.
The double buffer itself is implemented in a cache-friendly way by splitting in two parts
the internal contiguous buffer of an std::vector.  The swapping of the buffers is also 
lock-free.
 
It was also fun to let my young children play with it and discover their own voice with
a small delay producing an echo.

This small framework can be a start for more complex sound processing on the feedback voice.

Note: miniaudio has some full-duplex features that allow latency optimization on this 
scenario, but I didn't want to rely on that for the test.

# How
Can be compiled and Linux and Windows, and should be easily
adapted for other platforms supported by miniaudio (Mac, Android, BSD).

The underlying miniaudio uses the standard default sound API of the OS (Linux/ALSA, Windows/WinMM, 
Android/OpenSLES, Mac/CoreAudio...) and not a specific low latency one (Jack, ASIO, ...),
with default parameters.

On Linux, just use cmake .
On Windows, just use cmake-gui to generate a Visual Studio solution

Tested on Windows 10 and Ubuntu/Debian Linux (on native host and in VMplayer).
It should work also on Raspberry Pi. 
It just use default capture and playback devices.

Launch in a shell using "csidetone.exe" or "csidetone"
Press "enter" to exit

You can play with different parameters in the audio/audio_config.h
file: format, sample rate, channels, period.

Recommended parameters are 22050 hz, mono (1 channel), signed 16, and a period between 
10 and 40 ms.  On specific audio boards and on native host, lower parameters can probably
be used.  

Compiling in release with O3/Ox/fast-math/SIMD optimisations can help to lower 
the period.  Disabling the progress display can also help to lower the period.

Enjoy

# Author
Laurent Lardinois / Type One (TFL-TDV)

https://be.linkedin.com/in/laurentlardinois

https://demozoo.org/sceners/19691/
