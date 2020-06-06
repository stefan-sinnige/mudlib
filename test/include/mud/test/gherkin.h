#ifndef _MUDLIB_TEST_GHERKIN_H_
#define _MUDLIB_TEST_GHERKIN_H_

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <mud/test/ns.h>
#include <mud/test/exception.h>

BEGIN_MUDLIB_TEST_NS

/*
 * @brief Streaming operator for enumerated values, using their underlying
 * type to output the value.
 */
template<typename T>
std::ostream&
operator<<(
        typename std::enable_if<std::is_enum<T>::value,
        std::ostream>::type& ostr,
        const T& e)
{
    return ostr << static_cast<typename std::underlying_type<T>::type>(e);
}

/*
 * Forward declararions
 */
template<typename Context> class _scenario;
template<typename Context> class _feature;

/**
 * @brief Define the intial context of the scenario.
 *
 * A 'given' step defines a particular initial condition of a scenario that
 * is required for the additional steps to run successfully. Multiple of these
 * 'given' conditions can be added to a scenario by using the @c And method.
 */
template<typename Context>
class _given
{
public:
    typedef Context context_type;
    typedef std::function<void(context_type&)> function_type;

    /**
     * @brief Constructor, referencing a predefined 'given' step.
     * @param feature [in] The associated feature.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    _given(_feature<context_type>& feature, const std::string& id);

    /**
     * @brief Constructor, defining a new 'given' step.
     * @param feature [in] The associated feature.
     * @param id [in] The description ID.
     * @param func [in] The function detailing the initial condition of the
     *        @c context_type.
     */
    _given(_feature<context_type>& feature,
            const std::string& id,
            function_type func);

    /**
     * @brief Add an additional predefined 'given' step.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    _given<context_type>& And(const std::string&);

    /**
     * @brief Add an additional new 'given' step.
     * @param id [in] The feature-unique ID referencing the predefined step.
     * @param func [in] The function detailing the initial condition of the
     *        @c context_type.
     */
    _given<context_type>& And(const std::string&, function_type func);

    /**
     * @brief Execute the 'given' step, and all its chained 'given' steps.
     */
    void operator()(context_type& ctx);

    /**
     * @return The ID (description) of the 'given' step.
     */
    const std::string& id() const;

    /**
     * @brief Dump the 'given' step and it's chain to standard output.
     */
    void dump(const char* prefix = "Given");

private:
    _feature<context_type>& _feature;
    std::string _id;
    function_type _func;
    mud::test::_given<context_type>* _chain;
};

template<typename Context>
mud::test::_given<Context>::_given(
        ::mud::test::_feature<context_type>& feature,
        const std::string& id)
    : _feature(feature), _id(id),
      _func(nullptr), _chain(nullptr)
{
}

template<typename Context>
mud::test::_given<Context>::_given(
        mud::test::_feature<context_type>& feature,
        const std::string& id,
        function_type func)
    : _feature(feature), _id(id),
      _func(func), _chain(nullptr)
{
}

template<typename Context>
mud::test::_given<Context>&
mud::test::_given<Context>::And(const std::string& id)
{
    _chain = _feature.construct_given(id);
    return *_chain;
}

template<typename Context>
mud::test::_given<Context>&
mud::test::_given<Context>::And(
        const std::string& id,
        function_type func)
{
    _chain = new mud::test::_given<Context>(_feature, id, func);
    return *_chain;
}

template<typename Context>
void
mud::test::_given<Context>::operator()(
        context_type& ctx)
{
    if (_func == nullptr)
    {
        std::cerr << "'" << _id << "' has no function" << std::endl;
        throw std::bad_function_call();
    }
    _func(ctx);
    if (_chain != nullptr)
    {
        (*_chain)(ctx);
    }
}

template<typename Context>
const std::string&
mud::test::_given<Context>::id() const
{
    return _id;
}

