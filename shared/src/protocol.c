#include "protocol.h"

#include <stdlib.h>

typedef struct {
    const char *name;
    const char *description;
} UpgradeInfo;

static const UpgradeInfo UPGRADES[UPGRADE_COUNT] = {
    [UPGRADE_NEW_WEAPON]   = {"Nouvelle arme", "Debloque une\nnouvelle arme"},
    [UPGRADE_MAX_HEALTH]   = {"Vitalite",      "+20 points de\nvie max"},
    [UPGRADE_SPEED]        = {"Agilite",       "+10% vitesse de\ndeplacement"},
    [UPGRADE_DAMAGE]       = {"Puissance",     "+15% degats"},
    [UPGRADE_ATTACK_SPEED] = {"Frenesie",      "+10% vitesse\nd'attaque"},
};

const char *upgrade_name(UpgradeId id)
{
    return (unsigned)id < UPGRADE_COUNT ? UPGRADES[id].name : "???";
}

const char *upgrade_description(UpgradeId id)
{
    return (unsigned)id < UPGRADE_COUNT ? UPGRADES[id].description : "";
}

void upgrade_roll_choices(uint8_t out[UPGRADE_CHOICES])
{
    /* Fisher-Yates partiel : les UPGRADE_CHOICES premiers sont distincts. */
    uint8_t pool[UPGRADE_COUNT];
    for (int i = 0; i < UPGRADE_COUNT; i++)
        pool[i] = (uint8_t)i;
    for (int i = 0; i < UPGRADE_CHOICES; i++) {
        int j = i + rand() % (UPGRADE_COUNT - i);
        uint8_t tmp = pool[i];
        pool[i] = pool[j];
        pool[j] = tmp;
        out[i] = pool[i];
    }
}
