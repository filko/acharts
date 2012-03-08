#ifndef SOLAR_OBJECT_HH
#define SOLAR_OBJECT_HH 1

#include <libnova/ln_types.h>
#include <memory>
#include <string>

class SolarObject
{
protected:
    SolarObject() = default;

public:
    virtual ~SolarObject() { };
    SolarObject(const SolarObject &) = delete;
    SolarObject & operator=(const SolarObject &) = delete;

    virtual const std::string name() const = 0;

    virtual ln_equ_posn get_equ_coords(double JD) const = 0;
    virtual double get_magnitude(double JD) const = 0;
    virtual double get_sdiam(double JD) const = 0;
};

class SolarObjectManager
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

public:
    SolarObjectManager();
    ~SolarObjectManager();
    SolarObjectManager(const SolarObjectManager &) = delete;
    SolarObjectManager & operator=(const SolarObjectManager &) = delete;

    const std::shared_ptr<const SolarObject> get(const std::string & name) const;
    void put(const std::shared_ptr<const SolarObject> & object);
};

#endif
