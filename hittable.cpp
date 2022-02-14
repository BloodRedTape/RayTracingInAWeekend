#include "math.hpp"
#include <core/optional.hpp>
#include <core/unique_ptr.hpp>
#include <core/list.hpp>

struct HitRecord{
    Vector3f Normal = {0, 0, 0};
    float T = 0.f;
};

class Hittable{
public:
    virtual Optional<HitRecord> Hit(const Ray3f &ray)const = 0;
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

    Optional<HitRecord> Hit(const Ray3f &r)const override{
        Vector3f o = r.Origin() - m_Position;
        float a = Dot(r.Direction(), r.Direction());
        float b = 2*Dot(o, r.Direction());
        float c = Dot(o, o) - (m_Radius * m_Radius);

        float d = b*b - 4*a*c;
        if(d < 0)
            return {};

        float t = float(-b - Math::Sqrt(d)) / (2.0f * a);

        return {{Normalize(r.At(t) - m_Position), t}};
    }
};

class HittableList: public List<HittableRef>, public Hittable{
public:
    Optional<HitRecord> Hit(const Ray3f &r)const override{
        Optional<HitRecord> best;

        for(const auto &hittable: *this){
            auto result = hittable->Hit(r);

            if(!best.HasValue() || (result.HasValue() && best.Value().T > result.Value().T))
                best = result; 
        }
        return best;
    }
};
