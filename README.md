# MarkovGen

MarkovGen is a simple implementation of a [Markov model](https://en.wikipedia.org/wiki/Markov_model) for use in password cracking. There are many implementations that use Markovian properties to guess passwords. Both [John the Ripper](https://github.com/pmittaldev/john-the-ripper/blob/master/doc/MARKOV) and [Hashcat](https://hashcat.net/forum/thread-6245.html) are able to use such models to generate passwords, but this is my implementation.

# Functionality

MarkovGen takes an input wordlist to train a set of weights to be used in generation. It can then generate all passwords that satisfy a given probability.

# Building

```bash
mkdir build;
cd build;
cmake -DCMAKE_BUILD_TYPE=Release ..
make;
```

# Running

Running MarkovGen takes place in three steps.

## Step 1: Build Model

Build the model by piping an input wordlist into `markovgen` in `build` mode.

```bash
cat /usr/share/dict/words | markovgen build model.markov;
```

## Step 2: Analyse Model

Analyse the model by running `markovgen` in "report" mode.

```bash
markovgen report model.markov | less;
```

The output of the report shows the statistical probabilities of the first character, and the probablilities of any digraph.

```text
***Start Character***
A 0.01448
B 0.01467
C 0.01606
D 0.00850
E 0.00662
F 0.00558
G 0.00846
...
Q 0.00070
...
```
The above sample output shows that the probability of a password starting with the letter `A` is 0.01448, significantly statistically more likely than the letter `Q` with a probability of 0.00070.

Then follows the probabilities `P` for all characters `Y`, given the previous character `X` in the format `XY P`.

```text
***Non-start Characters***
'A 0.00013
'B 0.00007
'C 0.00020
'D 0.00007
'E 0.00007
...
a' 0.02570
aD 0.00003
aS 0.00009
aa 0.00101
ab 0.03474
ac 0.05680
ad 0.04017
```

If any digraph `XY` does not appear in the report then it was not seen in the training data and as such is omitted. For example, there was no input in which the character pair `aA` was seen in the training set.

## Step 3: Generate Passwords

Once the model has been trained it can be used to generate passwords of length `L` that meet the statistical probability `P`. Run `markovgen` with the argument `generate` and the path to the model file generated in Step 1. The minimum and maximum length of password must be specified.

```bash
./markovgen generate MODEL MIN MAX P;
```

For example, to generate passwords of length 3 to 4 characters with the statistical probability of at least 0.01 you would run the following.

```bash
./markovgen generate model.markov 3 4 0.01
```

Sample output:

```text
A's
AS'
ASA
ASa
ASc
ASe
ASh
ASi
ASl
ASm
ASn
ASo
ASp
ASt
ASu
```
