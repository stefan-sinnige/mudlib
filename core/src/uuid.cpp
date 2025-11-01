#include "mud/core/uuid.h"
#include <random>

BEGIN_MUDLIB_CORE_NS

/* ======================================================================
 * Version 4
 * ====================================================================== */

uuid_v4::uuid_v4(bool isNull)
{
    if (!isNull) {
        static std::random_device g_device;
        std::mt19937_64 rng(g_device());
        _lower = rng();
        _upper = rng();
        to_string();
    }
    else {
        clear();
    }
}

uuid_v4::uuid_v4(const std::string& str)
{
    _lower = _upper = 0;

    // If the string is not large enough to hold the UUID, it is invalid.
    if (str.size() < 36) {
        clear();
        return;
    }

    // Iterate over all characters and convert them, unless we find an
    // unexpected character. Start with the upper part.
    bool valid = true;
    char ch;
    uint8_t shift = 60;
    for (int i = 0; valid && i < 18; ++i) {
        ch = str[i];

        /* Check for '-' */
        if (i == 8 || i == 13) {
            valid = (ch == '-');
            continue;
        }

        /* Convert hex digit */
        if (ch >= '0' && ch <= '9') {
            _upper |= (uint64_t(ch - '0') << shift);
            shift -= 4;
        }
        else
        if (ch >= 'a' && ch <= 'f') {
            _upper |= (uint64_t(ch - 'a' + 10) << shift);
            shift -= 4;
        }
        else
        if (ch >= 'A' && ch <= 'F') {
            _upper |= (uint64_t(ch - 'A' + 10) << shift);
            shift -= 4;
        }
        else {
            valid = false;
        }
    }
    if (valid && str[18] != '-') {
        valid = false;
    }

    // The lower part
    shift = 60;
    for (int i = 19; valid && i < 36; ++i) {
        ch = str[i];

        /* Check for '-' */
        if (i == 23) {
            valid = (ch == '-');
            continue;
        }

        /* Convert hex digit */
        if (ch >= '0' && ch <= '9') {
            _lower |= (uint64_t(ch - '0') << shift);
            shift -= 4;
        }
        else
        if (ch >= 'a' && ch <= 'f') {
            _lower |= (uint64_t(ch - 'a' + 10) << shift);
            shift -= 4;
        }
        else
        if (ch >= 'A' && ch <= 'F') {
            _lower |= (uint64_t(ch - 'A' + 10) << shift);
            shift -= 4;
        }
        else {
            valid = false;
        }
    }
    if (!valid) {
        clear();
        return;
    }
    to_string();
}

uuid_v4::uuid_v4(uuid_v4&& other)
{
    (void) operator=(std::move(other));
}

uuid_v4&
uuid_v4::operator=(uuid_v4&& other)
{
    _lower = other._lower;
    _upper = other._upper;
    _str = other._str;
    other.clear();
    return *this;
}

bool
uuid_v4::operator==(const uuid_v4& other) const
{
    return _lower == other._lower && _upper == other._upper;
}

bool
uuid_v4::operator!=(const uuid_v4& other) const
{
    return !operator==(other);
}

const std::string&
uuid_v4::str() const
{
    return _str;
}

uuid_v4::operator const std::string&() const
{
    return _str;
}

bool
uuid_v4::null() const
{
    return _upper == 0 &&  _lower == 0;
}

void
uuid_v4::clear()
{
    _lower = _upper = 0;
    _str = "00000000-0000-0000-0000-000000000000";
}

void
uuid_v4::to_string()
{
    // Reserve the string.
    _str.clear();
    _str.reserve(36);

    /* Upper part */
    uint8_t shift = 60;
    uint64_t mask = 0xF000000000000000;
    for (int i = 0; i < 8; ++i) {
        uint8_t nibble = (_upper & mask) >> shift;
        if (nibble < 10) {
            _str.push_back('0' + nibble);
        }
        else {
            _str.push_back('A' + nibble - 10);
        }
        shift -= 4;
        mask >>= 4;
    }
    _str.push_back('-');
    for (int i = 0; i < 4; ++i) {
        uint8_t nibble = (_upper & mask) >> shift;
        if (nibble < 10) {
            _str.push_back('0' + nibble);
        }
        else {
            _str.push_back('A' + nibble - 10);
        }
        shift -= 4;
        mask >>= 4;
    }
    _str.push_back('-');
    for (int i = 0; i < 4; ++i) {
        uint8_t nibble = (_upper & mask) >> shift;
        if (nibble < 10) {
            _str.push_back('0' + nibble);
        }
        else {
            _str.push_back('A' + nibble - 10);
        }
        shift -= 4;
        mask >>= 4;
    }
    _str.push_back('-');

    /* Lower part */
    shift = 60;
    mask = 0xF000000000000000;
    for (int i = 0; i < 4; ++i) {
        uint8_t nibble = (_lower & mask) >> shift;
        if (nibble < 10) {
            _str.push_back('0' + nibble);
        }
        else {
            _str.push_back('A' + nibble - 10);
        }
        shift -= 4;
        mask >>= 4;
    }
    _str.push_back('-');
    for (int i = 0; i < 12; ++i) {
        uint8_t nibble = (_lower & mask) >> shift;
        if (nibble < 10) {
            _str.push_back('0' + nibble);
        }
        else {
            _str.push_back('A' + nibble - 10);
        }
        shift -= 4;
        mask >>= 4;
    }
}

END_MUDLIB_CORE_NS

std::ostream&
operator<<(std::ostream& ostr, const mud::core::uuid_v4& id)
{
    ostr << id.str();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, mud::core::uuid_v4& id)
{
    std::string str;
    istr >> str;
    id  = mud::core::uuid_v4(str);
    return istr;
}

/* vi: set ai ts=4 expandtab: */
