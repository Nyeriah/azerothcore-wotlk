-- Reusable option sets for creature text groups
CREATE TABLE IF NOT EXISTS `creature_text_option_sets` (
    `SetID`         TINYINT UNSIGNED NOT NULL,
    `Cooldown`      INT UNSIGNED NOT NULL DEFAULT 0      COMMENT 'Group cooldown in ms before it can fire again',
    `TriggerChance` TINYINT UNSIGNED NOT NULL DEFAULT 100 COMMENT '0-100 pct chance to fire at all',
    `PlayerOnly`    TINYINT UNSIGNED NOT NULL DEFAULT 0   COMMENT 'Only fire if target is a player',
    `comment`       VARCHAR(255) DEFAULT '',
    PRIMARY KEY (`SetID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `creature_text_option_sets` (`SetID`, `Cooldown`, `TriggerChance`, `PlayerOnly`, `comment`) VALUES
(1, 6000, 100, 0, 'Standard kill yell: 6s cooldown'),
(2, 6000, 100, 1, 'Player-only kill yell: 6s cooldown'),
(3, 5000, 100, 1, 'Short cooldown player-only kill yell: 5s cooldown'),
(4, 0,    30,  0, 'Chance-based: 30% trigger, no cooldown');

-- Assign option sets to creature text groups
CREATE TABLE IF NOT EXISTS `creature_text_options` (
    `CreatureID`    INT UNSIGNED NOT NULL,
    `GroupID`       TINYINT UNSIGNED NOT NULL,
    `OptionSetID`   TINYINT UNSIGNED NOT NULL,
    PRIMARY KEY (`CreatureID`, `GroupID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `creature_text_options` (`CreatureID`, `GroupID`, `OptionSetID`) VALUES
-- Black Temple
(22887, 2, 3),  -- High Warlord Najentus SAY_SLAY
(22871, 2, 2),  -- Teron Gorefiend SAY_SLAY
(22917, 1, 1),  -- Illidan Stormrage SAY_ILLIDAN_KILL
(22947, 3, 1),  -- Mother Shahraz SAY_SLAY
(22949, 3, 1),  -- Gathios the Shatterer SAY_COUNCIL_SLAY
(22950, 3, 1),  -- High Nethermancer Zerevor SAY_COUNCIL_SLAY
(22951, 3, 1),  -- Lady Malande SAY_COUNCIL_SLAY
(22952, 3, 1),  -- Veras Darkshadow SAY_COUNCIL_SLAY

-- Azjol-Nerub
(28684, 1, 1),  -- Krik'thir the Gatewatcher SAY_SLAY
(29120, 1, 1),  -- Anub'arak SAY_SLAY

-- Drak'Tharon Keep
(26631, 1, 1),  -- Novos the Summoner SAY_KILL
(26632, 1, 1),  -- The Prophet Tharon'ja SAY_KILL
(26630, 1, 1),  -- Trollgore SAY_KILL

-- Gundrak
(29306, 1, 1),  -- Gal'darah SAY_SLAY
(29305, 1, 1),  -- Moorabi SAY_SLAY
(29304, 1, 1),  -- Slad'ran SAY_SLAY

-- The Nexus
(26723, 1, 1),  -- Keristrasza SAY_SLAY
(26731, 1, 1),  -- Grand Magus Telestra SAY_KILL
(26794, 5, 1),  -- Ormorok the Tree-Shaper SAY_KILL

-- Ruby Sanctum
(39863, 6, 2),  -- Halion SAY_KILL
(39751, 2, 1),  -- Baltharus the Warborn SAY_KILL
(39746, 1, 1),  -- General Zarithrian SAY_KILL
(39747, 3, 1),  -- Saviana Ragefire SAY_KILL

-- Icecrown Citadel
(36597, 10, 3), -- The Lich King SAY_LK_KILL

-- Naxxramas
(16061, 1, 4);  -- Instructor Razuvious SAY_SLAY
