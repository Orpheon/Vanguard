#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdexcept>

int main(int argc, char **argv)
{
    // Initialize Allegro
    if (!al_init())
    {
        throw new std::runtime_error("Allegro initialization failed");
    }

    // Initialize the Allegro Image addon, used to load sprites and maps
    if (!al_init_image_addon())
    {
        throw new std::runtime_error("Allegro image addon initialization failed");
    }

    // Initialize display
    int display_width = 1280, display_height = 1024;
    ALLEGRO_DISPLAY *display = al_create_display(display_width, display_height);

    if(!display)
    {
        throw new std::runtime_error("Could not create display");
    }

    while (true)
    {
        // This only happens with certain sprites in ALLEGRO_VIDEO_BITMAP mode,
        // but in ALLEGRO_MEMORY_BITMAP mode on this laptop it happens consistently for everything, so using that
        al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        // Load png sprite
        ALLEGRO_BITMAP *first_sprite = al_load_bitmap("sprites/heroes/mccree/idle/1_sprite.png");
        
        int w=al_get_bitmap_width(first_sprite), h=al_get_bitmap_height(first_sprite);
        // Now we create a second bitmap of same width and height, and attempt to draw the sprite over to it
        ALLEGRO_BITMAP *second_sprite = al_create_bitmap(w, h);
        al_set_target_bitmap(second_sprite);
        al_draw_bitmap(first_sprite, 0, 0, 0);
        // You'd expect them to be identical after this, but they're not (see picture)
        
        // However, formats and flags are identical
        if (al_get_bitmap_format(first_sprite) != al_get_bitmap_format(second_sprite) or al_get_bitmap_flags(first_sprite)!= al_get_bitmap_flags(second_sprite))
        {
            // None of these conditions ever happen, so it's not format incompatibility
            throw new std::runtime_error("Format incompatibility.");
        }
        
        al_set_target_backbuffer(display); // Draw to display
        al_clear_to_color(al_map_rgba(0, 0, 0, 0)); // Make background black
        al_draw_bitmap(first_sprite, display_width/2.0 - 100, display_height/2.0, 0); // Draw original sprite on the left
        al_draw_bitmap(second_sprite, display_width/2.0 + 100, display_height/2.0, 0); // Draw new sprite on the right

        al_flip_display();
    }
    return 0;
}
