#include <wayfire/debug.hpp>

#include "deco-effects.hpp"

#include <wayfire/option-wrapper.hpp>


namespace wf
{
namespace decor
{


static const char *motion_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 3) uniform int px;
layout(location = 4) uniform int py;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;
layout(location = 7) uniform int rand1;
layout(location = 8) uniform int rand2;

float rand(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void motion(int x, int y)
{
	int i, i0, i1, j, j0, j1, d = 5;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	if (x - d < 1)
		i0 = 1;
	else
		i0 = x - d;
	if (i0 + 2 * d > width - 1)
		i1 = width - 1;
	else
		i1 = i0 + 2 * d;

	if (y - d < 1)
		j0 = 1;
	else
		j0 = y - d;
	if (j0 + 2 * d > height - 1)
		j1 = height - 1;
	else
		j1 = j0 + 2 * d;

	for (i = i0; i < i1; i++)
	{
		for (j = j0; j < j1; j++) {
			if (i > border_size && i < width - border_size && j > title_height && j < height - border_size)
			{
				continue;
			}
			vec4 b0u = imageLoad(in_b0u, ivec2(i, j));
			vec4 b0v = imageLoad(in_b0v, ivec2(i, j));
			vec4 b0d = imageLoad(in_b0d, ivec2(i, j));
			float u = b0u.x;
			float v = b0v.x;
			float d = b0d.x;
			imageStore(out_b0u, ivec2(i, j), vec4(u + float(256 - (rand1 & 512)), 0.0, 0.0, 0.0));
			imageStore(out_b0v, ivec2(i, j), vec4(v + float(256 - (rand2 & 512)), 0.0, 0.0, 0.0));
			imageStore(out_b0d, ivec2(i, j), vec4(d + 1.0, 0.0, 0.0, 0.0));
		}
	}
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x == px && pos.y == py)
    {
        motion(pos.x, pos.y);
    }
}
)";

