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

#include "AutobroadcastMgr.h"
#include "Chat.h"
#include "CommandScript.h"
#include "Config.h"
#include "Language.h"

using namespace Acore::ChatCommands;

class autobroadcast_commandscript : public CommandScript
{
public:
    autobroadcast_commandscript() : CommandScript("autobroadcast_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable autobroadcastCommandTable =
        {
            { "list",   HandleAutobroadcastListCommand,   SEC_GAMEMASTER,    Console::Yes },
            { "add",    HandleAutobroadcastAddCommand,    SEC_ADMINISTRATOR, Console::Yes },
            { "locale", HandleAutobroadcastLocaleCommand, SEC_ADMINISTRATOR, Console::Yes },
            { "remove", HandleAutobroadcastRemoveCommand, SEC_ADMINISTRATOR, Console::Yes }
        };

        static ChatCommandTable commandTable =
        {
            { "autobroadcast", autobroadcastCommandTable }
        };

        return commandTable;
    }

    static bool HandleAutobroadcastListCommand(ChatHandler* handler)
    {
        uint32 realmId = sConfigMgr->GetOption<int32>("RealmID", 0);

        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTOBROADCAST);
        stmt->SetData(0, realmId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (!result)
        {
            handler->SendSysMessage(LANG_AUTOBROADCAST_LIST_EMPTY);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->SendSysMessage(LANG_AUTOBROADCAST_LIST_HEADER);

        do
        {
            Field* fields = result->Fetch();
            uint8 id = fields[0].Get<uint8>();
            uint8 weight = fields[1].Get<uint8>();
            std::string text = fields[2].Get<std::string>();

            handler->PSendSysMessage(LANG_AUTOBROADCAST_LIST_ENTRY, id, weight, text);

            // Show locales for this entry
            LoginDatabasePreparedStatement* localeStmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTOBROADCAST_LOCALIZED);
            localeStmt->SetData(0, realmId);
            PreparedQueryResult localeResult = LoginDatabase.Query(localeStmt);

            if (localeResult)
            {
                do
                {
                    Field* localeFields = localeResult->Fetch();
                    uint8 localeId = localeFields[0].Get<uint8>();

                    if (localeId != id)
                        continue;

                    std::string locale = localeFields[1].Get<std::string>();
                    std::string localeText = localeFields[2].Get<std::string>();
                    handler->PSendSysMessage(LANG_AUTOBROADCAST_LOCALE_ENTRY, locale, localeText);
                } while (localeResult->NextRow());
            }
        } while (result->NextRow());

        return true;
    }

    static bool HandleAutobroadcastAddCommand(ChatHandler* handler, uint8 weight, Tail text)
    {
        if (text.empty())
            return false;

        uint32 realmId = sConfigMgr->GetOption<int32>("RealmID", 0);

        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_AUTOBROADCAST);
        stmt->SetData(0, static_cast<int32>(-1));
        stmt->SetData(1, weight);
        stmt->SetData(2, std::string(text));
        LoginDatabase.DirectExecute(stmt);

        // Retrieve the newly inserted ID
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTOBROADCAST);
        stmt->SetData(0, realmId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        uint8 newId = 0;
        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint8 id = fields[0].Get<uint8>();
                if (id > newId)
                    newId = id;
            } while (result->NextRow());
        }

        sAutobroadcastMgr->LoadAutobroadcasts();
        sAutobroadcastMgr->LoadAutobroadcastsLocalized();

        handler->PSendSysMessage(LANG_AUTOBROADCAST_ADD_SUCCESS, newId);
        return true;
    }

    static bool HandleAutobroadcastLocaleCommand(ChatHandler* handler, uint8 id, std::string locale, Tail text)
    {
        if (text.empty())
            return false;

        if (!IsLocaleValid(locale))
        {
            handler->SendErrorMessage("Invalid locale. Valid locales: enUS, koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU.");
            return false;
        }

        uint32 realmId = sConfigMgr->GetOption<int32>("RealmID", 0);

        // Verify the autobroadcast entry exists
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTOBROADCAST);
        stmt->SetData(0, realmId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        bool found = false;
        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                if (fields[0].Get<uint8>() == id)
                {
                    found = true;
                    break;
                }
            } while (result->NextRow());
        }

        if (!found)
        {
            handler->SendErrorMessage(LANG_AUTOBROADCAST_NOT_FOUND, id);
            return false;
        }

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_AUTOBROADCAST_LOCALE);
        stmt->SetData(0, static_cast<int32>(-1));
        stmt->SetData(1, static_cast<int32>(id));
        stmt->SetData(2, locale);
        stmt->SetData(3, std::string(text));
        LoginDatabase.DirectExecute(stmt);

        sAutobroadcastMgr->LoadAutobroadcasts();
        sAutobroadcastMgr->LoadAutobroadcastsLocalized();

        handler->PSendSysMessage(LANG_AUTOBROADCAST_LOCALE_SUCCESS, locale, id);
        return true;
    }

    static bool HandleAutobroadcastRemoveCommand(ChatHandler* handler, uint8 id)
    {
        uint32 realmId = sConfigMgr->GetOption<int32>("RealmID", 0);

        // Verify the autobroadcast entry exists
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTOBROADCAST);
        stmt->SetData(0, realmId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        bool found = false;
        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                if (fields[0].Get<uint8>() == id)
                {
                    found = true;
                    break;
                }
            } while (result->NextRow());
        }

        if (!found)
        {
            handler->SendErrorMessage(LANG_AUTOBROADCAST_NOT_FOUND, id);
            return false;
        }

        // Delete the autobroadcast entry
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_AUTOBROADCAST);
        stmt->SetData(0, id);
        stmt->SetData(1, static_cast<int32>(realmId));
        LoginDatabase.DirectExecute(stmt);

        // Delete associated locale entries
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_AUTOBROADCAST_LOCALE);
        stmt->SetData(0, static_cast<int32>(id));
        stmt->SetData(1, static_cast<int32>(realmId));
        LoginDatabase.DirectExecute(stmt);

        sAutobroadcastMgr->LoadAutobroadcasts();
        sAutobroadcastMgr->LoadAutobroadcastsLocalized();

        handler->PSendSysMessage(LANG_AUTOBROADCAST_REMOVE_SUCCESS, id);
        return true;
    }
};

void AddSC_autobroadcast_commandscript()
{
    new autobroadcast_commandscript();
}
