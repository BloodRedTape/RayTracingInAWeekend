#include <core/print.hpp>
#include <core/span.hpp>
#include <core/os/clock.hpp>
#include <core/optional.hpp>
#include <graphics/image.hpp>
#include <core/assert.hpp>
#include "math.hpp"
#include "hittable.cpp"

#include <thread>

constexpr float ViewportHeight = 2;

struct Camera {
    const float Aspect;
    const float ViewportHeight;
    const float ViewportWidth;
    const float FocalLength;
    const Vector3f RayOrigin;
    const Vector3f LowerLeftCorner;
    const Vector3f ViewportSize;

    Camera(int width, int height) :
	    Aspect(width / (float)height),
        ViewportHeight(2),
        ViewportWidth(ViewportHeight * Aspect),
        FocalLength(1.f),
        RayOrigin(0, 0, 0),
        LowerLeftCorner(-ViewportWidth/2.f, -ViewportHeight/2.f, 0),
        ViewportSize(ViewportWidth, ViewportHeight, 0)
    {}

    Ray3f GenRay(float u, float v) const{
        return {RayOrigin, LowerLeftCorner + Vector3f(u, v, 0) * ViewportSize - Vector3f(0, 0, FocalLength)};
    }
};

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

class RayTracer {
private:
    static constexpr size_t s_SamplesCount = 6;

    const Camera m_Camera; 
    std::vector<std::thread> m_Pool;
    const size_t m_ChunksCount;
    const size_t m_ChunkSize;
    const size_t m_ImageWidth;
    const size_t m_ImageHeight;
    HittableList m_List;

    struct Chunk{
        size_t ID = 0;
        size_t Begin = 0;
        size_t End = 0;
        Image *Output = nullptr;
        const HittableList *List = nullptr;
        const Camera *Camera = nullptr;
    };
public:
    RayTracer(int width, int height) :
        m_Camera(width, height),
        m_ChunksCount(std::thread::hardware_concurrency()),
        m_ChunkSize(height / m_ChunksCount),
        m_ImageWidth(width),
        m_ImageHeight(height)
    {
        SX_ASSERT(height % m_ChunksCount == 0);

        m_Pool.reserve(m_ChunksCount);

        m_List.Add(new Sphere({ 0, 0, -1 }, 0.5));
        m_List.Add(new Sphere({ 0, -100.5, -1 }, 100));
        
    }

    Image Trace() {
        Image output(m_ImageWidth, m_ImageHeight);

        for(int i = 0; i<m_ChunksCount; i++){
            Chunk chunk;
            chunk.ID = i;
            chunk.Begin = m_ChunkSize * i;
            chunk.End = chunk.Begin + m_ChunkSize;
            chunk.Output = &output;
            chunk.List = &m_List;
            chunk.Camera = &m_Camera;
            m_Pool.emplace_back(ChunkTrace, chunk);
        }

        for(auto &thread: m_Pool)
            thread.join();

        return output;
    }

private:
    static void ChunkTrace(Chunk chunk) {
        Image &image = *chunk.Output;
        const Camera &camera = *chunk.Camera;
        const HittableList &list = *chunk.List;

        for (int y = chunk.Begin; y < chunk.End; y++) {
            for(int x = 0; x < image.Width(); x++){
                Color pixel = Color::Black;

                for(int i = 0; i < s_SamplesCount; i++){
                
                    Vector2f uv((x + RandomFloat())/(image.Width() - 1.f), (y + RandomFloat())/(image.Height() - 1.f));
                    Ray3f ray = camera.GenRay(uv.x, uv.y);

			        pixel = pixel + RayColor(ray, list, 40) * (1.f / s_SamplesCount);
                }
                pixel.A = 1.f;
                Set(image, pixel, x, y);
            }
        }

    }
};




int StraitXMain(Span<const char*> args){
    Clock cl;
    Image image = RayTracer(1280, 720).Trace();
    Println("Trace took: % seconds", cl.Restart().AsSeconds());
    image.SaveToFile("output.jpg");
    Println("Saving took: % seconds", cl.Restart().AsSeconds());
    return 0;
}
