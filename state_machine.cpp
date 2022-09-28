#include "state_machine.hpp"
#include <cassert>
#include <cstdlib>
#include <time.h>
#include <string>
#include <iostream>


StateMachine::StateMachine() {
    player.loc = LOC1;
    player.hp = HEALTHY;
    player.attack_state = 0;

    enemy.loc = LOC4;
    enemy.hp = HEALTHY;
    enemy.attack_state = 0;

    srand(time(NULL) % 25);
}

StateMachine::health damage(StateMachine::health health, unsigned int times) {
    while (times) {
        if (health == StateMachine::HEALTHY) health = StateMachine::WOUNDED;
        else if (health == StateMachine::WOUNDED) health = StateMachine::DYING;
        else if (health == StateMachine::DYING) health = StateMachine::DEAD;
        else health = StateMachine::DEAD;
        times--;
    }
    return health;
}

void StateMachine::samuri::attack() {
    assert(attack_state <= 0);
    attack_state -= 1;
    attack_state = -attack_state;
}

void StateMachine::samuri::move(location new_loc) {
    assert(attack_state <= 0);
    loc = new_loc;
    attack_state = 0;
}

void StateMachine::samuri::charge() {
    assert(attack_state <= 0);
    attack_state -= 1;
};

void StateMachine::player_action(action next_move) {
    // player's move
    if (next_move == ATTACK) {
        player.attack();
    } else if (next_move == MOVE) {
        player.move(static_cast<location>(rand() % 4));
    } else if (next_move == CHARGE) {
        player.charge();
    } else {
        assert(false);    // unknown action
    }

    // enemy's move (random AI)
    unsigned int enemy_move = rand() % 100;
    if (enemy_move < 40) {
        enemy.attack();
    } else if (enemy_move < 70) {
        enemy.move(static_cast<location>(rand() % 4));
    } else {
        enemy.charge();
    }

    // resolve attack
    if (player.loc == enemy.loc && (player.attack_state > 0 || enemy.attack_state > 0)) {
        if (player.attack_state > enemy.attack_state) {
            enemy.hp = damage(enemy.hp, 1);
        } else if (player.attack_state < enemy.attack_state) {
            player.hp = damage(player.hp, 1);
        }
    }

    player.attack_state = player.attack_state > 0 ? 0 : player.attack_state;
    enemy.attack_state = enemy.attack_state > 0 ? 0 : enemy.attack_state;

    print_state();
}

void StateMachine::print_state() {
    std::cout << "Player: " << std::to_string(player.loc) + " " << std::to_string(player.hp) + " " << player.attack_state << std::endl;
    std::cout << "Enemy: " << std::to_string(enemy.loc) + " " << std::to_string(enemy.hp) + " " << enemy.attack_state << std::endl;
}
