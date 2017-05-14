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

Renderer::Renderer() : cam_x(0), cam_y(0), zoom(1), myself(0), WINDOW_WIDTH(0), WINDOW_HEIGHT(0), spriteloader(false)
{
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
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

void Renderer::render(ALLEGRO_DISPLAY *display, Gamestate &state, EntityPtr myself_)
{
    myself = myself_;

    if (WINDOW_WIDTH != al_get_display_width(display) or WINDOW_HEIGHT != al_get_display_height(display))
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
    }
    
    al_set_target_backbuffer(display);
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *first_sprite = al_load_bitmap("sprites/heroes/mccree/idle/1_sprite.png");
    int w=al_get_bitmap_width(first_sprite), h=al_get_bitmap_height(first_sprite);
    ALLEGRO_BITMAP *second_sprite = al_create_bitmap(w, h);
    al_set_target_bitmap(second_sprite);
    al_draw_bitmap(first_sprite, 0, 0, 0);
    al_set_target_backbuffer(display);
    
    if (al_get_bitmap_format(first_sprite) != al_get_bitmap_format(second_sprite) or al_get_bitmap_flags(first_sprite)!= al_get_bitmap_flags(second_sprite))
    {
        Global::logging().panic(__FILE__, __LINE__, "Format incompatibility.");
    }
    
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_bitmap(first_sprite, WINDOW_WIDTH/2.0 - 100, WINDOW_HEIGHT/2.0, 0);
    al_draw_bitmap(second_sprite, WINDOW_WIDTH/2.0 + 100, WINDOW_HEIGHT/2.0, 0);

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
