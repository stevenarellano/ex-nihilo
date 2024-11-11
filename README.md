# ex-nihilo

build from scratch

# Running tests

Using the presents in the `CMakePresets.json` file:

```
cmake --preset build-<preset_name>
cmake --build --preset <preset_name>
```

Then navigate to the build directory and run the tests:

```
cd build/<preset-name>
make
ctest
```

When building `all`, the build folder will just be `build/`.
