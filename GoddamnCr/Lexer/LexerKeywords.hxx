// **************************************************************** //
//                                                                  //
//                Goddamn "C for Rendering" project                 //
//   Copyright (C) Goddamn Industries 2016. All Rights Reserved.    //
//        ( https://github.com/GoddamnIndustries/GoddamnCr )        //
//                                                                  //
//  This software or any its part is distributed under the terms of //
// Goddamn Industries End User License Agreement. By downloading or //
// using this software or any its part you agree with the terms of  //
// Goddamn Industries End User License Agreement.                   //
//                                                                  //
// **************************************************************** //

#ifndef KEYWORD
#define KEYWORD(kw, ...)
#endif	// ifndef KEYWORD

#ifndef PPKEYWORD
#define PPKEYWORD(kw, ...) 
#endif	// ifndef PPKEYWORD

#ifndef KEYWORDMxN
#define KEYWORDMxN(kw, ...) KEYWORD(kw, __VA_ARGS__) \
		KEYWORD(kw##1, __VA_ARGS__, 1) KEYWORD(kw##1x1, __VA_ARGS__, 1, x, 1) KEYWORD(kw##1x2, __VA_ARGS__, 1, x, 2) KEYWORD(kw##1x3, __VA_ARGS__, 1, x, 3) KEYWORD(kw##1x4, __VA_ARGS__, 1, x, 4) \
		KEYWORD(kw##2, __VA_ARGS__, 2) KEYWORD(kw##2x1, __VA_ARGS__, 2, x, 1) KEYWORD(kw##2x2, __VA_ARGS__, 2, x, 2) KEYWORD(kw##2x3, __VA_ARGS__, 2, x, 3) KEYWORD(kw##2x4, __VA_ARGS__, 2, x, 4) \
		KEYWORD(kw##3, __VA_ARGS__, 3) KEYWORD(kw##3x1, __VA_ARGS__, 3, x, 1) KEYWORD(kw##3x2, __VA_ARGS__, 3, x, 2) KEYWORD(kw##3x3, __VA_ARGS__, 3, x, 3) KEYWORD(kw##3x4, __VA_ARGS__, 3, x, 4) \
		KEYWORD(kw##4, __VA_ARGS__, 4) KEYWORD(kw##4x1, __VA_ARGS__, 4, x, 1) KEYWORD(kw##4x2, __VA_ARGS__, 4, x, 2) KEYWORD(kw##4x3, __VA_ARGS__, 4, x, 3) KEYWORD(kw##4x4, __VA_ARGS__, 4, x, 4)
#endif	// ifndef KEYWORDMxN

// **************************************************************** //
// This are keywords copied directly from MSDN.                     //
// **************************************************************** //
KEYWORD   (AppendStructuredBuffer, A, p, p, e, n, d, S, t, r, u, c, t, u, r, e, d, B, u, f, f, e, r)
KEYWORD   (BlendState, B, l, e, n, d, S, t, a, t, e)
KEYWORD   (Buffer, B, u, f, f, e, r)
KEYWORD   (ByteAddressBuffer, B, y, t, e, A, d, d, r, e, s, s, B, u, f, f, e, r)
KEYWORD   (CompileShader, C, o, m, p, i, l, e, S, h, a, d, e, r)
KEYWORD   (ComputeShader, C, o, m, p, u, t, e, S, h, a, d, e, r)
KEYWORD   (ConsumeStructuredBuffer, C, o, n, s, u, m, e, S, t, r, u, c, t, u, r, e, d, B, u, f, f, e, r)
KEYWORD   (DepthStencilState, D, e, p, t, h, S, t, e, n, c, i, l, S, t, a, t, e)
KEYWORD   (DepthStencilView, D, e, p, t, h, S, t, e, n, c, i, l, V, i, e, w)
KEYWORD   (DomainShader, D, o, m, a, i, n, S, h, a, d, e, r)
KEYWORD   (GeometryShader, G, e, o, m, e, t, r, y, S, h, a, d, e, r)
KEYWORD   (Hullshader, H, u, l, l, s, h, a, d, e, r)
KEYWORD   (InputPatch, I, n, p, u, t, P, a, t, c, h)
KEYWORD   (LineStream, L, i, n, e, S, t, r, e, a, m)
KEYWORD   (NULL, N, U, L, L)
KEYWORD   (OutputPatch, O, u, t, p, u, t, P, a, t, c, h)
KEYWORD   (PixelShader, P, i, x, e, l, S, h, a, d, e, r)
KEYWORD   (PointStream, P, o, i, n, t, S, t, r, e, a, m)
KEYWORD   (RWBuffer, R, W, B, u, f, f, e, r)
KEYWORD   (RWByteAddressBuffer, R, W, B, y, t, e, A, d, d, r, e, s, s, B, u, f, f, e, r)
KEYWORD   (RWStructuredBuffer, R, W, S, t, r, u, c, t, u, r, e, d, B, u, f, f, e, r)
KEYWORD   (RWTexture1D, R, W, T, e, x, t, u, r, e, 1, D)
KEYWORD   (RWTexture1DArray, R, W, T, e, x, t, u, r, e, 1, D, A, r, r, a, y)
KEYWORD   (RWTexture2D, R, W, T, e, x, t, u, r, e, 2, D)
KEYWORD   (RWTexture2DArray, R, W, T, e, x, t, u, r, e, 2, D, A, r, r, a, y)
KEYWORD   (RWTexture3D, R, W, T, e, x, t, u, r, e, 3, D)
KEYWORD   (RasterizerState, R, a, s, t, e, r, i, z, e, r, S, t, a, t, e)
KEYWORD   (RenderTargetView, R, e, n, d, e, r, T, a, r, g, e, t, V, i, e, w)
KEYWORD   (SamplerComparisonState, S, a, m, p, l, e, r, C, o, m, p, a, r, i, s, o, n, S, t, a, t, e)
KEYWORD   (SamplerState, S, a, m, p, l, e, r, S, t, a, t, e)
KEYWORD   (StructuredBuffer, S, t, r, u, c, t, u, r, e, d, B, u, f, f, e, r)
KEYWORD   (Texture1D, T, e, x, t, u, r, e, 1, D)
KEYWORD   (Texture1DArray, T, e, x, t, u, r, e, 1, D, A, r, r, a, y)
KEYWORD   (Texture2D, T, e, x, t, u, r, e, 2, D)
KEYWORD   (Texture2DArray, T, e, x, t, u, r, e, 2, D, A, r, r, a, y)
KEYWORD   (Texture2DMS, T, e, x, t, u, r, e, 2, D, M, S)
KEYWORD   (Texture2DMSArray, T, e, x, t, u, r, e, 2, D, M, S, A, r, r, a, y)
KEYWORD   (Texture3D, T, e, x, t, u, r, e, 3, D)
KEYWORD   (TextureCube, T, e, x, t, u, r, e, C, u, b, e)
KEYWORD   (TextureCubeArray, T, e, x, t, u, r, e, C, u, b, e, A, r, r, a, y)
KEYWORD   (TriangleStream, T, r, i, a, n, g, l, e, S, t, r, e, a, m)
KEYWORD   (VertexShader, V, e, r, t, e, x, S, h, a, d, e, r)
KEYWORD   (asm, a, s, m)
KEYWORD   (asm_fragment, a, s, m, _, f, r, a, g, m, e, n, t)
KEYWORD   (bool, b, o, o, l)
KEYWORD   (break, b, r, e, a, k)
KEYWORD   (case, c, a, s, e)
KEYWORD   (cbuffer, c, b, u, f, f, e, r)
KEYWORD   (centroid, c, e, n, t, r, o, i, d)
KEYWORD   (class, c, l, a, s, s)
KEYWORD   (column_major, c, o, l, u, m, n, _, m, a, j, o, r)
KEYWORD   (compile, c, o, m, p, i, l, e)
KEYWORD   (compile_fragment, c, o, m, p, i, l, e, _, f, r, a, g, m, e, n, t)
KEYWORD   (const, c, o, n, s, t)
KEYWORD   (continue, c, o, n, t, i, n, u, e)
KEYWORD   (default, d, e, f, a, u, l, t)
KEYWORD   (discard, d, i, s, c, a, r, d)
KEYWORD   (do, d, o)
KEYWORDMxN(double, d, o, u, b, l, e)
KEYWORDMxN(dword, d, w, o, r, d)
KEYWORD   (else, e, l, s, e)
KEYWORD   (export, e, x, p, o, r, t)
KEYWORD   (extern, e, x, t, e, r, n)
KEYWORD   (false, f, a, l, s, e)
KEYWORDMxN(float, f, l, o, a, t)
KEYWORD   (for, f, o, r)
KEYWORD   (fxgroup, f, x, g, r, o, u, p)
KEYWORD   (groupshared, g, r, o, u, p, s, h, a, r, e, d)
KEYWORDMxN(half, h, a, l, f)
KEYWORD   (if, i, f)
KEYWORD   (in, i, n)
KEYWORD   (inline, i, n, l, i, n, e)
KEYWORD   (inout, i, n, o, u, t)
KEYWORDMxN(int, i, n, t)
KEYWORD   (interface, i, n, t, e, r, f, a, c, e)
KEYWORD   (line, l, i, n, e)
KEYWORD   (lineadj, l, i, n, e, a, d, j)
KEYWORD   (linear, l, i, n, e, a, r)
KEYWORD   (matrix, m, a, t, r, i, x)
KEYWORDMxN(min10float, m, i, n, 1, 0, f, l, o, a, t)
KEYWORDMxN(min12int, m, i, n, 1, 2, i, n, t)
KEYWORDMxN(min16float, m, i, n, 1, 6, f, l, o, a, t)
KEYWORDMxN(min16int, m, i, n, 1, 6, i, n, t)
KEYWORDMxN(min16uint, m, i, n, 1, 6, u, i, n, t)
KEYWORD   (namespace, n, a, m, e, s, p, a, c, e)
KEYWORD   (nointerpolation, n, o, i, n, t, e, r, p, o, l, a, t, i, o, n)
KEYWORD   (noperspective, n, o, p, e, r, s, p, e, c, t, i, v, e)
KEYWORD   (out, o, u, t)
KEYWORD   (packoffset, p, a, c, k, o, f, f, s, e, t)
KEYWORD   (pass, p, a, s, s)
KEYWORD   (pixelfragment, p, i, x, e, l, f, r, a, g, m, e, n, t)
KEYWORD   (point, p, o, i, n, t)
KEYWORD   (precise, p, r, e, c, i, s, e)
KEYWORD   (register, r, e, g, i, s, t, e, r)
KEYWORD   (return, r, e, t, u, r, n)
KEYWORD   (row_major, r, o, w, _, m, a, j, o, r)
KEYWORD   (sample, s, a, m, p, l, e)
KEYWORD   (sampler, s, a, m, p, l, e, r)
KEYWORD   (shared, s, h, a, r, e, d)
KEYWORD   (snorm, s, n, o, r, m)
KEYWORD   (stateblock, s, t, a, t, e, b, l, o, c, k)
KEYWORD   (stateblock_state, s, t, a, t, e, b, l, o, c, k, _, s, t, a, t, e)
KEYWORD   (static, s, t, a, t, i, c)
KEYWORD   (string, s, t, r, i, n, g)
KEYWORD   (struct, s, t, r, u, c, t)
KEYWORD   (switch, s, w, i, t, c, h)
KEYWORD   (tbuffer, t, b, u, f, f, e, r)
KEYWORD   (technique, t, e, c, h, n, i, q, u, e)
KEYWORD   (technique10, t, e, c, h, n, i, q, u, e, 1, 0)
KEYWORD   (technique11, t, e, c, h, n, i, q, u, e, 1, 1)
KEYWORD   (texture, t, e, x, t, u, r, e)
KEYWORD   (triangle, t, r, i, a, n, g, l, e)
KEYWORD   (triangleadj, t, r, i, a, n, g, l, e, a, d, j)
KEYWORD   (true, t, r, u, e)
KEYWORD   (typedef, t, y, p, e, d, e, f)
KEYWORDMxN(uint, u, i, n, t)
KEYWORD   (uniform, u, n, i, f, o, r, m)
KEYWORD   (unorm, u, n, o, r, m)
KEYWORD   (unsigned, u, n, s, i, g, n, e, d)
KEYWORD   (vector, v, e, c, t, o, r)
KEYWORD   (vertexfragment, v, e, r, t, e, x, f, r, a, g, m, e, n, t)
KEYWORD   (void, v, o, i, d)
KEYWORD   (volatile, v, o, l, a, t, i, l, e)
KEYWORD   (while, w, h, i, l, e)

#undef KEYWORD

// **************************************************************** //
// This are keywords, used by preprcessor.                          //
// **************************************************************** //

//PPKEYWORD(__DATE__, _, _, D, A, T, E, _, _)
//PPKEYWORD(__FILE__, _, _, F, I, L, E, _, _)
//PPKEYWORD(__FUNCTION__, _, _, F, U, N, C, T, I, O, N, _, _)
//PPKEYWORD(__LINE__, _, _, L, I, N, E, _, _)
//PPKEYWORD(__TIME__, _, _, T, I, M, E, _, _)
//PPKEYWORD(__VA_ARGS__, _, _, V, A, _, A, R, G, S, _, _)
PPKEYWORD(define, d, e, f, i, n, e)
PPKEYWORD(defined, d, e, f, i, n, e, d)
PPKEYWORD(disable, d, i, s, a, b, l, e)
PPKEYWORD(elif, e, l, i, f)
PPKEYWORD(endif, e, n, d, i, f)
PPKEYWORD(error, e, r, r, o, r)
PPKEYWORD(ifdef, i, f, d, e, f)
PPKEYWORD(ifndef, i, f, n, d, e, f)
PPKEYWORD(include, i, n, c, l, u, d, e)
PPKEYWORD(line, l, i, n, e)
PPKEYWORD(message, m, e, s, s, a, g, e)
PPKEYWORD(once, o, n, c, e)
PPKEYWORD(pack_matrix, p, a, c, k, _, m, a, t, r, i, x)
PPKEYWORD(pragma, p, r, a, g, m, a)
PPKEYWORD(undef, u, n, d, e, f)
PPKEYWORD(warning, w, a, r, n, i, n, g)

#undef PPKEYWORD
