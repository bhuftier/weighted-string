#pragma once

#include <unordered_map>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <algorithm>

namespace wstr
{

//! A class for weighted element of any type (not just weighted character)
/*!
  * \tparam T           The type of element which are weighted
  * \tparam Hash        Hash template of std::unordered_map
  * \tparam KeyEqual    KeyEqual template of std::unordered_map
  * \tparam Allocator   Allocator template of std::unordered_map
  *
  * \sa std::unordered_map
 */
template <
    class T,
    class Hash = std::hash<T>,
    class KeyEqual = std::equal_to<T>,
    class Allocator = std::allocator<std::pair<const T, double>>
>
class weighted_element
{
    public:

        typedef std::unordered_map<T, double, Hash, KeyEqual, Allocator>    container_t;
        typedef weighted_element<T, Hash, KeyEqual, Allocator>              elem_t;

    private:

        container_t _probabilities;
        double _precision;

    public:

        //! Constructor with all values/probabilities given
        /*!
          * \param probabilities    All probabilities
          * \param strict           Throw an error if the sum of probabilities is not equals to 1
          * \param precision        Set the precision for the `is_good` method
          *
          * \throw std::invalid_argument if strict equal to true and the sum is not equals to 1.
         */
        weighted_element(container_t probabilities, bool strict = true, double precision = 0.) : _probabilities(probabilities), _precision(precision)
        {
            if (strict && !is_good()) { 
                throw std::invalid_argument("The sum of probabilities is not equal to one");
            }
        }

        //! Constructor alias to give different precision
        weighted_element(container_t probabilities, double precision) : weighted_element(probabilities, true, precision)
        {
            
        }
    
        //! Constructor when there is only one element with probability 1
        weighted_element(const T& val) : _probabilities({{val, 1.}}), _precision(0.)
        {
            
        }

        //! == operator
        bool operator==(const elem_t& oth) const
        {
            for (auto& pr : _probabilities) {
                if (oth.p(pr.first) != pr.second) {
                    return false;
                }
            }

            for (auto& pr : oth._probabilities) {
                if (p(pr.first) != pr.second) {
                    return false;
                }
            }

            return true;
        }

        //! != operator
        bool operator!=(const elem_t& oth) const
        {
            return !operator==(oth);
        }
        
        //! []-operator access to the element with given key, if there is not such element, create it
        double& operator[](const T& key)
        {
            return _probabilities[key];
        }

        //! Modification of the precision for the `is_good` method
        void set_precision(double precision)
        {
            _precision = precision;
        }
        
        //! Return the probability for a certain key, by convention, if the key don't exists, return 0
        double p(const T& key) const
        {
            if (_probabilities.end() == _probabilities.find(key)) {
                return 0.0;
            }

            return _probabilities.at(key);
        }

        //! Return the value which has the heighest probability
        const T& heaviest_value() const
        {
            return heaviest()->first;
        }

        //! Return the probability of the heaviest value
        const double& heaviest_proba() const
        {
            return heaviest()->second;
        }

        //! Check if the sum of all probabilities equals to 1.
        bool is_good() const
        {
            double sum = 0.;

            for (const auto& proba : _probabilities) {
                sum += proba.second;
            }

            return fabs(1.0 - sum) < _precision + std::numeric_limits<double>::epsilon();
        }

    private:

        typename container_t::const_iterator heaviest() const
        {
            if (_probabilities.size() == 0) {
                throw std::runtime_error("There is no value with probability greater than 0.");
            }

            return std::max_element(_probabilities.begin(), _probabilities.end(), [](auto &p1, auto &p2) {
                return p1.second < p2.second;
            });
        }
};

}