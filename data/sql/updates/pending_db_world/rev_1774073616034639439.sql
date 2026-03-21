-- Add autobroadcast commands
DELETE FROM `command` WHERE `name` IN ('autobroadcast', 'autobroadcast list', 'autobroadcast add', 'autobroadcast locale', 'autobroadcast remove');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('autobroadcast', 3, 'Syntax: .autobroadcast $subcommand\nType .autobroadcast to see a list of subcommands or .help autobroadcast $subcommand to see info on subcommands.'),
('autobroadcast list', 2, 'Syntax: .autobroadcast list\nList all autobroadcast entries.'),
('autobroadcast add', 3, 'Syntax: .autobroadcast add $weight $text\nAdd a new autobroadcast entry with the given weight and text.'),
('autobroadcast locale', 3, 'Syntax: .autobroadcast locale $id $locale $text\nAdd or replace a localized text for the autobroadcast entry with the given ID.'),
('autobroadcast remove', 3, 'Syntax: .autobroadcast remove $id\nRemove the autobroadcast entry with the given ID and its locale entries.');

-- Add acore_string entries for autobroadcast commands
DELETE FROM `acore_string` WHERE `entry` IN (5126, 5127, 5128, 5129, 5130, 5131, 5132, 5133);
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(5126, 'Autobroadcast entries:', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5127, '  ID: {} | Weight: {} | Text: {}', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5128, 'No autobroadcast entries found.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5129, 'Autobroadcast entry added with ID {}.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5130, 'Autobroadcast entry #{} removed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5131, 'Autobroadcast entry #{} not found.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5132, 'Autobroadcast locale ''{}'' added for entry #{}.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(5133, '    Locale: {} | Text: {}', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