static const char *diffuse1_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void diffuse1(int x, int y)
{
	int k, stride;
	float t, a = 0.0002;

	stride = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 s = imageLoad(in_b0u, ivec2(x, y));
	vec4 d1 = imageLoad(in_b1u, ivec2(x - 1, y));
	vec4 d2 = imageLoad(in_b1u, ivec2(x + 1, y));
	vec4 d3 = imageLoad(in_b1u, ivec2(x, y - 1));
	vec4 d4 = imageLoad(in_b1u, ivec2(x, y + 1));
	float sx = s.x;
	float du1 = d1.x;
	float du2 = d2.x;
	float du3 = d3.x;
	float du4 = d4.x;
	float t1 = du1 + du2 + du3 + du4;
	s = imageLoad(in_b0v, ivec2(x, y));
	d1 = imageLoad(in_b1v, ivec2(x - 1, y));
	d2 = imageLoad(in_b1v, ivec2(x + 1, y));
	d3 = imageLoad(in_b1v, ivec2(x, y - 1));
	d4 = imageLoad(in_b1v, ivec2(x, y + 1));
	float sy = s.x;
	du1 = d1.x;
	du2 = d2.x;
	du3 = d3.x;
	du4 = d4.x;
	float t2 = du1 + du2 + du3 + du4;
	imageStore(out_b1u, ivec2(x, y), vec4((sx + a * t1) / (1.0 + 4.0 * a) * 0.995, 0.0, 0.0, 0.0));
	imageStore(out_b1v, ivec2(x, y), vec4((sy + a * t2) / (1.0 + 4.0 * a) * 0.995, 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    diffuse1(pos.x, pos.y);
}
)";

static const char *project1_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void project1(int x, int y)
{
	int k, l, s;
	float h;

	h = 1.0 / float(width);
	s = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 s1 = imageLoad(in_b1u, ivec2(x - 1, y));
	vec4 s2 = imageLoad(in_b1u, ivec2(x + 1, y));
	vec4 s3 = imageLoad(in_b1v, ivec2(x, y - 1));
	vec4 s4 = imageLoad(in_b1v, ivec2(x, y + 1));
	float u1 = s1.x;
	float u2 = s2.x;
	float v1 = s3.x;
	float v2 = s4.x;
	imageStore(out_b0u, ivec2(x, y), vec4(0.0, 0.0, 0.0, 0.0));
	imageStore(out_b0v, ivec2(x, y), vec4(-0.5 * h * (u2 - u1 + v2 - v1), 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    project1(pos.x, pos.y);
}
)";

static const char *project2_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void project2(int x, int y)
{
	int k, l, s;
	float h;

	h = 1.0 / float(width);
	s = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 s0 = imageLoad(in_b0v, ivec2(x, y));
	vec4 s1 = imageLoad(in_b0u, ivec2(x - 1, y));
	vec4 s2 = imageLoad(in_b0u, ivec2(x + 1, y));
	vec4 s3 = imageLoad(in_b0u, ivec2(x, y - 1));
	vec4 s4 = imageLoad(in_b0u, ivec2(x, y + 1));
	float u1 = s1.x;
	float u2 = s2.x;
	float u3 = s3.x;
	float u4 = s4.x;
	imageStore(out_b0u, ivec2(x, y), vec4((s0.x + u1 + u2 + u3 + u4) / 4.0, 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    project2(pos.x, pos.y);
}
)";

static const char *project3_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void project3(int x, int y)
{
	int k, l, s;
	float h;

	h = 1.0 / float(width);
	s = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 s0x = imageLoad(in_b1u, ivec2(x, y));
	vec4 s1 = imageLoad(in_b0u, ivec2(x - 1, y));
	vec4 s2 = imageLoad(in_b0u, ivec2(x + 1, y));
	vec4 s0y = imageLoad(in_b1v, ivec2(x, y));
	vec4 s3 = imageLoad(in_b0u, ivec2(x, y - 1));
	vec4 s4 = imageLoad(in_b0u, ivec2(x, y + 1));
	float su = s0x.x;
	float u1 = s1.x;
	float u2 = s2.x;
	float sv = s0y.x;
	float u3 = s3.x;
	float u4 = s4.x;
	imageStore(out_b1u, ivec2(x, y), vec4(su - 0.5 * (u2 - u1) / h, 0.0, 0.0, 0.0));
	imageStore(out_b1v, ivec2(x, y), vec4(sv - 0.5 * (u4 - u3) / h, 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    project3(pos.x, pos.y);
}
)";

static const char *advect1_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void advect1(int x, int y) /* b1.u, b1.v, b1.u, b0.u */
{
	int stride;
	int i, j;
	float fx, fy;

	stride = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 sx = imageLoad(in_b1u, ivec2(x, y));
	vec4 sy = imageLoad(in_b1v, ivec2(x, y));
	float ix = float(x) - sx.x;
	float iy = float(y) - sy.x;
	if (ix < 0.5)
		ix = 0.5;
	if (iy < 0.5)
		iy = 0.5;
	if (ix > float(width) - 1.5)
		ix = float(width) - 1.5;
	if (iy > float(height) - 1.5)
		iy = float(height) - 1.5;
	i = int(ix);
	j = int(iy);
	fx = ix - float(i);
	fy = iy - float(j);
	vec4 s0x = imageLoad(in_b1u, ivec2(i,     j));
	vec4 s1x = imageLoad(in_b1u, ivec2(i + 1, j));
	vec4 s2x = imageLoad(in_b1u, ivec2(i,     j + 1));
	vec4 s3x = imageLoad(in_b1u, ivec2(i + 1, j + 1));
	float p1 = (s0x.x * (1.0 - fx) + s1x.x * fx) * (1.0 - fy) + (s2x.x * (1.0 - fx) + s3x.x * fx) * fy;
	imageStore(out_b0u, ivec2(x, y), vec4(p1, 0.0, 0.0, 0.0));
	ix = float(x) - sx.x;
	iy = float(y) - sy.x;
	if (ix < 0.5)
		ix = 0.5;
	if (iy < 0.5)
		iy = 0.5;
	if (ix > float(width) - 1.5)
		ix = float(width) - 1.5;
	if (iy > float(height) - 1.5)
		iy = float(height) - 1.5;
	i = int(ix);
	j = int(iy);
	fx = ix - float(i);
	fy = iy - float(j);
	vec4 s0y = imageLoad(in_b1v, ivec2(i,     j));
	vec4 s1y = imageLoad(in_b1v, ivec2(i + 1, j));
	vec4 s2y = imageLoad(in_b1v, ivec2(i,     j + 1));
	vec4 s3y = imageLoad(in_b1v, ivec2(i + 1, j + 1));
	float p2 = (s0y.x * (1.0 - fx) + s1y.x * fx) * (1.0 - fy) + (s2y.x * (1.0 - fx) + s3y.x * fx) * fy;
	imageStore(out_b0v, ivec2(x, y), vec4(p2, 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    advect1(pos.x, pos.y);
}
)";

static const char *project4_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void project4(int x, int y)
{
	int k, l, s;
	float h;

	h = 1.0 / float(width);
	s = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 s1 = imageLoad(in_b0u, ivec2(x - 1, y));
	vec4 s2 = imageLoad(in_b0u, ivec2(x + 1, y));
	vec4 s3 = imageLoad(in_b0v, ivec2(x, y - 1));
	vec4 s4 = imageLoad(in_b0v, ivec2(x, y + 1));
	float u1 = s1.x;
	float u2 = s2.x;
	float v1 = s3.x;
	float v2 = s4.x;
	imageStore(out_b1u, ivec2(x, y), vec4(0.0, 0.0, 0.0, 0.0));
	imageStore(out_b1v, ivec2(x, y), vec4(-0.5 * h * (u2 - u1 + v2 - v1), 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    project4(pos.x, pos.y);
}
)";

static const char *project5_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void project5(int x, int y)
{
	int k, l, s;
	float h;

	h = 1.0 / float(width);
	s = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 s0 = imageLoad(in_b1v, ivec2(x, y));
	vec4 s1 = imageLoad(in_b1u, ivec2(x - 1, y));
	vec4 s2 = imageLoad(in_b1u, ivec2(x + 1, y));
	vec4 s3 = imageLoad(in_b1u, ivec2(x, y - 1));
	vec4 s4 = imageLoad(in_b1u, ivec2(x, y + 1));
	float u1 = s1.x;
	float u2 = s2.x;
	float u3 = s3.x;
	float u4 = s4.x;
	imageStore(out_b1u, ivec2(x, y), vec4((s0.x + u1 + u2 + u3 + u4) / 4.0, 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    project5(pos.x, pos.y);
}
)";

static const char *project6_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void project6(int x, int y)
{
	int k, l, s;
	float h;

	h = 1.0 / float(width);
	s = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 s0x = imageLoad(in_b0u, ivec2(x, y));
	vec4 s1 = imageLoad(in_b1u, ivec2(x - 1, y));
	vec4 s2 = imageLoad(in_b1u, ivec2(x + 1, y));
	vec4 s0y = imageLoad(in_b0v, ivec2(x, y));
	vec4 s3 = imageLoad(in_b1u, ivec2(x, y - 1));
	vec4 s4 = imageLoad(in_b1u, ivec2(x, y + 1));
	float su = s0x.x;
	float u1 = s1.x;
	float u2 = s2.x;
	float sv = s0y.x;
	float u3 = s3.x;
	float u4 = s4.x;
	imageStore(out_b0u, ivec2(x, y), vec4(su - 0.5 * (u2 - u1) / h, 0.0, 0.0, 0.0));
	imageStore(out_b0v, ivec2(x, y), vec4(sv - 0.5 * (u4 - u3) / h, 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    project6(pos.x, pos.y);
}
)";

static const char *diffuse2_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void diffuse2(int x, int y)
{
	int k, stride;
	float t, a = 0.0002;

	stride = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 s = imageLoad(in_b0d, ivec2(x, y));
	vec4 d1 = imageLoad(in_b1d, ivec2(x - 1, y));
	vec4 d2 = imageLoad(in_b1d, ivec2(x + 1, y));
	vec4 d3 = imageLoad(in_b1d, ivec2(x, y - 1));
	vec4 d4 = imageLoad(in_b1d, ivec2(x, y + 1));
	float sz = s.x;
	float du1 = d1.x;
	float du2 = d2.x;
	float du3 = d3.x;
	float du4 = d4.x;
	t = du1 + du2 + du3 + du4;
	imageStore(out_b1d, ivec2(x, y), vec4((sz + a * t) / (1.0 + 4.0 * a) * 0.995, 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    diffuse2(pos.x, pos.y);
}
)";

static const char *advect2_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform readonly image2D in_tex;
layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 1, r32f) uniform readonly image2D in_b0u;
layout (binding = 1, r32f) uniform writeonly image2D out_b0u;
layout (binding = 2, r32f) uniform readonly image2D in_b0v;
layout (binding = 2, r32f) uniform writeonly image2D out_b0v;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (binding = 3, r32f) uniform writeonly image2D out_b0d;
layout (binding = 4, r32f) uniform readonly image2D in_b1u;
layout (binding = 4, r32f) uniform writeonly image2D out_b1u;
layout (binding = 5, r32f) uniform readonly image2D in_b1v;
layout (binding = 5, r32f) uniform writeonly image2D out_b1v;
layout (binding = 6, r32f) uniform readonly image2D in_b1d;
layout (binding = 6, r32f) uniform writeonly image2D out_b1d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;

void advect2(int x, int y) /*b0u, b0v, b1d, b0d*/
{
	int stride;
	int i, j;
	float fx, fy;

	stride = width;

	if (x <= 0 || y <= 0 || x >= width - 1 || y >= height - 1 ||
	    (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size))
	{
		return;
	}

	vec4 sx = imageLoad(in_b0u, ivec2(x, y));
	vec4 sy = imageLoad(in_b0v, ivec2(x, y));
	float ix = float(x) - sx.x;
	float iy = float(y) - sy.x;
	if (ix < 0.5)
		ix = 0.5;
	if (iy < 0.5)
		iy = 0.5;
	if (ix > float(width) - 1.5)
		ix = float(width) - 1.5;
	if (iy > float(height) - 1.5)
		iy = float(height) - 1.5;
	i = int(ix);
	j = int(iy);
	fx = ix - float(i);
	fy = iy - float(j);
	vec4 s0z = imageLoad(in_b1d, ivec2(i,     j));
	vec4 s1z = imageLoad(in_b1d, ivec2(i + 1, j));
	vec4 s2z = imageLoad(in_b1d, ivec2(i,     j + 1));
	vec4 s3z = imageLoad(in_b1d, ivec2(i + 1, j + 1));
	float p1 = (s0z.x * (1.0 - fx) + s1z.x * fx) * (1.0 - fy) + (s2z.x * (1.0 - fx) + s3z.x * fx) * fy;
	imageStore(out_b0d, ivec2(x, y), vec4(p1, 0.0, 0.0, 0.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    advect2(pos.x, pos.y);
}
)";

static const char *render_source =
    R"(
#version 320 es

precision lowp image2D;

layout (binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout (binding = 3, r32f) uniform readonly image2D in_b0d;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 1) uniform int title_height;
layout(location = 2) uniform int border_size;
layout(location = 4) uniform bool ink;
layout(location = 5) uniform int width;
layout(location = 6) uniform int height;
layout(location = 7) uniform vec4 smoke_color;
layout(location = 8) uniform vec4 decor_color;

void render(int x, int y)
{
	float c, r, g, b, a;

	if (x >= border_size && x <= width - border_size && y >= title_height && y <= height - border_size)
	{
		return;
	}

	vec4 s = imageLoad(in_b0d, ivec2(x, y));
	c = s.x * 800.0;
	if (c > 255.0)
		c = 255.0;
	a = c * smoke_color.a;
	vec3 color;
	if (ink)
	{
		if (c > 2.0)
			color = mix(decor_color.rgb, smoke_color.rgb, clamp(a, 0.0, 1.0));
		else
			color = mix(decor_color.rgb, vec3(0.0, 0.0, 0.0), clamp(a, 0.0, 1.0));
		if (c > 1.5)
			imageStore(out_tex, ivec2(x, y), vec4(color, 1.0));
		else
			imageStore(out_tex, ivec2(x, y), decor_color);
		return;
	} else
	{
		color = mix(decor_color.rgb, smoke_color.rgb, clamp(a, 0.0, 1.0));
	}
	imageStore(out_tex, ivec2(x, y), vec4(color, 1.0));
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    render(pos.x, pos.y);
}
)";


static const char *render_source_clouds =
    R"(

#version 320 es
precision highp float;
precision highp image2D;

layout(binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(location = 9) uniform float current_time;
float cloudscale=2.1;  // Added cloudscale parameter

const mat2 m = mat2(1.6, 1.2, -1.2, 1.6);

vec2 hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(vec2 p) {
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    vec2 i = floor(p + (p.x + p.y) * K1);
    vec2 a = p - i + (i.x + i.y) * K2;
    vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0 * K2;
    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    vec3 n = h * h * h * h * vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, vec3(70.0));
}

float fbm(vec2 n) {
    float total = 0.0, amplitude = 0.1;
    for (int i = 0; i < 7; i++) {
        total += noise(n) * amplitude;
        n = m * n;
        amplitude *= 0.4;
    }
    return total;
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = vec2(pos) / vec2(gl_NumWorkGroups.x * gl_WorkGroupSize.x, gl_NumWorkGroups.y * gl_WorkGroupSize.y);

    float time = 0.003 * current_time;  // Time variable for animation

    float cloudPattern1 = fbm(uv * 10.0 * cloudscale + time);
    float cloudPattern2 = fbm(uv * 5.0 * cloudscale + time);
    float cloudPattern3 = fbm(uv * 3.0 * cloudscale + time);

    // Combine different cloud patterns with different weights
    float cloudPattern = 0.5 * cloudPattern1 + 0.3 * cloudPattern2 + 0.2 * cloudPattern3;

    // Ridge noise shape
    float ridgeNoiseShape = 0.0;
    uv *= cloudscale * 1.1;  // Adjust scale
    float weight = 0.8;
    for (int i = 0; i < 8; i++) {
        ridgeNoiseShape += abs(weight * noise(uv));
        uv = m * uv + time;
        weight *= 0.7;
    }

    // Noise shape
    float noiseShape = 0.0;
    uv = vec2(pos) / vec2(gl_NumWorkGroups.x * gl_WorkGroupSize.x, gl_NumWorkGroups.y * gl_WorkGroupSize.y);
    uv *= cloudscale * 1.1;  // Adjust scale
    weight = 0.7;
    for (int i = 0; i < 8; i++) {
        noiseShape += weight * noise(uv);
        uv = m * uv + time;
        weight *= 0.6;
    }

    noiseShape *= ridgeNoiseShape + noiseShape;

    // Noise color
    float noiseColor = 0.0;
    uv = vec2(pos) / vec2(gl_NumWorkGroups.x * gl_WorkGroupSize.x, gl_NumWorkGroups.y * gl_WorkGroupSize.y);
    uv *= 2.0 * cloudscale;  // Adjust scale
    weight = 0.4;
    for (int i = 0; i < 7; i++) {
        noiseColor += weight * noise(uv);
        uv = m * uv + time;
        weight *= 0.6;
    }

    // Noise ridge color
    float noiseRidgeColor = 0.0;
    uv = vec2(pos) / vec2(gl_NumWorkGroups.x * gl_WorkGroupSize.x, gl_NumWorkGroups.y * gl_WorkGroupSize.y);
    uv *= 3.0 * cloudscale;  // Adjust scale
    weight = 0.4;
    for (int i = 0; i < 7; i++) {
        noiseRidgeColor += abs(weight * noise(uv));
        uv = m * uv + time;
        weight *= 0.6;
    }

    noiseColor += noiseRidgeColor;

    // Sky tint
    float skytint = 0.5;
    vec3 skyColour1 = vec3(0.1, 0.2, 0.3);
    vec3 skyColour2 = vec3(0.4, 0.7, 1.0);
    vec3 skycolour = mix(skyColour1, skyColour2, smoothstep(0.4, 0.6, uv.y));

    // Cloud darkness
    float clouddark = 0.5;

    // Cloud Cover, Cloud Alpha
    float cloudCover = 0.01;
    float cloudAlpha = 2.0;

    // Movement effect
    uv = uv + time;

    // Use a bright color for clouds
    vec3 cloudColor = vec3(1.0, 1.0, 1.0); ;  // Bright white color for clouds

    // Mix the cloud color with the background, considering darkness, cover, and alpha
    vec3 finalColor = mix(skycolour, cloudColor * clouddark, cloudPattern + noiseShape + noiseColor) * (1.0 - cloudCover) + cloudColor * cloudCover;
    finalColor = mix(skycolour, finalColor, cloudAlpha);

    imageStore(out_tex, pos, vec4(finalColor, 1.0));
}


)";








static const char *render_source_halftone =
    R"(
#version 310 es
precision highp float;
precision highp image2D;

layout(binding = 0, rgba32f) writeonly uniform highp image2D out_tex;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 9) uniform float  time;

const vec2 resolution = vec2(1280.0, 720.0);
const float timeFactor = 0.025;

float rand(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 rotate(vec2 p, float theta) {
    vec2 sncs = vec2(sin(theta), cos(theta));
    return vec2(p.x * sncs.y - p.y * sncs.x, p.x * sncs.x + p.y * sncs.y);
}

float swirl(vec2 coord, float t) {
    float l = length(coord) / resolution.x;
    float phi = atan(coord.y, coord.x + 1e-6);
    return sin(l * 10.0 + phi - t * 4.0) * 0.5 + 0.5;
}

float halftone(vec2 coord, float angle, float t, float amp) {
    coord -= resolution * 0.5;
    float size = resolution.x / (60.0 + sin(time * timeFactor * 0.5) * 50.0);
    vec2 uv = rotate(coord / size, angle / 180.0 * 3.14);
    vec2 ip = floor(uv); // column, row
    vec2 odd = vec2(0.5 * mod(ip.y, 2.0), 0.0); // odd line offset
    vec2 cp = floor(uv - odd) + odd; // dot center
    float d = length(uv - cp - 0.5) * size; // distance
    float r = swirl(cp * size, t) * size * 0.5 * amp; // dot radius
    return 1.0 - clamp(d  - r, 0.0, 1.0);
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    vec3 c1 = 1.0 - vec3(1.0, 0.0, 0.0) * halftone(vec2(pos),   0.0, time * timeFactor * 1.00, 0.7);
    vec3 c2 = 1.0 - vec3(0.0, 1.0, 0.0) * halftone(vec2(pos),  30.0, time * timeFactor * 1.33, 0.7);
    vec3 c3 = 1.0 - vec3(0.0, 0.0, 1.0) * halftone(vec2(pos), -30.0, time * timeFactor * 1.66, 0.7);
    vec3 c4 = 1.0 - vec3(1.0, 1.0, 1.0) * halftone(vec2(pos),  60.0, time * timeFactor * 2.13, 0.4);

    // Output the final color
    imageStore(out_tex, pos, vec4(c1 * c2 * c3 * c4, 1.0));
}

)";

static const char *render_source_lava =
    R"(
#version 320 es
precision highp float;
precision highp image2D;

layout(binding = 0, rgba32f) uniform readonly image2D in_tex;
layout(binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 9) uniform float current_time;

vec3 effect(float speed, vec2 uv, float time, float scale) {
    float t = mod(time * 0.005, 6.0);
    float rt = 0.00000000000001 * sin(t * 0.45);

    mat2 m1 = mat2(cos(rt), -sin(rt), -sin(rt), cos(rt));
    vec2 uva = uv * m1 * scale;
    float irt = 0.005 * cos(t * 0.05);
    mat2 m2 = mat2(sin(irt), cos(irt), -cos(irt), sin(irt));

    for (int i = 1; i < 40; i += 1) {
        float it = float(i);
        uva *= m2;
        uva.y += -1.0 + (0.6 / it) * cos(t + it * uva.x + 0.5 * it) * float(mod(it, 0.5) == 0.0);
        uva.x += 1.0 + (0.5 / it) * cos(t + it * uva.y * 0.1 / 5.0 + 0.5 * (it + 15.0));  // Adjust the scaling factor for y-coordinate
    }

    float n = 0.5;
    float r = n + n * sin(4.0 * uva.x + t);
    float gb = n + n * sin(3.0 * uva.y);
    return vec3(r, gb * 0.8 * r, gb * r);
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = vec2(pos) / vec2(1000, 2000);
    uv = 2.0 * uv - 1.0;
    uv *= (10.3 + 0.1 * sin(current_time * 0.01));
   // uv.y -= current_time * 0.013;
//uv.x -= current_time * 0.026;
    vec3 col = effect(0.001, uv, current_time, 0.5);
    // col += effect(0.5, uv * 3.0, 2.0 * current_time + 10.0, 0.5) * 0.3;
    // col += effect(0.5, sin(current_time * 0.01) * uv * 2.0, 2.0 * current_time + 10.0, 0.5) * 0.1;

    // Output the final color to the output texture
    imageStore(out_tex, pos, vec4(col, 1.0));
}
)";

static const char *render_source_pattern =
    R"(
#version 310 es
precision highp float;
precision highp image2D;

layout(binding = 0, rgba32f) writeonly uniform highp image2D out_tex;
layout(local_size_x = 30, local_size_y = 30, local_size_z = 1) in;

layout(location = 9) uniform float time;
const vec2 resolution = vec2(1280.0, 720.0);  // Adjusted to 720p

float rand(vec2 uv)
{
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 hue2rgb(float h)
{
    h = fract(h) * 6.0 - 2.0;
    return clamp(vec3(abs(h - 1.0) - 1.0, 2.0 - abs(h), 2.0 - abs(h - 2.0)), 0.0, 1.0);
}

vec3 eyes(vec2 coord)
{
    const float pi = 3.141592;
    float t = 0.4 * time * 0.05; 
    float aspectRatio = resolution.x / resolution.y;
    float div = 20.0;
    float sc = resolution.y / div ;

    vec2 p = (coord - resolution / 2.0) / sc - 0.5;

    // center offset
    float dir = floor(rand(floor(p) + floor(t) * 0.11) * 4.0) * pi / 2.0;
    vec2 offs = vec2(sin(dir), cos(dir)) * 0.6;
    offs *= smoothstep(0.0, 0.1, fract(t));
    offs *= smoothstep(0.4, 0.5, 1.0 - fract(t));

    // circles
    float l = length(fract(p) + offs - 0.5);
    float rep = sin((rand(floor(p)) * 2.0 + 2.0) * t) * 4.0 + 5.0;
    float c = (abs(0.5 - fract(l * rep + 0.5)) - 0.25) * sc / rep;

    // grid lines
    vec2 gr = (abs(0.5 - fract(p + 0.5)) - 0.05) * sc;
    c = clamp(min(min(c, gr.x), gr.y), 0.0, 1.0);

    return hue2rgb(rand(floor(p) * 0.3231)) * c;
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    vec3 color = eyes(vec2(pos));

    // Output the final color
    imageStore(out_tex, pos, vec4(color, 1.0));
}


)";


static const char *render_source_hex =
    R"(
#version 320 es
precision highp float;
precision highp int;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(binding = 0, rgba32f) writeonly uniform highp image2D OutputImage;

layout(location = 9) uniform float iTime;
uniform vec2 iResolution;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

// Hexagon function
vec4 hexagon(vec2 p)
{
    vec2 q = vec2(p.x * 2.0 * 0.5773503, p.y + p.x * 0.5773503);

    vec2 pi = floor(q);
    vec2 pf = fract(q);

    float v = mod(pi.x * 9.0, 0.0);

    float ca = step(1.0, v);
    float cb = step(2.0, v);
    vec2 ma = step(pf.xy, pf.yx);

    float e = 0.0;
    float f = length((fract(p) - 10.5) * vec2(1.0, 0.85));

    return vec4(pi + ca - cb * ma, e, f);
}

void main() {
    ivec2 globalID = ivec2(gl_WorkGroupID.xy * gl_WorkGroupSize.xy + gl_LocalInvocationID.xy);
    vec2 uv = (vec2(globalID) + 0.5) / 1920.0; // Assuming iResolution is 1920x1080

    // Generate a random value
    float randVal = rand(uv);

    // Apply hexagon logic
    vec2 pos = (-1920.0 + 2.0 * vec2(globalID)) / 1080.0;
    vec4 h = hexagon(60.0 * pos + vec2(0.5 * iTime * 0.125));
    
 float col = 0.01 + 0.15 * rand(vec2(h.xy)) * 1.0;
    col *= 4.3 + 0.15 * sin(10.0 * h.z);
    // Use hardcoded colors for shades
// Use hardcoded colors for shades with gradient
vec3 shadeColor1 = vec3(0.1, 0.1, 0.1) + 1.1 * col * h.z;   // Dark gray with gradient
vec3 shadeColor2 = vec3(0.4, 0.4, 0.4) + 1.1 * col;  // Gray with gradient
vec3 shadeColor3 = vec3(0.9, 0.9, 0.9) + .1 * col;  // Light gray with gradient


   

    // Use different hardcoded colors based on the random value
    vec3 finalColor = mix(shadeColor1, mix(shadeColor2, shadeColor3, randVal), col);

    // Use imageStore instead of writing to buffer
    imageStore(OutputImage, globalID, vec4(finalColor, 1.0));
}



)";


static const char *render_source_zebra =
    R"(
#version 310 es
precision highp float;
precision highp image2D;

layout(binding = 0, rgba32f) writeonly uniform highp image2D out_tex;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 9) uniform float time;
const float resolutionY = 720.0; // Set to constant resolution of 720p
const float pi = 3.14159265359;
const float timeFactor = 0.05; // Adjust this factor to control the speed of the animation

float rand(vec2 uv)
{
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    float size = resolutionY / 10.0; // cell size in pixel

    vec2 p1 = vec2(pos) / size; // normalized pos
    vec2 p2 = fract(p1) - 0.5; // relative pos from cell center

    // random number
    float rnd = dot(floor(p1), vec2(12.9898, 78.233));
    rnd = fract(sin(rnd) * 43758.5453);

    // rotation matrix
    float phi = rnd * pi * 2.0 + time * 0.4 * timeFactor;
    mat2 rot = mat2(cos(phi), -sin(phi), sin(phi), cos(phi));

    vec2 p3 = rot * p2; // apply rotation
    p3.y += sin(p3.x * 5.0 + time * 2.0 * timeFactor) * 0.12; // wave

    float rep = fract(rnd * 13.285) * 8.0 + 2.0; // line repetition
    float gr = fract(p3.y * rep + time * 0.8 * timeFactor); // repeating gradient

    // make antialiased line by saturating the gradient
    float c = clamp((0.25 - abs(0.5 - gr)) * size * 0.75 / rep, 0.0, 1.0);
    c *= max(0.0, 1.0 - length(p2) * 0.6); // darken corners

    vec2 bd = (0.5 - abs(p2)) * size - 2.0; // border lines
    c *= clamp(min(bd.x, bd.y), 0.0, 1.0);

    // Output the final color
    imageStore(out_tex, pos, vec4(c, c, c, 1.0));
}

)";



static const char *render_source_neural_network =
    R"(

#version 320 es
precision highp float;
precision highp image2D;

layout(binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 9) uniform float time; // Animated time

uniform vec2 iResolution;
uniform float iTime;

mat2 rotate2D(float r) {
    float c = cos(r);
    float s = sin(r);
    return mat2(c, -s, s, c);
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = (vec2(pos) - 0.5 * float(gl_NumWorkGroups.x)) / float(gl_WorkGroupSize.x);
    
    // Scale the uv coordinates by a factor of 8
    uv *= 0.05;

    vec3 col = vec3(0);
    float t = 0.05 * time;

    vec2 n = vec2(0), q;
    vec2 N = vec2(0);
    vec2 p = uv + sin(t * 0.1) / 10.0;
    float S = 10.0;
    mat2 m = rotate2D(1.0);

    for (float j = 0.0; j < 30.0; j++) {
        p *= m;
        n *= m;
        q = p * S + j + n + t;
        n += sin(q);
        N += cos(q) / S;
        S *= 1.2;
    }

    col = vec3(1, 2, 4) * pow((N.x + N.y + 0.2) + 0.005 / length(N), 2.1);

    imageStore(out_tex, pos, vec4(col, 1.0));
}
)";



static const char *render_source_hexagon_maze =
    R"(
#version 320 es

precision highp float;
precision highp int;
precision highp image2D;

//layout (location = 0) out vec4 fragColor;
layout (binding = 0, rgba32f) writeonly uniform image2D fragColor;


layout(location = 9) uniform float iTime;
uniform vec2 iResolution;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;


// Interlaced variation - Interesting, but patched together in a hurry.
//#define INTERLACING

// A quick hack to get rid of the winding overlay - in order to show the maze only.
//#define MAZE_ONLY



// Helper vector. If you're doing anything that involves regular triangles or hexagons, the
// 30-60-90 triangle will be involved in some way, which has sides of 1, sqrt(3) and 2.
const vec2 s = vec2(1, 1.7320508);

// Standard vec2 to float hash - Based on IQ's original.
float hash21(vec2 p){ return fract(sin(dot(p, vec2(141.173, 289.927)))*43758.5453); }


// Standard 2D rotation formula.
mat2 r2(in float a){ float c = cos(a), s = sin(a); return mat2(c, -s, s, c); }


// The 2D hexagonal isosuface function: If you were to render a horizontal line and one that
// slopes at 60 degrees, mirror, then combine them, you'd arrive at the following.
float hex(in vec2 p){
    
    p = abs(p);
    
    // Below is equivalent to:
    //return max(p.x*.5 + p.y*.866025, p.x); 

    return max(dot(p, s*.5), p.x); // Hexagon.
    
}

// This function returns the hexagonal grid coordinate for the grid cell, and the corresponding 
// hexagon cell ID - in the form of the central hexagonal point. That's basically all you need to 
// produce a hexagonal grid.
//
// When working with 2D, I guess it's not that important to streamline this particular function.
// However, if you need to raymarch a hexagonal grid, the number of operations tend to matter.
// This one has minimal setup, one "floor" call, a couple of "dot" calls, a ternary operator, etc.
// To use it to raymarch, you'd have to double up on everything - in order to deal with 
// overlapping fields from neighboring cells, so the fewer operations the better.
vec4 getHex(vec2 p){
    
    // The hexagon centers: Two sets of repeat hexagons are required to fill in the space, and
    // the two sets are stored in a "vec4" in order to group some calculations together. The hexagon
    // center we'll eventually use will depend upon which is closest to the current point. Since 
    // the central hexagon point is unique, it doubles as the unique hexagon ID.
    vec4 hC = floor(vec4(p, p - vec2(.5, 1))/s.xyxy) + .5;
    
    // Centering the coordinates with the hexagon centers above.
    vec4 h = vec4(p - hC.xy*s, p - (hC.zw + .5)*s);
    
    // Nearest hexagon center (with respect to p) to the current point. In other words, when
    // "h.xy" is zero, we're at the center. We're also returning the corresponding hexagon ID -
    // in the form of the hexagonal central point. Note that a random constant has been added to 
    // "hC.zw" to further distinguish it from "hC.xy."
    //
    // On a side note, I sometimes compare hex distances, but I noticed that Iomateron compared
    // the Euclidian version, which seems neater, so I've adopted that.
    return dot(h.xy, h.xy)<dot(h.zw, h.zw) ? vec4(h.xy, hC.xy) : vec4(h.zw, hC.zw + vec2(.5, 1));
    
}

// Dot pattern.
float dots(in vec2 p){
    
    p = abs(fract(p) - .5);
    
    return length(p); // Circles.
    
    //return (p.x + p.y)/1.5 + .035; // Diamonds.
    
    //return max(p.x, p.y) + .03; // Squares.
    
    //return max(p.x*.866025 + p.y*.5, p.y) + .01; // Hexagons.
    
    //return min((p.x + p.y)*.7071, max(p.x, p.y)) + .08; // Stars.
    
    
}


// Distance field for the arcs. I think it's called poloidal rotation, or something like that.
float dfPol(vec2 p){
     
    return length(p); // Circular arc.
    
    // There's no rule that says the arcs have to be rounded. Here's a hexagonal one.
    //return hex(p);
    
    // Dodecahedron.
    //return max(hex(p), hex(r2(3.14159/6.)*p));
    
    // Triangle.
    //return max(abs(p.x)*.866025 - p.y, p.y);
    
}


// Truchet pattern distance field.
float df(vec2 p, float dir){
     
    // Weird UV coordinates. The first entry is the Truchet distance field itself,
    // and the second is the polar angle of the arc pixel. The extra ".1" is just a bit
    // of mutational scaling, or something... I can't actually remember why it's there. :)
    vec2 uv = vec2(p.x + .1, p.y);//*vec2(1, 1); // Scaling.
    
    // A checkered dot pattern. At present the pattern needs to have flip symmetry about
    // the center, but I'm pretty sure regular textures could be applied with a few
    // minor changes. Due to the triangular nature of the Truchet pattern, factors of "3"
    // were necessary, but factors of "1.5" seemed to work too. Hence the "4.5."
    return min(dots(uv*4.5), dots(uv*4.5 + .5)) - .3;
    
}


// Polar coordinate of the arc pixel.
float getPolarCoord(vec2 q, float dir){
    
    // The actual animation. You perform that before polar conversion.
    q = r2((iTime*0.05)*dir)*q;
    
    // Polar angle.
    const float aNum = 1.;
    float a = atan(q.y, q.x);
   
    // Wrapping the polar angle.
    return mod(a/3.14159, 2./aNum) - 1./aNum;
   
    
}



void main() {
	vec2 iResolution = vec2(1280.0, 720.0);
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    vec2 fragCoord = vec2(id);
    float res = clamp(iResolution.y, 300.0, 600.0);
    vec2 u = (fragCoord - iResolution.xy*.5)/res;
    vec2 sc = u*4. + s.yx*(iTime*0.05)/12.;
    vec4 h = getHex(sc);
    vec4 h2 = getHex(sc - 1.0 / s);
    vec4 h3 = getHex(sc + 1.0 / s);
    vec2 p = h.xy;
    float eDist = hex(p);
    float cDist = dot(p, p);
    float rnd = hash21(h.zw);
 //float aRnd = sin(rnd*6.283 + (iTime*0.05)*1.5)*.5 + .5; // Animating the random number.
    
    #ifdef INTERLACING
    // Random vec3 - used for some overlapping.
    //vec3 lRnd = vec3(rnd*14.4 + .81, fract(rnd*21.3 + .97), fract(rnd*7.2 + .63));
    vec3 lRnd = vec3(hash21(h.zw + .23), hash21(h.zw + .96), hash21(h.zw + .47));
    #endif
    
    // It's possible to control the randomness to form some kind of repeat pattern.
    //rnd = mod(h.z + h.w, 2.);
    

    // Redundant here, but I might need it later.
    float dir = 1.;
    

    
    // Storage vector.
    vec2 q;
    
    
    // If the grid cell's random ID is above a threshold, flip the Y-coordinates.
    if(rnd>.5) p.y = -p.y;
        
    

    // Determining the closest of the three arcs to the current point, the keeping a copy
    // of the vector used to produce it. That way, you'll know just to render that particular
    // decorated arc, lines, etc - instead of all three. 
    const float r = 1.;
    const float th = .2; // Arc thickness.
    
    // Arc one.
    q = p - vec2(0, r)/s;
    vec3 da = vec3(q, dfPol(q));
    
    // Arc two. "r2" could be hardcoded, but this is a relatively cheap 2D example.
    q = r2(3.14159*2./3.)*p - vec2(0, r)/s;
    vec3 db = vec3(q, dfPol(q));

     // Arc three. 
    q = r2(3.14159*4./3.)*p - vec2(0, r)/s;
    vec3 dc = vec3(q, dfPol(q));
    
    // Compare distance fields, and return the vector used to produce the closest one.
    vec3 q3 = da.z<db.z && da.z<dc.z? da : db.z<dc.z ? db : dc;
    
    
    // TRUCHET PATTERN
    //
    // Set the poloidal arc radius: You can change the poloidal distance field in 
    // the "dfPol" function to a different curve shape, but you'll need to change
    // the radius to one of the figures below.
    //
    q3.z -= .57735/2. + th/2.;  // Circular and dodecahedral arc/curves.
    //q3.z -= .5/2. + th/2.;  // Hexagon curve.
    //q3.z -= .7071/2. + th/2.;  // Triangle curve.
    
    q3.z = max(q3.z, -th - q3.z); // Chop out the smaller radius. The result is an arc.
    
    // Store the result in "d" - only to save writing "q3.z" everywhere.
    float d = q3.z;
    
    // If you'd like to see the maze by itself.
    #ifdef MAZE_ONLY
    d += 1e5;
    #endif
    
    // Truchet border.
    float dBord = max(d - .015, -d);
    

    
 
    
    // MAZE BORDERS
    // Producing the stright-line arc borders. Basically, we're rendering some hexagonal borders around
    // the arcs. The result is the hexagonal maze surrounding the Truchet pattern.
    q = q3.xy;
    const float lnTh = .05;
    q = abs(q);
    
    float arcBord = hex(q);
    //float arcBord = length(q); // Change arc length to ".57735."
    //float arcBord = max(hex(q), hex(r2(3.14159/6.)*q)); // Change arc length to ".57735."
    
    // Making the hexagonal arc.
    float lnOuter = max(arcBord - .5, -(arcBord - .5 + lnTh)); //.57735
    
    
    #ifdef INTERLACING
    float ln = min(lnOuter, (q.y*.866025 + q.x*.5, q.x) - lnTh);
    #else
    float ln = min(lnOuter, arcBord - lnTh);
    #endif
    float lnBord = ln - .03; // Border lines to the maze border, if that makes any sense. :)
     
       
    ///////
    // The moving Truchet pattern. The polar coordinates consist of a wrapped angular coordinate,
    // and the distance field itself.
    float a = getPolarCoord(q3.xy, dir);
    float d2 = df(vec2(q3.z, a), dir);
    float dMask = smoothstep(0., .015, d);
    vec3 bg =  mix(vec3(0, .0, .6), vec3(0, .9, .0), dot(sin(u*6. - cos(u*3.)), vec2(.4/2.)) + .4); 
    
    
    
    bg = mix(bg, bg.xzy, dot(sin(u*6. - cos(u*3.)), vec2(.4/2.)) + .4);
    bg = mix(bg, bg.zxy, dot(sin(u*3. + cos(u*3.)), vec2(.1/2.)) + .1);
   
    #ifdef INTERLACING
    // Putting in background cube lines for the interlaced version.
    float hLines = smoothstep(0., .02, eDist - .5 + .02);
    bg = mix(bg, vec3(0), smoothstep(0., .02, ln)*dMask*hLines);
    #endif
    
    // Lines over the maze lines. Applying difference logic, depending on whether the 
    // pattern is interlaced or not.
    const float tr = 1.;

    float eDist2 = hex(h2.xy);
    float hLines2 = smoothstep(0., .02, eDist2 - .5 + .02);
    #ifdef INTERLACING
    if(rnd>.5 && lRnd.x<.5) hLines2 *= smoothstep(0., .02, ln);
    if(lRnd.x>.5) hLines2 *= dMask;
    #else
    if(rnd>.5) hLines2 *= smoothstep(0., .02, ln);
    hLines2 *= dMask;
    #endif
    bg = mix(bg, vec3(0), hLines2*tr);
    
    float eDist3 = hex(h3.xy);
    float hLines3 = smoothstep(0., .02, eDist3 - .5 + .02);
    #ifdef INTERLACING
    if(rnd<=.5 && lRnd.x>.5) hLines3 *= smoothstep(0., .02, ln);
    if(lRnd.x>.5) hLines3 *= dMask;
    #else
    if(rnd<=.5) hLines3 *= smoothstep(0., .02, ln);
    hLines3 *= dMask;
    #endif
    bg = mix(bg, vec3(0), hLines3*tr);


    // Using the two off-centered hex coordinates to give the background a bit of highlighting.
    float shade = max(1.25 - dot(h2.xy, h2.xy)*2., 0.);
    shade = min(shade, max(dot(h3.xy, h3.xy)*3. + .25, 0.));
    bg = mix(bg, vec3(0), (1.-shade)*.5); 
    
    // I wanted to change the colors of everything at the last minute. It's pretty hacky, so
    // when I'm feeling less lazy, I'll tidy it up. :)
    vec3 dotCol = bg.zyx*vec3(1.5, .4, .4);
    vec3 bCol = mix(bg.zyx, bg.yyy, .25);
    bg = mix(bg.yyy, bg.zyx, .25);
    
    

    // Under the random threshold, and we draw the lines under the Truchet pattern.
    #ifdef INTERLACING
    if(lRnd.x>.5){
       bg = mix(bg, vec3(0), (1. - smoothstep(0., .015, lnBord)));
       bg = mix(bg, bCol, (1. - smoothstep(0., .015, ln))); 
       // Center lines.
       bg = mix(bg, vec3(0), smoothstep(0., .02, eDist3 - .5 + .02)*tr);
    }
    #else
    bg = mix(bg, vec3(0), (1. - smoothstep(0., .015, lnBord)));
    bg = mix(bg, bCol, (1. - smoothstep(0., .015, ln)));
    #endif


   
     // Apply the Truchet shadow to the background.
    bg = mix(bg, vec3(0), (1. - smoothstep(0., .07, d))*.5);
    
    
    // Place the Truchet field to the background, with some additional shading to give it a 
    // slightly rounded, raised feel.
    //vec3 col = mix(bg, vec3(1)*max(-d*3. + .7, 0.), (1. - dMask)*.65);
    // Huttarl suggest slightly more shading on the snake-like pattern edges, so I added just a touch.
    vec3 col = mix(bg, vec3(1)*max(-d*9. + .4, 0.), (1. - dMask)*.65);



    
    // Apply the moving dot pattern to the Truchet.
    //dotCol = mix(dotCol, dotCol.xzy, dot(sin(u*3.14159*2. - cos(u.yx*3.14159*2.)*3.14159), vec2(.25)) + .5);
    col = mix(col, vec3(0), (1. - dMask)*(1. - smoothstep(0., .02, d2)));
    col = mix(col, dotCol, (1. - dMask)*(1. - smoothstep(0., .02, d2 + .125)));
    
    // Truchet border.
    col = mix(col, vec3(0), 1. - smoothstep(0., .015, dBord));
    
    #ifdef INTERLACING
    // Over the random threshold, and we draw the lines over the Truchet.
    if(lRnd.x<=.5){
        col = mix(col, vec3(0), (1. - smoothstep(0., .015, lnBord)));
        col = mix(col, bCol, (1. - smoothstep(0., .015, ln)));  
        // Center lines.
        col = mix(col, vec3(0), smoothstep(0., .02, eDist2 - .5 + .02)*tr);
    }
    #endif


        
    
    // Subtle vignette.
    u = fragCoord/iResolution.xy;
    col *= pow(16.*u.x*u.y*(1. - u.x)*(1. - u.y) , .125) + .25;
    // Colored variation.
    //col = mix(pow(min(vec3(1.5, 1, 1)*col, 1.), vec3(1, 3, 16)), col, 
            //pow(16.*u.x*u.y*(1. - u.x)*(1. - u.y) , .25)*.75 + .25);    
    
    

 

 
    
    // Using the edge distance to produce some repeat contour lines. Standard stuff.
    //if (rnd>.5) h.xy = -h.yx;
    //float cont = clamp(cos(hex(h.xy)*6.283*12.)*1.5 + 1.25, 0., 1.);
    //col = mix(col, vec3(0), (1. - smoothstep(0., .015, ln))*(smoothstep(0., .015, d))*(1.-cont)*.5);
    
    
 
    // Very basic hatch line effect.
    float gr = dot(col, vec3(.299, .587, .114));
    float hatch = (gr<.45)? clamp(sin((sc.x - sc.y)*3.14159*40.)*2. + 1.5, 0., 1.) : 1.;
    float hatch2 = (gr<.25)? clamp(sin((sc.x + sc.y)*3.14159*40.)*2. + 1.5, 0., 1.) : 1.;

    col *= min(hatch, hatch2)*.5 + .5;    
    col *= clamp(sin((sc.x - sc.y)*3.14159*80.)*1.5 + .75, 0., 1.)*.25 + 1.;  
    
 
 
    vec2 u2 = fragCoord / iResolution.xy;
    col *= pow(16.0 * u2.x * u2.y * (1.0 - u2.x) * (1.0 - u2.y), 0.125) + 0.25;

    imageStore(fragColor, id, vec4(sqrt(max(col, 0.0)), 1.0));
}



)";


