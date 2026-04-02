/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#ifndef _MUDLIB_CORE_FACTORY_H_
#define _MUDLIB_CORE_FACTORY_H_

#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <vector>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Forward declarartion to allow specializations.
 */
template<typename, class, class...>
class factory;

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
class MUDLIB_CORE_API_EXPORT factory
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
        registrar()
        {
            factory<AbstractKey, AbstractClass, Args...>::instance()
                .register_registrar(ConcreteKey, creator);
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
        static std::unique_ptr<AbstractClass> creator(Args... args)
        {
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
     * @return a list of all the registered keys.
     */
    std::vector<AbstractKey> keys() const {
        std::vector<AbstractKey> keys;
        for (const auto& [key, value]: _map) {
            keys.push_back(key);
        }
        return keys;
    }

    /**
     * @brief The instance of the factory.
     */
    static factory& instance();

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
     * of a concrete class and is associated to a particular key.
     */
    void register_registrar(AbstractKey key, creator_type creator);

    /**
     * The factory map between the AbstractKey and creator function
     */
    creator_map_type _map;
};

template<typename AbstractKey, class AbstractClass, class... Args>
std::unique_ptr<AbstractClass>
factory<AbstractKey, AbstractClass, Args...>::create(AbstractKey key,
                                                     Args... args)
{
    auto find = _map.find(key);
    if (find == _map.end()) {
        std::stringstream sstr;
        sstr << "no creator registered in factory " << typeid(*this).name();
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
        registrar()
        {
            factory<std::type_index, AbstractClass, Args...>::instance()
                .register_registrar(typeid(ConcreteClass), creator);
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
        static std::unique_ptr<AbstractClass> creator(Args... args)
        {
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
     * @return a list of all the registered keys.
     */
    std::vector<std::type_index> keys() const {
        std::vector<std::type_index> keys;
        for (const auto& [key, value]: _map) {
            keys.push_back(key);
        }
        return keys;
    }

    /**
     * @brief The instance of the factory.
     */
    static factory& instance()
    {
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
     * of a concrete class and is associated to a particular key.
     */
    void register_registrar(std::type_index key, creator_type creator);

    /**
     * The factory map between the std::index key and creator function
     */
    creator_map_type _map;
};

template<class AbstractClass, class... Args>
std::unique_ptr<AbstractClass>
factory<std::type_index, AbstractClass, Args...>::create(std::type_index key,
                                                         Args... args)
{
    auto find = _map.find(key);
    if (find == _map.end()) {
        std::stringstream sstr;
        sstr << "no creator registered in factory " << typeid(*this).name();
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

/**
 * @brief Template class to create a generic factory by class type.
 *
 * The factory template creates concrete instances of an abstract class. The
 * concrete instance to create is defined by a string literal.
 *
 * A factory registar needs to be defined for each concrete class and the
 * associated string literal, that is registered within the factory.
 *
 * @note String literals serving as keys are not passed as template arguments
 * due to a string being a non-type template parameter (NTTP). Instead, the
 * key is passed as an argument to the reistration function instead.
 */
template<class AbstractClass, class... Args>
class factory<std::string, AbstractClass, Args...>
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
        registrar(const std::string& key)
        {
            factory<std::string, AbstractClass, Args...>::instance()
                .register_registrar(key, creator);
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
        static std::unique_ptr<AbstractClass> creator(Args... args)
        {
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
    std::unique_ptr<AbstractClass> create(const std::string& key, Args... args);

    /**
     * @return a list of all the registered keys.
     */
    std::vector<std::string> keys() const {
        std::vector<std::string> keys;
        for (const auto& [key, value]: _map) {
            keys.push_back(key);
        }
        return keys;
    }

    /**
     * @brief The instance of the factory.
     */
    static factory& instance()
    {
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
    typedef std::map<std::string, factory::creator_type> creator_map_type;

    /**
     * @brief Register a registrar that handles the creation of an instance
     * of a concrete class and is associated to a particular key.
     */
    void register_registrar(const std::string& key, creator_type creator);

    /**
     * The factory map between string literal and creator function.
     */
    creator_map_type _map;
};

template<class AbstractClass, class... Args>
std::unique_ptr<AbstractClass>
factory<std::string, AbstractClass, Args...>::create(
        const std::string& key, Args... args)
{
    auto find = _map.find(key);
    if (find == _map.end()) {
        std::stringstream sstr;
        sstr << "no creator registered in factory " << typeid(*this).name();
        throw std::invalid_argument(sstr.str());
    }
    return find->second(std::forward<Args>(args)...);
}

template<class AbstractClass, class... Args>
void
factory<std::string, AbstractClass, Args...>::register_registrar(
        const std::string& key, creator_type creator)
{
    _map[key] = creator;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_FACTORY_H_ */
