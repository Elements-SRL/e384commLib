#ifndef E384COMMLIB_GLOBAL_ADDENDUM_H
#define E384COMMLIB_GLOBAL_ADDENDUM_H

#include <vector>
#include <string>
#include <math.h>
#include <limits>
#include <unordered_map>

#include "e384commlib_global.h"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif

/*! \brief Increments a prefix by 1.
 *
 * \param value [out] incremented prefix.
 * \return true if the prefix was successfully incremented; false if the prefix has already the maximum possible value.
 */
static inline bool incrementUnit(E384CL_ARGOUT UnitPfx_t &value);

/*! \brief Increments a prefix by n.
 *
 * \param value [out] incremented prefix.
 * \param n [in] increment.
 * \return true if the prefix was successfully incremented; false if the prefix surpasses the maximum possible value.
 */
static inline bool incrementUnit(E384CL_ARGOUT UnitPfx_t &value, E384CL_ARGIN int n);

/*! \brief Decrements a prefix by 1.
 *
 * \param value [out] decremented prefix.
 * \return true if the prefix was successfully decremented; false if the prefix has already the minimum possible value.
 */
static inline bool decrementUnit(E384CL_ARGOUT UnitPfx_t &value);

/*! \brief Decrements a prefix by n.
 *
 * \param value [out] decremented prefix.
 * \param n [in] decrement.
 * \return true if the prefix was successfully decremented; false if the prefix surpasses the minimum possible value.
 */
static inline bool decrementUnit(E384CL_ARGOUT UnitPfx_t &value, E384CL_ARGIN int n);

static inline bool incrementUnit(E384CL_ARGOUT UnitPfx_t &value) {
    if (value < UnitPfxNum-1) {
        int intValue = static_cast <int> (value) +1;
        value = static_cast <UnitPfx_t> (intValue);
        return true;

    } else {
        return false;
    }
}

static inline bool incrementUnit(E384CL_ARGOUT UnitPfx_t &value, E384CL_ARGIN int n) {
    if (value < UnitPfxNum-n) {
        int intValue = static_cast <int> (value) +n;
        value = static_cast <UnitPfx_t> (intValue);
        return true;

    } else {
        value = UnitPfxNum;
        decrementUnit(value);
        return false;
    }
}

static inline bool decrementUnit(E384CL_ARGOUT UnitPfx_t &value) {
    if (value > 0) {
        int intValue = static_cast <int> (value) -1;
        value = static_cast <UnitPfx_t> (intValue);
        return true;

    } else {
        return false;
    }
}

static inline bool decrementUnit(E384CL_ARGOUT UnitPfx_t &value, E384CL_ARGIN int n) {
    if (value > n-1) {
        int intValue = static_cast <int> (value) -n;
        value = static_cast <UnitPfx_t> (intValue);
        return true;

    } else {
        value = static_cast <UnitPfx_t> (0);
        return false;
    }
}

inline UnitPfx_t operator * (const UnitPfx_t &a, const UnitPfx_t &b) {
    const auto noneInt = static_cast <int> (UnitPfx::UnitPfxNone);
    const auto aNoNone = static_cast <int> (a) - noneInt;
    const auto bNoNone = static_cast <int> (b) - noneInt;
    const auto res = aNoNone + bNoNone + noneInt;
    if (res >= 0 || res < UnitPfx::UnitPfxNum) {
        return static_cast <UnitPfx_t> (res);
    } else {
        return static_cast <UnitPfx_t> (0);
    }
}


inline UnitPfx_t operator / (const UnitPfx_t &a, const UnitPfx_t &b) {
    const auto noneInt = static_cast <int> (UnitPfx::UnitPfxNone);
    const auto aNoNone = static_cast <int> (a) - noneInt;
    const auto bNoNone = static_cast <int> (b) - noneInt;
    const auto res = aNoNone - bNoNone + noneInt;
    if (res >= 0 || res < UnitPfx::UnitPfxNum) {
        return static_cast <UnitPfx_t> (res);
    } else {
        return static_cast <UnitPfx_t> (0);
    }
}

/*! \brief Array with the strings corresponding to the unit prefixes of type UnitPfx_t. */
static const std::string unitPrefixes[UnitPfxNum] = {
    "f",
    "p",
    "n",
    "u",
    "m",
    " ",
    "k",
    "M",
    "G",
    "T",
    "P"
};

