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

#include "CreatureScript.h"
#include "ScriptedCreature.h"
#include "violet_hold.h"

enum eSpells
{
    SPELL_BLOODLUST                             = 54516,
    SPELL_BREAK_BONDS                           = 59463,
    SPELL_CHAIN_HEAL                            = 54481,
    SPELL_EARTH_SHIELD                          = 54479,
    SPELL_EARTH_SHOCK                           = 54511,
    SPELL_LIGHTNING_BOLT                        = 53044,
    SPELL_STORMSTRIKE                           = 51876,
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_SPAWN                                   = 3,
    SAY_ADD_KILLED                              = 4,
    SAY_BOTH_ADDS_KILLED                        = 5
};

enum eEvents
{
    EVENT_SPELL_BLOODLUST = 1,
    EVENT_SPELL_BREAK_BONDS,
    EVENT_SPELL_CHAIN_HEAL,
    EVENT_SPELL_EARTH_SHIELD,
    EVENT_SPELL_EARTH_SHOCK,
    EVENT_SPELL_LIGHTNING_BOLT,
    EVENT_SPELL_STORMSTRIKE,
};

struct boss_erekemAI : public BossAI
{
    boss_erekemAI(Creature* c) : BossAI(c, BOSS_EREKEM) { }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        Talk(SAY_AGGRO);
        DoCast(me, SPELL_EARTH_SHIELD);
        events.RescheduleEvent(EVENT_SPELL_BLOODLUST, 15s);
        events.RescheduleEvent(EVENT_SPELL_BREAK_BONDS, 9s, 14s);
        events.RescheduleEvent(EVENT_SPELL_CHAIN_HEAL, 0ms);
        events.RescheduleEvent(EVENT_SPELL_EARTH_SHIELD, 20s);
        events.RescheduleEvent(EVENT_SPELL_EARTH_SHOCK, 2s, 8s);
        events.RescheduleEvent(EVENT_SPELL_LIGHTNING_BOLT, 5s, 10s);
        if (IsHeroic())
            events.RescheduleEvent(EVENT_SPELL_STORMSTRIKE, 3s);

