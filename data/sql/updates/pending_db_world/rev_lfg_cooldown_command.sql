-- Add lfg cooldown commands to the command table
DELETE FROM `command` WHERE `name` IN ('lfg cooldown', 'lfg cooldown clear');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('lfg cooldown', 3, 'Syntax: .lfg cooldown $subcommand\nType .lfg cooldown to see the list of possible subcommands or .help lfg cooldown $subcommand to see info on subcommands.'),
('lfg cooldown clear', 3, 'Syntax: .lfg cooldown clear\nClears all LFG dungeon cooldowns for all players.');

-- Add acore_string for cooldown cleared message (English, German, Spanish)
DELETE FROM `acore_string` WHERE `entry` = 10000;
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(10000, 'LFG dungeon cooldowns cleared for all players.', NULL, NULL, 'LFG-Dungeon-Abklingzeiten für alle Spieler zurückgesetzt.', NULL, NULL, 'Tiempos de reutilización de mazmorras LFG eliminados para todos los jugadores.', 'Tiempos de reutilización de mazmorras LFG eliminados para todos los jugadores.', NULL);
