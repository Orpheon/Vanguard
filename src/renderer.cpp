#include <allegro5/allegro.h>
#include <cstdio>
#include <vector>
#include <string>

#include "renderer.h"
#include "global_constants.h"
#include "entity.h"

Renderer::Renderer() : cam_x(0), cam_y(0), spriteloader(false)
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

    //load font
    //gg2 font as placeholder for now i guess
    al_init_font_addon();
    al_init_ttf_addon();
    font = al_load_font("gg2bold.ttf", 12, 0);
    if (!font)
    {
      fprintf(stderr, "Could not load 'gg2bold.ttf'.\n");
      throw -1;
    }

    // fps stuff
    startframe = al_get_time();
}

Renderer::~Renderer()
{
    // Cleanup
    al_destroy_display(display);
    al_destroy_font(font);
    al_shutdown_font_addon();
    al_shutdown_ttf_addon();
    al_destroy_bitmap(background);
    al_destroy_bitmap(midground);
    al_destroy_bitmap(foreground);
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
        e.second->render(this, currentstate);
    }

    // Set render target to be the display
    al_set_target_backbuffer(display);

    // Clear black
    al_clear_to_color(al_map_rgba(0, 0, 0, 1));

    // Draw the map background first
    currentstate->currentmap->renderbackground(cam_x, cam_y);

    // Then draw each layer
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(midground, 0, 0, 0);
    al_draw_bitmap(foreground, 0, 0, 0);

    // Draw the map wallmask on top of everything, to prevent sprites that go through walls
    currentstate->currentmap->renderwallground(cam_x, cam_y);

    //fps counter
    endframe = al_get_time();
    al_draw_text(font, al_map_rgb(255,255,255), 0, 0,ALLEGRO_ALIGN_LEFT, ("Frame time: " + std::to_string((endframe - startframe)) + "ms").c_str());
    al_draw_text(font, al_map_rgb(255,255,255), 0, 12,ALLEGRO_ALIGN_LEFT, ("FPS: " + std::to_string(1.0 / (endframe - startframe)) + "ms").c_str());

    al_flip_display();
    startframe = al_get_time();
}
