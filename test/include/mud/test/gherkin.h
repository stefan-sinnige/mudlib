#ifndef _MUDLIB_TEST_GHERKIN_H_
#define _MUDLIB_TEST_GHERKIN_H_

#include <any>
#include <functional>
#include <iostream>
#include <map>
#include <mud/test/exception.h>
#include <mud/test/ns.h>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

BEGIN_MUDLIB_TEST_NS

/*
 * @brief Streaming operator for enumerated values, using their underlying
 * type to output the value.
 */
template<typename T>
std::ostream&
operator<<(
    typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& ostr,
    const T& e)
{
    return ostr << static_cast<typename std::underlying_type<T>::type>(e);
}

/*
 * Forward declararions
 */
template<typename Context>
class _scenario;
template<typename Context>
class _feature;

/**
 * @brief A generic multi-type data table container.
 *
 * A multi-type data table container with optional named columns. The data can
 * be used to store both data-tables as well as sample information. All data
 * that is stored are required to
 *    - Have a copy constructor
 *    _ Have conversion constructors
 *
 * The _base_table is a base class interface that exposes the accessors and
 * mutators used to set-up the table and to acquire access to its values. Each
 * creation of a specific data or sample table in a scenario will create an
 * implementation of the base table and identifies the number of columns and
 * their associated type.
 */
class MUDLIB_TEST_API _base_table
{
public:
    /**
     * @brief Type definition of a pointer to the table.
     */
    typedef std::shared_ptr<_base_table> ptr;

    /**
     * @brief Constructor.
     */
    _base_table() = default;

    /**
     * @brief Destructor
     */
    virtual ~_base_table() = default;

    /**
     * @brief Return the number of rows of the table.
     */
    virtual size_t rows() const = 0;

    /**
     * @brief Return the number of columns of the table.
     */
    virtual size_t columns() const = 0;

    /**
     * @brief Return the table entry by row and column.
     * @param row The zero-based row number of the value to access.
     * @param column The zero-based columns number of the value to access.
     * @tparam T The type of the data to return.
     *
     * @details
     * Return a particular value in the table that is accessed by its zero-based
     * row and column numbers. The data is expected to be of the same type @c T
     * as the type of the associated column when the data table has been
     * defined.
     *
     * @throws std::bad_any_cast The type @c T does not match the type of the
     * associated column.
     * @throws std::out_of_range The @c row and/or @c column values are out of
     * the range of the defined table.
     */
    template<typename T>
    T entry(size_t row, size_t column) {
        return std::any_cast<T>(entry(row, column));
    }

    /**
     * @brief Return the table entry by row and column.
     * @param row The zero-based row number of the value to access.
     * @param column The name of the column to access.
     * @tparam T The type of the data to return.
     *
     * @details
     * Return a particular value in the table that is accessed by its zero-based
     * row number and column name. The data is expected to be of the same type
     * @c T as the type of the associated column when the data table has been
     * defined.
     *
     * @throws std::bad_any_cast The type @c T does not match the type of the
     * associated column.
     * @throws std::out_of_range The @c row and/or @c column values are out of
     * the range of the defined table.
     */
    template<typename T>
    T entry(size_t row, const std::string& column_name) {
        auto find = std::find(_headings.begin(), _headings.end(), column_name);
        if (find == _headings.end()) {
            throw std::out_of_range("unknown column name");
        }
        return std::any_cast<T>(entry(row, find - _headings.begin()));
    }

    /**
     * @brief Define the column heading names.
     * @param names The names of the columns.
     * @details
     * Data entries can be identified by row and column number, but the columns
     * can also be identified by a unique name. The naming of the columns is
     * optional.
     */
    void headings(const std::vector<std::string>& names) {
        _headings = names;
    }

protected:
    /**
     * @brief Return the table entry by row and column.
     * @param row The zero-based row number of the value to access.
     * @param column The zero-based columns number of the value to access.
     *
     * @details
     * Return a particular value in the table that is accessed by its zero-based
     * row and column numbers. The data that is retrieved is returned as a @c
     * std::any and can be converted to its original datatype by using the @c
     * std::any_cast<T>() function.
     *
     * @throws std::out_of_range The @c row and/or @c column values are out of
     * the range of the defined table.
     */
    virtual std::any entry(size_t row, size_t column) const = 0;

private:
    /**
     * @brief The (optional) column names.
     */
    std::vector<std::string> _headings;
};

