# Simulations

## Spring

Spring generation:
- issue draw call with empty vertex buffer, LINE_STRIP and vertex count equal to helix length density
- vertex shader generates helix isoline points based on parameters
- tessellation shader takes two-point isolines, generates points around helix isoline beg and end, and emits triangles