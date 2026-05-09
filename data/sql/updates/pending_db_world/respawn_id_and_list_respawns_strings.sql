--
-- Strings for `respawn id` command and updated `list respawns` command
--
DELETE FROM `acore_string` WHERE `entry` IN (35441, 35442, 35443, 35444, 35445, 35446, 35447);
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(35441, 'Spawn ID {} not found in creature or gameobject data.', '스폰 ID {}를 생물 또는 게임오브젝트 데이터에서 찾을 수 없습니다.', 'L''ID d''apparition {} est introuvable dans les données de créature ou d''objet.', 'Spawn-ID {} wurde in den Kreatur- oder Spielobjektdaten nicht gefunden.', '在生物或游戏对象数据中找不到生成 ID {}。', '在生物或遊戲物件資料中找不到生成 ID {}。', 'El ID de generación {} no se encontró en los datos de criatura u objeto.', 'El ID de generación {} no se encontró en los datos de criatura u objeto.', 'Идентификатор спавна {} не найден в данных существ или объектов.'),
(35442, 'Map {} is not currently loaded.', '맵 {}이(가) 현재 로드되어 있지 않습니다.', 'La carte {} n''est pas actuellement chargée.', 'Karte {} ist derzeit nicht geladen.', '地图 {} 当前未加载。', '地圖 {} 目前未載入。', 'El mapa {} no está cargado actualmente.', 'El mapa {} no está cargado actualmente.', 'Карта {} в данный момент не загружена.'),
(35443, 'Creature (spawn ID {}, entry {}) is already alive.', '생물 (스폰 ID {}, 항목 {})이(가) 이미 살아 있습니다.', 'La créature (ID d''apparition {}, entrée {}) est déjà en vie.', 'Kreatur (Spawn-ID {}, Eintrag {}) ist bereits am Leben.', '生物（生成 ID {}，条目 {}）已经存活。', '生物（生成 ID {}，條目 {}）已經存活。', 'La criatura (ID de generación {}, entrada {}) ya está viva.', 'La criatura (ID de generación {}, entrada {}) ya está viva.', 'Существо (идентификатор спавна {}, запись {}) уже живо.'),
(35444, 'Creature (spawn ID {}, entry {}) queued for respawn.', '생물 (스폰 ID {}, 항목 {})이(가) 리스폰 대기열에 추가되었습니다.', 'La créature (ID d''apparition {}, entrée {}) est mise en file d''attente pour réapparition.', 'Kreatur (Spawn-ID {}, Eintrag {}) für Respawn in Warteschlange.', '生物（生成 ID {}，条目 {}）已加入重生队列。', '生物（生成 ID {}，條目 {}）已加入重生佇列。', 'La criatura (ID de generación {}, entrada {}) está en cola para reaparecer.', 'La criatura (ID de generación {}, entrada {}) está en cola para reaparecer.', 'Существо (идентификатор спавна {}, запись {}) добавлено в очередь на респаун.'),
(35445, 'Gameobject (spawn ID {}, entry {}) is already active.', '게임오브젝트 (스폰 ID {}, 항목 {})이(가) 이미 활성화되어 있습니다.', 'L''objet de jeu (ID d''apparition {}, entrée {}) est déjà actif.', 'Spielobjekt (Spawn-ID {}, Eintrag {}) ist bereits aktiv.', '游戏对象（生成 ID {}，条目 {}）已经激活。', '遊戲物件（生成 ID {}，條目 {}）已經激活。', 'El objeto de juego (ID de generación {}, entrada {}) ya está activo.', 'El objeto de juego (ID de generación {}, entrada {}) ya está activo.', 'Игровой объект (идентификатор спавна {}, запись {}) уже активен.'),
(35446, 'Gameobject (spawn ID {}, entry {}) queued for respawn.', '게임오브젝트 (스폰 ID {}, 항목 {})이(가) 리스폰 대기열에 추가되었습니다.', 'L''objet de jeu (ID d''apparition {}, entrée {}) est mis en file d''attente pour réapparition.', 'Spielobjekt (Spawn-ID {}, Eintrag {}) für Respawn in Warteschlange.', '游戏对象（生成 ID {}，条目 {}）已加入重生队列。', '遊戲物件（生成 ID {}，條目 {}）已加入重生佇列。', 'El objeto de juego (ID de generación {}, entrada {}) está en cola para reaparecer.', 'El objeto de juego (ID de generación {}, entrada {}) está en cola para reaparecer.', 'Игровой объект (идентификатор спавна {}, запись {}) добавлен в очередь на респаун.'),
(35447, 'A map ID is required when using this command from the console.', '콘솔에서 이 명령을 사용하려면 맵 ID가 필요합니다.', 'Un ID de carte est requis lors de l''utilisation de cette commande depuis la console.', 'Eine Karten-ID ist erforderlich, wenn dieser Befehl über die Konsole verwendet wird.', '从控制台使用此命令时需要提供地图 ID。', '從控制台使用此命令時需要提供地圖 ID。', 'Se requiere un ID de mapa al usar este comando desde la consola.', 'Se requiere un ID de mapa al usar este comando desde la consola.', 'При использовании этой команды из консоли требуется ID карты.');

--
-- Update `list respawns` command help text
--
DELETE FROM `command` WHERE `name` = 'list respawns';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('list respawns', 2, 'Syntax: .list respawns [entryId]\r\nIn-game: shows all pending creature and gameobject respawns on the current map, filtered by entryId if provided.\r\nConsole: .list respawns #mapId [#instanceId [#entryId]] - specify map and optional instance ID and entry filter.');

--
-- Register `respawn id` command
--
DELETE FROM `command` WHERE `name` = 'respawn id';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('respawn id', 3, 'Syntax: .respawn id #spawnId\r\nForces a creature or gameobject with the given spawn ID (database GUID) to respawn. Usable from console.');
