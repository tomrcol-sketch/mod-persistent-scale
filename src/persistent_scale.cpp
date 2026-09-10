#include "Chat.h"
#include "CommandScript.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "ScriptMgr.h"

#include <cmath>

using namespace Acore::ChatCommands;

namespace PersistentScale
{
    constexpr float MIN_SCALE = 0.10f;
    constexpr float MAX_SCALE = 10.0f;

    float GetScale(ObjectGuid guid)
    {
        std::string query = fmt::format(
            "SELECT scale FROM persistent_character_scale WHERE guid = {}",
            guid.GetCounter());

        if (QueryResult result = CharacterDatabase.Query(query))
            return (*result)[0].Get<float>();

        return 1.0f;
    }

    void SaveScale(ObjectGuid guid, float scale)
    {
        std::string query = fmt::format(
            "REPLACE INTO persistent_character_scale (guid, scale) VALUES ({}, {})",
            guid.GetCounter(), scale);

        CharacterDatabase.Execute(query);
    }

    void ApplyScale(Player* player)
    {
        if (!player)
            return;

        float scale = GetScale(player->GetGUID());

        if (!std::isfinite(scale) ||
            scale < MIN_SCALE ||
            scale > MAX_SCALE)
        {
            scale = 1.0f;
        }

        player->SetObjectScale(scale);
    }
}

class PersistentScaleWorldScript : public WorldScript
{
public:
    PersistentScaleWorldScript()
        : WorldScript("PersistentScaleWorldScript")
    {
    }

    void OnStartup() override
    {
        CharacterDatabase.Execute(
            "CREATE TABLE IF NOT EXISTS persistent_character_scale ("
            "guid INT UNSIGNED NOT NULL,"
            "scale FLOAT NOT NULL DEFAULT 1.0,"
            "PRIMARY KEY (guid)"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"
        );

        LOG_INFO("server.loading",
            "Persistent Scale: database table initialized.");
    }
};

class PersistentScalePlayerScript : public PlayerScript
{
public:
    PersistentScalePlayerScript()
        : PlayerScript("PersistentScalePlayerScript")
    {
    }

    void OnPlayerLoadFromDB(Player* player) override
    {
        PersistentScale::ApplyScale(player);
    }

    void OnPlayerLogin(Player* player) override
    {
        PersistentScale::ApplyScale(player);
    }
};

class PersistentScaleUnitScript : public UnitScript
{
public:
    PersistentScaleUnitScript()
        : UnitScript("PersistentScaleUnitScript")
    {
    }

    void OnUnitSetShapeshiftForm(Unit* unit, uint8 /*form*/) override
    {
        if (Player* player = unit ? unit->ToPlayer() : nullptr)
            PersistentScale::ApplyScale(player);
    }

    void OnDisplayIdChange(Unit* unit, uint32 /*displayId*/) override
    {
        if (Player* player = unit ? unit->ToPlayer() : nullptr)
            PersistentScale::ApplyScale(player);
    }
};

class PersistentScaleCommandScript : public CommandScript
{
public:
    PersistentScaleCommandScript()
        : CommandScript("PersistentScaleCommandScript")
    {
    }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandTable =
        {
            { "persistscale", HandlePersistentScaleCommand,
                rbac::RBAC_PERM_COMMAND_MODIFY_SCALE, Console::No }
        };

        return commandTable;
    }

    static bool HandlePersistentScaleCommand(
        ChatHandler* handler, float scale)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        if (!std::isfinite(scale) ||
            scale < PersistentScale::MIN_SCALE ||
            scale > PersistentScale::MAX_SCALE)
        {
            handler->PSendSysMessage(
                "Scale must be between {:.1f} and {:.1f}.",
                PersistentScale::MIN_SCALE,
                PersistentScale::MAX_SCALE);

            return false;
        }

        PersistentScale::SaveScale(player->GetGUID(), scale);
        player->SetObjectScale(scale);

        handler->PSendSysMessage(
            "Persistent character scale set to {:.2f}.",
            scale);

        return true;
    }
};

void Addmod_persistent_scaleScripts()
{
    new PersistentScaleWorldScript();
    new PersistentScalePlayerScript();
    new PersistentScaleUnitScript();
    new PersistentScaleCommandScript();
}
