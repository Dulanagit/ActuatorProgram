#include "mode_handler.h"

void ModeHandler::onModeStart(GameMode mode) {
    switch (mode) {
        case MODE_VALORANT:
            onValorantStart();
            break;
        case MODE_RACING:
            onRacingStart();
            break;
        default:
            break;
    }
}

void ModeHandler::onModeStop(GameMode mode) {
    switch (mode) {
        case MODE_VALORANT:
            onValorantStop();
            break;
        case MODE_RACING:
            onRacingStop();
            break;
        default:
            break;
    }
}

void ModeHandler::onValorantStart() {
    // TODO: Valorant-specific initialization
}

void ModeHandler::onValorantStop() {
    // TODO: Valorant-specific cleanup
}

void ModeHandler::onRacingStart() {
    // TODO: Racing-specific initialization (stepper motor, paddle servos)
}

void ModeHandler::onRacingStop() {
    // TODO: Racing-specific cleanup
}