static const char *render_source_raymarched_truchet  =
    R"(
#version 320 es

precision highp float;
precision highp int;
precision highp image2D;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(binding = 0, rgba8) writeonly uniform image2D imageOut;


layout(location = 9) uniform float  iTime;
uniform vec2 iResolution;

float heightMap(in vec2 p) { 
    
    p *= 5.;
    
    // Hexagonal coordinates.
    vec2 h = vec2(p.x + p.y*.57735, p.y*1.1547);
    
    // Closest hexagon center.
    vec2 f = fract(h); h -= f;
    float c = fract((h.x + h.y)/3.);
    h =  c<.666 ?   c<.333 ?  h  :  h + 1.  :  h  + step(f.yx, f); 

    p -= vec2(h.x - h.y*.5, h.y*.8660254);
    
    // Rotate (flip, in this case) random hexagons. Otherwise, you'd have a bunch of circles only.
    // Note that "h" is unique to each hexagon, so we can use it as the random ID.
    c = fract(cos(dot(h, vec2(41, 289)))*43758.5453); // Reusing "c."
    p -= p*step(c, .5)*2.; // Equivalent to: if (c<.5) p *= -1.;
    
    // Minimum squared distance to neighbors. Taking the square root after comparing, for speed.
    // Three partitions need to be checked due to the flipping process.
    p -= vec2(-1, 0);
    c = dot(p, p); // Reusing "c" again.
    p -= vec2(1.5, .8660254);
    c = min(c, dot(p, p));
    p -= vec2(0, -1.73205);
    c = min(c, dot(p, p));
    
    return sqrt(c);
    
    // Wrapping the values - or folding the values over (abs(c-.5)*2., cos(c*6.283*1.), etc) - to produce 
    // the nicely lined-up, wavy patterns. I"m perfoming this step in the "map" function. It has to do 
    // with coloring and so forth.
    //c = sqrt(c);
    //c = cos(c*6.283*1.) + cos(c*6.283*2.);
    //return (clamp(c*.6+.5, 0., 1.));

}

