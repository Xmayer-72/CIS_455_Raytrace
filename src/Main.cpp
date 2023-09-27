#include "Misc.h"
#include "RaytracingCanvas.h"

int main(int argc, char const *argv[])
{
    RaytracingCanvas c{"", 600, 600};

    Sphere red {{0.0f, -1.0f, 3.0f}, 1.0f};
    Sphere green {{-2.0f, 0.0f, 4.0f}, 1.0f};
    Sphere blue {{2.0f, 0.0f, 4.0f}, 1.0f};

    c.add_sphere(red, Color::red);
    c.add_sphere(green, Color::green);
    c.add_sphere(blue, Color::blue);

    while (should_keep_rendering())
    {
        c.clear();

        c.draw();

        c.present();
    }    

    return 0;
}
