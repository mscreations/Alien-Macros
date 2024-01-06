# :alien: Alien Macros

[![MSBuild](https://github.com/mscreations/Alien-Macros/actions/workflows/msbuild.yml/badge.svg)](https://github.com/mscreations/Alien-Macros/actions/workflows/msbuild.yml)
[![CodeQL](https://github.com/mscreations/Alien-Macros/actions/workflows/codeql.yml/badge.svg)](https://github.com/mscreations/Alien-Macros/actions/workflows/codeql.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/5dfa11db7603480aaf3d61655035c9f1)](https://app.codacy.com/gh/mscreations/Alien-Macros/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![License](https://badgen.net/github/license/mscreations/Alien-Macros)](https://github.com/mscreations/Alien-Macros/blob/master/LICENSE)

## What is it?

This is a program that stays running in the background and istens for specific keypress messages for the macro keys from the builtin Alienware keyboard. When they are detected, it generates a new keypress for F13-F16. This can be used to have AutoHotKey trigger a specific action on those keypresses now.

## Why is it necessary?

The macro keys only work with the Alienware Command Center app. To many, myself included, this is considered Dell bloatware. In my case, I was having overheating issues because no matter what I setup, Alienware Command Center would refuse to raise the fans above idle. I began using the excellent [AlienFX-Tools](https://github.com/T-Troll/alienfx-tools) to manage my computer's fan speed and lighting effects. The only thing lacking was the macro functionality. That project utilizes the Dell AlienFX SDK to implement it's functionality, which does not expose the macro keys.

## How did you figure this out? How does it work?

I utilized [Wireshark](https://www.wireshark.org/) alongside [USBPcap](https://desowin.org/usbpcap/) to snoop on the USB traffic in my computer. Identifying the correct device proved troublesome because it is impossible to unplug and replug the internal keyboard to determine which device it is. In addition, I had issues where once I figured out that on my system the keyboard was generating messages from address 1.6.1, I had trouble determining the device that was attached to 1.6.0 (the device at endpoint 0). For some reason the device changed everytime I ran a trace and was never sensible.

Once I figured out the device issue, I looked at what was showing up when I pressed the macro keys. When the macro keys are pressed, they provide a HID report on the Consumer Usage Page in the range `0x4c-0x4f`. I found a Microsoft sample project that was able to connect to the proper device and could interpret the messages from the USB. I pulled the relevant parts from the sample project into this which starts an asynchronous read thread that simply monitors for the macro keypresses and translates them to F13-F16.

# How do I use this?

Currently I'd advise against using it as this is more proof of concept than anything else. The window has to stay open for the translation to work and I haven't implemented any kinds of settings or anything to allow customization.

:warning: ***If you decide to use it, you do so at your own risk.*** :warning:

This is intended to be used alongside a tool such as [AutoHotKey](https://www.autohotkey.com/) to perform the actual macro functionality as AHK can do everything already without me having to figure out how to add that functionality. An example script can be found [here](./ExampleAHKScript.ahk).

# Tested Systems

| System | VID | PID | Macro Range |
| ------ | --- | --- | ----------- |
| Alienware m17 R4 w/Per-Key RGB | 0x0d62 | 0x1a1c | 0x4c-0x4f |

# HELP! It doesn't work

That's probably because you have a different keyboard VID/PID than my laptop's keyboard. Create an issue and we can look at adding it in. This would require a [Wireshark trace](https://github.com/mscreations/Alien-Macros/wiki/Wireshark-Trace) to verify the device and what your particular keyboard is sending.

After determining the correct VID/PID for your device, you can change them by using command line arguments:

`.\Alien-Macros.exe --vid 0x0d62 --pid 0x1a1c`

# TODO

- [ ] Determine other VID/PIDs that are used in other systems. Will require users to report what they encounter in their own systems. Please report by commenting on [Issue #1](https://github.com/mscreations/Alien-Macros/issues/1)
- [ ] Check system for any available/supported VID/PID automatically.
- [ ] Allow customization of macro action

# Contributing

I welcome any contributions. Please open a pull request if you have anything to contribute.

### Did this help you? [Buy me a Coffee :coffee:](https://www.buymeacoffee.com/mscreations82)
