#include "GlobalMPProbe.hpp"
#include <imgui.h>
GlobalMPProbe::GlobalMPProbe() {
    info_.name = "globalmp-probe";
    info_.fullName = "GlobalMP Native Integration Probe";
    info_.authorInfo = {"GlobalMP", ""};
    info_.website = "https://globalmp.online";
    info_.version = hry::Version{0, 1, 0};
    info_.desc = "Non-invasive probe for hry-core compatibility.";
}
GlobalMPProbe::Result GlobalMPProbe::init(const InitParams&& params) {
    logger_ = params.logger;
    if (logger_) logger_->info("GlobalMP native probe initialized");
    return Result::Ok;
}
void GlobalMPProbe::initEvents(hry::EventDispatcher* dispatcher) {
    dispatcher->system.onImGuiRender.connect<&GlobalMPProbe::render>(this);
}
void GlobalMPProbe::render() {
    if (!visible_) return;
    ImGui::Begin("GlobalMP - Native Integration Probe", &visible_);
    ImGui::TextUnformatted("hry-core plugin loaded");
    ImGui::TextUnformatted("No game memory hooks installed.");
    ImGui::TextUnformatted("Remote vehicle spawning is NOT implemented.");
    ImGui::End();
}
INIT_PLUGIN(GlobalMPProbe)
