#include <allegro5/allegro.h>
#include <cstdio>

#include "../include/renderer.h"
#include "../include/global_constants.h"

Renderer::Renderer()
{
	// Initialize Allegro | FIXME: Maybe move this elsewhere? Is sound to be managed here too?
    if (!al_init())
    {
		// TODO: Figure out a way to throw a fatal error and kill everything
		// We -could- just do some broken array access or some other guaranteed-to-fail code, but surely there's a better way
        fprintf(stderr, "Fatal Error: Allegro initialization failed!\n");
	}

	// Create a display
    display = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT);
    if(!display)
    {
		// TODO: Figure out a way to throw a fatal error and kill everything
		// We -could- just do some broken array access or some other guaranteed-to-fail code, but surely there's a better way
        fprintf(stderr, "Fatal Error: Could not create display\n");
    }
}

Renderer::~Renderer()
{
	// Cleanup
	al_destroy_display(display);
}

void Renderer::render()
{
	// Draw everything
    al_clear_to_color(al_map_rgb(0,0,0));

    al_flip_display();
}