/**
 * @brief The templated specialisation of a @c _base_table.
 * @tparam Args The list of types that identifies the type of each table column.
 *
 * @details
 * Each sample or data table is an specifialisation of this template where the
 * template arguments define the data-types of each column.
 */
template<typename... Args>
class _table: public _base_table
{
public:
    /**
     * @brief Type definition of the columns.
     */
    typedef std::tuple<Args...> column_type;

    /**
     * @brief Type definition of the table data. This is essentially a vector of
     * rows where each row is a tuple of columns.
     */
    typedef std::vector<column_type> table_type;

    /**
     * @brief Constructor, creating an empty table.
     */
    _table() {}

    /**
     * @brief Return the number of rows of the table.
     */
    size_t rows() const override {
        return _data.size();
    }

    /**
     * @brief Return the number of columns of the table.
     */
    size_t columns() const override {
        return std::tuple_size<column_type>::value;
    }

    /**
     * @brief Push a new row of data to the end of the table.
     * @param values The column data values.
     * The list of values should match the size and type of the data table. Each
     * value will be created using the associated column-type's conversion
     * constructor.
     */
    void push_back(Args&&... args) {
        _data.push_back(std::make_tuple(std::forward<Args>(args)...));
    }

protected:
    /**
     * @brief Return the table entry by row and column.
     * @param row The zero-based row number of the value to access.
     * @param column The zero-based columns number of the value to access.
     *
     * @details
     * Return a particular value in the table that is accessed by its zero-based
     * row and column numbers. The data that is retrieved is returned as a @c
     * std::any and can be converted to its original datatype by using the @c
     * std::any_cast<T>() function.
     *
     * Note that up to 16 columns are supported.
     *
     * @throws std::out_of_range The @c row and/or @c column values are out of
     * the range of the defined table.
     */
    std::any entry(size_t row, size_t column) const override {
        const column_type& data_row = _data[row];
        return _entry(column, data_row);
    }
     
private:
    /**
     * @brief Return the value of a single column.
     * @param index The zero-based column index to return.
     * @param data The tuple of values.
     * @param indices The index sequence
     * @details
     * This helper function uses a fold expression with a comma expression to
     * extract the @index'th column. It utilises the tuple of data together
     * with a tuple of the same size containing the tuple index sequence. Only
     * when the matching index is encountered, will it copy the tuple value to
     * the end result.
     * @return The data value at the requested column.
     * @throw @c std::out_of_range if the index it not within the tuple size.
     */
    template<size_t... Idx>
    std::any
    _entry(size_t index, const column_type& data,
           std::index_sequence<Idx...>) const
    {
        std::any value;
        (..., ((Idx == index) ? value = std::get<Idx>(data) : value = value));
        return value;
    }

    /**
     * @brief Return the value of a single column.
     * @param index The zero-based column index to return.
     * @param data The tuple of values.
     * @return The data value at the requested column.
     * @throw @c std::out_of_range if the index it not within the tuple size.
     */
    std::any
    _entry(size_t index, const column_type& data) const {
        if (index >= std::tuple_size<column_type>{}) {
            throw std::out_of_range("index out of range");
        }
        return _entry(index, data, std::make_index_sequence<sizeof...(Args)>());
    }

