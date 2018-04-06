# tomarto 🍅

A pomodoro timer you probably shouldn't use

This is based on my quite specific requirements for a timer. They are:
- Keyboard control only.
- Windows only.
- Lightweight (single exe, easy to move around).
- Easy to ignore.

## Keybindings

    s   = start/stop timer
    r   = reset and switch to work timer
    b   = reset and switch to break timer
    -   = decrease current timer
    =/+ = if stopped, increase current timer duration by 1 minute
          if running, increase current timer by 1 minute without affecting the base timer duration.
    q   = quit

The normal pomodoro flow would look like this:
1. Press `s` to start the work timer.
2. 25 minutes pass...
3. Press `b` to reset and switch to the break timer.
4. Press `s` to start the break timer.
5. 5 minutes pass...
6. Press `r` to reset and switch to the work timer.

## Building/customisation

A Visual Studio 2017 project is included. You can use premake 5 to generate projects for other tools, [see here for info](https://github.com/premake/premake-core/wiki/Using-Premake).

You can edit `config.h` to change the colours and some defaults.

## Notes

The timer will be default appear in the bottom right of the primary monitor. You can click on the window to draw it around.

The work timer appears in green, the break timer in orange. When a timer completes, the taskbar icon will be highlighted and the timer background will change colour. This is designed to draw your attention for a moment but be easy to ignore afterwards.

The `+` and `-` keys can be used to change the base duration of the timer when the timer is stopped (e.g. after pressing `r` or `b`). The `+` key can also be used to add time while the timer is running.

The work timer defaults to 25 minutes, the break timer to 5 minutes. Changes to these do not persist across restarts.

## Why `+` key when the timer is running thing?

It's simple, I found that abandoning a pomodoro entirely when I got distracted by something that could not be ignored (e.g. a person coming to my desk to ask a question) was more harmful to my productivity than simply adding time back to the timer when I return to my task.
