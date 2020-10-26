#pragma once

#include "hitable.h"

class sphere: public hitable {
  public:
    sphere() {}
    sphere(vec3 cen, double r, std::shared_ptr<material> m) : 
      center(cen), radius(r), mat_ptr_(m) {};
    virtual bool hit(const ray& r, double tmin, double tmax, 
        hit_record& rec) const;
    vec3 center;
    double radius;
    std::shared_ptr<material> mat_ptr_;
};

bool sphere::hit(const ray& r, double t_min, double t_max, 
    hit_record& rec) const {
  vec3 oc = r.origin() - center;
  double a = dot(r.direction(), r.direction());
  double b = dot(oc, r.direction());
  double c = dot(oc, oc) - radius*radius;
  double descriminant = b*b - a*c;
  if (descriminant > 0) {
    double temp = (-b - sqrt(b*b-a*c))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p =  r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr_;
      return true;
    }
    temp = (-b + sqrt(b*b-a*c))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr_;
      return true;
    }
  }
  return false;
}

