# zmk-rgb-ble-indicator

A [ZMK](https://zmk.dev) module that shows the Bluetooth profile on the RGB LEDs when it changes,
whether from a `&bt BT_SEL` key, `BT_NXT`/`BT_PRV`, or an app such as
[AutoSwitch](https://github.com/casab/zmk-autoswitch).

`leds` (listed left to right) is split evenly into one area per profile in use, in profile order,
and the active profile's area lights up for `duration-ms`. Profiles in use are the paired ones,
plus the active one when it's empty (you just selected it to pair). With two in use the keyboard
is split in halves, with three in thirds, and so on.

It shows even while the keyboard is idle and the underglow has turned itself off, since an app
can switch the profile without a keypress. It is drawn on top of the normal underglow and needs
[zmk-rgb-led-overlay](https://github.com/casab/zmk-rgb-led-overlay).

## Add it to your zmk-config

1. `config/west.yml`: add `zmk-rgb-led-overlay` and this module.

   ```yaml
       - name: zmk-rgb-led-overlay
         remote: casab          # url-base: https://github.com/casab
         revision: main
       - name: zmk-rgb-ble-indicator
         remote: casab
         revision: main
   ```

2. Set up zmk-rgb-led-overlay (see its README), then add to your keymap:

   ```dts
   / {
       rgb_ble_indicator {
           compatible = "zmk,rgb-ble-indicator";
           leds = <2 1 3 0 4 5 9 6 8 7>;   // left to right
           color-hsb = <240 100 50>;    // blue, half brightness
       };
   };
   ```

## Properties

| Property | Type | Default | Meaning |
| --- | --- | --- | --- |
| `leds` | array | required | LED indexes in display order (e.g. left to right). |
| `color-hsb` | array | `<240 100 50>` | Hue (0-360), saturation, brightness (0-100). |
| `duration-ms` | int | 2000 | How long the profile stays shown. |
| `slot` | int | 3 | zmk-rgb-led-overlay slot; higher slots are drawn over lower ones. |

## License

MIT