    /**
     * @brief The data table.
     */
    table_type _data;
};

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
    /**
     * @brief Set the data table.
     */
    void data(_base_table::ptr data) { _data = data; }

    /**
     * @brief Return the data table.
     */
    _base_table::ptr data() const { return _data; }

    /**
     * @brief Set a samples.
     */
    void samples(_base_table::ptr samples) { _samples = samples; }

    /**
     * @brief Return the Samples.
     */
    _base_table::ptr samples() const { return _samples; }

    /**
     * @brief Set the current sample row to execute.
     */
    void row(size_t index) { _row = index; }

    /**
     * @brief Get the current sample row to execute.
     */
    size_t row() const { return _row; }
          
    /**
     * @brief Return the sample table entry for the current row.
     * @param column The zero-based columns number of the value to access.
     * @tparam T The type of the data to return.
     *
     * @details
     * Return a particular value in the table that is accessed by its zero-based
     * column number. The data is expected to be of the same type @c T as the
     * type of the associated column when the data table has been defined.
     *
     * @throws std::bad_any_cast The type @c T does not match the type of the
     * associated column.
     * @throws std::out_of_range The @c row and/or @c column values are out of
     * the range of the defined table.
     */
    template<typename T>
    T sample(size_t column) {
        if (!_samples) {
            throw std::out_of_range("no samples defined");
        }
        return _samples->entry<T>(_row, column);
    }

    /**
     * @brief Return the sample table entry for the current row.
     * @param column The name of the column to access.
     * @tparam T The type of the data to return.
     *
     * @details
     * Return a particular value in the table that is accessed by its column
     * name. The data is expected to be of the same type @c T as the type of
     * the associated column when the data table has been defined.
     *
     * @throws std::bad_any_cast The type @c T does not match the type of the
     * associated column.
     * @throws std::out_of_range The @c row and/or @c column values are out of
     * the range of the defined table.
     */
    template<typename T>
    T sample(const std::string& column_name) {
        if (!_samples) {
            throw std::out_of_range("no samples defined");
        }
        return _samples->entry<T>(_row, column_name);
    }

    /**
     * @brief Return the data table entry by row and column.
     * @param row The zero-based row number of the value to access.
     * @param column The zero-based columns number of the value to access.
     * @tparam T The type of the data to return.
     *
     * @details
     * Return a particular value in the table that is accessed by its zero-based
     * row and column numbers. The data is expected to be of the same type @c T
     * as the type of the associated column when the data table has been
     * defined.
     *
     * @throws std::bad_any_cast The type @c T does not match the type of the
     * associated column.
     * @throws std::out_of_range The @c row and/or @c column values are out of
     * the range of the defined table.
     */
    template<typename T>
    T data(size_t row, size_t column) {
        if (!_data) {
            throw std::out_of_range("no data defined");
        }
        return _data->entry<T>(row, column);
    }

    /**
     * @brief Return the data table entry by row and column.
     * @param row The zero-based row number of the value to access.
     * @param column The name of the column to access.
     * @tparam T The type of the data to return.
     *
     * @details
     * Return a particular value in the table that is accessed by its zero-based
     * row number and column name. The data is expected to be of the same type
     * @c T as the type of the associated column when the data table has been
     * defined.
     *
     * @throws std::bad_any_cast The type @c T does not match the type of the
     * associated column.
     * @throws std::out_of_range The @c row and/or @c column values are out of
     * the range of the defined table.
     */
    template<typename T>
    T data(size_t row, const std::string& column_name) {
        if (!_data) {
            throw std::out_of_range("no data defined");
        }
        return _data->entry<T>(row, column_name);
    }

private:
    // The data table.
    _base_table::ptr _data;

    // The sample table
    _base_table::ptr _samples;

    // The current row
    size_t _row = -1;
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
    _given(_feature<context_type>& feature, const std::string& id,
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
    _given<context_type>& Data(std::function<_base_table::ptr()> func);

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
    _feature<context_type>& _feat;
    std::string _id;
    function_type _func;
    _base_table::ptr _data;
    mud::test::_given<context_type>* _chain;
};

template<typename Context>
mud::test::_given<Context>::_given(::mud::test::_feature<context_type>& feature,
                                   const std::string& id)
  : _feat(feature), _id(id), _func(nullptr), _chain(nullptr)
{}

template<typename Context>
mud::test::_given<Context>::_given(mud::test::_feature<context_type>& feature,
                                   const std::string& id, function_type func)
  : _feat(feature), _id(id), _func(func), _chain(nullptr)
{}

template<typename Context>
mud::test::_given<Context>&
mud::test::_given<Context>::And(const std::string& id)
{
    _chain = _feat.construct_given(id);
    return *_chain;
}