template<typename Context>
void
mud::test::_given<Context>::dump(const char* prefix /* = "Given" */)
{
    std::cout << "  " << prefix << " " << _id << std::endl;
    if (_chain != nullptr)
    {
        _chain->dump("  And");
    }
}

/**
 * @brief Define the action of the scenario.
 *
 * A 'when' step defines a particular action of a scenario. Multiple of these
 * 'when' conditions can be added to a scenario by using the @c And method.
 * It is generally advised to have a very limited number of 'when' steps.
 */
template<typename Context>
class _when
{
public:
    typedef Context context_type;
    typedef std::function<void(context_type&)> function_type;

    /**
     * @brief Constructor, referencing a predefined 'when' step.
     * @param feature [in] The associated feature.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    _when(_feature<context_type>&, const std::string& id);

    /**
     * @brief Constructor, defining a new 'when' step.
     * @param feature [in] The associated feature.
     * @param id [in] The description ID.
     * @param func [in] The function detailing the action to the taken.
     */
    _when(_feature<context_type>&, const std::string& id, function_type func);

    /**
     * @brief Add an additional predefined 'when' step.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    _when<context_type>& And(const std::string&);

    /**
     * @brief Add an additional new 'when' step.
     * @param id [in] The feature-unique ID referencing the predefined step.
     * @param func [in] The function detailing the action to the taken.
     */
    _when<context_type>& And(const std::string&, function_type func);

    /**
     * @brief Execute the 'given' step, and all its chained 'given' steps.
     */
    void operator()(context_type& ctx);

    /**
     * @return The ID (description) of the 'when' step.
     */
    const std::string& id() const;

    /**
     * @brief Dump the 'When' step and it's chain to standard output.
     */
    void dump(const char* prefix = "When");
private:
    _feature<context_type>& _feature;
    std::string _id;
    function_type _func;
    mud::test::_when<context_type>* _chain;
};

template<typename Context>
mud::test::_when<Context>::_when(
        mud::test::_feature<Context>& feature,
        const std::string& id)
    : _feature(feature), _id(id),
      _func(nullptr), _chain(nullptr)
{
}

template<typename Context>
mud::test::_when<Context>::_when(
        mud::test::_feature<Context>& feature,
        const std::string& id,
        function_type func)
    : _feature(feature), _id(id),
      _func(func), _chain(nullptr)
{
}

template<typename Context>
mud::test::_when<Context>&
mud::test::_when<Context>::And(const std::string& id)
{
    _chain = _feature.construct_when(id);
    return *_chain;
}

template<typename Context>
mud::test::_when<Context>&
mud::test::_when<Context>::And(
        const std::string& id,
        function_type func)
{
    _chain = new mud::test::_when<Context>(_feature, id, func);
    return *_chain;
}

template<typename Context>
const std::string&
mud::test::_when<Context>::id() const
{
    return _id;
}

template<typename Context>
void
mud::test::_when<Context>::operator()(
        context_type& ctx)
{
    if (_func == nullptr)
    {
        std::cerr << "'" << _id << "' has no function" << std::endl;
        throw std::bad_function_call();
    }
    _func(ctx);
    if (_chain != nullptr)
    {
        (*_chain)(ctx);
    }
}

template<typename Context>
void
mud::test::_when<Context>::dump(const char* prefix /* = "When" */)
{
    std::cout << "   " << prefix << " " << _id << std::endl;
    if (_chain != nullptr)
    {
        _chain->dump(" And");
    }
}

/**
 * @brief Define the expected outcome of the scenario.
 *
 * A 'then' step defines a particular expected outcome after having performed
 * the action of a scenario. Multiple of these 'then' steps can be added to a
 * scenario by using the @c And method. These steps would verify the observable
 * outcome through an assertion.
 */
template<typename Context>
class _then
{
public:
    typedef Context context_type;
    typedef std::function<void(context_type&)> function_type;

