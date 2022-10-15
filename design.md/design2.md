# Design of scheduling

The following code should be supported

## Scheduling events

```js
// time value is any max time value
// outputs to pipe
output('delay', timeValue, anything="delay")

// sets repeating metro, include quantization?
output('set_metro', timeValue)

// fraction is a decimal
output('event_at', bar, beat, fraction, anything="event")
```

## Receiving events

```js
delayed(anything, bar, beat, fraction)
metro(bar, beat, fraction)
event(anything, bar, beat, fraction)
```

## Further receipts

```js
floatFader(num, value)
toggle(num, value)
intFader(num, value)
```

## Further outputs

```js
output('midinote', pitch, vel, dur_ms)
output('midi', 'note'|'cc', note|num, val|vel, dur_ms)
```