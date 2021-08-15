#ifndef _MUDLIB_CORE_FACTORY_H_
#define _MUDLIB_CORE_FACTORY_H_

#include <map>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <typeindex>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Forward declarartion to allow specializations.
 */
template <typename, class, class...> class factory;

/**
 * @brief Template class to create a generic factory by enumeration key.
 *
 * The factory template creates concrete instances of an abstract class. The
 * concrete instance to create is defined by a value of an abstract key.
 *
 * A factory registar needs to be defined for each concrete class and the
 * associated concrete key, that is registered within the factory.
 */
template<typename AbstractKey, class AbstractClass, class... Args>
class factory
{
public:
    /**
     * @brief The registrar that defines a concerete class and associated
     * concrete key to be registered in the factory.
     */
    template<AbstractKey ConcreteKey, class ConcreteClass>
    class registrar
    {
    public:
        /**
         * @brief Constructor that registers the class and key into the
         * factory.
         */
        registrar() {
            factory<AbstractKey, AbstractClass, Args...>::instance().
            register_registrar(ConcreteKey, creator);
        }

        /**
         * @brief Destructo.
         */
        ~registrar() {}

    private:
        /**
         * @brief The creator function that creates an instance of the
         * concrete class.
         */
        static std::unique_ptr<AbstractClass>
        creator(Args... args) {
            return std::unique_ptr<AbstractClass>(
                            new ConcreteClass(std::forward<Args>(args)...));
        }

        /**
         * Not copyable.
         */
        registrar(const registrar&) = delete;
        registrar& operator=(const registrar&) = delete;

        /**
         * Not moveable.
         */
        registrar(registrar&&) = delete;
        registrar& operator=(registrar&&) = delete;
    };

    /**
     * Constructor.
     */
    factory() {}

    /**
     * Destructor.
     */
    ~factory() {}

    /**
     * @brief Create function to return a new instance associated to the
     * key.
     */
    std::unique_ptr<AbstractClass> create(AbstractKey key, Args... args);

    /**
     * @brief The instance of the factory.
     */
    static factory&
    instance() {
        static factory _instance;
        return _instance;
    }

    /**
     * Not copyable.
     */
    factory(const factory&) = delete;
    factory& operator=(const factory&) = delete;

    /**
     * Not moveable.
     */
    factory(factory&&) = delete;
    factory& operator=(factory&&) = delete;

private:
    /**
     * Type definition of the creator function.
     */
    typedef std::function<std::unique_ptr<AbstractClass>(Args...)> creator_type;

    /**
     * Type definition of the concrete key and concrete class creator function
     * mapping.
     */
    typedef std::map<AbstractKey, factory::creator_type> creator_map_type;

    /**
     * @brief Register a registrar that handles the creation of an instance
     * of aconcrete * class and is associated to a particular key..
     */
    void register_registrar(AbstractKey key, creator_type creator);

    creator_map_type _map;
};

template<typename AbstractKey, class AbstractClass, class... Args>
std::unique_ptr<AbstractClass>
factory<AbstractKey, AbstractClass, Args...>::create(
        AbstractKey key, Args... args)
{
    auto find = _map.find(key);
    if (find == _map.end())
    {
        std::stringstream sstr;
        sstr << "no creator registered in factory "
                << typeid(*this).name();
        throw std::invalid_argument(sstr.str());
    }
    return find->second(std::forward<Args>(args)...);
}

template<typename AbstractKey, class AbstractClass, class... Args>
void
factory<AbstractKey, AbstractClass, Args...>::register_registrar(
        AbstractKey key, creator_type creator)
{
    _map[key] = creator;
}

/**
 * @brief Template class to create a generic factory by class type.
 *
 * The factory template creates concrete instances of an abstract class. The
 * concrete instance to create is defined by a value of an abstract key.
 *
 * A factory registar needs to be defined for each concrete class and the
 * associated concrete key, that is registered within the factory.
 */
template<class AbstractClass, class... Args>
class factory<std::type_index, AbstractClass, Args...>
{
public:
    /**
     * @brief The registrar that defines a concerete class and associated
     * concrete key to be registered in the factory.
     */
    template<class ConcreteClass>
    class registrar
    {
    public:
        /**
         * @brief Constructor that registers the class and key into the
         * factory.
         */
        registrar() {
            factory<std::type_index, AbstractClass, Args...>::instance().
            register_registrar(typeid(ConcreteClass), creator);
        }

        /**
         * @brief Destructo.
         */
        ~registrar() {}

    private:
        /**
         * @brief The creator function that creates an instance of the
         * concrete class.
         */
        static std::unique_ptr<AbstractClass>
        creator(Args... args) {
            return std::unique_ptr<AbstractClass>(
                            new ConcreteClass(std::forward<Args>(args)...));
        }

        /**
         * Not copyable.
         */
        registrar(const registrar&) = delete;
        registrar& operator=(const registrar&) = delete;

        /**
         * Not moveable.
         */
        registrar(registrar&&) = delete;
        registrar& operator=(registrar&&) = delete;
    };

    /**
     * Constructor.
     */
    factory() {}

    /**
     * Destructor.
     */
    ~factory() {}

    /**
     * @brief Create function to return a new instance associated to the
     * key.
     */
    std::unique_ptr<AbstractClass> create(std::type_index key, Args... args);

    /**
     * @brief The instance of the factory.
     */
    static factory&
    instance() {
        static factory _instance;
        return _instance;
    }

    /**
     * Not copyable.
     */
    factory(const factory&) = delete;
    factory& operator=(const factory&) = delete;

    /**
     * Not moveable.
     */
    factory(factory&&) = delete;
    factory& operator=(factory&&) = delete;

private:
    /**
     * Type definition of the creator function.
     */
    typedef std::function<std::unique_ptr<AbstractClass>(Args...)> creator_type;

    /**
     * Type definition of the concrete key and concrete class creator function
     * mapping.
     */
    typedef std::map<std::type_index, factory::creator_type> creator_map_type;

    /**
     * @brief Register a registrar that handles the creation of an instance
     * of aconcrete * class and is associated to a particular key..
     */
    void register_registrar(std::type_index key, creator_type creator);

    creator_map_type _map;
};

template<class AbstractClass, class... Args>
std::unique_ptr<AbstractClass>
factory<std::type_index, AbstractClass, Args...>::create(
        std::type_index key, Args... args)
{
    auto find = _map.find(key);
    if (find == _map.end())
    {
        std::stringstream sstr;
        sstr << "no creator registered in factory "
                << typeid(*this).name();
        throw std::invalid_argument(sstr.str());
    }
    return find->second(std::forward<Args>(args)...);
}

template<class AbstractClass, class... Args>
void
factory<std::type_index, AbstractClass, Args...>::register_registrar(
        std::type_index key, creator_type creator)
{
    _map[key] = creator;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_FACTORY_H_ */