/*! \brief Convenience array with the precomputed powers of 1000 used to make unit conversions. */
static const double powersOf1000[UnitPfxNum] = {
    1.0,
    1.0e3,
    1.0e6,
    1.0e9,
    1.0e12,
    1.0e15,
    1.0e18,
    1.0e21,
    1.0e24,
    1.0e27,
    1.0e30
};

/*! \brief Returns the string corresponding to the prefix.
 *
 * \return String corresponding to the prefix.
 */
inline std::string getPrefix(UnitPfx_t prefix) {
    return unitPrefixes[prefix];
}

/*! \brief Returns the prefix corresponding to the string.
 *  \note Returns UnitPfxNum if the string is not found
 *
 * \return String corresponding to the prefix.
 */
inline UnitPfx_t getPrefixString(std::string text) {
    unsigned int idx;
    for (idx = 0; idx < UnitPfxNum; idx++) {
        if (text == unitPrefixes[idx]) {
            break;
        }
    }
    return static_cast <UnitPfx_t> (idx);
}

/*! \typedef Measurement_t
 */

/*! \struct Measurement_t
 * \brief Structure used manage physical quantities that define a value with its unit and unit prefix.
 */
typedef struct Measurement {
    double value = 0.0; /*!< Numerical value. */
    UnitPfx_t prefix = UnitPfxNone; /*!< Unit prefix in the range [femto, Peta]. */
    std::string unit = ""; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */

    /*! \brief Returns the value without prefix.
     *
     * \return Value without prefix, e.g. for 1.5nA returns 1.5e-9.
     */
    double getNoPrefixValue(E384CL_ARGVOID) {
        return value*multiplier();
    }

    /*! \brief Returns the string corresponding to the prefix.
     *
     * \return String corresponding to the prefix.
     */
    std::string getPrefix(E384CL_ARGVOID) {
        return unitPrefixes[prefix];
    }

    /*! \brief Returns the string corresponding to the unit with the prefix.
     *
     * \return String corresponding to the unit with the prefix.
     */
    std::string getFullUnit(E384CL_ARGVOID) {
        return unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the prefix multiplier.
     *
     * \return Prefix multiplier, e.g. 1e-6 for micro.
     */
    double multiplier(E384CL_ARGVOID) {
        unsigned int delta;
        bool deltaPositive;

        if (prefix > UnitPfxNone) {
            delta = prefix-UnitPfxNone;
            deltaPositive = true;

        } else {
            delta = UnitPfxNone-prefix;
            deltaPositive = false;
        }

        if (deltaPositive) {
            return powersOf1000[delta];

        } else {
            return 1.0/powersOf1000[delta];
        }
    }

    /*! \brief Returns a string describing the value with its prefix and unit.
     *
     * \param maxChars [in] maximum number of characters for the returned string.
     * \return String describing the value with its prefix and unit.
     */
    std::string label(E384CL_ARGIN unsigned int maxChars = 8) {
        std::string valueSt = std::to_string(value);
        if (valueSt.length() >= maxChars) {
            valueSt.erase(maxChars);
        }
        size_t dot = valueSt.find_last_of(".");
        size_t notZero = valueSt.find_last_not_of("0");

        if (notZero != std::string::npos) {
            if (dot != std::string::npos) {
                return valueSt.erase(dot < notZero ? notZero+1 : dot) + " " + unitPrefixes[prefix] + unit;

            } else {
                return valueSt + " " + unitPrefixes[prefix] + unit;
            }

        } else {
            return "0 " + unitPrefixes[prefix] + unit;
        }
    }

    /*! \brief Returns the string describing the value with its prefix and unit in a nice fashion.
     *
     * \return String describing the value with its prefix and unit. The value and the prefix are converted so that the value is in the range [1.0, 1000.0[.
     */
    std::string niceLabel(E384CL_ARGVOID) {
        Measurement temp;
        temp.value = value;
        temp.prefix = prefix;
        temp.unit = unit;
        temp.nice();

        return temp.label(5);
    }

    /*! \brief Converts #value given the input unit prefix.
     *
     * \param newPrefix [in] Desired unit prefix.
     */
    void convertValue(E384CL_ARGIN UnitPfx_t newPrefix) {
        unsigned int delta;
        bool deltaPositive;

        if (prefix > newPrefix) {
            delta = prefix-newPrefix;
            deltaPositive = true;

        } else {
            delta = newPrefix-prefix;
            deltaPositive = false;
        }

        if (deltaPositive) {
            value *= powersOf1000[delta];

        } else {
            value /= powersOf1000[delta];
        }
        prefix = newPrefix;
    }

    /*! \brief Converts #value given the input unit multiplier.
     *
     * \param newMultiplier [in] Desired unit multiplier.
     */
    void convertValue(E384CL_ARGIN double newMultiplier) {
        double multiplier = this->multiplier();
        double gain;
        bool gainPositive;

        if (multiplier > newMultiplier) {
            gain = multiplier/newMultiplier;
            gainPositive = true;

        } else {
            gain = newMultiplier/multiplier;
            gainPositive = false;
        }

        double diff;
        double minDiff = (std::numeric_limits <double>::max)();
        unsigned int minDelta = 0;
        for (unsigned int delta = 0; delta < UnitPfxNum; delta++) {
            diff = fabs(powersOf1000[delta]-gain);
            if (diff < minDiff) {
                minDiff = diff;
                minDelta = delta;
            }
        }

        if (gainPositive) {
            value *= powersOf1000[minDelta];
            decrementUnit(prefix, (int)minDelta);

        } else {
            value /= powersOf1000[minDelta];
            incrementUnit(prefix, (int)minDelta);
        }
    }

    /*! \brief Converts #value and #prefix in order to have a final #value in range [1.0, 1000.0[.
     */
    void nice(E384CL_ARGVOID) {
        if ((value == 0.0) || isinf(value)) {
            prefix = UnitPfxNone;

        } else {
            while (fabs(value) >= 1000.0) {
                if (incrementUnit(prefix)) {
                    value /= 1000.0;

                } else {
                    break;
                }
            }

            while (fabs(value) < 1.0) {
                if (decrementUnit(prefix)) {
                    value *= 1000.0;

                } else {
                    break;
                }
            }
        }
    }
} Measurement_t;

static Measurement_t measErr;
static std::vector <Measurement_t> vecErr;

/*! \brief Overloaded equality check for #Measurement_t. \note No conversion is performed, cause the multiplication can introduce rounding errors.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a and \p b have same value, prefix and unit.
*/
inline bool operator == (const Measurement_t &a, const Measurement_t &b) {
    return ((a.value == b.value) && (a.prefix == b.prefix) && (a.unit == b.unit));
}

/*! \brief Overloaded inequality check for #Measurement_t. \note No conversion is performed, cause the multiplication can introduce rounding errors.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a and \p b have different value, prefix or unit.
*/
inline bool operator != (const Measurement_t &a, const Measurement_t &b) {
    return !(a == b);
}

/*! \brief Overloaded inequality check for #Measurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a is smaller than \p b after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator < (const Measurement_t &a, const Measurement_t &b) {
    if (a.unit != b.unit) {
        return false;

    } else {
        /*! Not using convertValue method to avoid changing the input structures */
        if (a.prefix < b.prefix) {
            return a.value < (b.value*powersOf1000[b.prefix-a.prefix]);

        } else {
            return (a.value*powersOf1000[a.prefix-b.prefix]) < b.value;
        }
    }
}

