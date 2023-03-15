#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "pokemon.h"

struct my_pid_s my_global = {.sig1 = 0, .sig2 = 0};

void connectBD (MYSQL *conn){

    if(conn == NULL) { /*If it returns NULL, the initialization failed*/
        printf("mysql_init failed!\n");
    }

    conn=mysql_real_connect(conn,"c8u4r7fp8i8qaniw.chr7pe7iynqr.eu-west-1.rds.amazonaws.com","rjwto5lw5nyvprcl","geuhiwb6tq0qq5yu","jq6kc1yecb69vdxf",3306,NULL,0);
    if (conn) {
        printf("\033[0;34mConnection success! \033[0m\n");
    }else{
        printf("\033[0;31mConnection failed! \033[0m\n");
    }
}

int checkBD (MYSQL *conn){ 
    MYSQL_ROW row;
    MYSQL_FIELD* field;
    unsigned int num_fields;
    unsigned int i = 0;
    int max_id;

    mysql_query(conn,"select MAX(id) AS id from pokemons");  //selctionner tt les pkm de bdd et si il y en a moins de 6 -> quit
    MYSQL_RES *result = mysql_store_result(conn);
    num_fields = mysql_field_count(conn);
    while((row = mysql_fetch_row(result)) != NULL)
    {
        for(i = 0; i < num_fields; i++)
        {
            field = mysql_fetch_field_direct(result, i);
            max_id = atoi(row[i]);
        }
    }
    mysql_free_result(result);
    if(max_id < 6){
        printf("Pas assez d'animaux dans la base de données\n");
        return (-1);
    }
    return max_id;
}

int getstats(MYSQL *conn, int id, char* type){  
    MYSQL_ROW row;
    MYSQL_FIELD* field;
    unsigned int num_fields;
    unsigned int i = 0;
    int speed = 0;
    char buff[255];

    sprintf(buff, "select %s from pokemons where id = %d", type, id);
    mysql_query(conn,buff);
    MYSQL_RES *result = mysql_store_result(conn);
    num_fields = mysql_field_count(conn);
    while((row = mysql_fetch_row(result)) != NULL)
    {
        for(i = 0; i < num_fields; i++){
            field = mysql_fetch_field_direct(result, i);
            speed = atoi(row[i]);
        }

    }
    mysql_free_result(result);

    return speed;
}

char *getname(MYSQL *conn, int id){
    MYSQL_ROW row;
    MYSQL_FIELD* field;
    unsigned int num_fields;
    unsigned int i;
    char* name = NULL;
    char buff[255];

    sprintf(buff, "select nom from pokemons where id = %d", id);
    mysql_query(conn,buff);
    MYSQL_RES *result = mysql_store_result(conn);
    num_fields = mysql_field_count(conn);
    while((row = mysql_fetch_row(result)) != NULL)
    {
        for(i = 0; i < num_fields; i++){
            field = mysql_fetch_field_direct(result, i);
            name = row[i];
        }

    }
    mysql_free_result(result);

    return name;
}

void select_pkms(MYSQL *conn, int nb_pkm, pokemon_t *pkm) 
{
    pkm->player.info_pokemons = malloc(sizeof(info_pokemon_t) * 6);
    printf("Select 6 pokemons within this list :\n");
    for (int i = 1; i <= nb_pkm; i += 1) {
        printf("[%d] - %s\n", i, getname(conn, i));
    }
    printf("$> ");
    scanf("%d %d %d %d %d %d", &pkm->player.info_pokemons[0].id, &pkm->player.info_pokemons[1].id, &pkm->player.info_pokemons[2].id, &pkm->player.info_pokemons[3].id, &pkm->player.info_pokemons[4].id, &pkm->player.info_pokemons[5].id);
    for (int i = 0; i < 6; i += 1) {  
        pkm->player.info_pokemons[i].name = getname(conn, pkm->player.info_pokemons[i].id);
        pkm->player.info_pokemons[i].attack = getstats(conn, pkm->player.info_pokemons[i].id, "attack");
        pkm->player.info_pokemons[i].defence = getstats(conn, pkm->player.info_pokemons[i].id, "defense");
        pkm->player.info_pokemons[i].speed = getstats(conn, pkm->player.info_pokemons[i].id, "vitesse");
        pkm->player.info_pokemons[i].life = getstats(conn, pkm->player.info_pokemons[i].id, "vie");
        pkm->player.info_pokemons[i].energy = getstats(conn, pkm->player.info_pokemons[i].id, "energie");
        pkm->player.info_pokemons[i].type = getstats(conn, pkm->player.info_pokemons[i].id, "type");
    }
}

// int get_team()
// {
//     int x = 0;

//     my_global.sig2 = 0;
//     my_global.sig1 = 0;
//     while (my_global.sig2 == 0);
//     my_global.sig2 = 0;
//     x = my_global.sig1;
//     my_global.sig1 = 0;
//     kill(my_global.global_pid, SIGUSR2);
//     return (x);
// }

