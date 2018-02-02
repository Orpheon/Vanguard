#include <cstdio>
#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <engine.h>

#include "renderer.h"
#include "global_constants.h"
#include "entity.h"
#include "configloader.h"
#include "global.h"
#include "visuals/hud.h"
#include "visuals/defaulthud.h"

Renderer::Renderer() : cam_x(0), cam_y(0), zoom(1), myself(0), WINDOW_WIDTH(0), WINDOW_HEIGHT(0), spriteloader(false)
{
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    background = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    midground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    foreground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    surfaceground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);

    // fps stuff
    lasttime = al_get_time();

    font20 = al_load_font("Vanguard Text Font.ttf", 20, ALLEGRO_TTF_MONOCHROME);
    font12 = al_load_font("Vanguard Text Font.ttf", 12, ALLEGRO_TTF_MONOCHROME);
    font8 = al_load_font("Vanguard Text Font.ttf", 8, ALLEGRO_TTF_MONOCHROME);

    currenthud = std::unique_ptr<Hud>(new DefaultHud());
}

Renderer::~Renderer()
{
    // Cleanup
    al_destroy_font(font20);
    al_destroy_font(font12);
    al_destroy_font(font8);
    al_destroy_bitmap(background);
    al_destroy_bitmap(midground);
    al_destroy_bitmap(foreground);
    al_destroy_bitmap(surfaceground);
}

void Renderer::render(ALLEGRO_DISPLAY *display, Gamestate &state, EntityPtr myself_, Networker &networker)
{
    myself = myself_;

    if (WINDOW_WIDTH != al_get_display_width(display) or WINDOW_HEIGHT != al_get_display_height(display) or changedzoom)
    {
        al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
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

        zoom = 1.0*WINDOW_WIDTH / VIEWPORT_WIDTH;
        spriteloader.setzoom(zoom);

        al_destroy_font(font20);
        al_destroy_font(font12);
        al_destroy_font(font8);
        font20 = al_load_font("Vanguard Text Font.ttf", 20 * zoom, ALLEGRO_TTF_MONOCHROME);
        font12 = al_load_font("Vanguard Text Font.ttf", 12 * zoom, ALLEGRO_TTF_MONOCHROME);
        font8 = al_load_font("Vanguard Text Font.ttf", 8 * zoom, ALLEGRO_TTF_MONOCHROME);

        changedzoom = false;
    }

    // Set camera
    if (state.exists(myself))
    {
        Player &p = state.get<Player>(myself);
        if (state.exists(p.character))
        {
            Character &c = p.getcharacter(state);
            cam_x = c.x - VIEWPORT_WIDTH/2.0;
            cam_y = c.y - WINDOW_HEIGHT/zoom/2.0;
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
    for (auto &e : state.entitylist)
    {
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->render(*this, state);
        }
    }

    // Set render target to be the display
    al_set_target_backbuffer(display);

    // Clear black
    al_clear_to_color(al_map_rgba(0, 0, 0, 1));

    // Draw the map background first
    state.currentmap->renderbackground(*this);

    // Then draw each layer
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(midground, 0, 0, 0);
    al_draw_bitmap(foreground, 0, 0, 0);

    // Draw the map wallmask on top of everything, to prevent sprites that go through walls
    state.currentmap->renderwallground(*this);

    // Draw the final layer on top of even that, for certain things like character healthbars
    al_draw_bitmap(surfaceground, 0, 0, 0);

    if (state.exists(myself))
    {
        currenthud->render(*this, state, state.get<Player>(myself));
    }

    al_flip_display();
}

ALLEGRO_DISPLAY* Renderer::createnewdisplay()
{
    //default display values are set on header file
    int display_width, display_height, display_type;

    display_width = Global::settings().at("Display resolution").at(0);
    display_height = Global::settings().at("Display resolution").at(1);
    display_type = Global::settings().at("Display type");

    al_set_new_display_option(ALLEGRO_VSYNC, Global::settings().at("Vsync"), ALLEGRO_SUGGEST);

    ALLEGRO_DISPLAY *display;
    al_set_new_display_flags(ALLEGRO_OPENGL | display_type);
    display = al_create_display(display_width, display_height);

    if(!display)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not create display");
    }
    return display;
}

void Renderer::changeviewport(int newsize)
{
    changedzoom = true;
    VIEWPORT_WIDTH = newsize;
}