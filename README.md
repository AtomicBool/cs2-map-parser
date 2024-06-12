# Map Parser
Convert vphys file to a `.tri` file (a list of vec3 point, each 3 points repersents a triangle) \
Reason why I do that is vk3 parser takes lots memory to save the `.vphys` text (especially for dust2)

## Format of `Triangle` in `.tri`

```c++
typedef struct Vector3 {
    float x, y, z;
};

typedef struct Triangle {
    Vector3 p1, p2, p3;
};
```

## Use

```
  1. complie
  2. put the vphys file you extract from (.vpk) map file.
  3. modify `vphys_parser.cpp` to set the file name of input and output
```

## TODO
Nothing.

## Credits

- KV3 Parser https://github.com/joepriit/cpp-kv3-parser
