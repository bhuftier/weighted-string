# Weighted String

A **weighted string** is a string $s$ over an alphabet $\Sigma$ where each position contains a character from the alphabet with a certain probability. In each position, the sum of all probabilities should be equal to one. Here is an example of a weighted string of length 5 over the alphabet $\{a, b\}$.

<center>

|  #  |  1  |  2  |  3  |  4  |  5  |
|:---:|:---:|:---:|:---:|:---:|:---:|
|**a**| .2  | .5  |  0  | .7  | .8  |
|**b**| .8  | .5  |  1  | .3  | .2  |

</center>

Weighted strings are often used to describe DNA sequences where nucleotids are not 100% sure, but it can be useful in other fields.

## This library

This is why this repository has been created. I needed to use some weighted sequence in my internship and I didn't find any library to handle these objects. I chose to create my own library so that other people wouldn't need to code it from scratch.

## Compilation

You need `cmake` and `make` installed on your machine in order to compile and install this library. You need a compiler with `c++14` or more. To compile the library you just have to run `make` in the root folder. You can launch test with `make test`.

## How to use

The main class of this library is `wstr::weighted_string` (or `wstr:w_string`) which extends from `std::vector<wstr::w_char>`. It means that you have access to all method of a vector in a weighted string. When you have big datas, you often have a file to store it. Our class allow to be created from input via the operator `>>`.

The file need a specific format.

### Simple weighted string

You need to give the size of the weighted string followed by the alphabet. Then each line represent a weighted character where each value is the probability of a character of the alphabet. For example, here is a file representing the weighted string given in the first paragraph of this readme:

```
5
ab
.2 .8
.5 .5
 0  1
.7 .3
.8 .2
```

Each line must have $\sigma$ space separated values where $\sigma$ is the size of the alphabet. If the probability is 0 you have to write it but the probability is not stored in the weighted string. 

### Collection of weighted string

There is another predefined type which is `wstr::ws_collection` which is a typedef for `std::vector<wstr::w_string>`. This allow you to easily keep multiple weighted strings in a collection. You can also create it from a file with the `>>` operator. You need to give the number of weighted string, then the alphabet (it has to be the same for each strings, but remember that you can freely put probability 0 so this is not a big issue). Then you should give `n` weighted strings which are represented by their size followed by their characters. Here is an example:

```
3
ACGT
4
.2 .3 .2 .3
 0  0 .5 .5
.2 .2 .6  0
 1  0  0  0
2
.2 .2 .2 .4
.5 .1 .1 .3
3
.4 .3 .2 .1
.2  0  0 .8
 0 .1 .9  0
```

### Option for weighted strings

Stream manipulators decribed in this section work for weighted strings and collection of weighted strings.

#### Gaps

You can allow a weighted string to have gaps. To do so, you can call the `wstr::w_string::set_gap()` method for each string, but you can do it in file by calling the `ws_gap` stream manipulator.

```cpp
using namespace wstr;

// ...

// First method
w_string ws;
ws.set_gap('-');

// Second method
std::ifstream input("my_file");
input >> ws_gap >> ws;
```

When using this, the last character of the alphabet will be considered as the gap character, so in the example, if you want `-` to be the gap character, the alphabet inside `my_file` must be something like `abcd-`.

To remove the gap for other ws, you have to call `input >> ws_no_gap`.

#### Strict mode

By default, each character will throw an exception if the sum of probabilities is not 1. You can remove this verification in constructor of weighted string by setting the second parameter to `false` (which is `true` by default). You can also use the stream manipulators `ws_strict` and `ws_not_strict`.

```cpp
using namespace wstr;

// ...

// First method for weighted chars
// Without the false, throw a std::invalid_argument error
w_char wc({{'a', .2}, {'b', .4}}, false);

// Second method for weighted strings
w_string ws;
std::ifstream input("my_file");
input >> ws_not_strict >> ws;
```

#### Precision

By default, the precision in strict mode is set to 0, it means that the sum of probabilities must be one (more or less the double maximum precision in your computer). If your datas has a specific precision (such as $10^{-5}$ for example), you can specify it at construction time. You can also change it afterwards with the `set_precision` method and you can specify it on stream via the `ws_set_precision` manipulator.

```cpp
using namespace wstr;

// ...

// First method for weighted chars
// Without the .1, throw a std::invalid_argument error
w_char wc1({{'a', .4}, {'b', .5}}, .1);

// Second method for weighted chars
// The sum is one so there is no throw
w_char wc2({{'a', .5}, {'b', .5}});
wc2['a'] = .4;
wc2.set_precision(.1);
// Without the set_precision, return false
// With the set_precision, return true
wc2.is_good();

// Third method for weighted strings
w_string ws;
std::ifstream input("my_file");
input >> ws_set_precision(0.00001) >> ws;
```

## Contribution

There is not a lot of features right now, but you can contribute to this project with your work. Do not hesitate to send me a message if you want to add some code in this repository, I'll be happy to help you. 

## Documentation

There is no documentation yet