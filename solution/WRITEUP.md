# Writeup

The program allows users to create accounts and make transactions between them.

The C code is compiled with Clang [Address Sanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) and [Undefined Behavior Sanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html), thus adding tons of runtime checks to catch most undefined behaviors (i.e, bugs). The flags I used to compile the source code are available [here](../Dockerfile#L19).

The program contains lots of bugs, and most of them are caught by the sanitizers, thus making them impossible to exploit! A challenger would probably spend most of its time trying to exploit different bugs, and watch ASAN catch them, with either admiration or frustration ;)

Unfortunately, ASAN isn't perfect, and one bug can be used to exploit the program and get a shell!

Let's see a non-exhaustive list of bugs:
* [cryptobank.c#L63](../src/cryptobank.c#L63): Memory leak, comments are not free() (caught by ASAN)
* [cryptobank.c#L74](../src/cryptobank.c#L74): Integer overflow, but very hard to trigger (caught by ASAN)
* [cryptobank.c#L76](../src/cryptobank.c#L76): Integer overflow, but very hard to trigger (caught by ASAN)
* [cryptobank.c#L79](../src/cryptobank.c#L79): Integer overflow, but very hard to trigger (caught by ASAN)
* [cryptobank.c#L120](../src/cryptobank.c#L120): Integer overflow, but very hard to trigger (caught by ASAN)
* [cryptobank.c#L122](../src/cryptobank.c#L122): Integer overflow, but very hard to trigger (caught by ASAN)
* [cryptobank.c#L125](../src/cryptobank.c#L125): Integer overflow, but very hard to trigger (caught by ASAN)
* [cryptobank.c#L175](../src/cryptobank.c#L175): Stack buffer overflow due to scanf("%s") (caught by ASAN)
* [cryptobank.c#L177](../src/cryptobank.c#L177): `n` might not be a valid UserAction (impossible to exploit)
* [cryptobank.c#L211](../src/cryptobank.c#L211): Stack buffer overflow due to scanf("%s") (caught by ASAN)
* [cryptobank.c#L222](../src/cryptobank.c#L222): If `strlen(comment) = 0`, write to `comment[-1]` (caught by ASAN)
* [cryptobank.c#L226](../src/cryptobank.c#L226): Null pointer dereference (caught by ASAN)
* [cryptobank.c#L226](../src/cryptobank.c#L226): Integer overflow (caught by ASAN)
* [cryptobank.c#L227](../src/cryptobank.c#L227): Integer overflow (caught by ASAN)
* [cryptobank.c#L253](../src/cryptobank.c#L253): Stack buffer overflow due to scanf("%s") (caught by ASAN)
* [cryptobank.c#L256](../src/cryptobank.c#L256): Null pointer dereference (caught by ASAN)
* [cryptobank.c#L284](../src/cryptobank.c#L284): If `strlen(comment) = 0`, write to `comment[-1]` (caught by ASAN)
* [cryptobank.c#L286](../src/cryptobank.c#L286): Integer overflow (caught by ASAN)
* [cryptobank.c#L287](../src/cryptobank.c#L287): Integer overflow (caught by ASAN)
* [cryptobank.c#L320](../src/cryptobank.c#L320): Heap buffer overflow (BINGO!)
* [cryptobank.c#L327](../src/cryptobank.c#L327): If `strlen(comment) = 0`, write to `comment[-1]` (caught by ASAN)
* [cryptobank.c#L388](../src/cryptobank.c#L388): Stack buffer overflow due to scanf("%s") (caught by ASAN)
* [cryptobank.c#L390](../src/cryptobank.c#L390): `n` might not be a valid GuestAction (impossible to exploit)
* [cryptobank.c#L406](../src/cryptobank.c#L406): Stack buffer overflow due to scanf("%s") (caught by ASAN)
* [cryptobank.c#L411](../src/cryptobank.c#L411): Stack buffer overflow due to scanf("%s") (caught by ASAN)
* [cryptobank.c#L437](../src/cryptobank.c#L437): Heap buffer overflow due to scanf("%s") (caught by ASAN)
* [cryptobank.c#L443](../src/cryptobank.c#L443): Heap buffer overflow due to scanf("%s") (caught by ASAN)

There is also a use-after-free: if you trigger a `realloc()` in `add_account()`, the `Account* src` and `Account* dest` pointers in transaction structures are not updated, and they become dangling pointers. Unfortunately, ASAN is able to catch it!

The only bug that ASAN does not catch correctly is [line 320](../src/cryptobank.c#L320), the variable `index` is controlled by the user, and can produce a heap buffer overflow. This is because ASAN uses "red zones" to catch buffer overflows. ASAN adds a space before and after each memory block, and triggers whenever we read or write on these bytes. By choosing a specific `index`, we can manager to read or write in another block, thus not triggering ASAN. And this is how we bypass ASAN!

The source code is equivalent to:
```
fgets(account->transactions.ptr[index].comment, 30, stdin);
```

With basic math, you can choose `index` so that `account->transactions.ptr[index].comment` points to a valid block.

In [my solution](exploit.py), I chose `index = 10737418244` so `comment = accounts.ptr[2].transactions.ptr`. I can overwrite the `Transaction` structure of the third account.

The `Transaction` structure is:
```
typedef struct {
    Account* src;
    Account* dest;
    char* comment;
    unsigned amount;
} Transaction;
```

We can easily get a **read-write-everywhere** from here. I use this to overwrite the `comment` pointer. To read the `comment`, I ask the program to show me my transactions (with option 1). To write, I ask the program to update the comment of an existing transaction (option 5).

Now, this is classic exploit. Note that ASAN makes addresses deterministic (ie, no PIE). I just leak the GOT, I replace `fgets` by `system`, and then I get a shell!
