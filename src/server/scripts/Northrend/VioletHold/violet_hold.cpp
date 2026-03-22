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

#include "violet_hold.h"
#include "CreatureScript.h"
#include "GameObjectScript.h"
#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"

/// @todo: Missing Sinclari Trigger announcements (32204) Look at its creature_text for more info.

enum Texts
{
    GOSSIP_MENU_START_1         = 9997,
    GOSSIP_MENU_START_2         = 9998,
    GOSSIP_MENU_LATE_JOIN       = 10275,

    NPC_TEXT_SINCLARI_IN        = 13853,
    NPC_TEXT_SINCLARI_START     = 13854,
    NPC_TEXT_SINCLARI_DONE      = 13910,
    NPC_TEXT_SINCLARI_LATE_JOIN = 14271,
};

/***********
** DEFENSE SYSTEM CRYSTAL
***********/

class go_vh_activation_crystal : public GameObjectScript
{
public:
    go_vh_activation_crystal() : GameObjectScript("go_vh_activation_crystal") { }

    bool OnGossipHello(Player*  /*player*/, GameObject* go) override
    {
        if (InstanceScript* Instance = go->GetInstanceScript())
        {
            Instance->SetData(DATA_ACTIVATE_DEFENSE_SYSTEM, 1);
            go->SetGameObjectFlag(GO_FLAG_NOT_SELECTABLE);
        }

        return true;
    }
};

/***********
** SINCLARI
***********/

class npc_vh_sinclari : public CreatureScript
{
public:
    npc_vh_sinclari() : CreatureScript("npc_vh_sinclari") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (InstanceScript* Instance = creature->GetInstanceScript())
            switch (Instance->GetData(DATA_ENCOUNTER_STATUS))
            {
                case NOT_STARTED:
                    AddGossipItemFor(player, GOSSIP_MENU_START_1, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    SendGossipMenuFor(player, NPC_TEXT_SINCLARI_IN, creature->GetGUID());
                    break;
                case IN_PROGRESS:
                    AddGossipItemFor(player, GOSSIP_MENU_LATE_JOIN, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    SendGossipMenuFor(player, NPC_TEXT_SINCLARI_LATE_JOIN, creature->GetGUID());
                    break;
                default:
                    SendGossipMenuFor(player, NPC_TEXT_SINCLARI_DONE, creature->GetGUID());
            }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                AddGossipItemFor(player, GOSSIP_MENU_START_2, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                SendGossipMenuFor(player, NPC_TEXT_SINCLARI_START, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                CloseGossipMenuFor(player);
                if (InstanceScript* Instance = creature->GetInstanceScript())
                    Instance->SetData(DATA_START_INSTANCE, 1);
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->NearTeleportTo(playerTeleportPosition.GetPositionX(), playerTeleportPosition.GetPositionY(), playerTeleportPosition.GetPositionZ(), playerTeleportPosition.GetOrientation(), true);
                CloseGossipMenuFor(player);
                break;
        }
        return true;
    }
};

/***********
** TELEPORTATION PORTAL
***********/

enum PortalEvents
{
    EVENT_SUMMON_KEEPER_OR_GUARDIAN = 1,
    EVENT_SUMMON_KEEPER_TRASH,
    EVENT_SUMMON_ELITES,
    EVENT_SUMMON_SABOTEOUR,
    EVENT_CHECK_DEATHS,
};

struct npc_vh_teleportation_portalAI : public NullCreatureAI
{
    npc_vh_teleportation_portalAI(Creature* c) : NullCreatureAI(c), listOfMobs(me)
    {
        Instance = c->GetInstanceScript();
        events.Reset();
        Wave = Instance->GetData(DATA_WAVE_COUNT);
        IsKeeperOrGuardian = false;
        Spawned = false;

        if (Wave < 12)
            AddValue = 0;
        else
            AddValue = 1;

        if (Wave % 6 != 0)
            events.RescheduleEvent(RAND(EVENT_SUMMON_KEEPER_OR_GUARDIAN, EVENT_SUMMON_ELITES), 10s);
        else
            events.RescheduleEvent(EVENT_SUMMON_SABOTEOUR, 3s);
    }

    InstanceScript* Instance;
    SummonList listOfMobs;
    EventMap events;
    uint8 Wave;
    uint8 AddValue;
    bool IsKeeperOrGuardian;
    bool Spawned;

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_SUMMON_KEEPER_OR_GUARDIAN:
                IsKeeperOrGuardian = true;
                Spawned = true;
                if (Creature* c = DoSummon(RAND(NPC_PORTAL_GUARDIAN, NPC_PORTAL_KEEPER_1, NPC_PORTAL_KEEPER_2), me, 2.0f, 0, TEMPSUMMON_DEAD_DESPAWN))
                    me->CastSpell(c, SPELL_PORTAL_CHANNEL, false);
                events.RescheduleEvent(EVENT_SUMMON_KEEPER_TRASH, 20s);
                break;
            case EVENT_SUMMON_KEEPER_TRASH:
                for (uint8 i = 0; i < 3 + AddValue; ++i)
                {
                    uint32 entry = RAND(NPC_AZURE_INVADER_1, NPC_AZURE_INVADER_2, NPC_AZURE_SPELLBREAKER_1, NPC_AZURE_SPELLBREAKER_2, NPC_AZURE_MAGE_SLAYER_1, NPC_AZURE_MAGE_SLAYER_2, NPC_AZURE_BINDER_1, NPC_AZURE_BINDER_2);
                    DoSummon(entry, me, 2.0f, 20000, TEMPSUMMON_DEAD_DESPAWN);
                }
                events.Repeat(20s);
                break;
            case EVENT_SUMMON_ELITES:
                Spawned = true;
                for (uint8 i = 0; i < 2 + AddValue; ++i)
                {
                    uint32 entry = RAND(NPC_AZURE_CAPTAIN, NPC_AZURE_RAIDER, NPC_AZURE_STALKER, NPC_AZURE_SORCEROR);
                    DoSummon(entry, me, 2.0f, 20000, TEMPSUMMON_DEAD_DESPAWN);
                }
                me->SetVisible(false);
                break;
            case EVENT_SUMMON_SABOTEOUR:
                DoSummon(NPC_SABOTEOUR, me, 2.0f, 0, TEMPSUMMON_CORPSE_DESPAWN);
                me->DespawnOrUnsummon(3s);
                break;
        }

        if (!Spawned)
            return;

        if (IsKeeperOrGuardian)
        {
            if (!me->IsNonMeleeSpellCast(false))
            {
                for (SummonList::iterator itr = listOfMobs.begin(); itr != listOfMobs.end(); ++itr)
                    if (Creature* c = Instance->instance->GetCreature(*itr))
                            if (c->IsAlive() && c->EntryEquals(NPC_PORTAL_GUARDIAN, NPC_PORTAL_KEEPER_1, NPC_PORTAL_KEEPER_2))
                            {
                                me->CastSpell(c, SPELL_PORTAL_CHANNEL, false);
                                return;
                            }

                Unit::Kill(me, me, false);
            }
        }
        else
        {
            if (listOfMobs.empty())
                Unit::Kill(me, me, false);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        events.Reset();
        if (Wave % 6 == 0)
            return;
        Instance->SetData(DATA_PORTAL_DEFEATED, 0);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned)
        {
            listOfMobs.Summon(summoned);
            Instance->SetGuidData(DATA_ADD_TRASH_MOB, summoned->GetGUID());
        }
    }

