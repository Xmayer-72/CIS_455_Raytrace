#include "Misc.h"
#include "RaytracingCanvas.h"

int main(int argc, char const *argv[])
{
    RaytracingCanvas c{"", 600, 600};

    Sphere red {{0.0f, -1.0f, 3.0f}, 1.0f};
    Sphere green {{-2.0f, 0.0f, 4.0f}, 1.0f};
    Sphere blue {{2.0f, 0.0f, 4.0f}, 1.0f};
    Sphere white {{0.0f, 1.0f, 6.0f}, 1.0f};

    c.add_sphere(red, Color::red);
    c.add_sphere(green, Color::green);
    c.add_sphere(blue, Color::blue);
    c.add_sphere(white, Color::white);

    Light l1{Light::LightType::Ambient, {0.2f, 0.2f, 0.2f}};
    Light l2(Light::LightType::Directional, {0.2f, 0.2f, 0.2f}, {1, 4, 4});
    Light l3(Light::LightType::Point, {0.2f, 0.2f, 0.6f}, {2, 1, 0});

    c.add_light(l1);
    c.add_light(l2);
    c.add_light(l3);

    while (should_keep_rendering())
    {
        c.clear();

        c.draw();

        c.present();
    }    

    return 0;
}