    /**
     * @brief Constructor, referencing a predefined 'then' step.
     * @param feature [in] The associated feature.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    _then(_feature<context_type>&, const std::string& id);

    /**
     * @brief Constructor, defining a new 'then' step.
     * @param feature [in] The associated feature.
     * @param id [in] The description ID.
     * @param func [in] The function detailing the verification of the
     *        expected outcome.
     */
    _then(_feature<context_type>&, const std::string& id, function_type func);

    /**
     * @brief Add an additional predefined 'then' step.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    _then<context_type>& And(const std::string&);

    /**
     * @brief Add an additional new 'then' step.
     * @param id [in] The feature-unique ID referencing the predefined step.
     * @param func [in] The function detailing the verification of the
     *        expected outcome.
     */
    _then<context_type>& And(const std::string&, function_type func);

    /**
     * @brief Execute the 'then' step, and all its chained 'given' steps.
     */
    bool operator()(context_type& ctx);

    /**
     * @return The ID (description) of the 'then' step.
     */
    const std::string& id() const;

    /**
     * @brief Dump the 'Then' step and it's chain to standard output.
     */
    void dump(const char* prefix = "Then");

private:
    _feature<context_type>& _feature;
    std::string _id;
    function_type _func;
    mud::test::_then<context_type>* _chain;
};

template<typename Context>
mud::test::_then<Context>::_then(
        mud::test::_feature<Context>& feature,
        const std::string& id)
    : _feature(feature), _id(id),
      _func(nullptr), _chain(nullptr)
{
}

template<typename Context>
mud::test::_then<Context>::_then(
        mud::test::_feature<Context>& feature,
        const std::string& id,
        function_type func)
    : _feature(feature), _id(id),
      _func(func), _chain(nullptr)
{
}

template<typename Context>
mud::test::_then<Context>&
mud::test::_then<Context>::And(const std::string& id)
{
    _chain = _feature.construct_then(id);
    return *_chain;
}

template<typename Context>
mud::test::_then<Context>&
mud::test::_then<Context>::And(
        const std::string& id,
        function_type func)
{
    _chain = new mud::test::_then<Context>(_feature, id, func);
    return *_chain;
}

template<typename Context>
bool
mud::test::_then<Context>::operator()(
        context_type& ctx)
{
    if (_func == nullptr)
    {
        std::cerr << "'" << _id << "' has no function" << std::endl;
        throw std::bad_function_call();
    }

    bool result = true;
    try
    {
        _func(ctx);
    }
    catch (mud::test::assertion_failed& ex)
    {
        // An assertion failed
        std::cerr << "'" << id() << "' threw an assertion:"
                << std::endl << ex.what() << std::endl;
        result = false;
    }
    catch (std::exception& ex)
    {
        // Any standard exception
        std::cerr << "'" << id() << "' threw an unexpected exception:"
                << std::endl << ex.what() << std::endl;
        result = false;

    }
    catch (...)
    {
        // Any unknown exception
        std::cerr << "'" << id() << "' threw an unknown exception."
                << std::endl;
        result = false;

    }
    if (_chain != nullptr)
    {
        result &= (*_chain)(ctx);
    }
    return result;
}

template<typename Context>
const std::string&
mud::test::_then<Context>::id() const
{
    return _id;
}

template<typename Context>
void
mud::test::_then<Context>::dump(const char* prefix /* = "Then" */)
{
    std::cout << "   " << prefix << " " << _id << std::endl;
    if (_chain != nullptr)
    {
        _chain->dump(" And");
    }
}

/**
 * @brief Define a particular business rule of a feature that would verify
 * an expected outcome when a particular action is performed on an initial
 * context.
 *
 * The scenario is combining the 'given', 'when' and 'then' steps to define
 * the initial context, the action and expected outcome.
 *
 * The scenario is often defined in Behaviour Driven Development as the
 * Acceptance Criteria, of the form 'Given [context], when [action'
 * then [expecetd outcome]'.
 */
template<typename Context>
class _scenario
{
public:
    typedef Context context_type;
    typedef std::function<void(context_type&)> function_type;

