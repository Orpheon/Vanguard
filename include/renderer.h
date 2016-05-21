#ifndef RENDERER_H
#define RENDERER_H

#include <allegro5/allegro.h>

class Renderer
{
	public:
		Renderer();
		~Renderer();
		void render();
	protected:
	private:
		ALLEGRO_DISPLAY *display = 0;
};

#endif // RENDERER_H
