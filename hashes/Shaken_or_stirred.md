Hash functions - shaken or stirred?
===================================

Like dry martinis, hash functions come in two flavours: stirred or shaken.

Most historical hashes - Knuth, Bernstein, FNV, etc. - are stirring
hashes. They add the data to be hashed one byte at a time to the previous
hash value, mixing gently as they proceed. When the end of the data is
reached, they simply serve.

The present trend is shaking. Shaking hashes incorporate the data
in larger amounts, at least one word (32 or 64 bits) at a time, but
often several. When the end of the data is reached, the resulting
internal state is vigorously shaken before being served.

James Bond famously preferred shaken. [Another
JB](http://en.wikipedia.org/wiki/Josiah_Bartlet), however,
strongly disagreed: *"Shaken, not stirred, will get you cold water
with a dash of gin and dry vermouth. [...] James is ordering a
weak martini and being snooty about it."* ([The West Wing, episode
"Stirred"](http://www.imdb.com/title/tt0745680/trivia?tab=qt).)

[This JB](https://github.com/jbaagoe) finds merits in both, at least for
hash functions. Shaking hashes are much faster for long messages and much
more collision-resistant. But if the data to be hashed is of the form,
say, `foo.barXXX` where `XXX` takes values like `0, 1, 2, ...`, plain
linear multiplicative hashes may actually provide **less** collisions than
a theoretically perfect hash. And for short messages, they may be faster,
too, especially on 32-bit architectures like X86 or ARM where registers
are scarce - stirring hashes are trivial to inline and use few registers.
They are therefore still preferred for at least one very important type
of applications: property or method lookup in scripting languages like
[Python](http://www.laurentluce.com/posts/python-dictionary-implementation/)
(look for the long comment starting with "Major subtleties ahead:").

I have written three hash functions to illustrate the merits of
each approach.

[`stir`](stir.c) is a simple stirring function.

[`shake64`](shake64.c) is a high performance shaking function for
X86_64 inspired by Bob Jenkins'
[SpookyHash](http://burtleburtle.net/bob/hash/spooky.html). It
always returns a 64 bit hash, and optionally a second.

[`shake32`](shake32.c) is a shaking function for X86 along the lines
of Yann Collet's [xxHash](https://code.google.com/p/xxhash/),
which in turn is based on Austin Appleby's
[MurmurHash](https://code.google.com/p/smhasher/wiki/MurmurHash3).
In terms of speed, `shake32` lies between the two, but it attempts to
be more resistant than either to HashDos attacks. (NB: this is an attempt,
cryptologists may prove me to be embarrassingly wrong.)

Note that `shake64` and `shake32` are quite different kinds of
animals.

`shake64` has 12 words of state (not counting the seed) and each
word of input affects them all. It is therefore cheap to return a
second hash value, for a total of 128 bits. (One could even return
more bits, up to 768, but there seems no point in doing so.)

`shake32` has only 4 words of state (not counting the seed) because
registers are scarce. More importantly, each word of input only
affects one word of state - the 4 words of state evolve in parallel
and never interact before they are merged at the end of the message.
I don't find this as satisfactory as the "scramble the state as you go"
approach, but in all my attempts so far, it turns out to be faster.
