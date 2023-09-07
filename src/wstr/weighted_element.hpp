#pragma once

#include <unordered_map>
#include <array>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <algorithm>

namespace wstr
{

//! A class for weighted element of any type (not just weighted character)
/*!
  * \tparam T           The type of element which are weighted
  * \tparam Container   The container type for probabilities.
  *
  * The Container type should have different implemented methods:
  * - Container(const Container&)               : A copy constructor
  * - bool operator==(const Container&) const   : Check if two containers are equals
  * - double& operator[](const T&)              : Create a new element with default probability and return it
  * - double at(const T&) const                 : Return the probability of an element (should be 0 if the element is not in the container!)
  * - const T& heaviest() const                 : Return the value with the heaviest probability
  * - double sum() const                        : Return the sum of all probabilities
  *
  * \sa wstr::w_map
  * \sa wstr::w_array
 */
template <
    class T,
    class Container
>
class weighted_element
{
    private:

        Container _probabilities;

    public:

        //! Constructor with all values/probabilities given
        /*!
          * \param probabilities    All probabilities
          * \param strict           Throw an error if the sum of probabilities is not equals to 1
          * \param precision        Set the precision for the `is_good` method
          *
          * \throw std::invalid_argument if strict equal to true and the sum is not equals to 1.
         */
        weighted_element(Container probabilities, bool strict = true, double precision = 0.) : _probabilities(probabilities)
        {
            if (strict && !is_good(precision)) { 
                throw std::invalid_argument("The sum of probabilities is not equal to one");
            }
        }

        //! Constructor alias to give different precision
        weighted_element(Container probabilities, double precision) : weighted_element(probabilities, true, precision)
        {
            
        }

        //! Empty constructor
        weighted_element() : weighted_element(Container(), false)
        {

        }

        //! == operator
        bool operator==(const weighted_element<T, Container>& oth) const
        {
            return _probabilities == oth._probabilities;
        }

        //! != operator
        bool operator!=(const weighted_element<T, Container>& oth) const
        {
            return !operator==(oth);
        }
        
        //! []-operator access to the element with given key, if there is not such element, create it
        double& operator[](const T& key)
        {
            return _probabilities[key];
        }
        
        //! Return the probability for a certain key, by convention, if the key don't exists, return 0
        double p(const T& key) const
        {
            return _probabilities.at(key);
        }

        //! Return the value which has the heighest probability
        const T& heaviest_value() const
        {
            return _probabilities.heaviest();
        }

        //! Return the probability of the heaviest value
        double heaviest_proba() const
        {
            return _probabilities.at(heaviest_value());
        }

        //! Check if the sum of all probabilities equals to 1.
        bool is_good(double precision = 0.) const
        {
            double sum = _probabilities.sum();

            return fabs(1.0 - sum) < precision + std::numeric_limits<double>::epsilon();
        }

        //! Get container and all probabilities
        const Container& probabilities() const
        {
            return _probabilities;
        }
};


//! Container for weighted element based on unordered map
/*!
  * \tparam T           The type of element which is weighted
  * \tparam Hash        Hash template of std::unordered_map
  * \tparam KeyEqual    KeyEqual template of std::unordered_map
  * \tparam Allocator   Allocator template of std::unordered_map
  *
  * \sa std::unordered_map
 */ 
template<
    class T,
    class Hash = std::hash<T>,
    class KeyEqual = std::equal_to<T>,
    class Allocator = std::allocator<std::pair<const T, double>>
>
class w_map : public std::unordered_map<T, double, Hash, KeyEqual, Allocator> 
{
    public:

        //! Use basic constructor to allow initialization with bracket such as {{'a', .2}, {'b', .8}}
        using std::unordered_map<T, double, Hash, KeyEqual, Allocator>::unordered_map;

        double at(const T& key) const
        {
            if (this->end() == this->find(key)) {
                return 0.0;
            }

            return std::unordered_map<T, double, Hash, KeyEqual, Allocator>::at(key);
        }
        
        const T& heaviest() const
        {
            if (0 == this->size()) {
                throw std::runtime_error("There is no value with probability greater than 0.");
            }

            return std::max_element(this->begin(), this->end(), [](auto &p1, auto &p2) {
                return p1.second < p2.second;
            })->first;
        }

        double sum() const
        {
            double sum = 0.;

            for (const auto& proba : *this) {
                sum += proba.second;
            }

            return sum;
        }
};


//! Container for weighted element based on array
/*!
  * \tparam T           The type of element which is weighted
  * \tparam N           The size of the array 
  * \tparam Translator  Type of object to translate
  *
  * The Translator type must have two static methods:
  * - const T& get_element(size_t)  : Return the element at a given indice.
  * - size_t get_indice(const T&)   : Return the indice of a given element (should be between 0 and N - 1).
  *                                   If the element do not belong to the array, should throw a std::runtime_error exception.
  *
  * \sa std::array
 */ 
template<
    class T,
    std::size_t N,
    class Translator
>
class w_array : public std::array<double, N>
{
    public:

        //! Use basic constructor to allow initialization with bracket such as {.2, .8}
        template<class... U>
        w_array(U&&... u) : std::array<double, N>{std::forward<U>(u)...}
        {

        }

        w_array() : std::array<double, N>()
        {
            // Values initialized to probability 0
            for (size_t i = 0; i < N; ++i) {
                std::array<double, N>::operator[](i) = 0.;
            }
        }

        double& operator[](const T& key)
        {   
            return std::array<double, N>::operator[](Translator::get_indice(key));   
        }

        double at(const T& key) const
        {
            try {
                return std::array<double, N>::at(Translator::get_indice(key));
            }
            catch (const std::runtime_error& e) {
                return 0.;
            }
        }
        
        const T& heaviest() const
        {
            return Translator::get_element(std::distance(this->begin(), std::max_element(this->begin(), this->end())));
        }

        double sum() const
        {
            double sum = 0.;

            for (double proba : *this) {
                sum += proba;
            }

            return sum;
        }
};

}