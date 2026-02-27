CREATE TABLE IF NOT EXISTS `lfg_dungeon_cooldown` (
  `guid` int unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `dungeonId` int unsigned NOT NULL DEFAULT '0' COMMENT 'Dungeon ID',
  `completionTime` int unsigned NOT NULL DEFAULT '0' COMMENT 'Completion timestamp',
  PRIMARY KEY (`guid`, `dungeonId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='LFG Dungeon Cooldown';