// Raymarching an XY-plane - raised a little by the hexagonal Truchet heightmap. Pretty standard.
float map(vec3 p){
    
    
    float c = heightMap(p.xy); // Height map.
    // Wrapping, or folding the height map values over, to produce the nicely lined-up, wavy patterns.
    c = cos(c*6.283*1.) + cos(c*6.283*2.);
    c = (clamp(c*.6+.5, 0., 1.));

    
    // Back plane, placed at vec3(0., 0., 1.), with plane normal vec3(0., 0., -1).
    // Adding some height to the plane from the heightmap. Not much else to it.
    return 1. - p.z - c*.025;

    
}

// The normal function with some edge detection and curvature rolled into it. Sometimes, it's possible to 
// get away with six taps, but we need a bit of epsilon value variance here, so there's an extra six.
vec3 getNormal(vec3 p, inout float edge, inout float crv) { 
    
    vec2 e = vec2(.01, 0); // Larger epsilon for greater sample spread, thus thicker edges.

    // Take some distance function measurements from either side of the hit point on all three axes.
    float d1 = map(p + e.xyy), d2 = map(p - e.xyy);
    float d3 = map(p + e.yxy), d4 = map(p - e.yxy);
    float d5 = map(p + e.yyx), d6 = map(p - e.yyx);
    float d = map(p)*2.;    // The hit point itself - Doubled to cut down on calculations. See below.
     
    // Edges - Take a geometry measurement from either side of the hit point. Average them, then see how
    // much the value differs from the hit point itself. Do this for X, Y and Z directions. Here, the sum
    // is used for the overall difference, but there are other ways. Note that it's mainly sharp surface 
    // curves that register a discernible difference.
    edge = abs(d1 + d2 - d) + abs(d3 + d4 - d) + abs(d5 + d6 - d);
    //edge = max(max(abs(d1 + d2 - d), abs(d3 + d4 - d)), abs(d5 + d6 - d)); // Etc.
    
    // Once you have an edge value, it needs to normalized, and smoothed if possible. How you 
    // do that is up to you. This is what I came up with for now, but I might tweak it later.
    edge = smoothstep(0., 1., sqrt(edge/e.x*2.));
    
    // We may as well use the six measurements to obtain a rough curvature value while we're at it.
    crv = clamp((d1 + d2 + d3 + d4 + d5 + d6 - d*3.)*32. + .6, 0., 1.);
    
    // Redoing the calculations for the normal with a more precise epsilon value.
    e = vec2(.0025, 0);
    d1 = map(p + e.xyy), d2 = map(p - e.xyy);
    d3 = map(p + e.yxy), d4 = map(p - e.yxy);
    d5 = map(p + e.yyx), d6 = map(p - e.yyx); 
    
    
    // Return the normal.
    // Standard, normalized gradient mearsurement.
    return normalize(vec3(d1 - d2, d3 - d4, d5 - d6));
}



