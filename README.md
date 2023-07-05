# Weighted String

## Definition

A **weighted string** is a probabilistic string in which each position can contain a character with a certain probability.

This is often used in DNA structure, when we don't really know for sure each character of a DNA sequence.

Here is a simple exemple of a weighted string $w$ on the alphabet A, C, G, T:

|  #  |  1  |  2  |  3  |  4  |  5  |
|:---:|:---:|:---:|:---:|:---:|:---:|
|**A**| .2  | .5  |  0  | .3  | .2  |
|**C**| .3  |  0  |  0  | .3  | .8  |
|**G**| .3  | .5  |  0  | .4  |  0  |
|**T**| .2  |  0  |  1  |  0  |  0  |