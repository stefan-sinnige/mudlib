#ifndef _MUDLIB_TEST_GHERKIN_H_
#define _MUDLIB_TEST_GHERKIN_H_

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
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
 * @brief A generic multi-type data table container.
 *
 * A multi-type data table container with optional named columns. The data
 * is stored inetrnally as strings, but can be inserted and retrieved by
 * any type, provided that there is an @c std::istream and @c std::ostream
 * streaming operators defined.
 */
class _table
{
public:
    typedef std::string                column_type;
    typedef std::map<column_type, int> columns_type;
    typedef std::vector<std::string>   row_type;
    typedef std::vector<row_type>      rows_type;

    /**
     * @brief Constructor, creating an empty table without any definition.
     */
    _table();

    /**
     * @brief Destructor
     */
    ~_table();

    /**
     * @brief Specify un-named columns. The number of columns is determined
     * by the inserted rows themselves.
     */
    void columns();

    /**
     * @brief Specify named columns
     */
    template<typename... A>
    void columns(A... args);

    /**
     * @brief Specify a new row of values
     * @param args [in] The values to insert.
     *
     * All data is converted and stored as a string. When the data is
     * retrieved, it can be converted back to the original form. It uses
     * the @c std::istream and @c std::ostream streaming operations to
     * convert to and from a string.
     */
    template<typename... A>
    void row(A... args);

    /**
     * @brief Return the entry at (row, column).
     * @param row [in] The row index, starting at 0 for the first row.
     * @param column [in] The named column.
     *
     * This method is particularly useful for accessing data-table data.
     */
    template<typename T>
    T entry(int row, const std::string& column) const;

    /**
     * @brief Return the entry at (row, column).
     * @param row [in] The row index, starting at 0 for the first row.
     * @param column [in] The column index, starting at 0 for the first row.
     *
     * This method is particularly useful for accessing data-table data.
     */
    template<typename T>
    T entry(int row, int column) const;

    /**
     * @brief Return the entry of a field.
     * @param field [in] The named field.
     *
     * This method is particularly useful for accessing sample data.
     */
    template<typename T>
    T entry(const std::string& field) const;

    /**
     * @brief Return the entry of a field.
     * @param field [in] The field index.
     *
     * This method is particularly useful for accessing sample data.
     */
    template<typename T>
    T entry(int column) const;

    /**
     * @brief Return the number of rows.
     */
    size_t row_count() const;

    /**
     * @brief Return a specific row.
     * @param row [in] The row index, starting at 0 for the first row.
     */
    const _table::row_type& operator[](int idx) const;

    /**
     * @brief Return a sample table that only contains the specified row.
     * @param row [in] The row index, starting at 0 for the first row.
     */
    _table sample(int idx) const;

private:
    /**
     * @brief Specify a named column and associatged index.
     */
    void column_elements(size_t idx, const char* element);

    template<typename T, typename... A>
    void column_elements(size_t idx, T element, A... args);

    /**
     * @brief Specify a row element of type string.
     */
    void row_elements(const char* element);

    /**
     * @brief Specify a row element of any type, other than a string.
     */
    template<typename T>
    void row_elements(T element);

    /**
     * @brief Specify a new row
     */
    template<typename T, typename... A>
    void row_elements(T element, A... args);

    /**
     * @brief Return a string entry at (row, column).
     * @param row [in] The row index, starting at 0 for the first row.
     * @param column [in] The column index, starting at 0 for the first row.
     * @param dummy [in] Dummy parameter used in template specialisation.
     */
    std::string entry(int row, int column, const std::string& dummy) const;

    /**
     * @brief Return a types entry at (row, column).
     * @param row [in] The row index, starting at 0 for the first row.
     * @param column [in] The column index, starting at 0 for the first row.
     * @param dummy [in] Dummy parameter used in template specialisation.
     */
    template<typename T>
    T entry(int row, int column, const T& dummy) const;

    /* The map of named columns and their associated column index. */
    columns_type _columns;

    /* The list of rows. */
    rows_type _rows;
};

template<typename... A>
void
mud::test::_table::columns(A... args) {
    column_elements(0, args...);
}

template<typename... A>
void
mud::test::_table::row(A... args) {
    _rows.push_back(row_type());
    row_elements(args...);
}

template<typename T>
T
mud::test::_table::entry(int row, const std::string& column) const {
    T dummy;
    int column_index = _columns.at(column);
    return entry(row, column_index, dummy);
}

template<typename T>
T
mud::test::_table::entry(int row, int column) const {
    T dummy;
    return entry(row, column, dummy);
}

template<typename T>
T
mud::test::_table::entry(const std::string& field) const {
    T dummy;
    int column_index = _columns.at(field);
    return entry(0, column_index, dummy);
}

