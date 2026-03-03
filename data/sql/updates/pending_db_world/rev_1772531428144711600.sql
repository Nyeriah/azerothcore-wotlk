--
DELETE FROM `acore_string` WHERE `entry` IN (5111, 5112, 5113, 5114, 5115, 5116, 5117);

INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(5111, 'Battlefield {} not found.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5112, 'Battlefield {} battle started.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5113, 'Battlefield {} battle stopped.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5114, 'Battlefield {} factions switched.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5115, 'Battlefield {} timer set to {} seconds.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5116, 'Battlefield {} enabled.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5117, 'Battlefield {} disabled.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
