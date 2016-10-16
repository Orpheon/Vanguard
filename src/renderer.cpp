#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>
#include <vector>
#include <string>

#include "renderer.h"
#include "global_constants.h"
#include "entity.h"

Renderer::Renderer(ALLEGRO_FONT *font_) : cam_x(0), cam_y(0), spriteloader(false), font(font_)
{
    background = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    midground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    foreground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);

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


    if (c != 0)
    {
        // Experimental healthbar
        double healthalpha = 1.0;
        double lack_healthalpha = 0.4;


        // Lucio shield
        // al_premul_rgba_f(242/255.0, 197/255.0, 84/255.0, healthalpha)
        // Torb armor
        // al_premul_rgba_f(69/255.0, 122/255.0, 255/255.0, healthalpha)

        // Normal, armor, shields
        ALLEGRO_COLOR EXISTING_HEALTH[] = { al_premul_rgba_f(225/255.0, 225/255.0, 225/255.0, healthalpha),
                                            al_premul_rgba_f(237/255.0, 223/255.0, 132/255.0, healthalpha),
                                            al_premul_rgba_f(101/255.0, 206/255.0, 240/255.0, healthalpha)};
        ALLEGRO_COLOR LACKING_HEALTH[] = {  al_premul_rgba_f(225/255.0, 225/255.0, 225/255.0, lack_healthalpha),
                                            al_premul_rgba_f(237/255.0, 223/255.0, 132/255.0, lack_healthalpha),
                                            al_premul_rgba_f(101/255.0, 206/255.0, 240/255.0, lack_healthalpha)};

        Health maxhp = c->getmaxhp();
        Health hp = c->hp;
        double x;
        float r[8]; // Array used to pass the polygon data for the actual drawing

        // Parameters
        int width = 20;
        int height = width;
        int space = width/9.0;
        double slant = 0.3;
        double y = 6.0*WINDOW_HEIGHT/7.0;
        double start_x = WINDOW_WIDTH/6.0;

        // Draw first normal health, then armor, then shields
        for (int healthtype=0; healthtype<3; ++healthtype)
        {
            int nrects = 0;
            double hppercent = 1.0;
            if (healthtype == 0)
            {
                nrects = std::ceil(maxhp.normal/25.0);
                if (nrects == 0)
                {
                    continue;
                }
                hppercent = hp.normal/maxhp.normal;
            }
            else if (healthtype == 1)
            {
                nrects = std::ceil(maxhp.armor/25.0);
                if (nrects == 0)
                {
                    continue;
                }
                hppercent = hp.armor/maxhp.armor;
            }
            else if (healthtype == 2)
            {
                nrects = std::ceil(maxhp.shields/25.0);
                if (nrects == 0)
                {
                    continue;
                }
                hppercent = hp.shields/maxhp.shields;
            }

            // Full existing health boxes
            for (int i=0; i<std::floor(nrects*hppercent); ++i)
            {
                x = start_x + i*width + (i-1)*space;
                r[0] = x+height*slant;
                r[1] = y;

                r[2] = x;
                r[3] = y+height;

                r[4] = x+width;
                r[5] = y+height;

                r[6] = x+width+height*slant;
                r[7] = y;
                al_draw_filled_polygon(r, 4, EXISTING_HEALTH[healthtype]);
            }
            // Interface between existing and lacking health
            double leftover = nrects*hppercent - std::floor(nrects*hppercent);
            if (leftover > 0.0)
            {
                // Draw the half-rectangle
                x = start_x + std::floor(nrects*hppercent)*width + (std::floor(nrects*hppercent)-1)*space;
                r[0] = x+height*slant;
                r[1] = y;

                r[2] = x;
                r[3] = y+height;

                r[4] = x+width*leftover;
                r[5] = y+height;

                r[6] = x+width*leftover+height*slant;
                r[7] = y;
                al_draw_filled_polygon(r, 4, EXISTING_HEALTH[healthtype]);

                // Draw the other half rectangle
                r[0] = x+width*leftover+height*slant;
                r[1] = y;

                r[2] = x+width*leftover;
                r[3] = y+height;

                r[4] = x+width;
                r[5] = y+height;

                r[6] = x+width+height*slant;
                r[7] = y;
                al_draw_filled_polygon(r, 4, LACKING_HEALTH[healthtype]);
            }
            // Full lacking health boxes
            for (int i=std::ceil(nrects*hppercent); i<nrects; ++i)
            {
                x = start_x + i*width + (i-1)*space;
                r[0] = x+height*slant;
                r[1] = y;

                r[2] = x;
                r[3] = y+height;

                r[4] = x+width;
                r[5] = y+height;

                r[6] = x+width+height*slant;
                r[7] = y;
                al_draw_filled_polygon(r, 4, LACKING_HEALTH[healthtype]);
            }
            // Outline
            for (int i=0; i<nrects; ++i)
            {
                x = start_x + i*width + (i-1)*space;
                r[0] = x+height*slant;
                r[1] = y;

                r[2] = x;
                r[3] = y+height;

                r[4] = x+width;
                r[5] = y+height;

                r[6] = x+width+height*slant;
                r[7] = y;
                al_draw_polygon(r, 4, ALLEGRO_LINE_JOIN_ROUND, EXISTING_HEALTH[healthtype], 0, 0);
            }
            // Offset starting position for the next health
            start_x += nrects*(width + space);
        }
    }

    al_flip_display();
}
