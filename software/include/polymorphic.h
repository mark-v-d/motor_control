#ifndef POLYMORPHIC_H
#define POLYMORPHIC_H

#include <utility>
#include <new>
#include <typeinfo>
#include <cstring>

template <typename base_t, int max_size>
class polymorphic_t {
    char buffer[max_size];
    char name[60];

protected:
    /* polymorphic_t cannot be copied or moved, this is only to make
       the factory function "make" work, which due to mandatory copy
       elision will function correctly.
    */
    polymorphic_t(polymorphic_t &&o)=default;
    polymorphic_t(void):buffer{0} {}

    template <typename T, typename... SArgs>
    void set_initial(SArgs&&... args) {
	static_assert(!(max_size&3), "max_size must be integer multiple of 4");
	static_assert(sizeof(T)<max_size, "Variable too large");
	static_assert(std::is_base_of<base_t, T>::value,
	    "Statemachine and state not compatible");

	strncpy(name,typeid(T).name(),sizeof(name));
	T *active=reinterpret_cast<T*>(buffer);
	new(active)T(std::forward<SArgs>(args)...);
    }
public:
    ~polymorphic_t(void) {
	auto active=reinterpret_cast<base_t*>(buffer);
	active->~base_t();
    }

    template <typename T, typename... SArgs>
    __attribute__ ((noinline))
    void set(SArgs&&... args) {
	auto active=reinterpret_cast<base_t*>(buffer);
	active->~base_t();
	set_initial<T>(std::forward<SArgs>(args)...);
    }

    template <typename T, typename... SArgs>
    static polymorphic_t make(SArgs&&... args) {
	polymorphic_t result;
	result.set_initial<T>(std::forward<SArgs>(args)...);
	return result;
    }

    base_t *operator->() { return reinterpret_cast<base_t*>(buffer); }

    template <typename T>
    T *get(void) {
	auto p=reinterpret_cast<base_t*>(buffer);
	return dynamic_cast<T*>(p);
    }
};

#endif
