/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "BattlegroundMgr.h"
#include "BattlegroundUtils.h"
#include "DBCStructure.h"
#include "WorldMock.h"
#include "gtest/gtest.h"

/**
 * Tests which maps the Random BG queue may pick for a match of a given size:
 * maps whose effective MinPlayersPerTeam exceeds the matched team size are
 * skipped, unless the match is full or that leaves nothing pickable.
 */
class RandomBGMinPlayersFilterTest : public ::testing::Test
{
protected:
    static constexpr uint32 RandomBGMaxPlayersPerTeam = 10;

    void SetUp() override
    {
        PreviousWorld = std::move(sWorld);
        MockWorld = new ::testing::NiceMock<WorldMock>();
        ON_CALL(*MockWorld, getIntConfig(::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(*MockWorld, getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            .WillByDefault(::testing::Return(80));
        sWorld.reset(MockWorld);

        WarsongGulch = MakeTemplate(BATTLEGROUND_WS, 4, 1);
        ArathiBasin = MakeTemplate(BATTLEGROUND_AB, 4, 1);
        StrandOfTheAncients = MakeTemplate(BATTLEGROUND_SA, 5, 1);
        AlteracValley = MakeTemplate(BATTLEGROUND_AV, 20, 1);
    }

    void TearDown() override
    {
        sWorld = std::move(PreviousWorld);
    }

    void SetIntConfig(ServerConfigs index, uint32 value)
    {
        ON_CALL(*MockWorld, getIntConfig(index))
            .WillByDefault(::testing::Return(value));
    }

    static BattlegroundTemplate MakeTemplate(BattlegroundTypeId id, uint16 minPlayersPerTeam, uint8 weight)
    {
        BattlegroundTemplate bgTemplate{};
        bgTemplate.Id = id;
        bgTemplate.MinPlayersPerTeam = minPlayersPerTeam;
        bgTemplate.Weight = weight;
        return bgTemplate;
    }

    static PvPDifficultyEntry MakeBracket(uint32 minLevel, uint32 maxLevel)
    {
        return PvPDifficultyEntry(/*mapId*/ 0, /*bracketId*/ 0, minLevel, maxLevel, /*difficulty*/ 0);
    }

    std::vector<BattlegroundTemplate const*> AllCandidates() const
    {
        return { &WarsongGulch, &ArathiBasin, &StrandOfTheAncients, &AlteracValley };
    }

    BattlegroundTemplate WarsongGulch;
    BattlegroundTemplate ArathiBasin;
    BattlegroundTemplate StrandOfTheAncients;
    BattlegroundTemplate AlteracValley;
    ::testing::NiceMock<WorldMock>* MockWorld = nullptr;
    std::unique_ptr<IWorld> PreviousWorld;
};

TEST_F(RandomBGMinPlayersFilterTest, SkipsMapsAboveMatchedTeamSize)
{
    PvPDifficultyEntry bracket = MakeBracket(71, 80);

    std::vector<BattlegroundTemplate const*> expected = { &WarsongGulch, &ArathiBasin };
    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 4, RandomBGMaxPlayersPerTeam), expected);
}

TEST_F(RandomBGMinPlayersFilterTest, KeepsMapsWhoseMinimumIsMet)
{
    PvPDifficultyEntry bracket = MakeBracket(71, 80);

    std::vector<BattlegroundTemplate const*> expected = { &WarsongGulch, &ArathiBasin, &StrandOfTheAncients };
    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 5, RandomBGMaxPlayersPerTeam), expected);
    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 9, RandomBGMaxPlayersPerTeam), expected);
}

// Maps needing more players than the Random BG cap fill up from the queue once started.
TEST_F(RandomBGMinPlayersFilterTest, FullMatchKeepsEveryMap)
{
    PvPDifficultyEntry bracket = MakeBracket(71, 80);

    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 10, RandomBGMaxPlayersPerTeam), AllCandidates());
}

TEST_F(RandomBGMinPlayersFilterTest, FallsBackToAllWhenNoMapFits)
{
    PvPDifficultyEntry bracket = MakeBracket(71, 80);

    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 3, RandomBGMaxPlayersPerTeam), AllCandidates());
}

TEST_F(RandomBGMinPlayersFilterTest, FallsBackToAllWhenOnlyZeroWeightMapsFit)
{
    PvPDifficultyEntry bracket = MakeBracket(71, 80);
    WarsongGulch.Weight = 0;
    ArathiBasin.Weight = 0;

    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 4, RandomBGMaxPlayersPerTeam), AllCandidates());
}

TEST_F(RandomBGMinPlayersFilterTest, LowLevelsOverrideReplacesTemplateMinimumBelowMaxLevel)
{
    SetIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS, 3);
    PvPDifficultyEntry bracket = MakeBracket(10, 19);

    EXPECT_EQ(GetRandomBGMinPlayersPerTeam(&StrandOfTheAncients, &bracket), 3u);
    EXPECT_EQ(GetRandomBGMinPlayersPerTeam(&AlteracValley, &bracket), 3u);
    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 3, RandomBGMaxPlayersPerTeam), AllCandidates());
}

TEST_F(RandomBGMinPlayersFilterTest, LowLevelsOverrideIgnoredAtMaxLevel)
{
    SetIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS, 3);
    PvPDifficultyEntry bracket = MakeBracket(71, 80);

    EXPECT_EQ(GetRandomBGMinPlayersPerTeam(&StrandOfTheAncients, &bracket), 5u);

    std::vector<BattlegroundTemplate const*> expected = { &WarsongGulch, &ArathiBasin };
    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 4, RandomBGMaxPlayersPerTeam), expected);
}

// A BG started from the Random BG queue keeps BATTLEGROUND_RB as its type,
// so only the global low-levels key applies to it, never the per-map one.
TEST_F(RandomBGMinPlayersFilterTest, PerMapLowLevelsOverrideIgnored)
{
    SetIntConfig(CONFIG_BATTLEGROUND_OVERRIDE_LOWLEVELS_MINPLAYERS_SA, 2);
    PvPDifficultyEntry bracket = MakeBracket(10, 19);

    EXPECT_EQ(GetRandomBGMinPlayersPerTeam(&StrandOfTheAncients, &bracket), 5u);

    std::vector<BattlegroundTemplate const*> expected = { &WarsongGulch, &ArathiBasin };
    EXPECT_EQ(FilterRandomBGCandidates(AllCandidates(), &bracket, 4, RandomBGMaxPlayersPerTeam), expected);
}
