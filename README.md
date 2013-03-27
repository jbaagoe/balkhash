balkhash
========

Tools to test non-cryptographic hash functions
----------------------------------------------

---

**NOTE (2013-03-27):  This project is a stub.**

I did not intend to publish it yet, but I came up with a hash function,
[`shake64`](hashes/shake64.c), that seemed sufficiently interesting
to rush matters a bit. According to SMHasher, its fast variant
(compiled with `-D NO_RISK_OF_HASHDOS`) passes all tests
with a reported speed of 10666.80 MiB/s @ 3 GHz for long input.

End of NOTE.

---

Austin Appleby has provided an excellent tool to test hash functions.
[SMHasher](https://code.google.com/p/smhasher/). However, it does
not provide an API or any other straightforward hooks for extensions.
If you want to test a function it doesn't know or cases it doesn't
provide, you have to dig into the code and compile your own version.

`balkhash` aims to a more modular approach in the spirit of Unix:
small programs that communicate via pipes, like this:

        <keyset> | <hash> | <test>
        <keyset> | <keyset modifier> ... | <hash> | <test>
        cat /usr/share/dict/words | <keyset modifier> ... | <hash> | <test>

Keysets
-------

Files in the `keyset` directory are intended to provide messages to
be hashed. A few are intended as absolute starting points, but most as
filters that modify their input, e.g., by outputting copies with up to
`n` bits flipped. Other absolute starting points may be `/dev/zero` or
`/dev/urandom` or user-provided files that are typical of the intended
use of the hash or directories of such files.

Hashes
------

This is intended to be a repository for about every known hash
function, in their original form together with wrappers to
provide a standard interface.

For the moment, though, it only contains my own attempts: `stir`,
`shake32` and `shake64`.

Tests
-----

Among the planned tests: collisions, avalanche, randomness,
speed and standard batteries of tests, in the spirit of L'Ecuyer's
[TestU01](http://www.iro.umontreal.ca/~simardr/testu01/tu01.html)
for PRNGs.