    _scenario(_feature<context_type>&, const std::string& id);

    /**
     * @brief Define a 'given' step, referncing a pre-defined one.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    mud::test::_given<context_type>& Given(const std::string&);

    /**
     * @brief Define a new 'given' step.
     * @param id [in] The referencing the predefined step.
     * @param func [in] The function detailing the initial condition of the
     *        @c context_type.
     */
    mud::test::_given<context_type>& Given(const std::string&, function_type);

    /**
     * @brief Define a 'when' step, referncing a pre-defined one.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    mud::test::_when<context_type>& When(const std::string&);

    /**
     * @brief Define a new 'when' step.
     * @param id [in] The ID referencing the predefined step.
     * @param func [in] The function detailing the action to the taken.
     */
    mud::test::_when<context_type>& When(const std::string&, function_type);

    /**
     * @brief Define a 'then' step, referncing a pre-defined one.
     * @param id [in] The feature-unique ID referencing the predefined step.
     */
    mud::test::_then<context_type>& Then(const std::string&);

    /**
     * @brief Define a new 'then' step.
     * @param id [in] The ID referencing the predefined step.
     * @param func [in] The function detailing the verification of the
     *        expected outcome.
     */
    mud::test::_then<context_type>& Then(const std::string&, function_type);

    /**
     * @return The ID (description) of the 'scenario'.
     */
    const std::string& id() const;

    /**
     * @brief Run the scenario.
     */
    bool run();

    /**
     * @brief Dump the entire scenario, including all its steps, to standard
     * output.
     */
    void dump();
private:
    _feature<context_type>& _feature;
    context_type _context;
    std::string _id;
    mud::test::_given<context_type>* _given;
    mud::test::_when<context_type>* _when;
    mud::test::_then<context_type>* _then;
};

template<typename Context>
mud::test::_scenario<Context>::_scenario(
        mud::test::_feature<Context>& feature,
        const std::string& id)
    : _feature(feature), _id(id)
{
}

template<typename Context>
mud::test::_given<Context>&
mud::test::_scenario<Context>::Given(const std::string& id)
{
    _given = _feature.construct_given(id);
    return *_given;
}

template<typename Context>
mud::test::_given<Context>&
mud::test::_scenario<Context>::Given(
        const std::string& id,
        function_type func)
{
    _given = new mud::test::_given<Context>(_feature, id, func);
    return *_given;
}

template<typename Context>
mud::test::_when<Context>&
mud::test::_scenario<Context>::When(const std::string& id)
{
    _when = _feature.construct_when(id);
    return *_when;
}

template<typename Context>
mud::test::_when<Context>&
mud::test::_scenario<Context>::When(
        const std::string& id,
        function_type func)
{
    _when = new mud::test::_when<Context>(_feature, id, func);
    return *_when;
}

template<typename Context>
mud::test::_then<Context>&
mud::test::_scenario<Context>::Then(const std::string& id)
{
    _then = _feature.construct_then(id);
    return *_then;
}

template<typename Context>
mud::test::_then<Context>&
mud::test::_scenario<Context>::Then(
        const std::string& id,
        function_type func)
{
    _then = new mud::test::_then<Context>(_feature, id, func);
    return *_then;
}

template<typename Context>
const std::string&
mud::test::_scenario<Context>::id() const
{
    return _id;
}

template<typename Context>
void
mud::test::_scenario<Context>::dump()
{
    std::cout << "Scenario: " << _id << std::endl;
    _given->dump();
    _when->dump();
    _then->dump();
}

template<typename Context>
bool
mud::test::_scenario<Context>::run()
{
    (*_given)(_context);
    (*_when)(_context);
    if (!(*_then)(_context)) {
        std::cout << "[FAIL] " << _id << std::endl;
        return false;
    }
    else {
        std::cout << "[PASS] " << _id << std::endl;
        return true;
    }
}