    void SummonedMobDied(Creature* summoned)
    {
        if (summoned)
        {
            listOfMobs.Despawn(summoned);
            Instance->SetGuidData(DATA_DELETE_TRASH_MOB, summoned->GetGUID());
        }
    }
};

/***********
** GENERAL TRASH AI
***********/

struct violet_hold_trashAI : public npc_escortAI
{
    violet_hold_trashAI(Creature* c) : npc_escortAI(c)
    {
        Instance = c->GetInstanceScript();
        PortalLoc = Instance->GetData(DATA_PORTAL_LOCATION);
        AddedWaypoints = false;
        UseAlternate = false;
    }

    InstanceScript* Instance;
    bool AddedWaypoints;
    uint32 PortalLoc;
    bool UseAlternate;

    void ClearDoorSealAura()
    {
        if (Creature* c = Instance->GetCreature(DATA_DOOR_SEAL))
            c->RemoveAura(SPELL_DESTROY_DOOR_SEAL, me->GetGUID());
    }

    void JustEngagedWith(Unit* who) override
    {
        if (!who->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE))
        {
            me->InterruptNonMeleeSpells(false);
            me->SetImmuneToNPC(false);
        }
    }

    void AttackStart(Unit* who) override
    {
        if (!who->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE))
            ScriptedAI::AttackStart(who);
    }

    void JustReachedHome() override
    {
        CreatureStartAttackDoor();
    }

    void WaypointReached(uint32 id) override
    {
        if (PortalLoc < 6)
            if (id == uint16(PortalLocWPCount[PortalLoc] - 1 - (UseAlternate ? 1 : 0)))
                CreatureStartAttackDoor();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!AddedWaypoints)
        {
            AddedWaypoints = true;
            switch (PortalLoc)
            {
                case 0:
                    for (int i = 0; i < 6; i++)
                        AddWaypoint(i, FirstPortalTrashWPs[i][0] + irand(-1, 1), FirstPortalTrashWPs[i][1] + irand(-1, 1), FirstPortalTrashWPs[i][2] + irand(-1, 1), 0);
                    me->SetHomePosition(FirstPortalTrashWPs[5][0], FirstPortalTrashWPs[5][1], FirstPortalTrashWPs[5][2], 3.149439f);
                    break;
                case 1:
                    UseAlternate = (bool)urand(0, 1);
                    if (!UseAlternate)
                    {
                        for (int i = 0; i < 9; i++)
                            AddWaypoint(i, SecondPortalTrashWPs1[i][0] + irand(-1, 1), SecondPortalTrashWPs1[i][1] + irand(-1, 1), SecondPortalTrashWPs1[i][2], 0);
                        me->SetHomePosition(SecondPortalTrashWPs1[8][0] + irand(-1, 1), SecondPortalTrashWPs1[8][1] + irand(-1, 1), SecondPortalTrashWPs1[8][2] + irand(-1, 1), 3.149439f);
                    }
                    else
                    {
                        for (int i = 0; i < 8; i++)
                            AddWaypoint(i, SecondPortalTrashWPs2[i][0] + irand(-1, 1), SecondPortalTrashWPs2[i][1] + irand(-1, 1), SecondPortalTrashWPs2[i][2], 0);
                        me->SetHomePosition(SecondPortalTrashWPs2[7][0], SecondPortalTrashWPs2[7][1], SecondPortalTrashWPs2[7][2], 3.149439f);
                    }
                    break;
                case 2:
                    for (int i = 0; i < 8; i++)
                        AddWaypoint(i, ThirdPortalTrashWPs[i][0] + irand(-1, 1), ThirdPortalTrashWPs[i][1] + irand(-1, 1), ThirdPortalTrashWPs[i][2], 0);
                    me->SetHomePosition(ThirdPortalTrashWPs[7][0], ThirdPortalTrashWPs[7][1], ThirdPortalTrashWPs[7][2], 3.149439f);
                    break;
                case 3:
                    for (int i = 0; i < 9; i++)
                        AddWaypoint(i, FourthPortalTrashWPs[i][0] + irand(-1, 1), FourthPortalTrashWPs[i][1] + irand(-1, 1), FourthPortalTrashWPs[i][2], 0);
                    me->SetHomePosition(FourthPortalTrashWPs[8][0], FourthPortalTrashWPs[8][1], FourthPortalTrashWPs[8][2], 3.149439f);
                    break;
                case 4:
                    for (int i = 0; i < 6; i++)
                        AddWaypoint(i, FifthPortalTrashWPs[i][0] + irand(-1, 1), FifthPortalTrashWPs[i][1] + irand(-1, 1), FifthPortalTrashWPs[i][2], 0);
                    me->SetHomePosition(FifthPortalTrashWPs[5][0], FifthPortalTrashWPs[5][1], FifthPortalTrashWPs[5][2], 3.149439f);
                    break;
                case 5:
                    for (int i = 0; i < 4; i++)
                        AddWaypoint(i, SixthPoralTrashWPs[i][0] + irand(-1, 1), SixthPoralTrashWPs[i][1] + irand(-1, 1), SixthPoralTrashWPs[i][2], 0);
                    me->SetHomePosition(SixthPoralTrashWPs[3][0], SixthPoralTrashWPs[3][1], SixthPoralTrashWPs[3][2], 3.149439f);
                    break;
            }
            SetDespawnAtEnd(false);
            Start(true);
        }

        npc_escortAI::UpdateAI(diff);
    }

    void JustDied(Unit* /*unit*/) override
    {
        if (Creature* portal = Instance->GetCreature(DATA_TELEPORTATION_PORTAL))
            CAST_AI(npc_vh_teleportation_portalAI, portal->AI())->SummonedMobDied(me);
    }

    void CreatureStartAttackDoor()
    {
        RemoveEscortState(STATE_ESCORT_ESCORTING | STATE_ESCORT_RETURNING | STATE_ESCORT_PAUSED);
        me->SetImmuneToNPC(true);
        me->CastSpell((Unit*)nullptr, SPELL_DESTROY_DOOR_SEAL, true);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            me->SetImmuneToNPC(false);
            me->SetHomePosition(1845.577759f + rand_norm() * 5 - 2.5f, 800.681152f + rand_norm() * 5 - 2.5f, 44.104248f, M_PI);
        }

        me->GetThreatMgr().ClearAllThreat();
        me->CombatStop(true);
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            AddEscortState(STATE_ESCORT_RETURNING);
            ReturnToLastPoint();
        }
        else
        {
            me->GetMotionMaster()->MoveTargetedHome();
            Reset();
        }
        me->ClearUnitState(UNIT_STATE_EVADE);
    }
};

