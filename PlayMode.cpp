#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	// std::cout << elapsed << std::endl;
	static float cooldown = 0;

	// update based on the key pressed
	if (cooldown > 0) cooldown -= elapsed;
	if (cooldown <= 0) cooldown = 0.0f;
	if (cooldown == 0.0f) {
		if (left.pressed) {
			state_machine.player_action(StateMachine::ATTACK);
			cooldown = 1;
		}
		else if (right.pressed) {
			state_machine.player_action(StateMachine::MOVE);
			cooldown = 1;
		}
		else if (down.pressed) {
			state_machine.player_action(StateMachine::CHARGE);
			cooldown = 1;
		}
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float delta_y = static_cast<float> (text_renderer.line_space);
	for (int i = 0; i < state_machine.current_lines.size(); i++) {
		text_renderer.render_text(state_machine.current_lines[i], 
			0, 450 - i * delta_y, 1.0f, glm::vec3(0,0,0));
	}
	
	GL_ERRORS();
}
