#include "math.hpp"
#include <core/optional.hpp>
#include <core/unique_ptr.hpp>
#include <core/list.hpp>

struct HitRecord{
    Vector3f Normal = {0, 0, 0};
    float T = 0.f;
};

bool InRange(float value, float min, float max) {
    return value >= min && value <= max;
}

class Hittable{
public:
    virtual Optional<HitRecord> Hit(const Ray3f &ray, float min_t, float max_t)const = 0;
};

using HittableRef = UniquePtr<Hittable>;

class Sphere: public Hittable{
private:
    Vector3f m_Position;
    float m_Radius;
public:
    Sphere(Vector3f position, float radius):
        m_Position(position),
        m_Radius(radius)
    {}

    Optional<HitRecord> Hit(const Ray3f &r, float min_t, float max_t)const override{
        Vector3f o = r.Origin() - m_Position;
        float a = Dot(r.Direction(), r.Direction());
        float b = 2*Dot(o, r.Direction());
        float c = Dot(o, o) - (m_Radius * m_Radius);

        float d = b*b - 4*a*c;
        if(d < 0)
            return {};
        
        float t1 = float(-b + Math::Sqrt(d)) / (2.0f * a);
        float t2 = float(-b - Math::Sqrt(d)) / (2.0f * a);
        
        float t = t2;
        if (!InRange(t, min_t, max_t)) {
            t = t1;
            if(!InRange(t, min_t, max_t))
                return {};
        }

        return {{Normalize(r.At(t) - m_Position), t}};
    }
};

class HittableList: public List<HittableRef>{
public:
    Optional<HitRecord> Hit(const Ray3f &r, float min_t, float max_t)const{
        Optional<HitRecord> best;
        float shortest_t = INFINITY;

        for(const auto &hittable: *this){
            auto result = hittable->Hit(r, 0, shortest_t);

            if (result.HasValue()) {
                SX_ASSERT(result.Value().T >= 0 && result.Value().T <= shortest_t);

                shortest_t = result.Value().T;
                best = result;
            }
        }
        return best;
    }
};