/**
 * @brief A base class for a gherkin feature.
 *
 * This class defines the required method for any feature and provides a means
 * to have all features to be registered within a feature factory.
 */
class _base_feature
{
public:
    _base_feature();
    virtual ~_base_feature();

    /**
     * Run the test scenarios.
     * @param test [in] The scenarios to run. If this is empty, all the
     *   scenarios are run.
     * @return The test result. The first field is the total test cases that
     *   have been run, the second field the number of successfull ones.
     */
    virtual std::pair<size_t, size_t> run(
            const std::string& filter = std::string()) = 0;
};

/**
 * @brief Defines a specific software feature, consisting of one or more
 * scenarios.
 *
 * The representation of a feature in terms of a set of scenarios is the core
 * principle of the Gherkin language. A feature should therefore be completed
 * being tested through its scenarios.
 *
 * A feature is often defined in Behaviour Drive Development with a narrative
 * like 'As a [role], I want [feature] so that [benefit]'.
 */
template<typename Context>
class _feature : public _base_feature
{
public:
    typedef Context context_type;
    typedef std::function<void(context_type&)> function_type;

    /**
     * @brief Define a pre-defined 'given' step.
     * @param id [in] The feature-unique ID.
     * @param func [in] The function detailing the initial condition of the
     *        @c context_type.
     */
    void register_given(const std::string&, function_type);

    /**
     * @brief Construct a predefined 'given' step.
     * @param id [in] The feature-unique ID.
     */
    mud::test::_given<context_type>* construct_given(const std::string&);

    /**
     * @brief Define a pre-defined 'when' step.
     * @param id [in] The feature-unique ID.
     * @param func [in] The function detailing the action to the taken.
     */
    void register_when(const std::string&, function_type);

    /**
     * @brief Construct a predefined 'when' step.
     * @param id [in] The feature-unique ID.
     */
    mud::test::_when<context_type>* construct_when(const std::string&);

    /**
     * @brief Define a pre-defined 'then' step.
     * @param id [in] The feature-unique ID.
     * @param func [in] The function detailing the verification of the
     *        expected outcome.
     */
    void register_then(const std::string&, function_type);

    /**
     * @brief Construct a predefined 'then' step.
     * @param id [in] The feature-unique ID.
     */
    mud::test::_then<context_type>* construct_then(const std::string&);

private:
    std::map<std::string, function_type> _given_library;
    std::map<std::string, function_type> _when_library;
    std::map<std::string, function_type> _then_library;
};

template<typename Context>
void
mud::test::_feature<Context>::register_given(
        const std::string& id, function_type func)
{
    _given_library[id] = func;
}

template<typename Context>
void
mud::test::_feature<Context>::register_when(
        const std::string& id, function_type func)
{
    _when_library[id] = func;
}

template<typename Context>
void
mud::test::_feature<Context>::register_then(
        const std::string& id, function_type func)
{
    _then_library[id] = func;
}

template<typename Context>
mud::test::_given<Context>*
mud::test::_feature<Context>::construct_given(const std::string& id)
{
    auto iter = _given_library.find(id);
    if (iter != _given_library.end())
    {
        return new mud::test::_given<Context>(*this, iter->first, iter->second);
    }
    throw mud::test::not_specified(id);
};

template<typename Context>
mud::test::_when<Context>*
mud::test::_feature<Context>::construct_when(const std::string& id)
{
    auto iter = _when_library.find(id);
    if (iter != _when_library.end())
    {
        return new mud::test::_when<Context>(*this, iter->first, iter->second);
    }
    throw mud::test::not_specified(id);
};

template<typename Context>
mud::test::_then<Context>*
mud::test::_feature<Context>::construct_then(const std::string& id)
{
    auto iter = _then_library.find(id);
    if (iter != _then_library.end())
    {
        return new mud::test::_then<Context>(*this, iter->first, iter->second);
    }
    throw mud::test::not_specified(id);
};

