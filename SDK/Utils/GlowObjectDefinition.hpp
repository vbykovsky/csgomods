#pragma once

#include "ColorRGBA.hpp"

enum struct en_glow_style: int {
	Invalid = -1,
	FullBody,
	InlineFlicker,
	Inline,
	Flicker,
	InvalidLast
};

struct s_glow_object_definition {
	int									next_free_slot;									//0x0000
	unsigned int				base_entity;										//0x0004
	float								g_color[4];											//0x0008
	unsigned long				_pad_0x0014;										//0x0018 
	float								_pad_0x0018;										//0x001C 
	float								bloom_amount;										//0x0020
	float								noise;													//0x0024
	bool								render_when_occluded;						//0x0028
	bool								render_when_unoccluded;					//0x0029
	bool								full_bloom_render;							//0x002A
	unsigned char				_pad_0x0027[0x1];								//0x002B
	int									full_bloom_stencil_test_value;	//0x002C
	int									style;													//0x0030
	int									split_screen_slot;							//0x0034

public:
	void color(float r, float g, float b, float a) {
		g_color[0] = r / 255.f;
		g_color[1] = g / 255.f;
		g_color[2] = b / 255.f;
		g_color[3] = a / 255.f;
	}
	void color(s_color_rgba color) {
		g_color[0] = color.r / 255.f;
		g_color[1] = color.g / 255.f;
		g_color[2] = color.b / 255.f;
		g_color[3] = color.a / 255.f;
	}
};