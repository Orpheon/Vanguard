#include <allegro5/allegro.h>
#include <cstdio>
#include <vector>

#include "renderer.h"
#include "global_constants.h"
#include "entity.h"

Renderer::Renderer() : spriteloader()
{
    // Create a display
    display = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT);
    if(!display)
    {
        // FIXME: Make the error argument mean anything?
        fprintf(stderr, "Fatal Error: Could not create display\n");
        throw -1;
    }

    background = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    midground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    foreground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
}

Renderer::~Renderer()
{
    // Cleanup
    al_destroy_display(display);
}

void Renderer::render(Gamestate *currentstate, PlayerPtr myself)
{
    // Set camera
    Character *c = static_cast<Character*>(currentstate->get(currentstate->get(myself)->character));
    if (c != 0)
    {
        cam_x = c->x - WINDOW_WIDTH/2.0;
        cam_y = c->y - WINDOW_HEIGHT/2.0;
    }

    al_set_target_bitmap(background);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(midground);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(foreground);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    // Go through all objects and let them render themselves on the layers
    for (auto& e : currentstate->entitylist)
    {
        e.second->render(this);
    }

    // Set render target to be the display
    al_set_target_backbuffer(display);

    // Clear black
    al_clear_to_color(al_map_rgba(0, 0, 0, 1));

    // Draw the map background first
    currentstate->currentmap->render(cam_x, cam_y);

    // Then draw each layer
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(midground, 0, 0, 0);
    al_draw_bitmap(foreground, 0, 0, 0);

    al_flip_display();
}
