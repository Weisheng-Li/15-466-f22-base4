#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "render_text.hpp"
#include "state_machine.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	TextRenderer text_renderer;
	StateMachine state_machine;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
