# Design notes

## `serialize` vs `operator &`

Some serialization libraries (e.g. Boost.Serialization) use the `operator &` for serializing a field.  
It's good for chaining fields, like `ar & x & y`, but it doesn't work for name-value pairs, not for JSON in particular.

The `operator &` also has a downside - it must have two (formal) parameters. With a regular function like `serialize` you can add default parameters for SFINAE.

    void serialize(Parser& p, T& t, enable_if_t<...>*=0)

## `Stream&` vs `jsoncpp::Stream&` vs `jsoncpp::Stream<X>&`

    void serialize(jsoncpp::Stream&, Foo&)

This is the best API from the POV of writing user code.  

It doesn't require to type many code (no `template<>` part), it doesn't collide with `serialize()` functions from other libraries.

But it requires type-erasure, which use some sort of dynamic dispatching, which prevents inlining of implementation's functions, so I consider it **slow**.

    template<class Stream> void serialize(Stream&, Foo&);

This API is "faster", but it can collide with other `serialize()` functions.

So, **json-cpp** uses a hybrid version:

    template<class X> void serialize(jsoncpp::Stream<X>&, Foo&)

It may be implemented as `Stream<StreamImpl<...>>`, but at the moment it's  `Stream<Traits<...>>`.

Note, that `X` can't be a parameter pack, because `Foo` may also require a parameter pack:

    template<class X, typename... Ts>
    void serialize(jsoncpp::Stream<X>&, tuple<Ts...>&)

## Separate overloads for Parser and Generator

Some classes require separate code for loading and saving from/to JSON.
The straightforward way is to use SFINAE, i.e.

    template<class X>
    void serialize(jsoncpp::Stream<X>& parser, Foo&,
        enable_if_t<X::is_parser>*=0)

but it requires too much of code, and also an additional line, as you can see above.

Thus **json-cpp** provides two template aliases:

    template<class X> using Parser = ...;
    template<class X> using Generator = ...;

which can be used instead of `Stream<X>&`.
