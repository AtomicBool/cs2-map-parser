# Map Parser
Convert vphys file to a `.tri` file (a list of vec3 point, each 3 points repersents a triangle) \
Reason why I do that is vk3 parser takes lots memory to save the `.vphys` text (especially for inferno [5.9Gb] ) \
and .vphys contains lots useless info. 

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

| |  Map Name | .vphys | .tri | |
| :----: | :----: | :----: | :----: | :----: |
| 1 | inferno | 674MB | 307MB | -54.46% |
| 2 | overpass | 113MB | 57.1MB | -49.47% | 
| 3 | ancient | 370MB | 22.1MB | -94.03% | 
| 4 | anubis | 256MB | 18.3MB |  -92.85% | 
| 5 | dust2 | 175MB | 13.3MB | -92.4% | 
| 6 | vertigo | 129MB | 9.44MB | -92.68% |
| 7 | nuke | 54.4MB | 8.18MB | -84.96% |
| 8 | mirage | 24.8MB | 5.67MB | -77.13% |
| 9 | office | 21.4MB | 4.40MB | -79.43% |

## Use

```
  1. complie
  2. put the vphys file you extract from (.vpk) map file.
  3. modify `vphys_parser.cpp` to set the file name of input and output
```

## Coding Visibility Check
A simple example is in `vischeck_example\`
GPT helped with KD-tree and slabs/moller-trumbore algo

## TODO
Nothing.

## Credits

- KV3 Parser https://github.com/joepriit/cpp-kv3-parser