void print_teams(pokemon_t *pkm, int nb_pkm, MYSQL *conn)
{
    printf("Your team is:\n");
    for (int i = 0; i < nb_pkm; i += 1) {
        printf("[%d] - %s\n", i + 1, getname(conn, pkm->player.info_pokemons[i].id));
    }
}

void get_enemy_pkm(pokemon_t *pkm, MYSQL *conn)
{
    for (int i = 0; i < 6; i += 1) { //écrit en brut car problemes avec les séléction de pkm
        pkm->enemy.info_pokemons[i].name = getname(conn, pkm->enemy.info_pokemons[i].id);
        pkm->enemy.info_pokemons[i].attack = getstats(conn, pkm->enemy.info_pokemons[i].id, "attack");
        pkm->enemy.info_pokemons[i].defence = getstats(conn, pkm->enemy.info_pokemons[i].id, "defense");
        pkm->enemy.info_pokemons[i].speed = getstats(conn, pkm->enemy.info_pokemons[i].id, "vitesse");
        pkm->enemy.info_pokemons[i].life = getstats(conn, pkm->enemy.info_pokemons[i].id, "vie");
        pkm->enemy.info_pokemons[i].energy = getstats(conn, pkm->enemy.info_pokemons[i].id, "energie");
        pkm->enemy.info_pokemons[i].type = getstats(conn, pkm->enemy.info_pokemons[i].id, "type");
    }
}

int init_struct(pokemon_t *pkm, MYSQL *conn, int nb_pkm) // initialiser la structure pokemon pour initialiser les teams
{
    if (nb_pkm == -1) {
        return (-1);
    }
    select_pkms(conn, nb_pkm, pkm);
    pkm->player.nb_pkm_alive = 6;
    pkm->player.pkm_battling = pkm->player.info_pokemons[0].id;
    pkm->enemy.nb_pkm_alive = 6;
    pkm->enemy.pkm_battling = 0;
    pkm->enemy.info_pokemons = malloc(sizeof(info_pokemon_t) * 6);
    for (int i = 1; i < 7; i += 1) {
        pkm->enemy.info_pokemons[i - 1].id = i;
    }
    get_enemy_pkm(pkm, conn);
    return (0);
}

void get_siga(int signal, siginfo_t *info, void *context) //permet de catch les signaux, fonctionne automatiquement 
{
    if (!my_global.global_pid) {
        my_global.global_pid = info->si_pid;
    }
    if (info->si_signo == SIGUSR1) {
        my_global.sig1 += 1;
    }
    if (info->si_signo == SIGUSR2) {
        my_global.sig2 += 1;
    }
}

int get_signal(void) //initialisation qui permet d'utiliser les signaux par la suite
{
    struct sigaction sig; // permet de récupérer les signaux, la structure permet d'interpréter les signaux après

    sig.sa_sigaction = &get_siga;
    sig.sa_flags = SA_SIGINFO;
    sigemptyset(&sig.sa_mask); // initialisation de la structure 
    if (sigaction(SIGUSR2, &sig, NULL) < 0) { //fontion sigation =/= structure sigaction mais les 2 sont qd même lié  
        return (84);
    }
    if (sigaction(SIGUSR1, &sig, NULL) < 0) {
        return (84);
    }
    return (0);
}

void f_connection(void) //connection joueur 1
{
    printf("my_pid: %d\nwaiting for enemy connection...\n", getpid());
    pause();  //attend un signal
    usleep(7500); //permet de s'assurer que le le J2 est bien en pause()
    kill(my_global.global_pid, SIGUSR1);
    usleep(4500);
    printf("\nenemy connected\n\n");
}

int s_connection(char **av) //connection joueur 2
{
    if (kill(atoi(av[1]), SIGUSR2) == -1) {
        return (84);
    }
    usleep(4500); 
    pause();
    usleep(4500);
    printf("my_pid: %d\nsuccessfully connected\n\n", getpid());
    return (0);
}

void init_pid(int ac, char **av) //diferencie les 2 joueurs pour definir un ordre de jeu 
{
    get_signal();
    if (ac < 2) { 
        f_connection();
    } else {
        s_connection(av);
    }
}

// int get_enemy_pkm_id()
// {
//     int x = 0;

//     kill(my_global.global_pid, SIGUSR2);
//     my_global.sig2 = 0;
//     my_global.sig1 = 0;
//     while (my_global.sig2 == 0);
//     my_global.sig2 = 0;
//     x = my_global.sig1;
//     my_global.sig1 = 0;
//     printf("x = %d\n", x);
//     return (x);
// }

// void send_pkm_id(info_pokemon_t *pkm)
// {
//     for (int i = 0; i < 6; i += 1) {
//         printf("id = %d\n", pkm[i].id);
//         for (int i = 0; i < pkm[i].id; i += 1) {
//             kill(my_global.global_pid, SIGUSR1);
//             usleep(4500);
//         }
//         kill(my_global.global_pid, SIGUSR2);
//         usleep(4500);
//     }
// }