/***********
** TRASH SPELLS
***********/

enum AzureInvaderSpells
{
    SPELL_CLEAVE        = 15496,
    SPELL_IMPALE        = 58459,
    SPELL_BRUTAL_STRIKE = 58460,
    SPELL_SUNDER_ARMOR  = 58461,
};

enum AzureSpellbreakerSpells
{
    SPELL_ARCANE_BLAST  = 58462,
    SPELL_SLOW          = 25603,
    SPELL_CHAINS_OF_ICE = 58464,
    SPELL_CONE_OF_COLD  = 58463,
};

enum AzureBinderSpells
{
    SPELL_ARCANE_BARRAGE   = 58456,
    SPELL_ARCANE_EXPLOSION = 58455,
    SPELL_FROST_NOVA       = 58458,
    SPELL_FROSTBOLT        = 58457,
};

enum AzureMageSlayerSpells
{
    SPELL_ARCANE_EMPOWERMENT = 58469,
    SPELL_SPELL_LOCK         = 30849
};

enum AzureCaptainSpells
{
    SPELL_MORTAL_STRIKE      = 32736,
    SPELL_WHIRLWIND_OF_STEEL = 41056
};

enum AzureSorcerorSpells
{
    SPELL_ARCANE_STREAM   = 60181,
    SPELL_MANA_DETONATION = 60182,
};

