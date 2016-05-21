#ifndef RENDERER_H
#define RENDERER_H

#include <allegro5/allegro.h>

class Renderer
{
	public:
		Renderer();
		~Renderer();
		void render();
		ALLEGRO_DISPLAY *display;
	protected:
	private:
};

#endif // RENDERER_H
