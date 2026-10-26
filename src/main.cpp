// Click Before Frame - por Zeer (github.com/zeer866)
// Solo Android. Discord: https://discord.gg/jG7xA6Yh8
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <algorithm>
#include <string>

#ifndef GEODE_IS_ANDROID
#error "Click Before Frame es exclusivo para celulares (Android)."
#endif

using namespace geode::prelude;

// Cuánto tiempo (en segundos) debe pasar DESPUÉS del toque dentro del fotograma.
static float getAfterSeconds(float dt) {
    auto mod = Mod::get();
    if (mod->getSettingValue<std::string>("mode") == "Auto") {
        return dt * 0.5f; // Android no da la hora real del toque: se estima a mitad de fotograma
    }
    float v = static_cast<float>(mod->getSettingValue<double>("delay"));
    bool seconds = mod->getSettingValue<std::string>("unit") == "Segundos";
    float s = seconds ? v : v / 1000.f;
    return std::clamp(s, 0.f, dt);
}

class $modify(CBFGameLayer, GJBaseGameLayer) {
    void update(float dt) {
        // Solo dividimos el fotograma cuando hay un toque pendiente: sin toques, costo cero.
        if (!Mod::get()->getSettingValue<bool>("enabled") ||
            dt <= 0.f ||
            m_queuedButtons.empty() ||
            !typeinfo_cast<PlayLayer*>(this)) {
            GJBaseGameLayer::update(dt);
            return;
        }

        float after = getAfterSeconds(dt);
        float before = dt - after;

        // Sin diferencia útil: fotograma normal.
        if (after <= 0.0001f) {
            GJBaseGameLayer::update(dt);
            return;
        }

        // 1) Avanza la física hasta el instante del toque, sin toques.
        if (before > 0.0001f) {
            auto pending = m_queuedButtons;
            m_queuedButtons.clear();
            GJBaseGameLayer::update(before);
            m_queuedButtons = pending;
        }

        // 2) Aplica el toque y avanza el resto del fotograma.
        GJBaseGameLayer::update(after);
    }
};
