#ifndef BATTLEGROUNDUTILS_H
#define BATTLEGROUNDUTILS_H

#include "Battleground.h"
#include <vector>

struct BattlegroundTemplate;

uint32 GetMinPlayersPerTeam(Battleground* bg, PvPDifficultyEntry const* bracketEntry);

// Effective low-levels MinPlayersPerTeam override for a BG type:
// the per-BG config if set, else the global one. 0 = no override.
// BGs entered through the Random BG queue have BATTLEGROUND_RB as
// their type, so they always resolve through the global key.
uint32 GetLowLevelsMinPlayersOverride(BattlegroundTypeId bgTypeId);

// MinPlayersPerTeam a BG on bgTemplate's map enforces once started from the Random BG queue in this bracket.
uint32 GetRandomBGMinPlayersPerTeam(BattlegroundTemplate const* bgTemplate, PvPDifficultyEntry const* bracketEntry);

// Random BG map candidates whose MinPlayersPerTeam is met by playersPerTeam players on each side.
// Returns all candidates for a full match (playersPerTeam reaching the Random BG maxPlayersPerTeam)
// or when none of them fits with a non-zero weight, so a match always gets a map.
std::vector<BattlegroundTemplate const*> FilterRandomBGCandidates(std::vector<BattlegroundTemplate const*> const& candidates,
    PvPDifficultyEntry const* bracketEntry, uint32 playersPerTeam, uint32 maxPlayersPerTeam);

#endif  // BATTLEGROUNDUTILS_H
