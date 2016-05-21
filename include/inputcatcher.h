#ifndef INPUTCATCHER_H
#define INPUTCATCHER_H

#include <allegro5/allegro.h>

class InputCatcher
{
	public:
		InputCatcher(ALLEGRO_DISPLAY *display);
		~InputCatcher();
		void run();
	private:
		ALLEGRO_EVENT_QUEUE *event_queue;
};

#endif // INPUTCATCHER_H