/**
 * @brief Throw a new assetion-failed exception with additional context and
 *        details.
 * @param file [in] The file name.
 * @param line [in] The line number.
 * @param details [in] Additinal context details about the assertion.
 */
bool AssertFailed(const char* file, int line, const std::string& details);

/**
 * @brief Template class that verifies if two values (result and expected) are
 *        the same. If they are not the same, and assertion is thrown.
 * @param file [in] The file name.
 * @param line [in] The line number.
 * @param result [in] The resulting value to verify.
 * @param expected [in] The expected outcome.
 */
template<typename T>
bool
Assert(const char* file, int line, T result, T expected)
{
    if (result != expected)
    {
        std::stringstream sstr;
        sstr << "  Expected: " << expected << std::endl
                << "  Result  : " << result;
        AssertFailed(file, line, sstr.str());
    }
    return true;
}

/**
 * @brief A factory to hold all the features.
 */
class _feature_factory
{
public:
    typedef std::function<mud::test::_base_feature*()> creator_func;

    /**
     * @brief Register a feature and its creator-function.
     * @param id [in] The feature unique ID.
     * @param creator_func [in] A function to create the feature.
     */
    static void register_feature(const std::string& id, creator_func);

    /**
     * @brief Run all the features.
     * @param filter [in] The feature and/or scenarios to filter on. This can
     *   be of the following format:
     *     - \a feature
     *     - \c#\a scenario
     *     - \a feature\c:\s cenario
     * @return The test result. The first field is the total test cases that
     *   have been run, the second field the number of successfull ones.
     */
    static std::pair<size_t,size_t> run(
            const std::string& filter = std::string());
private:
    static std::map<std::string, creator_func> _map;
};

/**
 * @brief A template wrapper class to register a feature in the factory.
 */
template<typename Feature>
class _feature_registrar
{
public:
    _feature_registrar(const std::string& id) {
        _feature_factory::register_feature(id, [] {
            return new Feature();
        });
    }
};

/**
 * @brief Macro to define a feature
 */

#define FEATURE(ID,CTX)                                                      \
    namespace /* unnamed */ {                                                \
    class TestFeature : public mud::test::_feature<CTX>                      \
    {                                                                        \
    public:                                                                  \
        static bool register_feature() {                                     \
            mud::test::_feature_factory::register_feature(ID, []{            \
                            return new TestFeature();                        \
                    });                                                      \
            return true;                                                     \
        }                                                                    \
        TestFeature() : _id(ID) {

/**
 * @brief Macro to define a pre-defined 'given' step.
 */
#define DEFINE_GIVEN(ID,FUNC)                                                \
    register_given(ID, FUNC);

/**
 * @brief Macro to define a pre-defined 'when' step.
 */
#define DEFINE_WHEN(ID,FUNC)                                                 \
    register_when(ID, FUNC);

/**
 * @brief Macro to define a pre-defined 'then' step.
 */
#define DEFINE_THEN(ID,FUNC)                                                 \
    register_then(ID, FUNC);

/**
 * @brief Macro to define the end of all pre-defined steps. This macro must
 *        always be called, even when there are no pre-defined steps.
 */
#define END_DEFINES()                                                        \
    }                                                                        \
    virtual std::pair<size_t, size_t> run(                                   \
            const std::string& filter) override                              \
    {                                                                        \
        std::cout << "FEATURE: " << _id << std::endl;                        \
        std::pair<size_t, size_t> result = {-1, -1};                         \
        {                                                                    \
            class Dummy { public: bool run() { return true; }};              \
            Dummy scnr;

/**
 * @brief Macro to define a scenario
 */
#define SCENARIO(ID)                                                         \
    ;                                                                        \
    ++result.first;                                                          \
    if (scnr.run()) {                                                        \
        ++result.second;                                                     \
    }                                                                        \
    }                                                                        \
    if (filter.empty() || filter == ID)                                      \
    {                                                                        \
        mud::test::_scenario<context> scnr(*this, ID);

/**
 * @brief Macro to define a 'given' step.
 */
#define GIVEN(...)                                                           \
    scnr.Given( __VA_ARGS__)

/**
 * @brief Macro to define a 'when' step.
 */
#define WHEN(...)                                                            \
    ;                                                                        \
    scnr.When(__VA_ARGS__)

/**
 * @brief Macro to define a 'then' step.
 */
#define THEN(...)                                                            \
    ;                                                                        \
    scnr.Then(__VA_ARGS__)

/**
 * @brief Macro to define a 'and' step that can be used after a 'given',
 *        'when' and 'then' step..
 */
#define AND(...)                                                             \
    .And(__VA_ARGS__)

/**
 * @brief Macro to define the end of the feature definition.
 */
#define END_FEATURE()                                                        \
    ;                                                                        \
    ++result.first;                                                          \
    if (scnr.run()) {                                                        \
        ++result.second;                                                     \
    }                                                                        \
    }                                                                        \
    return result;                                                           \
    }                                                                        \
    private:                                                                 \
    std::string _id;                                                         \
    };                                                                       \
    bool registered = TestFeature::register_feature();                       \
    } /* namespace unnamed */

