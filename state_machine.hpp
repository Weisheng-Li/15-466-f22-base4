#include <map>
#include <vector>
#include <string>

struct StateMachine {
    enum location {LOC1, LOC2};
    enum health{HEALTHY, WOUNDED, DYING, DEAD};

    struct samuri {
        location loc;
        health hp;
        int attack_state;

        void move(location loc);
        void attack();
        void make_noise();
        void charge();
    };

    samuri player;
    samuri enemy;

    bool is_end_game = false;

    enum action {
        ATTACK,
        MOVE,
        CHARGE
    };

    void print_state(); // for debugging
    void player_action(action next_move);

    // this function needs to be called before attack resolve
    void update_cur_lines(action player_action, action enemy_action);
    health damage(health health, unsigned int times);

    std::map<std::string, std::vector<std::string>> text_map;
    std::vector<std::string> current_lines;

    StateMachine();
};