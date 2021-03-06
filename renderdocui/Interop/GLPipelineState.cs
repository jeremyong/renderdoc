﻿/******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Crytek
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace renderdoc
{
    [StructLayout(LayoutKind.Sequential)]
    public class GLPipelineState
    {
        [StructLayout(LayoutKind.Sequential)]
        public class VertexInputs
        {
            [StructLayout(LayoutKind.Sequential)]
            public class VertexAttribute
            {
                public bool Enabled;
                [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
                public ResourceFormat Format;
                public FloatVector GenericValue;
                public UInt32 BufferSlot;
                public UInt32 RelativeOffset;
            };
            [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
            public VertexAttribute[] attributes;

            [StructLayout(LayoutKind.Sequential)]
            public class VertexBuffer
            {
                public ResourceId Buffer;
                public UInt32 Stride;
                public UInt32 Offset;
                public bool PerInstance;
                public UInt32 Divisor;
            };
            [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
            public VertexBuffer[] vbuffers;

            public ResourceId ibuffer;
            public bool primitiveRestart;
            public UInt32 restartIndex;
        };
        [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
        public VertexInputs m_VtxIn;

        [StructLayout(LayoutKind.Sequential)]
        public class ShaderStage
        {
            private void PostMarshal()
            {
                if (_ptr_ShaderDetails != IntPtr.Zero)
                    ShaderDetails = (ShaderReflection)CustomMarshal.PtrToStructure(_ptr_ShaderDetails, typeof(ShaderReflection), false);
                else
                    ShaderDetails = null;

                _ptr_ShaderDetails = IntPtr.Zero;
            }

            public ResourceId Shader;
            private IntPtr _ptr_ShaderDetails;
            [CustomMarshalAs(CustomUnmanagedType.Skip)]
            public ShaderReflection ShaderDetails;
            [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
            public ShaderBindpointMapping BindpointMapping;

            public ShaderStageType stage;
        };
        [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
        public ShaderStage m_VS, m_TCS, m_TES, m_GS, m_FS, m_CS;

        [StructLayout(LayoutKind.Sequential)]
        public class Texture
        {
            public ResourceId Resource;

            public UInt32 FirstSlice;
        };
        [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
        public Texture[] Textures;

        [StructLayout(LayoutKind.Sequential)]
        public class Sampler
        {
            public ResourceId Samp;
            [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
            public string AddressS, AddressT, AddressR;
            [CustomMarshalAs(CustomUnmanagedType.FixedArray, FixedLength = 4)]
            public float[] BorderColor;
            [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
            public string Comparison;
            [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
            public string MinFilter;
            [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
            public string MagFilter;
            public bool UseBorder;
            public bool UseComparison;
            public float MaxAniso;
            public float MaxLOD;
            public float MinLOD;
            public float MipLODBias;
        };
        [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
        public Sampler[] Samplers;

        [StructLayout(LayoutKind.Sequential)]
        public class Buffer
        {
            public ResourceId Resource;

            public UInt64 Offset;
            public UInt64 Size;
        };
        [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
        public Buffer[] UniformBuffers;

        [StructLayout(LayoutKind.Sequential)]
        public class Rasterizer
        {
            [StructLayout(LayoutKind.Sequential)]
            public class Viewport
            {
                public float Left, Bottom;
                public float Width, Height;
                public double MinDepth, MaxDepth;
            };
            [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
            public Viewport[] Viewports;

            [StructLayout(LayoutKind.Sequential)]
            public class Scissor
            {
                public Int32 Left, Bottom;
                public Int32 Width, Height;
                public bool Enabled;
            };
            [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
            public Scissor[] Scissors;

            [StructLayout(LayoutKind.Sequential)]
            public class RasterizerState
            {
                public TriangleFillMode FillMode;
                public TriangleCullMode CullMode;
                public bool FrontCCW;
                public float DepthBias;
                public float SlopeScaledDepthBias;
                public float OffsetClamp;
                public bool DepthClamp;
                public bool MultisampleEnable;
            };
            [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
            public RasterizerState m_State;
        };
        [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
        public Rasterizer m_RS;

        [StructLayout(LayoutKind.Sequential)]
        public class DepthState
        {
            public bool DepthEnable;
            [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
            public string DepthFunc;
            public bool DepthWrites;
            public bool DepthBounds;
            public double NearBound;
            public double FarBound;
        };
        [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
        public DepthState m_DepthState;

        [StructLayout(LayoutKind.Sequential)]
        public class StencilState
        {
            public bool StencilEnable;

            [StructLayout(LayoutKind.Sequential)]
            public class StencilOp
            {
                [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
                public string FailOp;
                [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
                public string DepthFailOp;
                [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
                public string PassOp;
                [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
                public string Func;
                public UInt32 Ref;
                public UInt32 ValueMask;
                public UInt32 WriteMask;
            };
            [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
            public StencilOp m_FrontFace, m_BackFace;
        };
        [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
        public StencilState m_StencilState;

        [StructLayout(LayoutKind.Sequential)]
        public class FrameBuffer
        {
            public ResourceId FBO;

            bool FramebufferSRGB;

            [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
            public ResourceId[] Color;
            public ResourceId Depth;
            public ResourceId Stencil;

            [StructLayout(LayoutKind.Sequential)]
            public class BlendState
            {
                [StructLayout(LayoutKind.Sequential)]
                public class RTBlend
                {
                    [StructLayout(LayoutKind.Sequential)]
                    public class BlendOp
                    {
                        [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
                        public string Source;
                        [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
                        public string Destination;
                        [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
                        public string Operation;
                    };
                    [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
                    public BlendOp m_Blend, m_AlphaBlend;

                    [CustomMarshalAs(CustomUnmanagedType.UTF8TemplatedString)]
                    public string LogicOp;

                    public bool Enabled;
                    public byte WriteMask;
                };
                [CustomMarshalAs(CustomUnmanagedType.TemplatedArray)]
                public RTBlend[] Blends;

                [CustomMarshalAs(CustomUnmanagedType.FixedArray, FixedLength = 4)]
                public float[] BlendFactor;
            };
            [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
            public BlendState m_BlendState;
        };
        [CustomMarshalAs(CustomUnmanagedType.CustomClass)]
        public FrameBuffer m_FB;
    }
}