enum AzureRaiderSpells
{
    SPELL_CONCUSSION_BLOW  = 52719,
    SPELL_MAGIC_REFLECTION = 60158
};

enum AzureStalkerSpells
{
    SPELL_BACKSTAB       = 58471,
    SPELL_TACTICAL_BLINK = 58470
};

/***********
** TRASH — AZURE INVADER
***********/

enum InvaderEvents
{
    EVENT_INVADER_CLEAVE = 1,
    EVENT_INVADER_IMPALE,
    EVENT_INVADER_BRUTAL_STRIKE,
    EVENT_INVADER_SUNDER_ARMOR,
};

struct npc_azure_invaderAI : public violet_hold_trashAI
{
    npc_azure_invaderAI(Creature* c) : violet_hold_trashAI(c) {}

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.RescheduleEvent(EVENT_INVADER_CLEAVE, 5s);
        events.RescheduleEvent(EVENT_INVADER_IMPALE, 4s);
        events.RescheduleEvent(EVENT_INVADER_BRUTAL_STRIKE, 5s);
        events.RescheduleEvent(EVENT_INVADER_SUNDER_ARMOR, 4s);
    }

    void UpdateAI(uint32 diff) override
    {
        violet_hold_trashAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_INVADER_CLEAVE:
                if (me->GetEntry() == NPC_AZURE_INVADER_1)
                    DoCast(me->GetVictim(), SPELL_CLEAVE);
                events.Repeat(5s);
                break;
            case EVENT_INVADER_IMPALE:
                if (me->GetEntry() == NPC_AZURE_INVADER_1)
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 5.0f, true))
                        DoCast(target, SPELL_IMPALE);
                events.Repeat(4s);
                break;
            case EVENT_INVADER_BRUTAL_STRIKE:
                if (me->GetEntry() == NPC_AZURE_INVADER_2)
                    DoCast(me->GetVictim(), SPELL_BRUTAL_STRIKE);
                events.Repeat(5s);
                break;
            case EVENT_INVADER_SUNDER_ARMOR:
                if (me->GetEntry() == NPC_AZURE_INVADER_2)
                    DoCast(me->GetVictim(), SPELL_SUNDER_ARMOR);
                events.Repeat(8s, 10s);
                break;
        }

        DoMeleeAttackIfReady();
    }
};

/***********
** TRASH — AZURE BINDER
***********/

