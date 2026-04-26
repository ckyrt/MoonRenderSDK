# User Material Textures

MoonRenderSDK does not require material textures to be stored in this repository.

Applications provide their own local texture root through `RuntimeDesc::textureRoot` or explicit file paths in `MaterialDesc::textures`.

Recommended runtime layout:

```text
{textureRoot}/materials/
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

Default texture suffixes:

- `_Color.png`
- `_NormalDX.png`
- `_AmbientOcclusion.png`
- `_Roughness.png`
- `_Metalness.png`
- `_Opacity.png`
