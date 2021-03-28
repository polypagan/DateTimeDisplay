# DateTimeDisplay
Yet Another Version of DS3231 on Mega with LCD

based on: http://educ8s.tv/arduino-project-real-time-clock-with-ds3231-module/
which I just happened to stumble upon.

The above project uses Arduino MEGA2560, a board I own and haven't used much.

Note that this can work just as well with any (5v) Arduino that can support
these common 16x2 LCD shields with the 6 (5 + reset) buttons.
Mega is handy because the needed extra connections extend out from under the shield without trouble.

The part of this project which seemed lacking to me was setting the date and time one the DS3231.
The original author used the "bake it in and then comment it out" method.
I figured we have those buttons, right? So a bit of thought and I came up with what I consider to be a usable scheme,
especially when you consider how seldom you'll need to do this.

I'm not saying this is actually a useful device. Even the slow-arse Mega is overkill to set and display the date, 
time, and temperature. This is a clock that you'll need to change twice a year (no network). DS3231 is a good time-keeper, its thermometer is meant to provide temperature compensation for the timekeeping functions, not to display room temperature (It shows marked self-heating).

So this is really just for fun.

As always, questions, comments, criticisms, corrections are welcome.

Original project does not appear to be licensed or copywrite. Unilicense applied here. Open to correction if I am mistaken.
