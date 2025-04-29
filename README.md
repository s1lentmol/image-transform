# Image Transform

A simple command-line utility written in C for performing geometric transformations on BMP images.

## Usage

```sh
./image-transform <source-image> <transformed-image> <transformation>
```

- `<source-image>` — Path to the input BMP file (must be 24-bit Windows BMP).
- `<transformed-image>` — Path where the output BMP will be saved.
- `<transformation>` — One of the supported transformation commands (see [Transformations](#transformations)).

## Transformations

| Command | Description                              |
| ------- | ---------------------------------------- |
| `none`  | No transformation (copy input to output) |
| `cw90`  | Rotate the image 90° clockwise           |
| `ccw90` | Rotate the image 90° counter-clockwise   |
| `fliph` | Flip the image horizontally              |
| `flipv` | Flip the image vertically                |

## Examples

Rotate an image 90° clockwise:

```sh
./image-transform input.bmp output.bmp cw90
```

Flip an image vertically:

```sh
./image-transform input.bmp flipped.bmp flipv
```

No transformation, just copy:

```sh
./image-transform input.bmp copy.bmp none
```