enum BinderEvents
{
    EVENT_BINDER_ARCANE_EXPLOSION = 1,
    EVENT_BINDER_ARCANE_BARRAGE,
    EVENT_BINDER_FROST_NOVA,
    EVENT_BINDER_FROSTBOLT,
};

struct npc_azure_binderAI : public violet_hold_trashAI
{
    npc_azure_binderAI(Creature* c) : violet_hold_trashAI(c) {}

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.RescheduleEvent(EVENT_BINDER_ARCANE_EXPLOSION, 5s);
        events.RescheduleEvent(EVENT_BINDER_ARCANE_BARRAGE, 4s);
        events.RescheduleEvent(EVENT_BINDER_FROST_NOVA, 5s);
        events.RescheduleEvent(EVENT_BINDER_FROSTBOLT, 4s);
    }

    void UpdateAI(uint32 diff) override
    {
        violet_hold_trashAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_BINDER_ARCANE_EXPLOSION:
                if (me->GetEntry() == NPC_AZURE_BINDER_1)
                    DoCast(SPELL_ARCANE_EXPLOSION);
                events.Repeat(5s);
                break;
            case EVENT_BINDER_ARCANE_BARRAGE:
                if (me->GetEntry() == NPC_AZURE_BINDER_1)
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true))
                        DoCast(target, SPELL_ARCANE_BARRAGE);
                events.Repeat(6s);
                break;
            case EVENT_BINDER_FROST_NOVA:
                if (me->GetEntry() == NPC_AZURE_BINDER_2)
                    DoCast(SPELL_FROST_NOVA);
                events.Repeat(5s);
                break;
            case EVENT_BINDER_FROSTBOLT:
                if (me->GetEntry() == NPC_AZURE_BINDER_2)
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 40.0f, true))
                        DoCast(target, SPELL_FROSTBOLT);
                events.Repeat(6s);
                break;
        }

        DoMeleeAttackIfReady();
    }
};

/***********
** TRASH — AZURE MAGE SLAYER
***********/

enum MageSlayerEvents
{
    EVENT_MAGE_SLAYER_ARCANE_EMPOWERMENT = 1,
    EVENT_MAGE_SLAYER_SPELL_LOCK,
};

struct npc_azure_mage_slayerAI : public violet_hold_trashAI
{
    npc_azure_mage_slayerAI(Creature* c) : violet_hold_trashAI(c) {}

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.RescheduleEvent(EVENT_MAGE_SLAYER_ARCANE_EMPOWERMENT, 5s);
        events.RescheduleEvent(EVENT_MAGE_SLAYER_SPELL_LOCK, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        violet_hold_trashAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_MAGE_SLAYER_ARCANE_EMPOWERMENT:
                if (me->GetEntry() == NPC_AZURE_MAGE_SLAYER_1)
                    DoCast(me, SPELL_ARCANE_EMPOWERMENT);
                events.Repeat(14s);
                break;
            case EVENT_MAGE_SLAYER_SPELL_LOCK:
                if (me->GetEntry() == NPC_AZURE_MAGE_SLAYER_2)
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true))
                        DoCast(target, SPELL_SPELL_LOCK);
                events.Repeat(9s);
                break;
        }

        DoMeleeAttackIfReady();
    }
};

/***********
** TRASH — AZURE RAIDER
***********/

enum RaiderEvents
{
    EVENT_RAIDER_CONCUSSION_BLOW = 1,
    EVENT_RAIDER_MAGIC_REFLECTION,
};

struct npc_azure_raiderAI : public violet_hold_trashAI
{
    npc_azure_raiderAI(Creature* c) : violet_hold_trashAI(c) {}

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.RescheduleEvent(EVENT_RAIDER_CONCUSSION_BLOW, 5s);
        events.RescheduleEvent(EVENT_RAIDER_MAGIC_REFLECTION, 8s);
    }

    void UpdateAI(uint32 diff) override
    {
        violet_hold_trashAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_RAIDER_CONCUSSION_BLOW:
                DoCast(me->GetVictim(), SPELL_CONCUSSION_BLOW);
                events.Repeat(5s);
                break;
            case EVENT_RAIDER_MAGIC_REFLECTION:
                DoCast(SPELL_MAGIC_REFLECTION);
                events.Repeat(10s, 15s);
                break;
        }

        DoMeleeAttackIfReady();
    }
};

/***********
** TRASH — AZURE STALKER
***********/

enum StalkerEvents
{
    EVENT_STALKER_BACKSTAB = 1,
};

