# Forge

Asset packer for supergoon engine

## Description

Writes all assets into a binary file so that you don't need to manage
multiple resources at once, and keep your assets versioned.  Includes a binary
that packs and a unpacker. See the test inside of the sound project to see it
in use.

## Dependencies

- sgtools, for logging.. you can easily just replace those calls with fprintf, etc.

## Usage

### Pack

```bash
sgforge ../EscapeTheFate/assets/audio/bgm/town2.ogg
```

### Unpack

```c
Directory* directory = DeserializeDirectoryFromFile("/tmp/test.sg");
char* soundData;
size_t soundDataSize;
int result = GetDataFromDirectory("../EscapeTheFate/assets/audio/bgm/town2.ogg",
&soundData, &soundDataSize, directory);
```
