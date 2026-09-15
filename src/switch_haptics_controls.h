#pragma once

#include "controller_state.h"

// Hardware-independent gesture handling. Values are tenths, not float increments.
class SwitchHapticsControls {
public:
    bool modifier_down = false;
    void reset() { modifier_down = false; previous_ = CONTROLLER_DPAD_NEUTRAL; blocked_ = false; }

    // Returns 0 for no gesture, 1 for a change, 2 for an endpoint.
    unsigned update(ControllerState &state, uint8_t &tenths) {
        modifier_down = (state.buttons & CONTROLLER_BUTTON_MUTE) != 0;
        const auto direction = state.dpad;
        unsigned result = 0;
        if (modifier_down && direction != CONTROLLER_DPAD_NEUTRAL) {
            blocked_ = true;
            // Require a neutral report between actions; diagonals never adjust.
            if (previous_ == CONTROLLER_DPAD_NEUTRAL) {
                const auto old = tenths;
                if (direction == CONTROLLER_DPAD_UP && tenths < 20) ++tenths;
                else if (direction == CONTROLLER_DPAD_DOWN && tenths > 10) --tenths;
                else if (direction == CONTROLLER_DPAD_LEFT) tenths = 10;
                if (direction == CONTROLLER_DPAD_UP || direction == CONTROLLER_DPAD_DOWN ||
                    direction == CONTROLLER_DPAD_LEFT) {
                    result = (tenths == old || tenths == 10 || tenths == 20) ? 2 : 1;
                }
            }
        }
        if (direction == CONTROLLER_DPAD_NEUTRAL) blocked_ = false;
        previous_ = direction;
        if (blocked_) state.dpad = CONTROLLER_DPAD_NEUTRAL;
        state.buttons &= ~CONTROLLER_BUTTON_MUTE;
        return result;
    }
private:
    uint8_t previous_ = CONTROLLER_DPAD_NEUTRAL;
    bool blocked_ = false;
};

void switch_haptics_input(ControllerState &state);
void switch_haptics_controls_task();
