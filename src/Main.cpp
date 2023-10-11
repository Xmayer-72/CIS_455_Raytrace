#include "Misc.h"
#include "RaytracingCanvas.h"

int main(int argc, char const *argv[])
{
    RaytracingCanvas c{"", 600, 600, 3};

    Sphere red {{0.0f, -1.0f, 3.0f}, 1.0f};
    Sphere green {{-2.0f, 0.0f, 4.0f}, 1.0f};
    Sphere blue {{2.0f, 0.0f, 4.0f}, 1.0f};
    Sphere white {{0.0f, 1.0f, 6.0f}, 1.0f};
    Sphere yellow {{0.0f, -8.0f, 0.0f}, 7.0f};

    c.add_sphere(red, Color::red, 500.0f, 0.2f);
    c.add_sphere(green, Color::green, 10.0f, 0.4f);
    c.add_sphere(blue, Color::blue, 500.0f, 0.3f);
    c.add_sphere(yellow, Color::yellow, 500.0f, 0.5f);

    Light l1{Light::LightType::Ambient, {0.2f, 0.2f, 0.2f}};
    Light l2(Light::LightType::Directional, {0.2f, 0.2f, 0.2f}, {1, 4, 4});
    Light l3(Light::LightType::Point, {0.6f, 0.6f, 0.9f}, {2, 1, 0});

    c.add_light(l1);
    c.add_light(l2);
    c.add_light(l3);

    c.set_camera_orient(Mat::get_rotation_matrix(35, {1, 0, 0}));

    float camera_angle = 0.0f;
    float light1_angle = 0.0f;
    float light2_angle = 0.0f;
    float sphere1_angle = 0.0f;
    float sphere2_angle = 0.0f;

    while (should_keep_rendering())
    {
        auto camera_x_pos = 2.0f * std::sin(camera_angle);
        auto camera_z_pos = 2.0f * std::cos(camera_angle) - 15.0f;

        auto sphere1_x_pos = 4.0f * std::sin(sphere1_angle) - 2.0f;
        auto sphere1_z_pos = 4.0f * std::cos(sphere1_angle) + 4.0f;

        auto sphere2_x_pos = 5.0f * std::sin(sphere1_angle) + 2.0f;
        auto sphere2_z_pos = 5.0f * std::cos(sphere1_angle) + 4.0f;

        c.set_camera_pos({camera_x_pos, 8.0f, camera_z_pos});

        c.get_spheres()[1].center = {sphere1_x_pos, 1, sphere1_z_pos};
        c.get_spheres()[2].center = {sphere2_x_pos, 2, sphere2_z_pos};

        c.clear();

        c.draw();

        c.present();

        camera_angle  += 0.1f;
        light1_angle  += 0.2f;
        light2_angle  += 0.3f;
        sphere1_angle += 0.15f; 
        sphere2_angle += 0.25f; 
    }    

    return 0;
}
