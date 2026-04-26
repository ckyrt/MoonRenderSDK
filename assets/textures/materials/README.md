# Material Texture Pack

Reusable material textures belong to the SDK, but the current Moon material texture set is large.

Current Moon source size:

- PNG textures: about 922 MB
- metadata files: small

Do not commit the full PNG set as ordinary Git blobs. Package it as one of:

- GitHub Release asset: `MoonRenderSDK-MaterialTextures.zip`
- Git LFS tracked files
- internal binary artifact package

Expected runtime layout after unpacking:

```text
assets/textures/materials/
  Concrete044D_2K-PNG/
  Brick_2K-PNG/
  Wood049_2K-PNG/
  WoodFloor_2K-PNG/
  Metal061B_2K-PNG/
  Glass/
  Fabric061_2K-PNG/
  Leather_2K-PNG/
  Carpet_2K-PNG/
  Plastic018A_2K-PNG/
  Rubber_2K-PNG/
```