struct npc_azure_stalkerAI : public violet_hold_trashAI
{
    npc_azure_stalkerAI(Creature* c) : violet_hold_trashAI(c) {}

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.RescheduleEvent(EVENT_STALKER_BACKSTAB, 1300ms);
    }

    void UpdateAI(uint32 diff) override
    {
        violet_hold_trashAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (events.ExecuteEvent() == EVENT_STALKER_BACKSTAB)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxDistance, 0, 5.0f, true))
                if (!target->HasInArc(M_PI, me))
                    DoCast(target, SPELL_BACKSTAB);
            events.Repeat(4s);
        }

        DoMeleeAttackIfReady();
    }
};

/***********
** TRASH — AZURE SPELLBREAKER
***********/

enum SpellbreakerEvents
{
    EVENT_SPELLBREAKER_ARCANE_BLAST = 1,
    EVENT_SPELLBREAKER_SLOW,
    EVENT_SPELLBREAKER_CHAINS_OF_ICE,
    EVENT_SPELLBREAKER_CONE_OF_COLD,
};

struct npc_azure_spellbreakerAI : public violet_hold_trashAI
{
    npc_azure_spellbreakerAI(Creature* c) : violet_hold_trashAI(c) {}

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.RescheduleEvent(EVENT_SPELLBREAKER_ARCANE_BLAST, 5s);
        events.RescheduleEvent(EVENT_SPELLBREAKER_SLOW, 4s);
        events.RescheduleEvent(EVENT_SPELLBREAKER_CHAINS_OF_ICE, 5s);
        events.RescheduleEvent(EVENT_SPELLBREAKER_CONE_OF_COLD, 4s);
    }

    void UpdateAI(uint32 diff) override
    {
        violet_hold_trashAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_SPELLBREAKER_ARCANE_BLAST:
                if (me->GetEntry() == NPC_AZURE_SPELLBREAKER_1)
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true))
                        DoCast(target, SPELL_ARCANE_BLAST);
                events.Repeat(6s);
                break;
            case EVENT_SPELLBREAKER_SLOW:
                if (me->GetEntry() == NPC_AZURE_SPELLBREAKER_1)
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true))
                        DoCast(target, SPELL_SLOW);
                events.Repeat(5s);
                break;
            case EVENT_SPELLBREAKER_CHAINS_OF_ICE:
                if (me->GetEntry() == NPC_AZURE_SPELLBREAKER_2)
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true))
                        DoCast(target, SPELL_CHAINS_OF_ICE);
                events.Repeat(7s);
                break;
            case EVENT_SPELLBREAKER_CONE_OF_COLD:
                if (me->GetEntry() == NPC_AZURE_SPELLBREAKER_2)
                    DoCast(SPELL_CONE_OF_COLD);
                events.Repeat(5s);
                break;
        }

        DoMeleeAttackIfReady();
    }
};

/***********
** TRASH — AZURE CAPTAIN
***********/

enum CaptainEvents
{
    EVENT_CAPTAIN_MORTAL_STRIKE = 1,
    EVENT_CAPTAIN_WHIRLWIND,
};

struct npc_azure_captainAI : public violet_hold_trashAI
{
    npc_azure_captainAI(Creature* c) : violet_hold_trashAI(c) {}

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.RescheduleEvent(EVENT_CAPTAIN_MORTAL_STRIKE, 5s);
        events.RescheduleEvent(EVENT_CAPTAIN_WHIRLWIND, 8s);
    }

    void UpdateAI(uint32 diff) override
    {
        violet_hold_trashAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_CAPTAIN_MORTAL_STRIKE:
                DoCast(me->GetVictim(), SPELL_MORTAL_STRIKE);
                events.Repeat(5s);
                break;
            case EVENT_CAPTAIN_WHIRLWIND:
                DoCastAOE(SPELL_WHIRLWIND_OF_STEEL);
                events.Repeat(8s);
                break;
        }

        DoMeleeAttackIfReady();
    }
};

/***********
** TRASH — AZURE SORCEROR
***********/

enum SorcerorEvents
{
    EVENT_SORCEROR_ARCANE_STREAM = 1,
    EVENT_SORCEROR_MANA_DETONATION,
};

struct npc_azure_sorcerorAI : public violet_hold_trashAI
{
    npc_azure_sorcerorAI(Creature* c) : violet_hold_trashAI(c) {}

    EventMap events;
    uint32 ArcaneStreamTimerStartingValue;