template<typename Context>
mud::test::_given<Context>&
mud::test::_given<Context>::And(const std::string& id, function_type func)
{
    _chain = new mud::test::_given<Context>(_feat, id, func);
    return *_chain;
}

template<typename Context>
mud::test::_given<Context>&
mud::test::_given<Context>::Data(std::function<_base_table::ptr()> func)
{
    _data = func();
    return *this;
}

template<typename Context>
bool
mud::test::_given<Context>::operator()(context_type& ctx)
{
    if (_func == nullptr) {
        std::cerr << "'" << _id << "' has no function" << std::endl;
        throw std::bad_function_call();
    }
    bool result = true;
    try {
        ctx.data(_data);
        _func(ctx);
    } catch (const mud::test::assertion_failed& ex) {
        // An assertion failed
        std::cerr << "'" << id() << "' threw an assertion:" << std::endl
                  << ex.what() << std::endl;
        result = false;
    } catch (const std::exception& ex) {
        // Any standard exception
        std::cerr << "'" << id()
                  << "' threw an unexpected exception:" << std::endl
                  << ex.what() << std::endl;
        result = false;

    } catch (...) {
        // Any unknown exception
        std::cerr << "'" << id() << "' threw an unknown exception."
                  << std::endl;
        result = false;
    }
    if (_chain != nullptr) {
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
    if (_chain != nullptr) {
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
    _when<context_type>& Data(std::function<_base_table::ptr()> func);

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
    _feature<context_type>& _feat;
    std::string _id;
    function_type _func;
    _base_table::ptr _data;
    mud::test::_when<context_type>* _chain;
};

template<typename Context>
mud::test::_when<Context>::_when(mud::test::_feature<Context>& feature,
                                 const std::string& id)
  : _feat(feature), _id(id), _func(nullptr), _chain(nullptr)
{}

template<typename Context>
mud::test::_when<Context>::_when(mud::test::_feature<Context>& feature,
                                 const std::string& id, function_type func)
  : _feat(feature), _id(id), _func(func), _chain(nullptr)
{}

template<typename Context>
mud::test::_when<Context>&
mud::test::_when<Context>::And(const std::string& id)
{
    _chain = _feat.construct_when(id);
    return *_chain;
}

template<typename Context>
mud::test::_when<Context>&
mud::test::_when<Context>::And(const std::string& id, function_type func)
{
    _chain = new mud::test::_when<Context>(_feat, id, func);
    return *_chain;
}

template<typename Context>
mud::test::_when<Context>&
mud::test::_when<Context>::Data(std::function<_base_table::ptr()> func)
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
mud::test::_when<Context>::operator()(context_type& ctx)
{
    if (_func == nullptr) {
        std::cerr << "'" << _id << "' has no function" << std::endl;
        throw std::bad_function_call();
    }
    bool result = true;
    try {
        ctx.data(_data);
        _func(ctx);
    } catch (const mud::test::assertion_failed& ex) {
        // An assertion failed
        std::cerr << "'" << id() << "' threw an assertion:" << std::endl
                  << ex.what() << std::endl;
        result = false;
    } catch (const std::exception& ex) {
        // Any standard exception
        std::cerr << "'" << id()
                  << "' threw an unexpected exception:" << std::endl
                  << ex.what() << std::endl;
        result = false;

    } catch (...) {
        // Any unknown exception
        std::cerr << "'" << id() << "' threw an unknown exception."
                  << std::endl;
        result = false;
    }
    if (_chain != nullptr) {
        result &= (*_chain)(ctx);
    }
    return result;
}

template<typename Context>
void
mud::test::_when<Context>::dump(const char* prefix /* = "When" */)
{
    std::cout << "   " << prefix << " " << _id << std::endl;
    if (_chain != nullptr) {
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
    _then<context_type>& Data(std::function<_base_table::ptr()> func);

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
    _feature<context_type>& _feat;
    std::string _id;
    function_type _func;
    _base_table::ptr _data;
    mud::test::_then<context_type>* _chain;
};

template<typename Context>
mud::test::_then<Context>::_then(mud::test::_feature<Context>& feature,
                                 const std::string& id)
  : _feat(feature), _id(id), _func(nullptr), _chain(nullptr)
{}

template<typename Context>
mud::test::_then<Context>::_then(mud::test::_feature<Context>& feature,
                                 const std::string& id, function_type func)
  : _feat(feature), _id(id), _func(func), _chain(nullptr)
{}

template<typename Context>
mud::test::_then<Context>&
mud::test::_then<Context>::And(const std::string& id)
{
    _chain = _feat.construct_then(id);
    return *_chain;
}

template<typename Context>
mud::test::_then<Context>&
mud::test::_then<Context>::And(const std::string& id, function_type func)
{
    _chain = new mud::test::_then<Context>(_feat, id, func);
    return *_chain;
}

template<typename Context>
mud::test::_then<Context>&
mud::test::_then<Context>::Data(std::function<_base_table::ptr()> func)
{
    _data = func();
    return *this;
}

template<typename Context>
bool
mud::test::_then<Context>::operator()(context_type& ctx)
{
    if (_func == nullptr) {
        std::cerr << "'" << _id << "' has no function" << std::endl;
        throw std::bad_function_call();
    }

    bool result = true;
    try {
        ctx.data(_data);
        _func(ctx);
    } catch (const mud::test::assertion_failed& ex) {
        // An assertion failed
        std::cerr << "'" << id() << "' threw an assertion:" << std::endl
                  << ex.what() << std::endl;
        result = false;
    } catch (const std::exception& ex) {
        // Any standard exception
        std::cerr << "'" << id()
                  << "' threw an unexpected exception:" << std::endl
                  << ex.what() << std::endl;
        result = false;

    } catch (...) {
        // Any unknown exception
        std::cerr << "'" << id() << "' threw an unknown exception."
                  << std::endl;
        result = false;
    }
    if (_chain != nullptr) {
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
    if (_chain != nullptr) {
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
    void Samples(std::function<_base_table::ptr()> func);

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

    _feature<context_type>& _feat;
    std::string _id;
    _base_table::ptr _samples;
    mud::test::_given<context_type>* _given;
    mud::test::_when<context_type>* _when;
    mud::test::_then<context_type>* _then;
};

template<typename Context>
mud::test::_scenario<Context>::_scenario(mud::test::_feature<Context>& feature,
                                         const std::string& id)
  : _feat(feature), _id(id)
{}

template<typename Context>
mud::test::_given<Context>&
mud::test::_scenario<Context>::Given(const std::string& id)
{
    _given = _feat.construct_given(id);
    return *_given;
}

template<typename Context>
mud::test::_given<Context>&
mud::test::_scenario<Context>::Given(const std::string& id, function_type func)
{
    _given = new mud::test::_given<Context>(_feat, id, func);
    return *_given;
}

template<typename Context>
mud::test::_when<Context>&
mud::test::_scenario<Context>::When(const std::string& id)
{
    _when = _feat.construct_when(id);
    return *_when;
}

template<typename Context>
mud::test::_when<Context>&
mud::test::_scenario<Context>::When(const std::string& id, function_type func)
{
    _when = new mud::test::_when<Context>(_feat, id, func);
    return *_when;
}

template<typename Context>
mud::test::_then<Context>&
mud::test::_scenario<Context>::Then(const std::string& id)
{
    _then = _feat.construct_then(id);
    return *_then;
}

template<typename Context>
mud::test::_then<Context>&
mud::test::_scenario<Context>::Then(const std::string& id, function_type func)
{
    _then = new mud::test::_then<Context>(_feat, id, func);
    return *_then;
}

template<typename Context>
void
mud::test::_scenario<Context>::Samples(std::function<_base_table::ptr()> func)
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
    if (_samples && _samples->rows() > 0) {
        bool result = true;
        for (size_t i = 0; i < _samples->rows(); ++i) {
            context_type ctx;
            ctx.samples(_samples);
            ctx.row(i);
            result &= run(ctx);
        }
        return result;
    } else {
        context_type ctx;
        return run(ctx);
    }
}

template<typename Context>
bool
mud::test::_scenario<Context>::run(context_type& ctx)
{
    static const char* color_default = "\x1b[0m";
    static const char* color_red = "\x1b[31m";
    static const char* color_green = "\x1b[32m";
    if (!(*_given)(ctx)) {
        std::cout << "[" << color_red << "FAIL" << color_default << "] " << _id;
        if (ctx.samples()) {
            std::cout << " [" << ctx.row()+1 << "/" << ctx.samples()->rows()
                      << "]";
        }
        std::cout << std::endl;
        return false;
    }
    if (!(*_when)(ctx)) {
        std::cout << "[" << color_red << "FAIL" << color_default << "] " << _id;
        if (ctx.samples()) {
            std::cout << " [" << ctx.row()+1 << "/" << ctx.samples()->rows()
                      << "]";
        }
        std::cout << std::endl;
        return false;
    }
    if (!(*_then)(ctx)) {
        std::cout << "[" << color_red << "FAIL" << color_default << "] " << _id;
        if (ctx.samples()) {
            std::cout << " [" << ctx.row()+1 << "/" << ctx.samples()->rows()
                      << "]";
        }
        std::cout << std::endl;
        return false;
    }
    std::cout << "[" << color_green << "PASS" << color_default << "] " << _id;
    if (ctx.samples()) {
        std::cout << " [" << ctx.row()+1 << "/" << ctx.samples()->rows() << "]";
    }
    std::cout << std::endl;
    return true;
}

/**
 * @brief A base class for a gherkin feature.
 *
 * This class defines the required method for any feature and provides a means
 * to have all features to be registered within a feature factory.
 */
class MUDLIB_TEST_API _base_feature
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
mud::test::_feature<Context>::register_given(const std::string& id,
                                             function_type func)
{
    _given_library[id] = func;
}

template<typename Context>
void
mud::test::_feature<Context>::register_when(const std::string& id,
                                            function_type func)
{
    _when_library[id] = func;
}

template<typename Context>
void
mud::test::_feature<Context>::register_then(const std::string& id,
                                            function_type func)
{
    _then_library[id] = func;
}

template<typename Context>
mud::test::_given<Context>*
mud::test::_feature<Context>::construct_given(const std::string& id)
{
    auto iter = _given_library.find(id);
    if (iter != _given_library.end()) {
        return new mud::test::_given<Context>(*this, iter->first, iter->second);
    }
    throw mud::test::not_specified(id);
};

template<typename Context>
mud::test::_when<Context>*
mud::test::_feature<Context>::construct_when(const std::string& id)
{
    auto iter = _when_library.find(id);
    if (iter != _when_library.end()) {
        return new mud::test::_when<Context>(*this, iter->first, iter->second);
    }
    throw mud::test::not_specified(id);
};

template<typename Context>
mud::test::_then<Context>*
mud::test::_feature<Context>::construct_then(const std::string& id)
{
    auto iter = _then_library.find(id);
    if (iter != _then_library.end()) {
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
MUDLIB_TEST_API bool
AssertFailed(const char* file, int line, const std::string& details);

/**
 * @brief Template class that verifies if two values (result and expected) are
 *        the same. If they are not the same, and assertion is thrown.
 *        The check is based on the @c result inequality operation.
 * @param file [in] The file name.
 * @param line [in] The line number.
 * @param expected [in] The expected outcome.
 * @param result [in] The resulting value to verify.
 */
template<typename T, typename Y>
bool
Assert(const char* file, int line, T expected, Y result)
{
    if (result != expected) {
        std::stringstream sstr;
        sstr << "  Expected: " << expected << std::endl
             << "  Result  : " << result;
        AssertFailed(file, line, sstr.str());
    }
    return true;
}

/**
 * @brief Template class that verifies if two values (result and expected) are
 *        the same. If they are not the same, and assertion is thrown.
 *        The check is based on the @c result inequality operation.
 * @param file [in] The file name.
 * @param line [in] The line number.
 * @param expected [in] The expected outcome (nullptr).
 * @param result [in] The resulting value to verify.
 */
template<typename Y>
bool
Assert(const char* file, int line, std::nullptr_t expected, Y result)
{
    if (result != expected) {
        std::stringstream sstr;
        sstr << "  Expected: nullptr" << std::endl
             << "  Result  : " << result;
        AssertFailed(file, line, sstr.str());
    }
    return true;
}

/**
 * @brief A factory to hold all the features.
 */
class MUDLIB_TEST_API _feature_factory
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
    static std::pair<size_t, size_t> run(
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
    _feature_registrar(const std::string& id)
    {
        _feature_factory::register_feature(id, [] { return new Feature(); });
    }
};

/* clang-format off */

/**
 * @brief Macro to define a context.
 */

#define CONTEXT()                                                            \
    namespace /* unnamed */ {                                                \
    class context : public mud::test::_base_context                          \
    {                                                                        \
    public:

#define CONTEXT_1(BASE)                                                      \
    namespace /* unnamed */ {                                                \
    class context : public mud::test::_base_context, public BASE             \
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
            static const char* color_default = "\x1b[0m";                    \
            static const char* color_bold    = "\x1b[1m";                    \
            std::cout << color_bold                                          \
                      << "FEATURE: " << _id                                  \
                      << color_default << std::endl;                         \
            std::pair<size_t, size_t> result = {-1, -1};                     \
            {                                                                \
                class Dummy { public: bool run() { return true; }};          \
                Dummy scnr;

/**
 * @brief Macro to define a @c SCENARIO
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
 * @brief Macro to define a @c GIVEN step.
 */
#define GIVEN(...)                                                           \
                scnr.Given( __VA_ARGS__)

/**
 * @brief Macro to define a @c WHEN step.
 */
#define WHEN(...)                                                            \
                ;                                                            \
                scnr.When(__VA_ARGS__)

/**
 * @brief Macro to define a @c THEN step.
 */
#define THEN(...)                                                            \
                ;                                                            \
                scnr.Then(__VA_ARGS__)

/**
 * @brief Macro to define an @c AND step that can be used after a @GIVEN,
 *        @c WHEN and @c THEN step.
 */
#define AND(...)                                                             \
                .And(__VA_ARGS__)

/**
 * @brief Macro to define a data table that can be used in a @c GIVEN, @c WHEN
 *        and @c THEN steps. The entire table is made available to all the steps
 *        in its entirety and the scenario is only executed once.
 *        This is in contract to the @C SAMPLES table that executes the @c
 *        SCENARIO for each row of the samples table.
 */
#define DATA(...)                                                            \
                .Data([]() -> mud::test::_base_table::ptr {                  \
                    auto tbl = new mud::test::_table<__VA_ARGS__>;

/**
 * @brief Macro to define a new @c DATA table row.
 */
#define DATA_ROW(...)                                                        \
                    tbl->push_back(__VA_ARGS__);

/**
 * @brief Macro to define the end of a @c DATA table definition.
 */
#define END_DATA()                                                           \
                    return std::shared_ptr<mud::test::_base_table>(tbl);     \
                })

/**
 * @brief Macro to define a samples table that can be used in a @c SCENARIO. The
 *        scenario is executed for each sample row and only the data that is
 *        defined in that single row is available for that scenario execution.
 *        This is in contrast to the @c DATA that makes the entire table
 *        availabel to each step and the scenario is only executed once.
 */
#define SAMPLES(...)                                                         \
                ;                                                            \
                scnr.Samples([]() -> mud::test::_base_table::ptr {           \
                    auto tbl = new mud::test::_table<__VA_ARGS__>;

/**
 * @brief Macro to define a new @c SAMPLE table row.
 */
#define SAMPLE(...)                                                          \
                    tbl->push_back(__VA_ARGS__);

/**
 * @brief Macro to define the end of a @c SAMPLES table definition.
 */
#define END_SAMPLES()                                                        \
                    return std::shared_ptr<mud::test::_base_table>(tbl);     \
                })

/**
 * @brief Macro to define the optional new @c DATA table headings.
 */
#define HEADINGS(...)                                                        \
                    tbl->headings({__VA_ARGS__});

/**
 * @brief Macro to define the end of the @c FEATURE definition.
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
        catch(const std::exception& ex) {                                    \
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
        catch(const std::exception& ex) {                                    \
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

/* clang-format on */

END_MUDLIB_TEST_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TEST_GHERKIN_H_ */
