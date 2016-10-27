#include <allegro5/allegro.h>
#include <cstdio>
#include <vector>
#include <string>

#include "renderer.h"
#include "global_constants.h"
#include "entity.h"

Renderer::Renderer() : WINDOW_WIDTH(1280), WINDOW_HEIGHT(720), spriteloader(false)
{
    background = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    midground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    foreground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    surfaceground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);

    // fps stuff
    lasttime = al_get_time();

    font20 = al_load_font("Vanguard Main Font.ttf", 20, ALLEGRO_TTF_MONOCHROME);
    font10 = al_load_font("Vanguard Main Font.ttf", 10, ALLEGRO_TTF_MONOCHROME);
    font6 = al_load_font("Vanguard Main Font.ttf", 6, ALLEGRO_TTF_MONOCHROME);
    gg2font = al_load_font("gg2bold.ttf", 12, ALLEGRO_TTF_MONOCHROME);
}

Renderer::~Renderer()
{
    // Cleanup
    al_destroy_font(font20);
    al_destroy_font(font10);
    al_destroy_font(font6);
    al_destroy_font(gg2font);
    al_destroy_bitmap(background);
    al_destroy_bitmap(midground);
    al_destroy_bitmap(foreground);
    al_destroy_bitmap(surfaceground);
}

void Renderer::render(ALLEGRO_DISPLAY *display, Gamestate *state, EntityPtr myself)
{
    if (WINDOW_WIDTH != al_get_display_width(display) or WINDOW_HEIGHT != al_get_display_height(display))
    {
        WINDOW_WIDTH = al_get_display_width(display);
        WINDOW_HEIGHT = al_get_display_height(display);

        al_destroy_bitmap(background);
        al_destroy_bitmap(midground);
        al_destroy_bitmap(foreground);
        al_destroy_bitmap(surfaceground);

        background = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
        midground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
        foreground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    // Set up transformations
    ALLEGRO_TRANSFORM *trans = const_cast<ALLEGRO_TRANSFORM*>(al_get_current_transform());
    al_identity_transform(trans);

    // Calculate zoom
    double zoom = 1.0*WINDOW_WIDTH / VIEWPORT_WIDTH;
    al_scale_transform(trans, zoom, zoom);

    // Set camera
    Player *p = state->get<Player>(myself);
    Character *c = 0;
    if (p != 0)
    {
        c = p->getcharacter(state);
        if (c != 0)
        {
            cam_x = c->x - WINDOW_WIDTH/2.0;
            cam_y = c->y - WINDOW_HEIGHT/2.0;
        }
    }

    al_set_target_bitmap(background);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(midground);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(foreground);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(surfaceground);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    // Go through all objects and let them render themselves on the layers
    for (auto& e : state->entitylist)
    {
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->render(this, state);
        }
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

    // Draw the final layer on top of even that, for certain things like character healthbars
    al_draw_bitmap(surfaceground, 0, 0, 0);

    // Go back to no scaling
    al_identity_transform(trans);


    //fps counter mostly borrowed from pygg2
    double frametime = al_get_time() - lasttime;
    lasttime = al_get_time();

    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 0, ALLEGRO_ALIGN_LEFT, ("Frametime: " + std::to_string(frametime * 1000) + "ms").c_str());
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 12, ALLEGRO_ALIGN_LEFT, ("FPS: " + std::to_string((int)(1/frametime))).c_str());
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 60, ALLEGRO_ALIGN_LEFT, ("pos: " + std::to_string(cam_x+WINDOW_WIDTH/2.0) + " " + std::to_string(cam_y+WINDOW_HEIGHT/2.0)).c_str());
    if (c != 0)
    {
        al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 72, ALLEGRO_ALIGN_LEFT, ("hspeed: " + std::to_string(c->hspeed)).c_str());
        al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 84, ALLEGRO_ALIGN_LEFT, ("vspeed: " + std::to_string(c->vspeed)).c_str());
    }
    else
    {
        al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 72, ALLEGRO_ALIGN_LEFT, "hspeed: 0.000000");
        al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 84, ALLEGRO_ALIGN_LEFT, "vspeed: 0.000000");
    }
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 96, ALLEGRO_ALIGN_LEFT, ("#Players: " + std::to_string(state->playerlist.size())).c_str());


    if (c != 0)
    {
        c->drawhud(this, state);
    }

    al_flip_display();
}