// I keep a collection of occlusion routines... OK, that sounded really nerdy. :)
// Anyway, I like this one. I'm assuming it's based on IQ's original.
float calculateAO(in vec3 p, in vec3 n)
{
    float sca = 2., occ = 0.;
    for(float i=0.; i<5.; i++){
    
        float hr = .01 + i*.5/4.;        
        float dd = map(n * hr + p);
        occ += (hr - dd)*sca;
        sca *= 0.7;
    }
    return clamp(1.0 - occ, 0., 1.);    
}


/*
// Surface bump function. Cheap, but with decent visual impact.
float bumpSurf3D( in vec3 p){
    
    float c = heightMap((p.xy + p.z*.025)*6.);
    c = cos(c*6.283*3.);
    //c = sqrt(clamp(c+.5, 0., 1.));
    c = (c*.5 + .5);
    
    return c;

}

// Standard function-based bump mapping function.
vec3 dbF(in vec3 p, in vec3 nor, float bumpfactor){
    
    const vec2 e = vec2(0.001, 0);
    float ref = bumpSurf3D(p);                 
    vec3 grad = (vec3(bumpSurf3D(p - e.xyy),
                      bumpSurf3D(p - e.yxy),
                      bumpSurf3D(p - e.yyx) )-ref)/e.x;                     
          
    grad -= nor*dot(nor, grad);          
                      
    return normalize( nor + grad*bumpfactor );
    
}
*/

