# Game ISEN

Projet de jeu vidéo développé dans le cadre de l'ISEN.

## Présentation

**Game ISEN** est un projet de **roguelike survival 2D multijoueur**, inspiré de jeux comme *Megabonk*. Les joueurs évoluent dans un environnement hostile, affrontent des ennemis et tentent de survivre le plus longtemps possible.

Le projet est développé en **C**, avec **raylib** pour le client graphique et **ENet (UDP)** pour la communication client/serveur. Le serveur est **autoritaire** : il gère l'état du monde et valide les actions des joueurs.

### Idées à explorer (non prioritaires)

- Plusieurs personnages jouables, avec des caractéristiques différentes.
- Génération procédurale de la carte.
- Programmation génétique / algorithmes génétiques, par exemple pour faire évoluer certains comportements d'ennemis (à étudier selon le temps disponible).

Ces idées ne font pas partie du premier objectif jouable.

## Équipe et répartition

| Groupe | Membres | Responsabilités | Préfixe de branche |
| --- | --- | --- | --- |
| **Serveur** | Luke, Lukas | Réseau ENet, sessions, état du monde, simulation, ennemis, combats | `server/` |
| **Client** | Louison, Nassim | Fenêtre raylib, affichage, déplacements, interface, connexion au serveur | `client/` |

Le dossier `shared/` et le protocole réseau sont définis **ensemble** avant toute modification incompatible.

## Architecture du dépôt

```text
game-isen/
├── client/
│   ├── include/       # En-têtes du client
│   └── src/           # Rendu, entrées, réseau client
├── server/
│   ├── include/       # En-têtes du serveur
│   └── src/           # Réseau serveur, simulation
├── shared/
│   ├── include/       # Protocole et types communs
│   └── src/           # Code partagé
├── assets/            # Sprites, sons, ressources
├── tests/             # Tests du projet
├── .zed/              # Configuration éventuelle de Zed
└── CMakeLists.txt     # Configuration de compilation
```

Le **client** affiche le jeu et envoie les commandes des joueurs. Le **serveur** calcule les positions, collisions et événements du monde, puis diffuse les états aux clients. Le **code partagé** définit les structures et messages utilisés des deux côtés.

## Récupérer le projet

Prérequis : **Git**, un compilateur C (GCC ou Clang), **CMake**, ainsi que les bibliothèques de développement **raylib** et **ENet** pour la version réseau du jeu.

Cloner le dépôt en SSH (après avoir configuré sa clé GitHub) :

```bash
git clone git@github.com:pythonEUW/game-isen.git
cd game-isen
```

Ou en HTTPS :

```bash
git clone https://github.com/pythonEUW/game-isen.git
cd game-isen
```

### Compiler avec CMake

Depuis la racine du dépôt :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Une fois les cibles `client` et `server` configurées dans CMake, elles pourront être compilées indépendamment :

```bash
cmake --build build --target client
cmake --build build --target server
```

> **État actuel :** le squelette du projet est en cours d'initialisation. Le fichier `CMakeLists.txt` doit encore être renseigné avant que ces commandes fonctionnent. Les dépendances raylib et ENet doivent aussi être installées et correctement détectées par CMake.

### Lancer le jeu (après initialisation des exécutables)

Dans un premier terminal :

```bash
./build/server
```

Dans un deuxième terminal :

```bash
./build/client
```

Pour tester le multijoueur sur une seule machine, lancer un second client dans un troisième terminal :

```bash
./build/client
```

Au départ, le client se connectera au serveur local (`127.0.0.1`) sur le port qui sera défini dans le protocole commun (port envisagé : `7777`). Le lancement sur des machines différentes sera documenté après les premiers tests réseau.

## Organisation Git

La branche `main` doit rester stable. Chaque développeur travaille dans une branche dédiée, selon son groupe :

- `server/nom-de-fonctionnalite` pour Luke et Lukas ;
- `client/nom-de-fonctionnalite` pour Louison et Nassim.

Exemple :

```bash
git switch main
git pull origin main
git switch -c client/player-movement

# Après les modifications :
git add .
git commit -m "feat(client): add player movement"
git push -u origin client/player-movement
```

Créer ensuite une **Pull Request** vers `main` pour faire relire et intégrer les changements. Les modifications de `shared/` doivent être coordonnées entre les deux groupes.

## Roadmap courte

### Étape 1 — Base technique

- **Ensemble :** compléter CMake, définir les premiers messages dans `shared/` et vérifier la compilation des deux exécutables.
- **Serveur :** initialiser ENet, accepter une connexion et attribuer un identifiant joueur.
- **Client :** ouvrir une fenêtre raylib, afficher un personnage et établir une connexion ENet.

### Étape 2 — Premier multijoueur jouable

- **Serveur :** recevoir les commandes de déplacement, calculer les positions et envoyer l'état des joueurs.
- **Client :** envoyer les entrées clavier et afficher les autres joueurs aux positions reçues.
- **Ensemble :** tester deux clients connectés au même serveur, ainsi que la déconnexion.

### Étape 3 — Première boucle de survie

- **Serveur :** gérer des ennemis simples, les collisions, les dégâts et les points de vie.
- **Client :** afficher les ennemis, les attaques, la vie et la fin de partie.

**Premier jalon : deux joueurs peuvent se connecter et se voir se déplacer dans le même monde.** La génération procédurale, les personnages supplémentaires et les algorithmes génétiques viendront uniquement si ce socle est stable.
