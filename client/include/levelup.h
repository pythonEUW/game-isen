#ifndef LEVELUP_H
#define LEVELUP_H

#include <stdint.h>
#include <raylib.h>
#include "protocol.h"

/* Nombre de niveaux de rarete (un cadre de couleur par niveau). */
#define CARD_LEVELS 8

/* Bandeau de choix d'amelioration affiche en bas de l'ecran a chaque level up.
 * Non bloquant : le joueur continue a se deplacer pendant le choix.
 * La couleur du cadre depend du niveau de l'amelioration : prendre deux fois
 * la meme carte la fait passer au niveau 2 (cadre vert), etc. */
typedef struct {
    int selected; /* carte survolee a la souris (-1 si aucune) */
    uint8_t owned[UPGRADE_COUNT]; /* nombre de fois ou chaque amelioration a ete prise */

    bool loaded;
    Texture2D frames[CARD_LEVELS];        /* cadres simples, niveaux 1..8 */
    Texture2D frames_ornate[CARD_LEVELS]; /* cadres ornes, niveaux 9..16 */
    Texture2D badges[CARD_LEVELS];        /* chevron de niveau, meme couleur */
    Texture2D digits[9];
    Texture2D art_border;
    Texture2D text_plate;
    Texture2D icons[UPGRADE_COUNT];
} LevelUpScreen;

/* Charge les sprites (apres InitWindow). En cas d'echec, dessin de secours. */
void levelup_load(LevelUpScreen *screen);
void levelup_unload(LevelUpScreen *screen);

/* Touches 1/2/3 ou clic souris. Retourne l'index de la carte validee, ou -1.
 * Met a jour le niveau de l'amelioration choisie. */
int levelup_update(LevelUpScreen *screen, const uint8_t choices[UPGRADE_CHOICES]);

/* Dessine les 3 petites cartes en bas, en coordonnees ecran (apres EndMode2D). */
void levelup_draw(const LevelUpScreen *screen, uint8_t level,
                  const uint8_t choices[UPGRADE_CHOICES]);

#endif