/*! \brief Overloaded inequality check for #Measurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a is smaller than or equal to \p b after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator <= (const Measurement_t &a, const Measurement_t &b) {
    if (a.unit != b.unit) {
        return false;

    } else {
        /*! Not using convertValue method to avoid changing the input structures */
        if (a.prefix < b.prefix) {
            return a.value <= (b.value*powersOf1000[b.prefix-a.prefix]);

        } else {
            return (a.value*powersOf1000[a.prefix-b.prefix]) <= b.value;
        }
    }
}

/*! \brief Overloaded inequality check for #Measurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a is greater than \p b after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator > (const Measurement_t &a, const Measurement_t &b) {
    if (a.unit != b.unit) {
        return false;

    } else {
        /*! Not using convertValue method to avoid changing the input structures */
        if (a.prefix < b.prefix) {
            return a.value > (b.value*powersOf1000[b.prefix-a.prefix]);

        } else {
            return (a.value*powersOf1000[a.prefix-b.prefix]) > b.value;
        }
    }
}

/*! \brief Overloaded inequality check for #Measurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a is greater than or equal to \p b after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator >= (const Measurement_t &a, const Measurement_t &b) {
    if (a.unit != b.unit) {
        return false;

    } else {
        /*! Not using convertValue method to avoid changing the input structures */
        if (a.prefix < b.prefix) {
            return a.value >= (b.value*powersOf1000[b.prefix-a.prefix]);

        } else {
            return (a.value*powersOf1000[a.prefix-b.prefix]) >= b.value;
        }
    }
}