template<typename T>
T
mud::test::_table::entry(int field) const {
    T dummy;
    return entry(0, field, dummy);
}

template<typename T, typename... A>
void
mud::test::_table::column_elements(size_t idx, T element, A... args) {
    column_elements(idx, element);
    column_elements(idx+1, args...);
};

template<typename T>
void
mud::test::_table::row_elements(T element) {
    std::stringstream sstr;
    sstr << element;
    _rows.back().push_back(sstr.str());
}

template<typename T, typename... A>
void
mud::test::_table::row_elements(T element, A... args) {
    row_elements(element);
    row_elements(args...);
};

template<typename T>
T
mud::test::_table::entry(int row, int column, const T& dummy) const {
    T var;
    std::string element = _rows[row][column];
    std::stringstream sstr(element);
    sstr >> var;
    return var;
}

/**
 * @brief Base class for all context types.
 *
 * The @c base_context is a base class for all feature context classes. It
 * provides access to any data-table and sample information, when they are
 * defined.
 */
class _base_context
{
public:
    // Set the data table.
    void
    data(const _table& data) {
        _data = data;
    }

    // Return the data table.
    const _table&
    data() const {
        return _data;
    }

    // Set a specific sample for this test-run.
    void
    sample(const _table& sample) {
        _sample = sample;
    }

    // Return the Sample data.
    const _table&
    sample() const {
        return _sample;
    }

private:
    // The data table.
    _table _data;

    // The sample. This is a table with maximum one row!
    _table _sample;
};

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
     * @brief Add a data table.
     * @param table [in] The function that returns the table.
     */
    _given<context_type>& Data(std::function<_table()> func);

    /**
     * @brief Execute the 'given' step, and all its chained 'given' steps.
     */
    bool operator()(context_type& ctx);

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
    _table _data;
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
mud::test::_given<Context>&
mud::test::_given<Context>::Data(std::function<_table()> func)
{
    _data = func();
    return *this;
}

template<typename Context>
bool
mud::test::_given<Context>::operator()(
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
        ctx.data(_data);
        _func(ctx);
    }
    catch (const mud::test::assertion_failed& ex)
    {
        // An assertion failed
        std::cerr << "'" << id() << "' threw an assertion:"
                << std::endl << ex.what() << std::endl;
        result = false;
    }
    catch (const std::exception& ex)
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
     * @brief Add a data table.
     * @param table [in] The function that returns the table.
     */
    _when<context_type>& Data(std::function<_table()> func);

    /**
     * @brief Execute the 'given' step, and all its chained 'given' steps.
     */
    bool operator()(context_type& ctx);

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
    _table _data;
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
mud::test::_when<Context>&
mud::test::_when<Context>::Data(std::function<_table()> func)
{
    _data = func();
    return *this;
}

template<typename Context>
const std::string&
mud::test::_when<Context>::id() const
{
    return _id;
}

template<typename Context>
bool
mud::test::_when<Context>::operator()(
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
        ctx.data(_data);
        _func(ctx);
    }
    catch (const mud::test::assertion_failed& ex)
    {
        // An assertion failed
        std::cerr << "'" << id() << "' threw an assertion:"
                << std::endl << ex.what() << std::endl;
        result = false;
    }
    catch (const std::exception& ex)
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
     * @brief Add a data table.
     * @param table [in] The function that returns the table.
     */
    _then<context_type>& Data(std::function<_table()> func);

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
    _table _data;
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
mud::test::_then<Context>&
mud::test::_then<Context>::Data(std::function<_table()> func)
{
    _data = func();
    return *this;
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
        ctx.data(_data);
        _func(ctx);
    }
    catch (const mud::test::assertion_failed& ex)
    {
        // An assertion failed
        std::cerr << "'" << id() << "' threw an assertion:"
                << std::endl << ex.what() << std::endl;
        result = false;
    }
    catch (const std::exception& ex)
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
     * @brief Add a sample table.
     * @param table [in] The function that returns the table.
     *
     * The scenario is run for each row in the sample table.
     */
    void Samples(std::function<_table()> func);

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
    /**
     * @brief Run the scenario with a specific context.
     * @param ctx [in] The context.
     */
    bool run(context_type& ctx);

    _feature<context_type>& _feature;
    std::string _id;
    _table _samples;
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
void
mud::test::_scenario<Context>::Samples(std::function<_table()> func)
{
    _samples = func();
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
    if (_samples.row_count() > 0) {
        bool result = true;
        for (int i = 0; i < _samples.row_count(); ++i) {
            context_type ctx;
            ctx.sample(_samples.sample(i));
            result &= run(ctx);
        }
        return result;
    }
    else {
        context_type ctx;
        return run(ctx);
    }
}

