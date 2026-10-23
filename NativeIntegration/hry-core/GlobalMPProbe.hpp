#pragma once
#include <Hry/Plugin.hpp>
class GlobalMPProbe final : public hry::Plugin {
    hry::PluginInfo info_{};
    hry::Logger* logger_ = nullptr;
    bool visible_ = true;
public:
    GlobalMPProbe();
    Result init(const InitParams&& params) override;
    void initConfig(hry::Config*) override {}
    void initKeyBinds(hry::KeyBinds*) override {}
    void initEvents(hry::EventDispatcher* dispatcher) override;
    const hry::PluginInfo& getPluginInfo() const override { return info_; }
private:
    void render();
};
