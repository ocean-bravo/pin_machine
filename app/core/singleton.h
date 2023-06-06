#pragma once

template <typename T>
class Singleton
{
public:
    static T& instance()
    {
        static T instance;
        return instance;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton&) = delete;            // Prevent copy-construction
    Singleton& operator=(const Singleton&) = delete; // Prevent assignment
};
