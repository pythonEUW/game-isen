#include "protocol.h"

int inventory_packet_add(InventoryPacket *inv, uint32_t spell_id)
{
    for (uint32_t i = 0; i < inv->count; i++) {
        if (inv->slots[i].spell_id == spell_id) {
            inv->slots[i].level++;
            return (int)i;
        }
    }
    if (inv->count >= MAX_SPELLS)
        return -1;
    inv->slots[inv->count].spell_id = spell_id;
    return (int)inv->count++;
}
