#include <cassert>
#include <cstdint>
#include <vector>
#include "switch_haptics_controls.h"

static uint32_t now_ms = 0;
static bool connected = true, switch_mode = true, save_ok = true;
static uint8_t gain = 10, light = 0;
static unsigned saves = 0;
static std::vector<uint8_t> lights;
uint64_t get_absolute_time() { return static_cast<uint64_t>(now_ms) * 1000; }
bool bt_is_connected() { return connected; }
bool usb_mode_is_switch() { return switch_mode; }
uint8_t config_get_switch_haptics_tenths() { return gain; }
void config_set_switch_haptics_tenths(uint8_t value) { gain = value; }
bool config_save() { ++saves; return save_ok; }
uint8_t state_mute_light() { return light; }
void state_send_mute_light(uint8_t mode) { light = mode; lights.push_back(mode); }

static void input(bool mute, uint8_t direction) {
    ControllerState state{};
    state.buttons = mute ? CONTROLLER_BUTTON_MUTE : 0u;
    state.dpad = direction;
    switch_haptics_input(state);
}

int main() {
    input(true, CONTROLLER_DPAD_UP);
    assert(gain == 11 && saves == 0); // immediate gain, deferred flash
    switch_haptics_controls_task();
    assert(light == 1);
    now_ms = 120;
    switch_haptics_controls_task();
    assert(light == 0);
    now_ms = 240;
    switch_haptics_controls_task();
    assert(light == 0);
    now_ms = 3000;
    switch_haptics_controls_task();
    assert(saves == 0); // modifier held
    input(false, CONTROLLER_DPAD_NEUTRAL);
    save_ok = false;
    switch_haptics_controls_task();
    assert(saves == 1);
    now_ms = 4999;
    switch_haptics_controls_task();
    assert(saves == 1);
    save_ok = true;
    now_ms = 5000;
    switch_haptics_controls_task();
    assert(saves == 2);
    switch_haptics_controls_task();
    assert(saves == 2);

    // Two endpoint flashes, even without any game rumble.
    lights.clear();
    input(true, CONTROLLER_DPAD_LEFT);
    switch_haptics_controls_task();
    for (int i = 0; i < 4; ++i) {
        now_ms += 120;
        switch_haptics_controls_task();
    }
    assert((lights == std::vector<uint8_t>{1, 0, 1, 0, 0}));
    assert(gain == 10);
    connected = false;
    now_ms += 2000;
    switch_haptics_controls_task();
    assert(saves == 3); // pending setting persists after disconnect

    connected = true;
    input(true, CONTROLLER_DPAD_UP);
    switch_haptics_controls_task();
    assert(light == 1 && gain == 11);
    switch_mode = false;
    switch_haptics_controls_task();
    assert(light == 0); // restore LED on mode exit
    now_ms += 2000;
    switch_haptics_controls_task();
    assert(saves == 4);
    switch_mode = true;
    input(true, CONTROLLER_DPAD_UP);
    assert(gain == 12); // gesture state reset on mode exit
}
