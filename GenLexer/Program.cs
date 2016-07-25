#region

using System;
using System.Collections.Generic;
using System.Linq;

#endregion

namespace GenLexer
{
    internal class Program
    {
        private static readonly string[] s_KeywordsFromMsdn =
                //            @"
                //AppendStructuredBuffer, asm, asm_fragment
                //BlendState, bool, break, Buffer, ByteAddressBuffer
                //case, cbuffer, centroid, class, column_major, compile, compile_fragment, CompileShader, const, continue, ComputeShader, ConsumeStructuredBuffer
                //default, DepthStencilState, DepthStencilView, discard, do, double, DomainShader, dword
                //else, export, extern
                //false, float, for, fxgroup
                //GeometryShader, groupshared
                //half, Hullshader
                //if, in, inline, inout, InputPatch, int, interface
                //line, lineadj, linear, LineStream
                //matrix, min16float, min10float, min16int, min12int, min16uint
                //namespace, nointerpolation, noperspective, NULL
                //out, OutputPatch
                //packoffset, pass, pixelfragment, PixelShader, point, PointStream, precise
                //RasterizerState, RenderTargetView, return, register, row_major, RWBuffer, RWByteAddressBuffer, RWStructuredBuffer, RWTexture1D, RWTexture1DArray, RWTexture2D, RWTexture2DArray, RWTexture3D
                //sample, sampler, SamplerState, SamplerComparisonState, shared, snorm, stateblock, stateblock_state, static, string, struct, switch, StructuredBuffer
                //tbuffer, technique, technique10, technique11, texture, Texture1D, Texture1DArray, Texture2D, Texture2DArray, Texture2DMS, Texture2DMSArray, Texture3D, TextureCube, TextureCubeArray, true, typedef, triangle, triangleadj, TriangleStream
                //uint, uniform, unorm, unsigned
                //vector, vertexfragment, VertexShader, void, volatile
                //while
                //"
                @"
define
elif
else
endif
error
if
ifdef
ifndef
include
line
pragma
undef
defined
message
pack_matrix
warning
once
default
disable
__LINE__
__FILE__
__DATE__
__TIME__
__FUNCTION__
__VA_ARGS__
__FUNC__
                "
                .Split(',', '\n')
                .Select(s => s.Trim())
                .Where(s => !string.IsNullOrEmpty(s))
                .OrderBy(s => s, StringComparer.Ordinal)
                .ToArray();

        private static void Main(string[] args)
        {
            foreach (var keyword in s_KeywordsFromMsdn)
            {
                Console.Write($"PPKEYWORD ({keyword}, {keyword[0]}");
                foreach (var c in keyword.Substring(1))
                {
                    Console.Write($", {c}");
                }
                Console.WriteLine(")");
            }

            var rootKeywordsTree = new KeywordNode();
            foreach (var keyword in s_KeywordsFromMsdn)
            {
                rootKeywordsTree.Insert(keyword);
            }

            rootKeywordsTree.Render();
        }

        /// <summary>
        /// 
        /// </summary>
        private sealed class KeywordNode
        {
            private readonly Dictionary<char, KeywordNode> m_Children = new Dictionary<char, KeywordNode>();
            private bool m_IsLeaf;

            public void Insert(string keyword)
            {
                if (string.IsNullOrEmpty(keyword))
                {
                    m_IsLeaf = true;
                    return;
                }

                var letter = keyword[0];
                if (!m_Children.ContainsKey(letter))
                {
                    m_Children.Add(letter, new KeywordNode());
                }
                m_Children[letter].Insert(keyword.Substring(1));
            }

            public void Render()
            {
                Console.WriteLine(@"// $$***************************************************************$$ //
//                                                                     //
//                  Goddamn ""C for Rendering"" project                  //
//     Copyright (C) Goddamn Industries 2016. All Rights Reserved.     //
//          ( https://github.com/GoddamnIndustries/GoddamnCr )         //
//                                                                     //
//    This software or any its part is distributed under the terms of  //
//   Goddamn Industries End User License Agreement. By downloading or  //
//   using this software or any its part you agree with the terms of   //
//   Goddamn Industries End User License Agreement.                    //
//                                                                     //
// $$***************************************************************$$ //");
                Console.WriteLine();
                Console.WriteLine(@"// $$***************************************************************$$ //
//                                                                     //
//   THIS FILE IS AUTOMATICALLY GENERATED. ANY CHANGES WOULD BE DISCARDED //
//                                                                     //
// $$***************************************************************$$ //");

                foreach (var child in m_Children)
                {
                    Console.WriteLine($"case '{child.Key}': ");
                    Console.WriteLine("\tAdvanceChars();");

                    var tabsCount = 1;
                    var keyword = new string(child.Key, 1);
                    child.Value.RenderInternal(ref tabsCount, ref keyword, true);

                    Console.WriteLine("\tbreak;");
                    Console.WriteLine("\t");
                }
            }

            private void RenderInternal(ref int tabsCount, ref string keyword, bool wasBreak = false)
            {
                var tabulation = new string('\t', tabsCount);
                for (var i = 0; i < m_Children.Count; ++i)
                {
                    var childKey = m_Children.Keys.ElementAt(i);
                    var childNode = m_Children[childKey];

                    var shouldStartNewChain = m_Children.Count != 1 || m_IsLeaf || wasBreak;
                    var shouldStartNewScope = childNode.m_Children.Count != 1 || childNode.m_IsLeaf;
                    var shouldTabulate = childNode.m_Children.Count > 1 || childNode.m_IsLeaf;

                    if (shouldStartNewChain)
                    {
                        Console.WriteLine(tabulation
                                          + (i != 0 ? "else " : "")
                                          + $"if (PeekChar() == '{childKey}'"
                                          + (shouldStartNewScope ? ")" : ""));
                    }
                    else
                    {
                        Console.WriteLine(tabulation
                                          + $"\t&& PeekNextChar() == '{childKey}'"
                                          + (shouldStartNewScope ? ")" : ""));
                    }

                    if (shouldStartNewScope)
                    {
                        Console.WriteLine(tabulation + "{");
                        Console.WriteLine(tabulation + "\tAdvanceChars();");
                    }
                    if (shouldTabulate)
                    {
                        ++tabsCount;
                    }
                    keyword += childKey;

                    childNode.RenderInternal(ref tabsCount, ref keyword);
                    if (childNode.m_IsLeaf)
                    {
                        var additionalTabulation = "";

                        var shouldStartElse = childNode.m_Children.Count > 0;
                        if (shouldStartElse)
                        {
                            Console.WriteLine(tabulation + "\telse");
                            Console.WriteLine(tabulation + "\t{");
                            additionalTabulation = "\t";
                        }

                        Console.WriteLine(tabulation + additionalTabulation +
                                          $"\ttokenInfo.Type = ASyntaxLexeme::Type::Kw_{keyword};");

                        if (shouldStartElse)
                        {
                            Console.WriteLine(tabulation + "\t}");
                        }
                    }

                    keyword = keyword.Substring(0, keyword.Length - 1);
                    if (shouldTabulate)
                    {
                        --tabsCount;
                    }
                    if (shouldStartNewScope)
                    {
                        Console.WriteLine(tabulation + "}");
                    }
                }
            }
        } // private class KeywordNode
    }
}