// Compact, self-contained version of IQ's 3D value noise function.
float n3D(vec3 p){
    
    const vec3 s = vec3(7, 157, 113);
    vec3 ip = floor(p); p -= ip; 
    vec4 h = vec4(0., s.yz, s.y + s.z) + dot(ip, s);
    p = p*p*(3. - 2.*p); //p *= p*p*(p*(p * 6. - 15.) + 10.);
    h = mix(fract(sin(h)*43758.5453), fract(sin(h + s.x)*43758.5453), p.x);
    h.xy = mix(h.xz, h.yw, p.y);
    return mix(h.x, h.y, p.z); // Range: [0, 1].
}

// Simple environment mapping. Pass the reflected vector in and create some
// colored noise with it. The normal is redundant here, but it can be used
// to pass into a 3D texture mapping function to produce some interesting
// environmental reflections.
vec3 envMap(vec3 rd, vec3 sn){
    
    vec3 sRd = rd; // Save rd, just for some mixing at the end.
    
    // Add a time component, scale, then pass into the noise function.
    rd.xy -= iTime*.25 *0.0125;
    rd *= 3.;
    
    float c = n3D(rd)*.57 + n3D(rd*2.)*.28 + n3D(rd*4.)*.15; // Noise value.
    c = smoothstep(0.4, 1., c); // Darken and add contast for more of a spotlight look.
    
    vec3 col = vec3(c, c*c, c*c*c*c); // Simple, warm coloring.
    //vec3 col = vec3(min(c*1.5, 1.), pow(c, 2.5), pow(c, 12.)); // More color.
    
    // Mix in some more red to tone it down and return.
    return mix(col, col.yzx, sRd*.25+.25); 
    
}



// vec2 to vec2 hash.
vec2 hash22(vec2 p) { 

    // Faster, but doesn't disperse things quite as nicely as other combinations. :)
    float n = sin(dot(p, vec2(41, 289)));
    return fract(vec2(262144, 32768)*n)*.75 + .25; 
    
    // Animated.
    //p = fract(vec2(262144, 32768)*n); 
    //return sin( p*6.2831853 + iTime *0.0125)*.35 + .65; 
    
}

// 2D 2nd-order Voronoi: Obviously, this is just a rehash of IQ's original. I've tidied
// up those if-statements. Since there's less writing, it should go faster. That's how 
// it works, right? :)
//
float Voronoi(in vec2 p){
    
    vec2 g = floor(p), o; p -= g;
    
    vec3 d = vec3(1); // 1.4, etc. "d.z" holds the distance comparison value.
    
    for(int y = -1; y <= 1; y++){
        for(int x = -1; x <= 1; x++){
            
            o = vec2(x, y);
            o += hash22(g + o) - p;
            
            d.z = dot(o, o); 
            // More distance metrics.
            //o = abs(o);
            //d.z = max(o.x*.8666 + o.y*.5, o.y);// 
            //d.z = max(o.x, o.y);
            //d.z = (o.x*.7 + o.y*.7);
            
            d.y = max(d.x, min(d.y, d.z));
            d.x = min(d.x, d.z); 
                       
        }
    }
    
    return max(d.y/1.2 - d.x*1., 0.)/1.2;
    //return d.y - d.x; // return 1.-d.x; // etc.
    
}



void main() {

	vec2 iResolution = vec2(1920.0, 1080.0);
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);



    // Unit directional ray - Coyote's observation.
    vec3 rd = normalize(vec3(2. * vec2(id) - iResolution.xy, iResolution.y));


    float tm = (iTime / 2.) * 0.0125;

    // Rotate the XY-plane back and forth. Note that sine and cosine are kind of rolled into one.
    vec2 a = sin(vec2(1.570796, 0) + sin(tm / 4.) * .3);

    // Fabrice's observation.
    rd.xy = mat2(a, -a.y, a.x) * rd.xy;

    // Ray origin. Moving in the X-direction to the right.
  //  vec3 ro = vec3(tm, cos(tm / 4.), 0.);
