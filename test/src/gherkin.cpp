#include "mud/test/exception.h"
#include "mud/test/gherkin.h"
#include <memory>

BEGIN_MUDLIB_TEST_NS

mud::test::_table::_table() {
}

mud::test::_table::~_table() {
}

void
mud::test::_table::columns() {
}

size_t
mud::test::_table::row_count() const {
    return _rows.size();
}

const _table::row_type&
mud::test::_table::operator[](int idx) const {
    return _rows[idx];
}

_table
mud::test::_table::sample(int idx) const {
    _table tbl;
    tbl._columns = _columns;
    tbl._rows.push_back(_rows[idx]);
    return tbl;
}

void
mud::test::_table::column_elements(size_t idx, const char* element) {
    _columns[element] = idx;
}

void
mud::test::_table::row_elements(const char* element) {
    _rows.back().push_back(element);
}

std::string
mud::test::_table::entry(int row, int column, const std::string& dummy) const {
    std::string element = _rows[row][column];
    return element;
}

mud::test::_base_feature::_base_feature()
{
}

mud::test::_base_feature::~_base_feature()
{
}

std::map<std::string, _feature_factory::creator_func>
mud::test::_feature_factory::_map;

void
mud::test::_feature_factory::register_feature(
        const std::string& id,
        creator_func func)
{
    _map[id] = func;
}

std::pair<size_t,size_t>
mud::test::_feature_factory::run(
        const std::string& test /* = std::string() */)
{
    /* Get the feature and/or scenario to filter on. */
    std::string filter_feature;
    std::string filter_scenario;
    size_t pos = test.find("#");
    filter_feature = test.substr(0, pos);
    if (pos != std::string::npos) {
        filter_scenario = test.substr(pos+1);
    }

    /* Run though all filters */
    std::pair<size_t,size_t> result = {0, 0};
    for (auto entry: _map)
    {
        /* Skip all features that do not match any filter */
        if (!filter_feature.empty() && filter_feature != entry.first) {
            continue;
        }

        /* Execute the feature and (optionally) filter on scenario */
        std::unique_ptr<mud::test::_base_feature> feature(entry.second());
        std::pair<size_t, size_t> feature_result = feature->run(
                        filter_scenario);
        result.first += feature_result.first;
        result.second += feature_result.second;
    }
    std::cout << std::endl
            << "Total: " << result.first << "  "
            << "Pass: " << result.second << "  "
            << "Fail: " << result.first - result.second
            << std::endl;
    return result;
}

bool
AssertFailed(const char* file, int line, const std::string& details)
{
    std::stringstream sstr;
    sstr << "*** Assertion failed "
            << "(" << file << ":" << line << "):" << std::endl
            << details;
    throw mud::test::assertion_failed(sstr.str());
}

END_MUDLIB_TEST_NS

/* vi: set ai ts=4 expandtab: */

