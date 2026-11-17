/*
 * Copyright (c) 2026 Engin Al
 *
 * SPDX-License-Identifier: MIT
 *
 * Bluetooth profile indicator: when the active Bluetooth profile changes (a BT key, or the
 * AutoSwitch app), light the active profile's area for a moment. `leds` (in left-to-right order)
 * is split evenly into one area per profile in use, in profile order; profiles in use are the
 * paired ones plus the active one (an empty profile you just selected to pair gets an area too).
 * With two paired profiles the keyboard is split in halves, with three in thirds, and so on.
 * Drawn through zmk-rgb-led-overlay.
 */

#define DT_DRV_COMPAT zmk_rgb_ble_indicator

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>

#include <zmk_rgb_led_overlay/led_overlay.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

BUILD_ASSERT(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 1,
             "Exactly one zmk,rgb-ble-indicator node is supported");

#define NODE DT_DRV_INST(0)

static const uint8_t leds[] = DT_PROP(NODE, leds);
static const uint16_t hue = DT_PROP_BY_IDX(NODE, color_hsb, 0);
static const uint8_t sat = DT_PROP_BY_IDX(NODE, color_hsb, 1);
static const uint8_t brt = DT_PROP_BY_IDX(NODE, color_hsb, 2);
#define SLOT DT_PROP(NODE, slot)
#define DURATION_MS DT_PROP(NODE, duration_ms)

static void clear_work_handler(struct k_work *work) {
    zmk_led_overlay_clear(SLOT);
    zmk_led_overlay_commit();
}

static K_WORK_DELAYABLE_DEFINE(ble_indicator_clear_work, clear_work_handler);

static void show_profile(uint8_t active) {
    // Count the profiles in use and find the active one's place among them.
    size_t areas = 0, area = 0;
    for (uint8_t i = 0; i < ZMK_BLE_PROFILE_COUNT; i++) {
        if (i == active || !zmk_ble_profile_is_open(i)) {
            if (i == active) {
                area = areas;
            }
            areas++;
        }
    }

    // Split the LEDs evenly; the remainder spreads over the areas.
    size_t n = ARRAY_SIZE(leds);
    size_t start = area * n / areas;
    size_t end = (area + 1) * n / areas;
    struct led_rgb color = zmk_led_overlay_hsb(hue, sat, brt);

    LOG_DBG("rgb ble indicator: profile %d, area %d of %d, LEDs %d-%d", active + 1, area + 1,
            areas, start, end - 1);

    zmk_led_overlay_clear(SLOT);
    for (size_t i = start; i < end; i++) {
        zmk_led_overlay_set(SLOT, leds[i], color);
    }
    zmk_led_overlay_commit();

    k_work_reschedule(&ble_indicator_clear_work, K_MSEC(DURATION_MS));
}

static int rgb_ble_indicator_listener(const zmk_event_t *eh) {
    const struct zmk_ble_active_profile_changed *ev = as_zmk_ble_active_profile_changed(eh);

    if (ev != NULL && ev->index < ZMK_BLE_PROFILE_COUNT) {
        show_profile(ev->index);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(rgb_ble_indicator, rgb_ble_indicator_listener);
ZMK_SUBSCRIPTION(rgb_ble_indicator, zmk_ble_active_profile_changed);
