#ifndef POKEMON_H_
#define POKEMON_H_

typedef struct info_pokemon_s {
    int id;
    char *name;
    int attack;
    int defence;
    int speed;
    int life;
    int energy;
    int type;
} info_pokemon_t;

typedef struct team_s {
    struct info_pokemon_s *info_pokemons;
    int nb_pkm_alive;
    int pkm_battling;
} team_t;

typedef struct pokemon_s {
    struct team_s player;
    struct team_s enemy;
} pokemon_t;

typedef struct my_pid_s {
    int global_pid;
    int sig1;
    int sig2;
} my_pid_t;

#endif /* !POKEMON_H_ */