    void Reset() override
    {
        events.Reset();
        events.RescheduleEvent(EVENT_SORCEROR_ARCANE_STREAM, 4s);
        ArcaneStreamTimerStartingValue = 4000;
        events.RescheduleEvent(EVENT_SORCEROR_MANA_DETONATION, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        violet_hold_trashAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_SORCEROR_ARCANE_STREAM:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 35.0f, true))
                    DoCast(target, SPELL_ARCANE_STREAM);
                {
                    uint32 repeatTime = urand(5000, 10000);
                    ArcaneStreamTimerStartingValue = repeatTime;
                    events.Repeat(Milliseconds(repeatTime));
                }
                break;
            case EVENT_SORCEROR_MANA_DETONATION:
                DoCastAOE(SPELL_MANA_DETONATION);
                events.Repeat(2s, 6s);
                break;
        }

        DoMeleeAttackIfReady();
    }
};

/***********
** SABOTEUR
***********/

enum AzureSaboteurSpells
{
    SABOTEUR_SHIELD_DISRUPTION = 58291,
    SABOTEUR_SHIELD_EFFECT = 45775,
    SPELL_TELEPORT_VISUAL = 52096,
};

enum SaboteurEvents
{
    EVENT_SABOTEUR_SHIELD_DISRUPTION = 1,
    EVENT_SABOTEUR_RELEASE_BOSS,
    EVENT_SABOTEUR_DISAPPEAR,
};

struct npc_azure_saboteurAI : public npc_escortAI
{
    npc_azure_saboteurAI(Creature* c) : npc_escortAI(c)
    {
        Instance = c->GetInstanceScript();
        Boss = Instance->GetData(DATA_WAVE_COUNT) == 6
            ? Instance->GetPersistentData(PERSISTENT_DATA_FIRST_BOSS)
            : Instance->GetPersistentData(PERSISTENT_DATA_SECOND_BOSS);
        AddedWaypoints = false;
        IsOpening = false;
    }

    InstanceScript* Instance;
    bool AddedWaypoints;
    uint8 Boss;
    bool IsOpening;
    EventMap events;
    uint8 Count;

    void WaypointReached(uint32 waypointId) override
    {
        switch (Boss)
        {
            case 1:
                if (waypointId == 2)
                    FinishPointReached();
                break;
            case 2:
                if (waypointId == 2)
                    FinishPointReached();
                break;
            case 3:
                if (waypointId == 1)
                    FinishPointReached();
                break;
            case 4:
                if (waypointId == 0)
                    FinishPointReached();
                break;
            case 5:
                if (waypointId == 0)
                    FinishPointReached();
                break;
            case 6:
                if (waypointId == 4)
                    FinishPointReached();
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        npc_escortAI::UpdateAI(diff);

        if (!AddedWaypoints)
        {
            AddedWaypoints = true;
            switch (Boss)
            {
                case 1:
                    for (int i = 0; i < 3; i++)
                        AddWaypoint(i, SaboteurFinalPos1[i][0], SaboteurFinalPos1[i][1], SaboteurFinalPos1[i][2], 0);
                    me->SetHomePosition(SaboteurFinalPos1[2][0], SaboteurFinalPos1[2][1], SaboteurFinalPos1[2][2], 4.762346f);
                    break;
                case 2:
                    for (int i = 0; i < 3; i++)
                        AddWaypoint(i, SaboteurFinalPos2[i][0], SaboteurFinalPos2[i][1], SaboteurFinalPos2[i][2], 0);
                    me->SetHomePosition(SaboteurFinalPos2[2][0], SaboteurFinalPos2[2][1], SaboteurFinalPos2[2][2], 1.862674f);
                    break;
                case 3:
                    for (int i = 0; i < 2; i++)
                        AddWaypoint(i, SaboteurFinalPos3[i][0], SaboteurFinalPos3[i][1], SaboteurFinalPos3[i][2], 0);
                    me->SetHomePosition(SaboteurFinalPos3[1][0], SaboteurFinalPos3[1][1], SaboteurFinalPos3[1][2], 5.500638f);
                    break;
                case 4:
                    AddWaypoint(0, SaboteurFinalPos4[0], SaboteurFinalPos4[1], SaboteurFinalPos4[2], 0);
                    me->SetHomePosition(SaboteurFinalPos4[0], SaboteurFinalPos4[1], SaboteurFinalPos4[2], 3.991108f);
                    break;
                case 5:
                    AddWaypoint(0, SaboteurFinalPos5[0], SaboteurFinalPos5[1], SaboteurFinalPos5[2], 0);
                    me->SetHomePosition(SaboteurFinalPos5[0], SaboteurFinalPos5[1], SaboteurFinalPos5[2], 1.100841f);
                    break;
                case 6:
                    for (int i = 0; i < 5; i++)
                        AddWaypoint(i, SaboteurFinalPos6[i][0], SaboteurFinalPos6[i][1], SaboteurFinalPos6[i][2], 0);
                    me->SetHomePosition(SaboteurFinalPos6[4][0], SaboteurFinalPos6[4][1], SaboteurFinalPos6[4][2], 0.983031f);
                    break;
            }
            SetDespawnAtEnd(false);
            Start(true);
        }

        if (IsOpening)
        {
            events.Update(diff);
            switch (events.ExecuteEvent())
            {
                case EVENT_SABOTEUR_SHIELD_DISRUPTION:
                    me->CastSpell(me, SABOTEUR_SHIELD_DISRUPTION, false);
                    ++Count;
                    if (Count < 3)
                        events.RescheduleEvent(EVENT_SABOTEUR_SHIELD_DISRUPTION, 1s);
                    else
                    {
                        Instance->SetData(DATA_RELEASE_BOSS, 0);
                        events.RescheduleEvent(EVENT_SABOTEUR_DISAPPEAR, 500ms);
                    }
                    break;
                case EVENT_SABOTEUR_DISAPPEAR:
                    IsOpening = false;
                    me->SetUnitFlag(UNIT_FLAG_NOT_SELECTABLE);
                    me->SetDisplayId(11686);
                    me->CastSpell(me, SPELL_TELEPORT_VISUAL, true);
                    me->DespawnOrUnsummon(1s);
                    break;
            }
        }
    }

    void FinishPointReached()
    {
        IsOpening = true;
        Count = 0;
        me->CastSpell(me, SABOTEUR_SHIELD_DISRUPTION, false);
        events.RescheduleEvent(EVENT_SABOTEUR_SHIELD_DISRUPTION, 1s);
    }

    void MoveInLineOfSight(Unit* /*who*/) override {}
};

/***********
** DESTROY DOOR SEAL SPELL SCRIPT
***********/

class spell_destroy_door_seal_aura : public AuraScript
{
    PrepareAuraScript(spell_destroy_door_seal_aura);

