#ifndef MENU_H
#define MENU_H

typedef enum
{
    MENU_ACTION_PLAY,
    MENU_ACTION_QUIT
} MenuAction;

/* Affiche le menu principal et boucle jusqu'a un choix.
   Retourne MENU_ACTION_PLAY ou MENU_ACTION_QUIT. */
MenuAction run_menu(void);

#endif
