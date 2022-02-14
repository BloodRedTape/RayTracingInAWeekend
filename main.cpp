#include <core/print.hpp>
#include <core/span.hpp>
#include <core/os/clock.hpp>
#include <core/optional.hpp>
#include <graphics/image.hpp>
#include <core/assert.hpp>
#include "math.hpp"
#include "hittable.cpp"

constexpr float ViewportHeight = 2;




void Set(Image &image, Color color, size_t x, size_t y){
    image.Set(color, x, image.Height() - y - 1);
}

Color RayColor(const Ray3f &ray, const HittableList &list, float depth){
    if(depth <= 0)
        return Color::Black;

    auto result = list.Hit(ray, 0, INFINITY);
    if(result.HasValue()){
        HitRecord rec = result;
        Vector3f point = ray.At(rec.T);
        return 0.5 * RayColor(Ray3f(point, point + rec.Normal + RandomVector3fInUnitSphere()), list, depth - 1);

        Vector3f normal = result.Value().Normal;
        Vector3f color = (normal + 1.0f) * 0.5f;
        return Color(color.x, color.y, color.z, 1.f);
    }

    float v = (ray.Direction().y + 1.f) * 0.5f;
    return Lerp(Color::White, Color(0.5, 0.7, 1.f, 1.f), v);
}


void Trace(Image &image){
    const float aspect = image.Width() / float(image.Height());
    const float viewport_height = ViewportHeight;
    const float viewport_width  = viewport_height * aspect;
    const float focal_length = 1.0f;
    const Vector3f ray_origin{0, 0, 0};
    const Vector3f lower_left_corner = Vector3f(-viewport_width/2, -viewport_height/2, 0);
    const Vector3f viewport_size{viewport_width, viewport_height, 0};


    HittableList list;
    list.Add(new Sphere({0, 0, -1}, 0.5));
    //list.Add(new Sphere({0,-100.5,-1}, 100));
    list.Add(new Sphere({0, -100.5, -1}, 100));

    for(int y = 0; y<image.Height(); y++){
        for(int x = 0; x<image.Width(); x++){
            Vector2f uv(x/(image.Width() - 1.f), y/(image.Height() - 1.f));

            Ray3f ray{ray_origin, lower_left_corner + Vector3f(uv.x, uv.y, 0) * viewport_size - Vector3f(0, 0, focal_length)};

            Color pixel = RayColor(ray, list, 40);

            Set(image, pixel, x, y);
        }
    }
}

int StraitXMain(Span<const char*> args){
    Image image(1920, 1080);

    Clock cl;
    Trace(image);

    Println("Tracing took: % ms", cl.Restart().AsMilliseconds());

    Result res = image.SaveToFile("output.jpg");

    Println("Saving  took: % ms", cl.Restart().AsMilliseconds());

    return 0;
}
