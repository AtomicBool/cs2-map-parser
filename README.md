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

## File size

|  Map Name | .vphys | .tri |
| ---- | ---- | ---- |
| inferno | 674MB | 307MB |
| overpass | 113MB | 57.1MB |
| ancient | 370MB | 22.1MB |
| anubis | 256MB | 18.3MB |
| dust2 | 175MB | 13.3MB |
| vertigo | 129MB | 9.44MB |
| nuke | 54.4MB | 8.18MB |
| mirage | 24.8MB | 5.67MB |
| office | 21.4MB | 4.40MB |

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