/*! \brief Overloaded sum for #Measurement_t.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the sum of the values of the operands converted to the prefix of the first operand, and the unit equals the unit of the second operand.
 * \note This method assumes the units are compatible and won't check for the sake of speed.
*/
inline Measurement_t operator + (E384CL_ARGIN const Measurement_t &a, E384CL_ARGIN const Measurement_t &b) {
    Measurement_t c = b;
    c.convertValue(a.prefix);
    c.value += a.value;
    return c;
}

/*! \brief Overloaded incremental sum for #Measurement_t.
 *
 * \param a [in] Incremented operand.
 * \param b [in] Summed operand.
 * \return a incremented by b.
 * \note This method assumes the units are compatible and won't check for the sake of speed.
*/
inline Measurement_t operator += (E384CL_ARGIN Measurement_t &a, E384CL_ARGIN const Measurement_t &b) {
    Measurement_t c = b;
    c.convertValue(a.prefix);
    a.value += c.value;
    return a;
}

/*! \brief Overloaded subtraction for #Measurement_t.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the difference of the values of the operands converted to the prefix of the first operand, and the unit equals the unit of the second operand.
 * \note This method assumes the units are compatible and won't check for the sake of speed.
*/
inline Measurement_t operator - (E384CL_ARGIN const Measurement_t &a, E384CL_ARGIN const Measurement_t &b) {
    Measurement_t c = b;
    c.convertValue(a.prefix);
    c.value = a.value-c.value;
    return c;
}

/*! \brief Overloaded multiplication between #Measurement_t and a constant.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the product of the values and the unit equals the unit of the first operand.
*/
template <class T>
inline Measurement_t operator * (E384CL_ARGIN const Measurement_t &a, E384CL_ARGIN const T &b) {
    Measurement_t c = a;
    c.value *= (double)b;
    return c;
}

/*! \brief Overloaded multiplication between #Measurement_t and a constant.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the product of the values and the unit equals the unit of the second operand.
*/
template <class T>
inline Measurement_t operator * (E384CL_ARGIN const T &a, E384CL_ARGIN const Measurement_t &b) {
    return b*a;
}

/*! \brief Overloaded division between #Measurement_t and a constant.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the ratio of the values and the unit equals the unit of the first operand.
*/
template <class T>
inline Measurement_t operator / (E384CL_ARGIN const Measurement_t &a, E384CL_ARGIN const T &b) {
    Measurement_t c = a;
    c.value /= (double)b;
    return c;
}

/*! \struct RangedMeasurement_t
 * \brief Structure used manage physical ranges that define a range with its unit and unit prefix.
 */
