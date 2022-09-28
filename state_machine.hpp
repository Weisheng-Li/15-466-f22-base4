
#include <vector>

struct StateMachine {
    enum location {LOC1, LOC2, LOC3, LOC4};
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

    enum action {
        ATTACK,
        MOVE,
        CHARGE
    };

    void print_state(); // for debugging
    void player_action(action next_move);

    StateMachine();
};