template<typename Context>
bool
mud::test::_scenario<Context>::run(context_type& ctx)
{
    if (!(*_given)(ctx)) {
        std::cout << "[FAIL] " << _id << std::endl;
        return false;
    }
    if (!(*_when)(ctx)) {
        std::cout << "[FAIL] " << _id << std::endl;
        return false;
    }
    if (!(*_then)(ctx)) {
        std::cout << "[FAIL] " << _id << std::endl;
        return false;
    }
    std::cout << "[PASS] " << _id << std::endl;
    return true;
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

/* *INDENT-OFF* */

/**
 * @brief Macro to define a context.
 */

#define CONTEXT()                                                            \
    namespace /* unnamed */ {                                                \
    class context : public mud::test::_base_context                          \
    {                                                                        \
    public:

#define END_CONTEXT()                                                        \
    };

/**
 * @brief Macro to define a feature
 */

#define FEATURE(ID)                                                          \
    class TestFeature : public mud::test::_feature<context>                  \
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
        }                                                                    \
        virtual std::pair<size_t, size_t> run(                               \
                const std::string& filter) override                          \
        {                                                                    \
            std::cout << "FEATURE: " << _id << std::endl;                    \
            std::pair<size_t, size_t> result = {-1, -1};                     \
            {                                                                \
                class Dummy { public: bool run() { return true; }};          \
                Dummy scnr;

/**
 * @brief Macro to define a scenario
 */
#define SCENARIO(ID)                                                         \
                ;                                                            \
                ++result.first;                                              \
                if (scnr.run()) {                                            \
                    ++result.second;                                         \
                }                                                            \
            } ;                                                              \
            if (filter.empty() || filter == ID)                              \
            {                                                                \
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
                ;                                                            \
                scnr.When(__VA_ARGS__)

/**
 * @brief Macro to define a 'then' step.
 */
#define THEN(...)                                                            \
                ;                                                            \
                scnr.Then(__VA_ARGS__)

/**
 * @brief Macro to define a 'and' step that can be used after a 'given',
 *        'when' and 'then' step.
 */
#define AND(...)                                                             \
                .And(__VA_ARGS__)

/**
 * @brief Macro to define a data table that can be used in a 'given',
 *        'when' and 'then' step.
 */
#define DATA(...)                                                            \
                .Data([]() -> mud::test::_table {                            \
                    mud::test::_table data;                                  \
                    data.columns(__VA_ARGS__);

/**
 * @brief Macro to define a new data table row.
 */
#define DATA_ROW(...)                                                        \
                    data.row(__VA_ARGS__);

/**
 * @brief Macro to define the end of a data table definition.
 */
#define END_DATA()                                                           \
                    return data;                                             \
                })

/**
 * @brief Macro to define a samples table that can be used in a 'scenario.
 */
#define SAMPLES(...)                                                         \
                ;                                                            \
                scnr.Samples([]() -> mud::test::_table {                     \
                    mud::test::_table samples;                               \
                    samples.columns(__VA_ARGS__);

/**
 * @brief Macro to define a new data table row.
 */
#define SAMPLE(...)                                                          \
                    samples.row(__VA_ARGS__);

/**
 * @brief Macro to define the end of a data table definition.
 */
#define END_SAMPLES()                                                        \
                    return samples;                                          \
                })

/**
 * @brief Macro to define the end of the feature definition.
 */
#define END_FEATURE()                                                        \
                ;                                                            \
                ++result.first;                                              \
                if (scnr.run()) {                                            \
                    ++result.second;                                         \
                }                                                            \
            }                                                                \
            return result;                                                   \
        }                                                                    \
        private:                                                             \
            std::string _id;                                                 \
        };                                                                   \
        bool registered = TestFeature::register_feature();                   \
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
        catch(const std::exception& ex) {                                     \
            std::stringstream sstr;                                          \
            sstr << "  Expected exception: " << typeid(EX).name()            \
                 << std::endl                                                \
                 << "  Actual exception  : std::exception - or derived";     \
            mud::test::AssertFailed(__FILE__, __LINE__, sstr.str());         \
        }                                                                    \
        catch(...) {                                                         \
            std::stringstream sstr;                                          \
            sstr << "  Expected exception: " << typeid(EX).name()            \
                 << std::endl                                                \
                 << "  Actual exception  : <unidentified>";                  \
            mud::test::AssertFailed(__FILE__, __LINE__, sstr.str());         \
        }                                                                    \
        if (!thrown) {                                                       \
            std::stringstream sstr;                                          \
            sstr << "  Expected exception: " << typeid(EX).name()            \
                 << std::endl                                                \
                 << "  Actual exception  : not thrown";                      \
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
        catch(const std::exception& ex) {                                     \
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

/* *INDENT-ON* */

END_MUDLIB_TEST_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TEST_GHERKIN_H_ */