vec3 ro = vec3(0.0,0.0, 0.);
    // Light position, hovering around behind the camera.
    vec3 lp = ro + vec3(cos(tm / 2.) * .5, sin(tm / 2.) * .5, -.5);

    // Standard raymarching segment. Because of the straightforward setup, not many iterations are necessary.
    float d, t = 0.;
    for (int j = 0; j < 32; j++) {
        d = map(ro + rd * t); // distance to the function.
        t += d * .7;          // Total distance from the camera to the surface.

        // The plane "is" the far plane, so no far=plane break is needed.
        if (d < 0.001)
            break;
    }

    // Edge and curve value. Passed into, and set, during the normal calculation.
    float edge, crv;

    // Surface position, surface normal and light direction.
    vec3 sp = ro + rd * t;
    vec3 sn = getNormal(sp, edge, crv);
    vec3 ld = lp - sp;

    
    // Coloring and texturing the surface.
    //
    // Height map.
    float c = heightMap(sp.xy); 
    
    // Folding, or wrapping, the values above to produce the snake-like pattern that lines up with the randomly
    // flipped hex cells produced by the height map.
    vec3 fold = cos(vec3(1, 2, 4)*c*6.283);
    
    // Using the height map value, then wrapping it, to produce a finer grain Truchet pattern for the overlay.
    float c2 = heightMap((sp.xy + sp.z*.025)*6.);
    c2 = cos(c2*6.283*3.);
    c2 = (clamp(c2+.5, 0., 1.)); 

    
    // Function based bump mapping. I prefer none in this example, but it's there if you want it.   
    //if(temp.x>0. || temp.y>0.) sn = dbF(sp, sn, .001);
    
    // Surface color value.
    vec3 oC = vec3(1);

    if(fold.x>0.) oC = vec3(1, .05, .1)*c2; // Reddish pink with finer grained Truchet overlay.
    
    if(fold.x<0.05 && (fold.y)<0.) oC = vec3(1, .7, .45)*(c2*.25 + .75); // Lighter lined borders.
    else if(fold.x<0.) oC = vec3(1, .8, .4)*c2; // Gold, with overlay.
        
    //oC *= n3D(sp*128.)*.35 + .65; // Extra fine grained noisy texturing.

     
    // Sending some greenish particle pulses through the snake-like patterns. With all the shininess going 
    // on, this effect is a little on the subtle side.
    float p1 = 1.0 - smoothstep(0., .1, fold.x*.5+.5); // Restrict to the snake-like path.
    // Other path.
    //float p2 = 1.0 - smoothstep(0., .1, cos(heightMap(sp.xy + 1. + (iTime*0.0125)/4.)*6.283)*.5+.5);
    float p2 = 1.0 - smoothstep(0., .1, Voronoi(sp.xy*4. + vec2(tm, cos(tm/4.))));
    p1 = (p2 + .25)*p1; // Overlap the paths.
    oC += oC.yxz*p1*p1; // Gives a kind of electron effect. Works better with just Voronoi, but it'll do.
    
   
    
    
    float lDist = max(length(ld), 0.001); // Light distance.
    float atten = 1./(1. + lDist*.125); // Light attenuation.
    
    ld /= lDist; // Normalizing the light direction vector.
    
   
    float diff = max(dot(ld, sn), 0.); // Diffuse.
    float spec = pow(max( dot( reflect(-ld, sn), -rd ), 0.0 ), 16.); // Specular.
    float fre = pow(clamp(dot(sn, rd) + 1., .0, 1.), 3.); // Fresnel, for some mild glow.
    
    // Shading. Note, there are no actual shadows. The camera is front on, so the following
    // two functions are enough to give a shadowy appearance.
    crv = crv*.9 + .1; // Curvature value, to darken the crevices.
    float ao = calculateAO(sp, sn); // Ambient occlusion, for self shadowing.

 
    
    // Combining the terms above to light the texel.
 vec3 col = oC*(diff + .5) + vec3(1., .7, .4)*spec*2. + vec3(.4, .7, 1)*fre;

    
   col += (oC*.5+.5)*envMap(reflect(rd, sn), sn)*6.; // Fake environment mapping.
 
    
    // Edges.
    col *= 1. - edge*.85; // Darker edges.   
    
    // Applying the shades.
    col *= (atten*crv*ao);

col *= 2.0;
 // Rough gamma correction, then write to the output buffer.
    vec3 finalColor = sqrt(clamp(col, 0., 1.));
    ivec2 fragCoord = ivec2(id);  // Convert to ivec2
    imageStore(imageOut, fragCoord, vec4(finalColor, 1.0));  // Use vec4 instead of uvec4

}
)";



static const char *render_source_neon_pattern  =
    R"(

 #version 320 es
precision highp float;
precision highp image2D;

layout(binding = 0, rgba32f) uniform readonly image2D in_tex;
layout(binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

//uniform float current_time;  // Assuming this is the global time variable
layout(location = 9) uniform float current_time; 

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);
    return a + b * cos(6.28318 * (c * t + d));
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = vec2(pos) / vec2(800, 600);
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0);

    for (float i = 0.0; i < 4.0; i++) {
        uv = fract(uv * 1.5) - 0.5;

        float d = length(uv) * exp(-length(uv0));

        vec3 col = palette(length(uv0) + i * 0.4 + current_time * 0.000001);

        d = sin(d * 8.0 + current_time * 0.02) / 8.0;
        d = abs(d);

        d = pow(0.01 / d, 1.2);

        finalColor += col * d;
    }

    // Output the final color to the output texture
    imageStore(out_tex, pos, vec4(finalColor, 1.0));
}
)";



static const char *render_source_neon_rings  =
    R"(
#version 320 es
precision highp float;
precision highp image2D;

layout(binding = 0, rgba32f) uniform readonly image2D in_tex;
layout(binding = 0, rgba32f) uniform writeonly image2D out_tex;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(location = 3) uniform int width;
layout(location = 4) uniform int height;
layout(location = 9) uniform float current_time; // Animated time

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    // Number of circles
    int numCircles = 5;

    // Initialize final color
    vec3 finalColor = vec3(0.0);

    for (int i = 0; i < numCircles; ++i) {
        // Calculate UV coordinates for each circle
        vec2 uv = vec2(pos) / vec2(300, 300);

        // Calculate polar coordinates
        float a = atan(uv.y, uv.x);
        float r = length(uv) * (0.2 + 0.1 * float(i));

        // Map polar coordinates to UV
        uv = vec2(a / TWO_PI, r);

        // Apply horizontal movement based on time for each circle
        float xOffset = sin(current_time * 0.02 + float(i)) * 0.2;
        uv.x += xOffset;

        // Time-dependent color
        float xCol = (uv.x - (current_time / 300000.0)) * 3.0;
        xCol = mod(xCol, 3.0);
        vec3 horColour = vec3(0.25, 0.25, 0.25);

        if (xCol < 1.0) {
            horColour.r += 1.0 - xCol;
            horColour.g += xCol;
        } else if (xCol < 2.0) {
            xCol -= 1.0;
            horColour.g += 1.0 - xCol;
            horColour.b += xCol;
        } else {
            xCol -= 2.0;
            horColour.b += 1.0 - xCol;
            horColour.r += xCol;
        }

        // Draw color beam
        uv = (2.0 * uv) - 1.0;
        float beamWidth = (0.7 + 0.5 * cos(uv.x * 1.0 * TWO_PI * 0.15 * clamp(floor(5.0 + 1.0 * cos(current_time)), 0.0, 1.0))) * abs(1.0 / (30.0 * uv.y));
        vec3 horBeam = vec3(beamWidth);
        
        // Add the color for the current circle to the final color
        finalColor += ((horBeam) * horColour);
    }

    // Output the final color to the output texture
    imageStore(out_tex, pos, vec4(finalColor, 1.0));
}


)";


void setup_shader(GLuint *program, std::string source)
{
    auto compute_shader  = OpenGL::compile_shader(source.c_str(), GL_COMPUTE_SHADER);
    auto compute_program = GL_CALL(glCreateProgram());
    GL_CALL(glAttachShader(compute_program, compute_shader));
    GL_CALL(glLinkProgram(compute_program));

    int s = GL_FALSE;
#define LENGTH 1024 * 128
    char log[LENGTH];
    GL_CALL(glGetProgramiv(compute_program, GL_LINK_STATUS, &s));
    GL_CALL(glGetProgramInfoLog(compute_program, LENGTH, NULL, log));

    if (s == GL_FALSE)
    {
        LOGE("Failed to link shader:\n", source,
            "\nLinker output:\n", log);
    }

    GL_CALL(glDeleteShader(compute_shader));
    *program = compute_program;
}

static void seed_random()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srandom(ts.tv_nsec);
}
wf::option_wrapper_t<std::string> effect_type{"pixdecor/effect_type"};

smoke_t::smoke_t()
{
    OpenGL::render_begin();
    setup_shader(&motion_program, motion_source);
    setup_shader(&diffuse1_program, diffuse1_source);
    setup_shader(&diffuse2_program, diffuse2_source);
    setup_shader(&project1_program, project1_source);
    setup_shader(&project2_program, project2_source);
    setup_shader(&project3_program, project3_source);
    setup_shader(&project4_program, project4_source);
    setup_shader(&project5_program, project5_source);
    setup_shader(&project6_program, project6_source);
    setup_shader(&advect1_program, advect1_source);
    setup_shader(&advect2_program, advect2_source);

    setup_shader(&render_program, render_source);
    
 	if (std::string(effect_type) == "clouds")
 	{
   	setup_shader(&render_program, render_source_clouds);
	}
	else if (std::string(effect_type) == "halftone")
 	{
   	setup_shader(&render_program, render_source_halftone);
	}
	else if (std::string(effect_type) == "pattern")
 	{
   	setup_shader(&render_program, render_source_pattern);
	}
	else if (std::string(effect_type) == "lava")
 	{
   	setup_shader(&render_program, render_source_lava);
	}
	else if (std::string(effect_type) == "hex")
 	{
   	setup_shader(&render_program, render_source_hex);
	}
	else if (std::string(effect_type) == "zebra")
 	{
   	setup_shader(&render_program, render_source_zebra);
	}
	else if (std::string(effect_type) == "neural_network")
 	{
   	setup_shader(&render_program, render_source_neural_network);
	}
	else if (std::string(effect_type) == "hexagon_maze")
 	{
   	setup_shader(&render_program, render_source_hexagon_maze);
	}
	else if (std::string(effect_type) == "raymarched_truchet")
 	{
   	setup_shader(&render_program, render_source_raymarched_truchet);
	}
	else if (std::string(effect_type) == "neon_pattern")
 	{
   	setup_shader(&render_program, render_source_neon_pattern );
	}
	else if (std::string(effect_type) == "neon_rings")
 	{
   	setup_shader(&render_program, render_source_neon_rings );
	}

 
    texture = b0u = b0v = b0d = b1u = b1v = b1d = GLuint(-1);
    OpenGL::render_end();
    seed_random();
}