        if (Creature* c = instance->instance->GetCreature(instance->GetGuidData(DATA_EREKEM_GUARD_1_GUID)))
            if (!c->IsInCombat())
                c->AI()->AttackStart(who);
        if (Creature* c = instance->instance->GetCreature(instance->GetGuidData(DATA_EREKEM_GUARD_2_GUID)))
            if (!c->IsInCombat())
                c->AI()->AttackStart(who);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case EVENT_SPELL_BLOODLUST:
                me->CastSpell((Unit*)nullptr, SPELL_BLOODLUST, false);
                events.Repeat(35s, 45s);
                break;
            case EVENT_SPELL_BREAK_BONDS:
                me->CastSpell((Unit*)nullptr, SPELL_BREAK_BONDS, false);
                events.Repeat(16s, 22s);
                break;
            case EVENT_SPELL_CHAIN_HEAL:
                if (ObjectGuid targetGuid = GetChainHealTargetGuid())
                {
                    if (Creature* target = instance->instance->GetCreature(targetGuid))
                        me->CastSpell(target, SPELL_CHAIN_HEAL, false);

                    Creature* guard1 = instance->instance->GetCreature(instance->GetGuidData(DATA_EREKEM_GUARD_1_GUID));
                    Creature* guard2 = instance->instance->GetCreature(instance->GetGuidData(DATA_EREKEM_GUARD_2_GUID));
                    if ((guard1 && !guard1->IsAlive()) || (guard2 && !guard2->IsAlive()))
                    {
                        events.Repeat(3s, 6s);
                        break;
                    }
                }
                events.Repeat(8s, 11s);
                break;
            case EVENT_SPELL_EARTH_SHIELD:
                me->CastSpell(me, SPELL_EARTH_SHIELD, false);
                events.Repeat(20s);
                break;
            case EVENT_SPELL_EARTH_SHOCK:
                me->CastSpell(me->GetVictim(), SPELL_EARTH_SHOCK, false);
                events.Repeat(8s, 13s);
                break;
            case EVENT_SPELL_LIGHTNING_BOLT:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 35.0f, true))
                    me->CastSpell(target, SPELL_LIGHTNING_BOLT, false);
                events.Repeat(15s, 25s);
                break;
            case EVENT_SPELL_STORMSTRIKE:
                {
                    Creature* guard1 = instance->instance->GetCreature(instance->GetGuidData(DATA_EREKEM_GUARD_1_GUID));
                    Creature* guard2 = instance->instance->GetCreature(instance->GetGuidData(DATA_EREKEM_GUARD_2_GUID));
                    if (guard1 && !guard1->IsAlive() && guard2 && !guard2->IsAlive())
                        me->CastSpell(me->GetVictim(), SPELL_STORMSTRIKE, false);
                    events.Repeat(3s);
                }
                break;
        }
    }

    void JustDied(Unit* killer) override
    {
        Talk(SAY_DEATH);
        BossAI::JustDied(killer);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim && victim->GetGUID() == me->GetGUID())
            return;
        Talk(SAY_SLAY);
    }

    void MoveInLineOfSight(Unit* /*who*/) override {}

    void EnterEvadeMode(EvadeReason why) override
    {
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        _EnterEvadeMode(why);
    }

    ObjectGuid GetChainHealTargetGuid()
    {
        if (HealthBelowPct(85))
            return me->GetGUID();

        if (Creature* c = instance->instance->GetCreature(instance->GetGuidData(DATA_EREKEM_GUARD_1_GUID)))
            if (c->IsAlive() && !c->HealthAbovePct(75))
                return c->GetGUID();

        if (Creature* c = instance->instance->GetCreature(instance->GetGuidData(DATA_EREKEM_GUARD_2_GUID)))
            if (c->IsAlive() && !c->HealthAbovePct(75))
                return c->GetGUID();

        return me->GetGUID();
    }
};

enum GuardSpells
{
    SPELL_GUSHING_WOUND                         = 39215,
    SPELL_HOWLING_SCREECH                       = 54462,
    SPELL_STRIKE                                = 14516
};

enum eGuardEvents
{
    EVENT_SPELL_GUSHING_WOUND = 1,
    EVENT_SPELL_HOWLING_SCREECH,
    EVENT_SPELL_STRIKE
};

struct npc_erekem_guardAI : public ScriptedAI
{
    npc_erekem_guardAI(Creature* c) : ScriptedAI(c)
    {
        Instance = c->GetInstanceScript();
    }

    InstanceScript* Instance;
    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        DoZoneInCombat();
        events.Reset();
        events.RescheduleEvent(EVENT_SPELL_GUSHING_WOUND, 1s, 3s);
        events.RescheduleEvent(EVENT_SPELL_HOWLING_SCREECH, 8s, 13s);
        events.RescheduleEvent(EVENT_SPELL_STRIKE, 4s, 8s);

        if (Creature* c = Instance->GetCreature(BOSS_EREKEM))
            if (!c->IsInCombat())
                c->AI()->AttackStart(who);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
            case EVENT_SPELL_GUSHING_WOUND:
                me->CastSpell(me->GetVictim(), SPELL_GUSHING_WOUND, false);
                events.Repeat(7s, 12s);
                break;
            case EVENT_SPELL_HOWLING_SCREECH:
                me->CastSpell(me->GetVictim(), SPELL_HOWLING_SCREECH, false);
                events.Repeat(8s, 13s);
                break;
            case EVENT_SPELL_STRIKE:
                me->CastSpell(me->GetVictim(), SPELL_STRIKE, false);
                events.Repeat(4s, 8s);
                break;
        }

        DoMeleeAttackIfReady();
    }

    void MoveInLineOfSight(Unit* /*who*/) override {}
};

void AddSC_boss_erekem()
{
    RegisterVioletHoldCreatureAI(boss_erekemAI);
    RegisterVioletHoldCreatureAI(npc_erekem_guardAI);
}
