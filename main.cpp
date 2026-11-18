#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <matjson.hpp>
#include <fstream>

using namespace geode::prelude;

struct ClickEvent {
    int frame;      // physics step when the input happened
    bool down;      // press or release
    bool player1;
};

static std::vector<ClickEvent> g_clicks;
static CCLabelBMFont* g_label = nullptr;

static void exportClicks(GJGameLevel* level) {
    matjson::Value arr = std::vector<matjson::Value>{};
    int lastDown = -1;
    for (auto& c : g_clicks) {
        matjson::Value o = matjson::makeObject({
            {"frame", c.frame},
            {"down", c.down},
            {"p1", c.player1},
            {"gap", (c.down && lastDown >= 0) ? c.frame - lastDown : 0}
        });
        if (c.down) lastDown = c.frame;
        arr.push(o);
    }
    auto path = Mod::get()->getSaveDir() /
        fmt::format("clicks_{}.json", level->m_levelID.value());
    std::ofstream(path) << arr.dump();
}

class $modify(FWBaseGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
        // m_gameState.m_currentProgress = current physics step (verify in your bindings version)
        g_clicks.push_back({ m_gameState.m_currentProgress, down, isPlayer1 });

        if (g_label && Mod::get()->getSettingValue<bool>("show-hud")) {
            int gap = 0, prevDown = -1;
            for (size_t i = 0; i + 1 < g_clicks.size(); i++)
                if (g_clicks[i].down) prevDown = g_clicks[i].frame;
            if (down && prevDown >= 0) gap = g_clicks.back().frame - prevDown;
            g_label->setString(fmt::format("Click #{}  frame {}  gap {}",
                g_clicks.size(), g_clicks.back().frame, gap).c_str());
        }
    }
};

class $modify(FWPlayLayer, PlayLayer) {
    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        g_clicks.clear();
        g_label = CCLabelBMFont::create("", "bigFont.fnt");
        g_label->setScale(0.4f);
        g_label->setAnchorPoint({0.f, 1.f});
        g_label->setPosition({10.f, CCDirector::get()->getWinSize().height - 10.f});
        g_label->setZOrder(100);
        m_uiLayer->addChild(g_label);
    }
    void resetLevel() {
        PlayLayer::resetLevel();
        g_clicks.clear();
    }
    void onQuit() {
        if (Mod::get()->getSettingValue<bool>("export-json") && !g_clicks.empty())
            exportClicks(m_level);
        g_label = nullptr;
        PlayLayer::onQuit();
    }
};