    void HandleEffectPeriodic(AuraEffect const*   /*aurEff*/)
    {
        PreventDefaultAction();
        if (Unit* target = GetTarget())
            if (InstanceScript* Instance = target->GetInstanceScript())
                Instance->SetData(DATA_DECREASE_DOOR_HEALTH, 0);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_destroy_door_seal_aura::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

struct npc_violet_hold_defense_system : public ScriptedAI
{
    npc_violet_hold_defense_system(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        DoCast(RAND(SPELL_DEFENSE_SYSTEM_SPAWN_EFFECT, SPELL_DEFENSE_SYSTEM_VISUAL));
        events.ScheduleEvent(EVENT_ARCANE_LIGHTNING, 4s);
        events.ScheduleEvent(EVENT_ARCANE_LIGHTNING_INSTAKILL, 4s);
        me->DespawnOrUnsummon(7s, 0s);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_ARCANE_LIGHTNING:
                DoCastAOE(RAND(SPELL_ARCANE_LIGHTNING, SPELL_ARCANE_LIGHTNING_VISUAL));
                events.Repeat(2s);
                break;
            case EVENT_ARCANE_LIGHTNING_INSTAKILL:
                DoCastAOE(SPELL_ARCANE_LIGHTNING_INSTAKILL);
                events.Repeat(1s);
                break;
        }
    }
};

void AddSC_violet_hold()
{
    new go_vh_activation_crystal();
    new npc_vh_sinclari();

    RegisterVioletHoldCreatureAI(npc_vh_teleportation_portalAI);
    RegisterVioletHoldCreatureAI(npc_azure_saboteurAI);

    RegisterVioletHoldCreatureAI(npc_azure_invaderAI);
    RegisterVioletHoldCreatureAI(npc_azure_spellbreakerAI);
    RegisterVioletHoldCreatureAI(npc_azure_binderAI);
    RegisterVioletHoldCreatureAI(npc_azure_mage_slayerAI);
    RegisterVioletHoldCreatureAI(npc_azure_captainAI);
    RegisterVioletHoldCreatureAI(npc_azure_sorcerorAI);
    RegisterVioletHoldCreatureAI(npc_azure_raiderAI);
    RegisterVioletHoldCreatureAI(npc_azure_stalkerAI);

    RegisterSpellScript(spell_destroy_door_seal_aura);
    RegisterCreatureAI(npc_violet_hold_defense_system);
}