smoke_t::~smoke_t()
{
    GL_CALL(glDeleteProgram(motion_program));
    GL_CALL(glDeleteProgram(diffuse1_program));
    GL_CALL(glDeleteProgram(diffuse2_program));
    GL_CALL(glDeleteProgram(project1_program));
    GL_CALL(glDeleteProgram(project2_program));
    GL_CALL(glDeleteProgram(project3_program));
    GL_CALL(glDeleteProgram(project4_program));
    GL_CALL(glDeleteProgram(project5_program));
    GL_CALL(glDeleteProgram(project6_program));
    GL_CALL(glDeleteProgram(advect1_program));
    GL_CALL(glDeleteProgram(advect2_program));

    GL_CALL(glDeleteProgram(render_program));
    
    destroy_textures();
}

void smoke_t::create_textures()
{
    GL_CALL(glGenTextures(1, &texture));
    GL_CALL(glGenTextures(1, &b0u));
    GL_CALL(glGenTextures(1, &b0v));
    GL_CALL(glGenTextures(1, &b0d));
    GL_CALL(glGenTextures(1, &b1u));
    GL_CALL(glGenTextures(1, &b1v));
    GL_CALL(glGenTextures(1, &b1d));
}

void smoke_t::destroy_textures()
{
    if (texture == GLuint(-1))
    {
        return;
    }

    GL_CALL(glDeleteTextures(1, &texture));
    GL_CALL(glDeleteTextures(1, &b0u));
    GL_CALL(glDeleteTextures(1, &b0v));
    GL_CALL(glDeleteTextures(1, &b0d));
    GL_CALL(glDeleteTextures(1, &b1u));
    GL_CALL(glDeleteTextures(1, &b1v));
    GL_CALL(glDeleteTextures(1, &b1d));
}

void smoke_t::run_shader(GLuint program, int width, int height, int title_height, int border_size)
{
    GL_CALL(glUseProgram(program));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 1));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b0u));
    GL_CALL(glBindImageTexture(1, b0u, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 2));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b0v));
    GL_CALL(glBindImageTexture(2, b0v, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 3));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b0d));
    GL_CALL(glBindImageTexture(3, b0d, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 4));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b1u));
    GL_CALL(glBindImageTexture(4, b1u, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 5));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b1v));
    GL_CALL(glBindImageTexture(5, b1v, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 6));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b1d));
    GL_CALL(glBindImageTexture(6, b1d, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GL_CALL(glUniform1i(1, title_height + border_size * 2));
    GL_CALL(glUniform1i(2, border_size * 2));
    GL_CALL(glUniform1i(5, width));
    GL_CALL(glUniform1i(6, height));
    GL_CALL(glDispatchCompute(width / 15, height / 15, 1));
    GL_CALL(glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT));
}

void smoke_t::step_effect(const wf::render_target_t& fb, wf::geometry_t rectangle,
    bool ink,  wf::pointf_t p, wf::color_t decor_color, wf::color_t effect_color,
    int title_height, int border_size, int diffuse_iterations)
{
    if ((rectangle.width <= 0) || (rectangle.height <= 0))
    {
        return;
    }

    OpenGL::render_begin(fb);
    if ((rectangle.width != saved_width) || (rectangle.height != saved_height))
    {
        LOGI("recreating smoke textures: ", rectangle.width, " != ", saved_width, " || ", rectangle.height,
            " != ", saved_height);
        saved_width  = rectangle.width;
        saved_height = rectangle.height;

        destroy_textures();
        create_textures();

        std::vector<GLfloat> clear_data(rectangle.width * rectangle.height * 4, 0);

        GL_CALL(glActiveTexture(GL_TEXTURE0 + 0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, rectangle.width, rectangle.height));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rectangle.width, rectangle.height, GL_RGBA, GL_FLOAT,
            &clear_data[0]);
        GL_CALL(glActiveTexture(GL_TEXTURE0 + 1));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, b0u));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, rectangle.width, rectangle.height));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rectangle.width, rectangle.height, GL_RED, GL_FLOAT,
            &clear_data[0]);
        GL_CALL(glActiveTexture(GL_TEXTURE0 + 2));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, b0v));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, rectangle.width, rectangle.height));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rectangle.width, rectangle.height, GL_RED, GL_FLOAT,
            &clear_data[0]);
        GL_CALL(glActiveTexture(GL_TEXTURE0 + 3));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, b0d));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, rectangle.width, rectangle.height));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rectangle.width, rectangle.height, GL_RED, GL_FLOAT,
            &clear_data[0]);
        GL_CALL(glActiveTexture(GL_TEXTURE0 + 4));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, b1u));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, rectangle.width, rectangle.height));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rectangle.width, rectangle.height, GL_RED, GL_FLOAT,
            &clear_data[0]);
        GL_CALL(glActiveTexture(GL_TEXTURE0 + 5));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, b1v));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, rectangle.width, rectangle.height));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rectangle.width, rectangle.height, GL_RED, GL_FLOAT,
            &clear_data[0]);
        GL_CALL(glActiveTexture(GL_TEXTURE0 + 6));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, b1d));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, rectangle.width, rectangle.height));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rectangle.width, rectangle.height, GL_RED, GL_FLOAT,
            &clear_data[0]);
    }

    GL_CALL(glUseProgram(motion_program));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 1));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b0u));
    GL_CALL(glBindImageTexture(1, b0u, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 2));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b0v));
    GL_CALL(glBindImageTexture(2, b0v, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 3));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b0d));
    GL_CALL(glBindImageTexture(3, b0d, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    wf::point_t point{int(p.x), int(p.y)};
    // upload stuff
    GL_CALL(glUniform1i(1, title_height + border_size));
    GL_CALL(glUniform1i(2, border_size));
    GL_CALL(glUniform1i(3, point.x));
    GL_CALL(glUniform1i(4, point.y));
    GL_CALL(glUniform1i(5, rectangle.width));
    GL_CALL(glUniform1i(6, rectangle.height));
    GL_CALL(glUniform1i(7, random()));
    GL_CALL(glUniform1i(8, random()));
    GL_CALL(glDispatchCompute(rectangle.width / 15, rectangle.height / 15, 1));
    GL_CALL(glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT));
    for (int k = 0; k < diffuse_iterations; k++)
    {
        run_shader(diffuse1_program, rectangle.width, rectangle.height, title_height, border_size);
    }

    run_shader(project1_program, rectangle.width, rectangle.height, title_height, border_size);
    for (int k = 0; k < diffuse_iterations; k++)
    {
        run_shader(project2_program, rectangle.width, rectangle.height, title_height, border_size);
    }

    run_shader(project3_program, rectangle.width, rectangle.height, title_height, border_size);
    run_shader(advect1_program, rectangle.width, rectangle.height, title_height, border_size);
    run_shader(project4_program, rectangle.width, rectangle.height, title_height, border_size);
    for (int k = 0; k < diffuse_iterations; k++)
    {
        run_shader(project5_program, rectangle.width, rectangle.height, title_height, border_size);
    }

    run_shader(project6_program, rectangle.width, rectangle.height, title_height, border_size);
    for (int k = 0; k < diffuse_iterations; k++)
    {
        run_shader(diffuse2_program, rectangle.width, rectangle.height, title_height, border_size);
    }

    run_shader(advect2_program, rectangle.width, rectangle.height, title_height, border_size);
   
    GL_CALL(glUseProgram(render_program));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
    GL_CALL(glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 3));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, b0d));
    GL_CALL(glBindImageTexture(3, b0d, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));
    GLfloat effect_color_f[4] =
    {GLfloat(effect_color.r), GLfloat(effect_color.g), GLfloat(effect_color.b),
        GLfloat(effect_color.a)};
    GLfloat decor_color_f[4] =
    {GLfloat(decor_color.r), GLfloat(decor_color.g), GLfloat(decor_color.b), GLfloat(decor_color.a)};
    GL_CALL(glUniform1i(1, title_height + border_size * 2));
    GL_CALL(glUniform1i(2, border_size * 2));
    GL_CALL(glUniform1i(4, ink));
    GL_CALL(glUniform1i(5, rectangle.width));
    GL_CALL(glUniform1i(6, rectangle.height));
    GL_CALL(glUniform4fv(7, 1, effect_color_f));
    GL_CALL(glUniform4fv(8, 1, decor_color_f));
    GL_CALL(glUniform1f(9, wf::get_current_time() / 30.0));

    GL_CALL(glDispatchCompute(rectangle.width / 15, rectangle.height / 15, 1));
    GL_CALL(glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 1));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 2));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 3));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 4));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 5));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 6));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glUseProgram(0));
    OpenGL::render_end();
}

void smoke_t::render_effect(const wf::render_target_t& fb, wf::geometry_t rectangle,
    const wf::geometry_t& scissor)
{
    OpenGL::render_begin(fb);
    fb.logic_scissor(scissor);
    OpenGL::render_transformed_texture(wf::texture_t{texture}, rectangle,
        fb.get_orthographic_projection(), glm::vec4{1}, OpenGL::TEXTURE_TRANSFORM_INVERT_Y);
    OpenGL::render_end();
}
}
}
