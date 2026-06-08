# SXAT

Simple X (image) Annotation Tool

SXAT will read whatever image (jpeg, png or TIFF) from stdin and render it in its window. basic tools to modify the image
are provided, e.g. pencils/erarser, text, ... On exit SXAT will dump the edited version of the image to
stdout as a PNG file.

### basic usage

```
sxat /path/to/some/image.png > annotated-image.png
```

### Additions
1. config.h
2. rectangle

### keybinds

note that "exit" only works in "normal mode".

| tool/mode   | key  |
| ----        | ---  |
| normal      | ESC  |
| eraser      | e    |
| marker      | m    |
| pencil      | p    |
| rectangle   | r    |
| exit        | q    |

#### pencil tool

| action            | key |
| --                | --  |
| decrease pen size | [   |
| increase pen size | ]   |
| change color      | c   |

#### eraser tool

| action               | key |
| --                   | --  |
| decrease eraser size | [   |
| increase eraser size | ]   |

#### marker tool

| action                | key |
| --                    | --  |
| decrease marker size  | [   |
| increase marker size  | ]   |
| decrease number       | ;   |
| increase number       | '   |
| reset number to 1     | .   |
| change marker alpha   | a   |
| change marker color   | c   |
| toggle auto-increment | i   |

#### rectangle tool

| action                        | key |
| --                            | --  |
| decrease border thickness     | [   |
| increase border thickness     | ]   |
| fill                          | f   |
| change marker alpha           | a   |
| change marker color           | c   |
