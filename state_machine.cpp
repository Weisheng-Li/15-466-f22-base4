#include "state_machine.hpp"
#include <cassert>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>


StateMachine::StateMachine() {
    player.loc = LOC1;
    player.hp = HEALTHY;
    player.attack_state = 0;

    enemy.loc = LOC2;
    enemy.hp = HEALTHY;
    enemy.attack_state = 0;

    srand(time(NULL) % 25);

    // load and initialize the text map
    std::ifstream path("states.txt");

    unsigned int state_count;
    path >> state_count;
    std::string line;
    for (uint16_t i = 0; i < state_count; i++) {
        std::string state;
        int line_count;
        
        path >> line_count >> state;
        std::getline(path, line);   // consume the newline character

        std::vector<std::string> lines;
        for (uint16_t j = 0; j < line_count; j++) {
            std::getline(path, line);
            lines.push_back(line);
        }

        text_map.emplace(state, lines);
    }

    current_lines = text_map.find("START")->second;
}

StateMachine::health StateMachine::damage(health health, unsigned int times) {
    while (times) {
        if (health == StateMachine::HEALTHY) health = StateMachine::WOUNDED;
        else if (health == StateMachine::WOUNDED) health = StateMachine::DYING;
        else if (health == StateMachine::DYING) health = StateMachine::DEAD;
        else health = StateMachine::DEAD;
        times--;
    }

    if (player.hp == DEAD) {
        current_lines = text_map.find("LOST")->second;
        is_end_game = true;
    } else if (enemy.hp == DEAD) {
        current_lines = text_map.find("WIN")->second;
        is_end_game = true;
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

void StateMachine::player_action(action player_action) {
    if (is_end_game) return;

    // player's move
    if (player_action == ATTACK) {
        player.attack();
    } else if (player_action == MOVE) {
        player.move(static_cast<location>(rand() % 2));
    } else if (player_action == CHARGE) {
        player.charge();
    } else {
        assert(false);    // unknown action
    }

    // enemy's move (random AI)
    unsigned int enemy_move = rand() % 100;
    action enemy_action;
    if (enemy_move < 40) {
        enemy.attack();
        enemy_action = ATTACK;
    } else if (enemy_move < 70) {
        enemy.move(static_cast<location>(rand() % 2));
        enemy_action = MOVE;
    } else {
        enemy.charge();
        enemy_action = CHARGE;
    }

    current_lines.clear();
    update_cur_lines(player_action, enemy_action);

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

void StateMachine::update_cur_lines(action player_action, action enemy_action){
    std::vector<std::string>* text_pool = nullptr;
    if (player.loc != enemy.loc) {
        // DL : different location
        if (player_action == MOVE) {
            text_pool = & (text_map.find("MOVE_DL")->second);
        } else if (player_action == ATTACK) {
            text_pool = & (text_map.find("ATTACK_DL")->second);
        } else {
            text_pool = & (text_map.find("CHARGE_DL")->second);
        }
    } else {
        // someone is attacking
        if (player.attack_state > 0 || enemy.attack_state > 0) {
            if (player_action == MOVE) {
                // TD : take damage; DD : deal damage
                text_pool = & (text_map.find("MOVE_TD")->second);
            } else if (player_action == ATTACK) {
                if (player.attack_state > enemy.attack_state) {
                    text_pool = & (text_map.find("ATTACK_DD")->second);
                } else if (player.attack_state > enemy.attack_state) {
                    text_pool = & (text_map.find("ATTACK_TD")->second);
                } else {
                    text_pool = & (text_map.find("ATTACK_EVEN")->second);
                }
            } else if (player_action == CHARGE) {
                text_pool = & (text_map.find("CHARGE_TD")->second);
            }
        } else {
            // SL : same location
            if (player_action == MOVE) {
                text_pool = & (text_map.find("MOVE_SL")->second);
            } else {
                assert(player_action == CHARGE);
                text_pool = & (text_map.find("CHARGE_SL")->second);
            }
        }
    }
    assert(text_pool);
    current_lines.push_back(text_pool->at(rand() % text_pool->size()));
}

void StateMachine::print_state() {
    std::cout << "Player: " << std::to_string(player.loc) + " " << std::to_string(player.hp) + " " << player.attack_state << std::endl;
    std::cout << "Enemy: " << std::to_string(enemy.loc) + " " << std::to_string(enemy.hp) + " " << enemy.attack_state << std::endl;
}
