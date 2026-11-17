#ifndef LEVELUP_H
#define LEVELUP_H

#include <stdint.h>
#include "protocol.h"

/* Ecran de choix d'amelioration affiche par-dessus le jeu a chaque level up. */
typedef struct {
    int selected; /* carte surlignee (0 .. UPGRADE_CHOICES-1) */
} LevelUpScreen;

/* Gere clavier/souris. Retourne l'index de la carte validee, ou -1. */
int levelup_update(LevelUpScreen *screen);

/* Dessine le voile et les 3 cartes, en coordonnees ecran (apres EndMode2D). */
void levelup_draw(const LevelUpScreen *screen, uint8_t level,
                  const uint8_t choices[UPGRADE_CHOICES]);

#endif