/**
 * @brief Macro to define an assertion.
 */
#define ASSERT(...)                                                          \
    mud::test::Assert(__FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Macro to define an assertion on throwing an excpetion.
 */
#define ASSERT_THROW(EX,...)                                                 \
    do                                                                       \
    {                                                                        \
        bool thrown = false;                                                 \
        try {                                                                \
            __VA_ARGS__;                                                     \
        }                                                                    \
        catch(const EX& ex) {                                                \
            /* Expected */                                                   \
            thrown = true;                                                   \
        }                                                                    \
        catch(const std::exception ex) {                                     \
            std::stringstream sstr;                                          \
            sstr << "  Expected exception: " << typeid(EX).name()            \
                    << std::endl                                             \
                    << "  Actual exception  : std::exception - or derived";  \
            mud::test::AssertFailed(__FILE__, __LINE__, sstr.str());         \
        }                                                                    \
        catch(...) {                                                         \
            std::stringstream sstr;                                          \
            sstr << "  Expected exception: " << typeid(EX).name()            \
                    << std::endl                                             \
                    << "  Actual exception  : <unidentified>";               \
            mud::test::AssertFailed(__FILE__, __LINE__, sstr.str());         \
        }                                                                    \
        if (!thrown) {                                                       \
            std::stringstream sstr;                                          \
            sstr << "  Expected exception: " << typeid(EX).name()            \
                    << std::endl                                             \
                    << "  Actual exception  : not thrown";                   \
            mud::test::AssertFailed(__FILE__, __LINE__, sstr.str());         \
        }                                                                    \
    } while(false)

/**
 * @brief Macro to define an assertion on not-throwing an excpetion.
 */
#define ASSERT_NO_THROW(...)                                                 \
    do                                                                       \
    {                                                                        \
        try {                                                                \
            __VA_ARGS__;                                                     \
        }                                                                    \
        catch(const std::exception ex) {                                     \
            std::stringstream sstr;                                          \
            sstr << "  Unexpected exception: std::exception - or derived";   \
            mud::test::AssertFailed(__FILE__, __LINE__, sstr.str());         \
        }                                                                    \
        catch(...) {                                                         \
            std::stringstream sstr;                                          \
            sstr << "  Unexpected exception: <unidentified>";                \
            mud::test::AssertFailed(__FILE__, __LINE__, sstr.str());         \
        }                                                                    \
    } while(false)

/**
 * @brief Run all registered features and return their result as a pair of
 * two @c size_t values, the first recording the total number of scenarios
 * that have been run, and the second the total number of success.
 */
#define FEATURE_RUN(...)                                                    \
    mud::test::_feature_factory::run(__VA_ARGS__)

END_MUDLIB_TEST_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TEST_GHERKIN_H_ */