void print_stats(info_pokemon_t pkm, MYSQL *conn)
{
    printf("%s - %d\nhealth = %d\n", getname(conn, pkm.id), pkm.id, pkm.life);
}
int game_loop(pokemon_t *pkm, int ac, MYSQL *conn)
{
    while (pkm->player.nb_pkm_alive > 0 && pkm->enemy.nb_pkm_alive > 0) {
        if ((pkm->player.info_pokemons[pkm->player.pkm_battling].speed == pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].speed && ac == 1) || pkm->player.info_pokemons[pkm->player.pkm_battling].speed > pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].speed) {   //detection de la speed mob 1 qui en a le plus
            if ((pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].life + pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].defence * 0.5) - pkm->player.info_pokemons[pkm->player.pkm_battling].attack < 0){
                printf("enemy's %s is ko\n", getname(conn, pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].id));
                pkm->enemy.nb_pkm_alive -= 1;
                pkm->enemy.pkm_battling = pkm->enemy.info_pokemons[6 - pkm->enemy.nb_pkm_alive].id;
                continue;
            } else{
                pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].life = (pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].life + pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].defence * 0.5) - pkm->player.info_pokemons[pkm->player.pkm_battling].attack;
                if ((pkm->player.info_pokemons[pkm->player.pkm_battling].life + pkm->player.info_pokemons[pkm->player.pkm_battling].defence * 0.5) - pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].attack < 0){
                    printf("%s is ko\n", getname(conn, pkm->player.info_pokemons[pkm->player.pkm_battling - 1].id));
                    pkm->player.nb_pkm_alive -= 1;
                    pkm->player.pkm_battling = pkm->player.info_pokemons[6 - pkm->player.nb_pkm_alive].id;
                    continue;
                } else{
                    pkm->player.info_pokemons[pkm->player.pkm_battling].life = (pkm->player.info_pokemons[pkm->player.pkm_battling].life + pkm->player.info_pokemons[pkm->player.pkm_battling].defence * 0.5) - pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].attack;
                }
            }
        } else { //detection de la speed mob 2 qui en a le plus
            if ((pkm->player.info_pokemons[pkm->player.pkm_battling].life + pkm->player.info_pokemons[pkm->player.pkm_battling].defence * 0.5) - pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].attack < 0){
                printf("%s is ko\n", getname(conn, pkm->player.info_pokemons[pkm->player.pkm_battling - 1].id));
                pkm->player.nb_pkm_alive -= 1;
                pkm->player.pkm_battling = pkm->player.info_pokemons[6 - pkm->player.nb_pkm_alive].id;
                continue;
            } else{
                pkm->player.info_pokemons[pkm->player.pkm_battling].life = (pkm->player.info_pokemons[pkm->player.pkm_battling].life + pkm->player.info_pokemons[pkm->player.pkm_battling].defence * 0.5) - pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].attack;
                if ((pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].life + pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].defence * 0.5) - pkm->player.info_pokemons[pkm->player.pkm_battling].attack < 0){
                    printf("enemy's %s is ko\n", getname(conn, pkm->enemy.info_pokemons[pkm->enemy.pkm_battling - 1].id));
                    pkm->enemy.nb_pkm_alive -= 1;
                    pkm->enemy.pkm_battling = pkm->enemy.info_pokemons[6 - pkm->enemy.nb_pkm_alive].id;
                    continue;
                } else{
                    pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].life = (pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].life + pkm->enemy.info_pokemons[pkm->enemy.pkm_battling].defence * 0.5) - pkm->player.info_pokemons[pkm->player.pkm_battling].attack;
                }
            }
        }
    }
    return (pkm->player.nb_pkm_alive > 0);
}

// void send_first_pkm(int ac, pokemon_t *pkm, MYSQL *conn)
// {
//     if (ac == 1) {
//         send_pkm_id(pkm->player.info_pokemons);
//         get_enemy_pkm(pkm, conn);
//     } else {
//         get_enemy_pkm(pkm, conn);
//         send_pkm_id(pkm->player.info_pokemons);
//     }
// }

int main(int ac, char **av)
{
    MYSQL *conn = mysql_init(NULL);
    pokemon_t pkm;

    init_pid(ac, av);
    connectBD(conn);
    int nb_pkm = checkBD(conn);

    my_global.sig2 = 0;
    if (ac == 1) {
        if (init_struct(&pkm, conn, nb_pkm) == -1) {
            return (1);
        }
        kill(my_global.global_pid, SIGUSR2);
        pause();
    } else {
        pause();
        usleep(4500);
        if (init_struct(&pkm, conn, nb_pkm) == -1) {
            return (1);
        }
        kill(my_global.global_pid, SIGUSR2);
    }
    if (game_loop(&pkm, ac, conn) == 1) {
        printf("I won\n");
    } else {
        printf("Enemy won\n");
    }
    return 0;
}
