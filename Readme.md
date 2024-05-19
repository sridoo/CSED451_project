# CSED 451 project - weather simulator

## Use Guide
In default, you are in the 'move mode'. You can
* move xy plain by wasd.
* move front and back using mice scroll.
* tilt by qe.
* look around by moving mice.

You can enter the ''terminal mode'' by pressing 't'. In the terminal mode supports the folling instructions
* `cloudy {sky_covering_ratio}`
  * It makes non raining day with cloud covering `sky_covering_ratio`% amount.
  * `sky_covering_ratio` ranges from 0 to 100.
  * Setting `sky_covering_ratio` 0 makes sunny day.
* `rainy {precipitation}`
  * It makes rainy day with `precipitation` amount
  * `precipitation` ranges from 1 to 100
* `snowy {percipitation}`
  * Same with rainy instruction but this make it snow istead of rain.
* `time {hour}`
  * Set time to `hour`.
  * `hour` can be decimal ranges from 0 to 24.
