# ex-nihilo

build from scratch

# Building & Running

Using the presents in the `CMakePresets.json` file:

```
cmake --preset build-all
cmake --build --preset all
```

# commands

cmake --preset build-all

cd build/all
make

ctest

ctest --output-on-failure

ctest -N

ctest -R <test_name>
