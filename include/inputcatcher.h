#ifndef INPUTCATCHER_H
#define INPUTCATCHER_H

#include <allegro5/allegro.h>

#include "datastructures.h"
#include "renderer.h"
#include "engine.h"
#include "json.hpp"

class InputCatcher
{
	public:
		InputCatcher(ALLEGRO_DISPLAY *display);
		~InputCatcher();
		void run(EntityPtr myself, Engine *engine, Renderer *renderer);
	private:
		ALLEGRO_EVENT_QUEUE *event_queue;
		nlohmann::json config;
};

#endif // INPUTCATCHER_H