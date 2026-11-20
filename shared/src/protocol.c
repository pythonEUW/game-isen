#include "protocol.h"

int inventory_packet_add(InventoryPacket *inv, uint32_t item_id, uint32_t quantity)
{
    for (uint32_t i = 0; i < inv->count; i++) {
        if (inv->slots[i].item_id == item_id) {
            inv->slots[i].quantity += quantity;
            return (int)i;
        }
    }
    if (inv->count >= MAX_INVENTORY_SLOTS)
        return -1;
    inv->slots[inv->count].item_id = item_id;
    inv->slots[inv->count].quantity = quantity;
    return (int)inv->count++;
}
