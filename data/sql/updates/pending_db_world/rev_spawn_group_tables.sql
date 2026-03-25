--
DROP TABLE IF EXISTS `spawn_group_template`;
CREATE TABLE `spawn_group_template` (
  `groupId` int(10) unsigned NOT NULL,
  `groupName` varchar(100) NOT NULL,
  `groupFlags` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`groupId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

DROP TABLE IF EXISTS `spawn_group`;
CREATE TABLE `spawn_group` (
  `groupId` int(10) unsigned NOT NULL,
  `spawnType` tinyint(3) unsigned NOT NULL,
  `spawnId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`groupId`,`spawnType`,`spawnId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Insert default spawn groups
DELETE FROM `spawn_group_template` WHERE `groupId` IN (0, 1);
INSERT INTO `spawn_group_template` (`groupId`, `groupName`, `groupFlags`) VALUES
(0, 'Default Group', 0x01),    -- SYSTEM (dynamic respawn by default)
(1, 'Legacy Group', 0x03);     -- SYSTEM | COMPATIBILITY_MODE
