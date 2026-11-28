#include "BattlegroundUtils.h"
#include "BattlegroundMgr.h"
#include "World.h"

static bool IsMaxLevelBracket(PvPDifficultyEntry const* bracketEntry)
{
    auto maxPlayerLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    return bracketEntry->minLevel <= maxPlayerLevel && maxPlayerLevel <= bracketEntry->maxLevel;
}

uint32 GetMinPlayersPerTeam(Battleground* bg, PvPDifficultyEntry const* bracketEntry)
{
    // The problem addressed here is that methods such as bg->GetMinLevel() and bg->GetMaxLevel() have a different meaning
    // according to whether the BG is a template (then it's the value from the `battleground_template` table)
    // or if it's the real BG (then it's the specific bracket minimum level, e.g. "60" for "60-69").

    if (!bg->isTemplate() || bg->isArena())
    {
        return bg->GetMinPlayersPerTeam();
    }

    auto lowLevelsOverride = GetLowLevelsMinPlayersOverride(bg->GetBgTypeID());

    return (lowLevelsOverride && !IsMaxLevelBracket(bracketEntry)) ? lowLevelsOverride : bg->GetMinPlayersPerTeam();
}

uint32 GetLowLevelsMinPlayersOverride(BattlegroundTypeId bgTypeId)
{
    uint32 perBg = 0;
    switch (bgTypeId)
    {
        case BATTLEGROUND_AV: perBg = sWorld->getIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS_AV); break;
        case BATTLEGROUND_WS: perBg = sWorld->getIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS_WS); break;
        case BATTLEGROUND_AB: perBg = sWorld->getIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS_AB); break;
        case BATTLEGROUND_EY: perBg = sWorld->getIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS_EY); break;
        case BATTLEGROUND_SA: perBg = sWorld->getIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS_SA); break;
        case BATTLEGROUND_IC: perBg = sWorld->getIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS_IC); break;
        default: break;
    }
    return perBg ? perBg : sWorld->getIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS);
}

uint32 GetRandomBGMinPlayersPerTeam(BattlegroundTemplate const* bgTemplate, PvPDifficultyEntry const* bracketEntry)
{
    // Mirrors Battleground::GetMinPlayersPerTeam on the created BG, whose type stays BATTLEGROUND_RB.
    uint32 lowLevelsOverride = GetLowLevelsMinPlayersOverride(BATTLEGROUND_RB);
    return (lowLevelsOverride && !IsMaxLevelBracket(bracketEntry)) ? lowLevelsOverride : bgTemplate->MinPlayersPerTeam;
}

std::vector<BattlegroundTemplate const*> FilterRandomBGCandidates(std::vector<BattlegroundTemplate const*> const& candidates,
    PvPDifficultyEntry const* bracketEntry, uint32 playersPerTeam, uint32 maxPlayersPerTeam)
{
    // A match can't grow past the queue's cap, and maps needing more fill up from the queue once started.
    if (playersPerTeam >= maxPlayersPerTeam)
        return candidates;

    std::vector<BattlegroundTemplate const*> fitting;
    bool hasWeight = false;

    for (BattlegroundTemplate const* bgTemplate : candidates)
    {
        if (GetRandomBGMinPlayersPerTeam(bgTemplate, bracketEntry) > playersPerTeam)
            continue;

        fitting.push_back(bgTemplate);
        hasWeight |= bgTemplate->Weight > 0;
    }

    // A zero-weight map is never picked next to weighted ones, so it must not become the only choice either.
    return hasWeight ? fitting : candidates;
}
