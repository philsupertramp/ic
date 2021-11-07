# ic [aɪ/si:] (Image Classification Tool)
Tool solving the task of manual image classification

## TODOs
- [ ] scale bounding box coordinates to image scale
- [ ] implement map for bounding box - image mapping
- [ ] allow dynamic selection of input directory
- [ ] save content to json format
- [ ] allow dragging of bounding box, nobody's perfect

## Done
- [x] image loader
- [x] directory view and image selector
- [x] drag and drop bounding boxes

## Requirements
`glfw`, `OpenGL 3`, `cmake`, `gcc`

## Build
tested under `Arch Linux (Kernel: 5.10.68)`
```shell
$ cmake -DCMAKE_BUILD_TYPE=Debug -DBE_PLATFORM_LINUX=1 -DCMAKE_DEPENDS_USE_COMPILER=FALSE -G "CodeBlocks - Unix Makefiles" .
$ cmake --build [build target dir] --target ic -- -j 3
```

## Run
```shell
$ ./[build target dir]/ic
```

# Acknowledgement
- Heavy usage of [imgui](https://github.com/ocornut/imgui), big thanks!

