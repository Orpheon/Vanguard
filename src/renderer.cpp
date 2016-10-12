#include <allegro5/allegro.h>
#include <cstdio>
#include <vector>
#include <string>

#include "renderer.h"
#include "global_constants.h"
#include "entity.h"

Renderer::Renderer() : cam_x(0), cam_y(0), spriteloader(false)
{
    background = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    midground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    foreground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);

    //load font
    //gg2 font as placeholder for now i guess
    al_init_font_addon();
    al_init_ttf_addon();
    font = al_load_font("gg2bold.ttf", 12, ALLEGRO_TTF_MONOCHROME);
    if (!font)
    {
      fprintf(stderr, "Could not load 'gg2bold.ttf'.\n");
      throw -1;
    }

    // fps stuff
    lasttime = al_get_time();
}

Renderer::~Renderer()
{
    // Cleanup
    al_destroy_font(font);
    al_shutdown_font_addon();
    al_shutdown_ttf_addon();
    al_destroy_bitmap(background);
    al_destroy_bitmap(midground);
    al_destroy_bitmap(foreground);
}

void Renderer::render(ALLEGRO_DISPLAY *display, Gamestate *state, EntityPtr myself)
{
    // Set camera
    Character *c = state->get<Player>(myself)->getcharacter(state);

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
    for (auto& e : state->entitylist)
    {
        e.second->render(this, state);
    }

    // Set render target to be the display
    al_set_target_backbuffer(display);

    // Clear black
    al_clear_to_color(al_map_rgba(0, 0, 0, 1));

    // Draw the map background first
    state->currentmap->renderbackground(cam_x, cam_y);

    // Then draw each layer
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(midground, 0, 0, 0);
    al_draw_bitmap(foreground, 0, 0, 0);

    // Draw the map wallmask on top of everything, to prevent sprites that go through walls
    state->currentmap->renderwallground(cam_x, cam_y);

    //fps counter mostly borrowed from pygg2
    double frametime = al_get_time() - lasttime;
    lasttime = al_get_time();

    al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, ALLEGRO_ALIGN_LEFT, ("Frametime: " + std::to_string(frametime * 1000) + "ms").c_str());
    al_draw_text(font, al_map_rgb(255, 255, 255), 0, 12, ALLEGRO_ALIGN_LEFT, ("FPS: " + std::to_string((int)(1/frametime))).c_str());
    al_draw_text(font, al_map_rgb(255, 255, 255), 0, 60, ALLEGRO_ALIGN_LEFT, ("pos: " + std::to_string(cam_x+WINDOW_WIDTH/2.0) + " " + std::to_string(cam_y+WINDOW_HEIGHT/2.0)).c_str());
    if (c != 0)
    {
        al_draw_text(font, al_map_rgb(255, 255, 255), 0, 72, ALLEGRO_ALIGN_LEFT, ("hspeed: " + std::to_string(c->hspeed)).c_str());
        al_draw_text(font, al_map_rgb(255, 255, 255), 0, 84, ALLEGRO_ALIGN_LEFT, ("vspeed: " + std::to_string(c->vspeed)).c_str());
    }
    else
    {
        al_draw_text(font, al_map_rgb(255, 255, 255), 0, 72, ALLEGRO_ALIGN_LEFT, "hspeed: 0.000000");
        al_draw_text(font, al_map_rgb(255, 255, 255), 0, 84, ALLEGRO_ALIGN_LEFT, "vspeed: 0.000000");
    }

    al_flip_display();
}
