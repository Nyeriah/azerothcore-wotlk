--
-- RBAC permission for `respawn id` command (Console::Yes, security level 3 - GM)
--
DELETE FROM `rbac_permissions` WHERE `id` = 916;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(916, 'Command: respawn id');

DELETE FROM `rbac_linked_permissions` WHERE `linkedId` = 916;
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(197, 916);
