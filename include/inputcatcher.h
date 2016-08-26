#ifndef INPUTCATCHER_H
#define INPUTCATCHER_H

#include <allegro5/allegro.h>

#include "player.h"
#include "renderer.h"

class InputCatcher
{
	public:
		InputCatcher(ALLEGRO_DISPLAY *display);
		~InputCatcher();
		void run(Player *myself, Renderer *renderer);
	private:
		ALLEGRO_EVENT_QUEUE *event_queue;
};

#endif // INPUTCATCHER_H
