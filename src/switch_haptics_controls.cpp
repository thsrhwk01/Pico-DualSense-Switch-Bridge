#include "switch_haptics_controls.h"

#include "bt.h"
#include "config.h"
#include "pico/time.h"
#include "state_mgr.h"
#include "usb_mode.h"

namespace {
SwitchHapticsControls controls;
bool save_pending = false;
uint32_t save_after_ms = 0;
unsigned feedback_pending = 0;
unsigned led_phases = 0;
uint32_t led_after_ms = 0;
uint8_t saved_light = 0;

bool due(uint32_t now, uint32_t deadline) {
    return static_cast<int32_t>(now - deadline) >= 0;
}

} // namespace

void switch_haptics_input(ControllerState &state) {
    auto tenths = config_get_switch_haptics_tenths();
    const auto old = tenths;
    const auto feedback = controls.update(state, tenths);
    if (feedback) feedback_pending = feedback;
    if (tenths != old) {
        config_set_switch_haptics_tenths(tenths);
        save_pending = true;
        save_after_ms = to_ms_since_boot(get_absolute_time()) + 2000;
    }
}

void switch_haptics_controls_task() {
    const uint32_t now = to_ms_since_boot(get_absolute_time());
    const bool connected = bt_is_connected();
    if (!usb_mode_is_switch() || !connected) {
        controls.reset();
        feedback_pending = 0;
        if (led_phases && connected) state_send_mute_light(saved_light);
        led_phases = 0;
    } else {
        if (feedback_pending) {
            if (!led_phases) {
                saved_light = state_mute_light();
            }
            led_phases = feedback_pending * 2;
            feedback_pending = 0;
            state_send_mute_light(1);
            led_after_ms = now + 120;
        } else if (led_phases && due(now, led_after_ms)) {
            --led_phases;
            state_send_mute_light(led_phases == 0 ? saved_light :
                                 (led_phases % 2 ? 0 : 1));
            led_after_ms = now + 120;
        }
    }
    // Flash writes run in the main loop, never inside the Bluetooth callback.
    // Release the modifier and leave two seconds after the last adjustment.
    if (save_pending && !controls.modifier_down && due(now, save_after_ms)) {
        if (config_save()) save_pending = false;
        else save_after_ms = now + 2000;
    }
}