typedef struct RangedMeasurement {
    double min; /*!< Minimum value. */
    double max; /*!< Maximum value. */
    double step; /*!< Resolution. */
    UnitPfx_t prefix = UnitPfxNone; /*!< Unit prefix in the range [femto, Peta]. */
    std::string unit = ""; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */

    /*! \brief Returns the number of steps in the range.
     *
     * \return Number of steps in the range.
     */
    uint32_t steps(E384CL_ARGVOID) {
        return static_cast <uint32_t> (round(1.0+(max-min)/step));
    }

    /*! \brief Returns the string corresponding to the prefix.
     *
     * \return String corresponding to the prefix.
     */
    std::string getPrefix(E384CL_ARGVOID) {
        return unitPrefixes[prefix];
    }

    /*! \brief Returns the string corresponding to the unit with the prefix.
     *
     * \return String corresponding to the unit with the prefix.
     */
    std::string getFullUnit(E384CL_ARGVOID) {
        return unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the prefix multiplier.
     *
     * \return Prefix multiplier, e.g. 1e-6 for micro.
     */
    double multiplier(E384CL_ARGVOID) {
        unsigned int delta;
        bool deltaPositive;

        if (prefix > UnitPfxNone) {
            delta = prefix-UnitPfxNone;
            deltaPositive = true;

        } else {
            delta = UnitPfxNone-prefix;
            deltaPositive = false;
        }

        if (deltaPositive) {
            return powersOf1000[delta];

        } else {
            return 1.0/powersOf1000[delta];
        }
    }

    /*! \brief Returns a string describing the max value.
     *
     * \param maxChars [in] maximum number of characters for the returned string.
     * \return String describing the max value.
     */
    std::string valueLabel(E384CL_ARGIN unsigned int maxChars = 8) {
        std::string valueSt = std::to_string(max);
        if (valueSt.length() >= maxChars) {
            valueSt.erase(maxChars);
        }
        size_t dot = valueSt.find_last_of(".");
        size_t notZero = valueSt.find_last_not_of("0");

        if (notZero != std::string::npos) {
            if (dot != std::string::npos) {
                return valueSt.erase(dot < notZero ? notZero+1 : dot);

            } else {
                return valueSt;
            }

        } else {
            return "0";
        }
    }

    /*! \brief Returns a string describing the max value with its prefix and unit.
     *
     * \param maxChars [in] maximum number of characters for the returned string.
     * \return String describing the max value with its prefix and unit.
     */
    std::string label(E384CL_ARGIN unsigned int maxChars = 8) {
        return valueLabel(maxChars) + " " + unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the string describing the max value with its prefix and unit in a nice fashion.
     *
     * \return String describing the max value with its prefix and unit. The max value and the prefix are converted so that the value is in the range [1.0, 1000.0[.
     */
    std::string niceLabel(E384CL_ARGVOID) {
        Measurement temp;
        temp.value = max;
        temp.prefix = prefix;
        temp.unit = unit;
        temp.nice();

        return temp.label(5);
    }

    /*! \brief Converts #min, #max and #step given the input unit prefix.
     *
     * \param newPrefix [in] Desired unit prefix.
     */
    void convertValues(E384CL_ARGIN UnitPfx_t newPrefix) {
        unsigned int delta;
        bool deltaPositive;

        if (prefix > newPrefix) {
            delta = prefix-newPrefix;
            deltaPositive = true;

        } else {
            delta = newPrefix-prefix;
            deltaPositive = false;
        }

        if (deltaPositive) {
            min *= powersOf1000[delta];
            max *= powersOf1000[delta];
            step *= powersOf1000[delta];

        } else {
            min /= powersOf1000[delta];
            max /= powersOf1000[delta];
            step /= powersOf1000[delta];
        }
        prefix = newPrefix;
    }

    /*! \brief Converts #min, #max and #step given the input unit multiplier.
     *
     * \param newMultiplier [in] Desired unit multiplier.
     */
    void convertValues(E384CL_ARGIN double newMultiplier) {
        double multiplier = this->multiplier();
        double gain;
        bool gainPositive;

        if (multiplier > newMultiplier) {
            gain = multiplier/newMultiplier;
            gainPositive = true;

        } else {
            gain = newMultiplier/multiplier;
            gainPositive = false;
        }

        double diff;
        double minDiff = (std::numeric_limits <double>::max)();
        unsigned int minDelta = 0;
        for (unsigned int delta = 0; delta < UnitPfxNum; delta++) {
            diff = fabs(powersOf1000[delta]-gain);
            if (diff < minDiff) {
                minDiff = diff;
                minDelta = delta;
            }
        }

        if (gainPositive) {
            min *= powersOf1000[minDelta];
            max *= powersOf1000[minDelta];
            step *= powersOf1000[minDelta];
            decrementUnit(prefix, (int)minDelta);

        } else {
            min /= powersOf1000[minDelta];
            max /= powersOf1000[minDelta];
            step /= powersOf1000[minDelta];
            incrementUnit(prefix, (int)minDelta);
        }
    }

    /*! \brief Returns the range width.
     *
     * \return Difference between max and min.
     */
    double delta(E384CL_ARGVOID) {
        return max-min;
    }

    /*! \brief Returns a reasonable amount of decimals to represent the values in the range.
     *
     * \return A reasonable amount of decimals to represent the values in the range.
     */
    int decimals(E384CL_ARGVOID) {
        int decimals = 0;
        double temp = step;
        while ((fabs(temp-round(temp)) > 0.05 || temp < 1.0) &&
               decimals < 3) {
            decimals++;
            temp *= 10.0;
        }
        return decimals;
    }

    /*! \brief Returns a Measurement_t equivalent to the max value of the range.
     *
     * \return Measurement_t equivalent to the max value of the range.
     */
    Measurement_t getMax(E384CL_ARGVOID) const {
        Measurement_t extreme;
        extreme.value = max;
        extreme.prefix = prefix;
        extreme.unit = unit;
        return extreme;
    }

    /*! \brief Returns a Measurement_t equivalent to the min value of the range.
     *
     * \return Measurement_t equivalent to the min value of the range.
     */
    Measurement_t getMin(E384CL_ARGVOID) const {
        Measurement_t extreme;
        extreme.value = min;
        extreme.prefix = prefix;
        extreme.unit = unit;
        return extreme;
    }

    /*! \brief Returns a Measurement_t equivalent to the x-th step of the range.
     *
     * \return Measurement_t equivalent to the x-th step of the range.
     */
    Measurement_t getXth(E384CL_ARGIN uint32_t x) const {
        Measurement_t xth;
        xth.value = min+step*static_cast <double> (x);
        xth.prefix = prefix;
        xth.unit = unit;
        return xth;
    }

    /*! \brief Inverse function of getXth.
     *
     * \return Index x such that the x-th step of the range best matches the input Measurement_t.
     */
    uint32_t getClosestIndex(E384CL_ARGIN Measurement_t m) {
        m.convertValue(prefix);
        return static_cast <uint32_t> (round((m.value-getMin().value)/step));
    }

    /*! \brief Checks if a Measurement_t is within the range.
     *
     * \return true if the Measurement_t is within the range, false otherwise.
     */
    bool includes(E384CL_ARGIN const Measurement_t value) {
        if (value >= getMin() && value <= getMax()) {
            return true;

        } else {
            return false;
        }
    }
} RangedMeasurement_t;

/*! \brief Overloaded equality check for #RangedMeasurement_t. \note No conversion is performed, cause the multiplication can introduce rounding errors.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a and \p b have same value, prefix and unit.
*/
inline bool operator == (const RangedMeasurement_t &a, const RangedMeasurement_t &b) {
    return ((a.min == b.min) && (a.max == b.max) && (a.step == b.step) && (a.prefix == b.prefix) && (a.unit == b.unit));
}

/*! \brief Overloaded inequality check for #RangedMeasurement_t. \note No conversion is performed, cause the multiplication can introduce rounding errors.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a and \p b have different value, prefix or unit.
*/
inline bool operator != (const RangedMeasurement_t &a, const RangedMeasurement_t &b) {
    return !(a == b);
}

/*! \brief Overloaded inequality check for #RangedMeasurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a.getMax() is smaller than \p b.getMax() after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator < (const RangedMeasurement_t &a, const RangedMeasurement_t &b) {
    return a.getMax() < b.getMax();
}

/*! \brief Overloaded inequality check for #RangedMeasurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a.getMax() is smaller than or equal to \p b.getMax() after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator <= (const RangedMeasurement_t &a, const RangedMeasurement_t &b) {
    return a.getMax() <= b.getMax();
}

/*! \brief Overloaded inequality check for #RangedMeasurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a.getMax() is greater than \p b.getMax() after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator > (const RangedMeasurement_t &a, const RangedMeasurement_t &b) {
    return a.getMax() > b.getMax();
}

/*! \brief Overloaded inequality check for #RangedMeasurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a.getMax() is greater than or equal to \p b.getMax() after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator >= (const RangedMeasurement_t &a, const RangedMeasurement_t &b) {
    return a.getMax() >= b.getMax();
}

/*! \struct CompensationControl_t
 * \brief Structure used to return detailed information on a specific compensation implemented by the HW.
 */
typedef struct CompensationControl {
    bool implemented = false; /*!< True if the corresponding compensation is implemented by the device. */
    double min = 0.0; /*!< Minimum compensable value. */
    double max = 1.0; /*!< Maximum compensable value globally. */
    double minCompensable = 0.0; /*!< Minimum compensable value given also the value of the other compensations. */
    double maxCompensable = 1.0; /*!< Maximum compensable value given also the value of the other compensations. */
    double steps = 2; /*!< Number of steps between #min and #max. */
    double step = 1.0; /*!< Resolution. */
    int decimals = 0; /*!< Decimals to represent the compensated value. */
    double value = 0.0; /*!< Compensated value. */
    UnitPfx_t prefix = UnitPfxNone; /*!< Unit prefix in the range [femto, Peta]. */
    std::string unit = ""; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */
    std::string name = ""; /*!< Name of the compensation. */

    /*! \brief Returns the range of the control.
     *
     * \return Range of the control.
     */
    RangedMeasurement_t getRange(E384CL_ARGVOID) {
        RangedMeasurement_t range = {min, max, step, prefix, unit};
    }

    /*! \brief Returns the compensable range of the control given also the value of the other compensations.
     *
     * \return Compensable range of the control.
     */
    RangedMeasurement_t getCompensableRange(E384CL_ARGVOID) {
        RangedMeasurement_t range = {minCompensable, maxCompensable, step, prefix, unit};
        return range;
    }

    /*! \brief Returns the string corresponding to the prefix.
     *
     * \return String corresponding to the prefix.
     */
    std::string getPrefix(E384CL_ARGVOID) {
        return unitPrefixes[prefix];
    }

    /*! \brief Returns the string corresponding to the unit with the prefix.
     *
     * \return String corresponding to the unit with the prefix.
     */
    std::string getFullUnit(E384CL_ARGVOID) {
        return unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the string describing the compensation with its prefix and unit.
     *
     * \return String describing the compensation with its prefix and unit.
     */
    std::string title(E384CL_ARGVOID) {
        if (unit != "") {
            return name + " [" + unitPrefixes[prefix] + unit + "]";

        } else {
            return name;
        }
    }
} CompensationControl_t;

typedef enum CalibrationTypes {
    CalTypesVcGainAdc,
    CalTypesVcOffsetAdc,
    CalTypesVcGainDac,
    CalTypesVcOffsetDac,
    CalTypesRsCorrOffsetDac,
    CalTypesRShuntConductance,
    CalTypesCcGainAdc,
    CalTypesCcOffsetAdc,
    CalTypesCcGainDac,
    CalTypesCcOffsetDac,
    CalTypesNum
} CalibrationTypes_t;

/*! \struct CalibrationChannels_t
 * \brief Structure used to contain the calibration values for all the channels.
 */
typedef struct CalibrationChannels {
    std::vector <Measurement_t> channels;

    const std::vector <Measurement_t>& getValues() const {
        return channels;
    }

    const Measurement_t& getValue(int channelIdx) const {
        if (channelIdx < channels.size()) {
            return channels[channelIdx];
        }
        return measErr;
    }

    void setValues(std::vector <Measurement_t> values) {
        if (values.size() == channels.size()) {
            channels = values;
        }
    }

    void setValue(int channelIdx, Measurement_t value) {
        if (channelIdx < channels.size()) {
            channels[channelIdx] = value;
        }
    }

    void convertValue(int channelIdx, UnitPfx_t prefix) {
        if (channelIdx < channels.size()) {
            channels[channelIdx].convertValue(prefix);
        }
    }
} CalibrationChannels_t;

/*! \struct CalibrationRanges_t
 * \brief Structure used to contain the calibration values for all the relevant ranges.
 */
typedef struct CalibrationRanges {
    std::vector <CalibrationChannels_t> ranges;

    const std::vector <Measurement_t>& getValues(int rangeIdx) const {
        if (rangeIdx < ranges.size()) {
            return ranges[rangeIdx].getValues();
        }
        return vecErr;
    }

    const Measurement_t& getValue(int rangeIdx, int channelIdx) const {
        if (rangeIdx < ranges.size()) {
            return ranges[rangeIdx].getValue(channelIdx);
        }
        return measErr;
    }

    void setValues(int rangeIdx, std::vector <Measurement_t> values) {
        if (rangeIdx < ranges.size()) {
            ranges[rangeIdx].setValues(values);
        }
    }

    void setValue(int rangeIdx, int channelIdx, Measurement_t value) {
        if (rangeIdx < ranges.size()) {
            ranges[rangeIdx].setValue(channelIdx, value);
        }
    }

    void convertValue(int rangeIdx, int channelIdx, UnitPfx_t prefix) {
        if (rangeIdx < ranges.size()) {
            ranges[rangeIdx].convertValue(channelIdx, prefix);
        }
    }
} CalibrationRanges_t;

static CalibrationRanges_t rangeErr;

/*! \struct CalibrationSamplingModes_t
 * \brief Structure used to contain the calibration values for all the relevant sampling modes (groups of sampling rates).
 */
typedef struct CalibrationSamplingModes {
    std::vector <CalibrationRanges_t> modes;

    const CalibrationRanges_t& getRanges(int modeIdx) const {
        if (modeIdx < modes.size()) {
            return modes[modeIdx];
        }
        return rangeErr;
    }

    const std::vector <Measurement_t>& getValues(int modeIdx, int rangeIdx) const {
        if (modeIdx < modes.size()) {
            return modes[modeIdx].getValues(rangeIdx);
        }
        return vecErr;
    }

    const Measurement_t& getValue(int modeIdx, int rangeIdx, int channelIdx) const {
        if (modeIdx < modes.size()) {
            return modes[modeIdx].getValue(rangeIdx, channelIdx);
        }
        return measErr;
    }

    void setValues(int modeIdx, int rangeIdx, std::vector <Measurement_t> values) {
        if (modeIdx < modes.size()) {
            modes[modeIdx].setValues(rangeIdx, values);
        }
    }

    void setValue(int modeIdx, int rangeIdx, int channelIdx, Measurement_t value) {
        if (modeIdx < modes.size()) {
            modes[modeIdx].setValue(rangeIdx, channelIdx, value);
        }
    }

    void convertValue(int modeIdx, int rangeIdx, int channelIdx, UnitPfx_t prefix) {
        if (modeIdx < modes.size()) {
            modes[modeIdx].convertValue(rangeIdx, channelIdx, prefix);
        }
    }
} CalibrationSamplingModes_t;

/*! \struct CalibrationParams_t
 * \brief Structure used to return calibration values.
 */
typedef struct CalibrationParams {
    CalibrationSamplingModes_t types[CalTypesNum];
    std::unordered_map <uint32_t, uint32_t> sr2srm; /*! sampling rate to sampling rate mode mapping */

    void initialize(CalibrationTypes_t type, uint32_t samplingRateModesNum, uint32_t rangesNum, uint32_t channelsNum, Measurement_t defaultValue) {
        types[type].modes.resize(samplingRateModesNum);
        for (uint32_t srIdx = 0; srIdx < samplingRateModesNum; srIdx++) {
            types[type].modes[srIdx].ranges.resize(rangesNum);
            for (uint32_t rangeIdx = 0; rangeIdx < rangesNum; rangeIdx++) {
                types[type].modes[srIdx].ranges[rangeIdx].channels.resize(channelsNum);
                std::fill(types[type].modes[srIdx].ranges[rangeIdx].channels.begin(), types[type].modes[srIdx].ranges[rangeIdx].channels.end(), defaultValue);
            }
        }
    }

    const bool getSamplingMode(CalibrationTypes_t type, int samplingRateIdx, int &samplingModeIdx) const {
        switch (type) {
        case CalTypesVcGainAdc:
        case CalTypesVcOffsetAdc:
        case CalTypesCcGainAdc:
        case CalTypesCcOffsetAdc:
            if (sr2srm.count(samplingRateIdx) > 0) {
                samplingModeIdx = sr2srm.at(samplingRateIdx);
                return true;
            }
            return false;
        case CalTypesVcGainDac:
        case CalTypesVcOffsetDac:
        case CalTypesRsCorrOffsetDac:
        case CalTypesRShuntConductance:
        case CalTypesCcGainDac:
        case CalTypesCcOffsetDac:
            samplingModeIdx = 0;
            return true;
        }
        return false;
    }

    const CalibrationRanges_t& getRanges(CalibrationTypes_t type, int samplingRateIdx) const {
        int samplingModeIdx = 0;
        if (getSamplingMode(type, samplingRateIdx, samplingModeIdx)) {
            return types[type].getRanges(samplingModeIdx);
        }
        return rangeErr;
    }

    const std::vector <Measurement_t>& getValues(CalibrationTypes_t type, int samplingRateIdx, int rangeIdx) const {
        int samplingModeIdx = 0;
        if (getSamplingMode(type, samplingRateIdx, samplingModeIdx)) {
            return types[type].getValues(samplingModeIdx, rangeIdx);
        }
        return vecErr;
    }

    const Measurement_t& getValue(CalibrationTypes_t type, int samplingRateIdx, int rangeIdx, int channelIdx) const {
        int samplingModeIdx = 0;
        if (getSamplingMode(type, samplingRateIdx, samplingModeIdx)) {
            return types[type].getValue(samplingModeIdx, rangeIdx, channelIdx);
        }
        return measErr;
    }

    void setValues(CalibrationTypes_t type, int samplingRateIdx, int rangeIdx, std::vector <Measurement_t> values) {
        int samplingModeIdx = 0;
        if (getSamplingMode(type, samplingRateIdx, samplingModeIdx)) {
            types[type].setValues(samplingModeIdx, rangeIdx, values);
        }
    }

    void setValue(CalibrationTypes_t type, int samplingRateIdx, int rangeIdx, int channelIdx, Measurement_t value) {
        int samplingModeIdx = 0;
        if (getSamplingMode(type, samplingRateIdx, samplingModeIdx)) {
            types[type].setValue(samplingModeIdx, rangeIdx, channelIdx, value);
        }
    }

    void convertValue(CalibrationTypes_t type, int samplingRateIdx, int rangeIdx, int channelIdx, UnitPfx_t prefix) {
        int samplingModeIdx = 0;
        if (getSamplingMode(type, samplingRateIdx, samplingModeIdx)) {
            types[type].convertValue(samplingModeIdx, rangeIdx, channelIdx, prefix);
        }
    }
} CalibrationParams_t;

#ifndef E384COMMLIB_LABVIEW_WRAPPER
} // namespace e384CommLib
#endif

#endif // E384COMMLIB_GLOBAL_ADDENDUM_H
