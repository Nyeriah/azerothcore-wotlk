--
-- Strings for `respawn creature guid` / `respawn gameobject guid` commands and updated `list respawns` command
--
DELETE FROM `acore_string` WHERE `entry` IN (35441, 35442, 35443, 35444, 35445, 35446, 35447, 35448);
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(35441, 'No creature with spawn GUID {} found.', '스폰 GUID {}를 가진 생물을 찾을 수 없습니다.', 'Aucune créature avec le GUID d''apparition {} trouvée.', 'Keine Kreatur mit Spawn-GUID {} gefunden.', '未找到生成 GUID 为 {} 的生物。', '未找到生成 GUID 為 {} 的生物。', 'No se encontró ninguna criatura con el GUID de generación {}.', 'No se encontró ninguna criatura con el GUID de generación {}.', 'Существо с GUID спавна {} не найдено.'),
(35442, 'Map {} is not currently loaded.', '맵 {}이(가) 현재 로드되어 있지 않습니다.', 'La carte {} n''est pas actuellement chargée.', 'Karte {} ist derzeit nicht geladen.', '地图 {} 当前未加载。', '地圖 {} 目前未載入。', 'El mapa {} no está cargado actualmente.', 'El mapa {} no está cargado actualmente.', 'Карта {} в данный момент не загружена.'),
(35443, 'Creature (spawn GUID {}, entry {}) is already alive.', '생물 (스폰 GUID {}, 항목 {})이(가) 이미 살아 있습니다.', 'La créature (GUID d''apparition {}, entrée {}) est déjà en vie.', 'Kreatur (Spawn-GUID {}, Eintrag {}) ist bereits am Leben.', '生物（生成 GUID {}，条目 {}）已经存活。', '生物（生成 GUID {}，條目 {}）已經存活。', 'La criatura (GUID de generación {}, entrada {}) ya está viva.', 'La criatura (GUID de generación {}, entrada {}) ya está viva.', 'Существо (GUID спавна {}, запись {}) уже живо.'),
(35444, 'Creature (spawn GUID {}, entry {}) queued for respawn.', '생물 (스폰 GUID {}, 항목 {})이(가) 리스폰 대기열에 추가되었습니다.', 'La créature (GUID d''apparition {}, entrée {}) est mise en file d''attente pour réapparition.', 'Kreatur (Spawn-GUID {}, Eintrag {}) für Respawn in Warteschlange.', '生物（生成 GUID {}，条目 {}）已加入重生队列。', '生物（生成 GUID {}，條目 {}）已加入重生佇列。', 'La criatura (GUID de generación {}, entrada {}) está en cola para reaparecer.', 'La criatura (GUID de generación {}, entrada {}) está en cola para reaparecer.', 'Существо (GUID спавна {}, запись {}) добавлено в очередь на респаун.'),
(35445, 'Gameobject (spawn GUID {}, entry {}) is already active.', '게임오브젝트 (스폰 GUID {}, 항목 {})이(가) 이미 활성화되어 있습니다.', 'L''objet de jeu (GUID d''apparition {}, entrée {}) est déjà actif.', 'Spielobjekt (Spawn-GUID {}, Eintrag {}) ist bereits aktiv.', '游戏对象（生成 GUID {}，条目 {}）已经激活。', '遊戲物件（生成 GUID {}，條目 {}）已經激活。', 'El objeto de juego (GUID de generación {}, entrada {}) ya está activo.', 'El objeto de juego (GUID de generación {}, entrada {}) ya está activo.', 'Игровой объект (GUID спавна {}, запись {}) уже активен.'),
(35446, 'Gameobject (spawn GUID {}, entry {}) queued for respawn.', '게임오브젝트 (스폰 GUID {}, 항목 {})이(가) 리스폰 대기열에 추가되었습니다.', 'L''objet de jeu (GUID d''apparition {}, entrée {}) est mis en file d''attente pour réapparition.', 'Spielobjekt (Spawn-GUID {}, Eintrag {}) für Respawn in Warteschlange.', '游戏对象（生成 GUID {}，条目 {}）已加入重生队列。', '遊戲物件（生成 GUID {}，條目 {}）已加入重生佇列。', 'El objeto de juego (GUID de generación {}, entrada {}) está en cola para reaparecer.', 'El objeto de juego (GUID de generación {}, entrada {}) está en cola para reaparecer.', 'Игровой объект (GUID спавна {}, запись {}) добавлен в очередь на респаун.'),
(35447, 'A map ID is required when using this command from the console.', '콘솔에서 이 명령을 사용하려면 맵 ID가 필요합니다.', 'Un ID de carte est requis lors de l''utilisation de cette commande depuis la console.', 'Eine Karten-ID ist erforderlich, wenn dieser Befehl über die Konsole verwendet wird.', '从控制台使用此命令时需要提供地图 ID。', '從控制台使用此命令時需要提供地圖 ID。', 'Se requiere un ID de mapa al usar este comando desde la consola.', 'Se requiere un ID de mapa al usar este comando desde la consola.', 'При использовании этой команды из консоли требуется ID карты.'),
(35448, 'No gameobject with spawn GUID {} found.', '스폰 GUID {}를 가진 게임오브젝트를 찾을 수 없습니다.', 'Aucun objet de jeu avec le GUID d''apparition {} trouvé.', 'Kein Spielobjekt mit Spawn-GUID {} gefunden.', '未找到生成 GUID 为 {} 的游戏对象。', '未找到生成 GUID 為 {} 的遊戲物件。', 'No se encontró ningún objeto de juego con el GUID de generación {}.', 'No se encontró ningún objeto de juego con el GUID de generación {}.', 'Игровой объект с GUID спавна {} не найден.');

--
-- Update `list respawns` command help text
--
DELETE FROM `command` WHERE `name` = 'list respawns';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('list respawns', 2, 'Syntax: .list respawns [entryId]\r\nIn-game: shows all pending creature and gameobject respawns on the current map, filtered by entryId if provided.\r\nConsole: .list respawns #mapId [#instanceId [#entryId]] - specify map and optional instance ID and entry filter.');

--
-- Register `respawn creature guid` and `respawn gameobject guid` commands
--
DELETE FROM `command` WHERE `name` IN ('respawn id', 'respawn guid', 'respawn creature guid', 'respawn gameobject guid');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('respawn creature guid', 3, 'Syntax: .respawn creature guid #spawnGuid\r\nForces the creature with the given spawn GUID (database GUID) to respawn. Usable from console.'),
('respawn gameobject guid', 3, 'Syntax: .respawn gameobject guid #spawnGuid\r\nForces the gameobject with the given spawn GUID (database GUID) to respawn. Usable from console.');
