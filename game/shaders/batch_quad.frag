#version 330 core
out vec4 o_color;

in vec4 v_color;
in vec2 v_uvs;
flat in int v_texture_slot;

uniform sampler2D texture_slot_0;
uniform sampler2D texture_slot_1;
uniform sampler2D texture_slot_2;
uniform sampler2D texture_slot_3;
uniform sampler2D texture_slot_4;
uniform sampler2D texture_slot_5;
uniform sampler2D texture_slot_6;
uniform sampler2D texture_slot_7;
uniform sampler2D texture_slot_8;
uniform sampler2D texture_slot_9;

void main() {
    switch (v_texture_slot) {
        case 0: o_color = texture(texture_slot_0, v_uvs) * v_color; break;
        case 1: o_color = texture(texture_slot_1, v_uvs) * v_color; break;
        case 2: o_color = texture(texture_slot_2, v_uvs) * v_color; break;
        case 3: o_color = texture(texture_slot_3, v_uvs) * v_color; break;
        case 4: o_color = texture(texture_slot_4, v_uvs) * v_color; break;
        case 5: o_color = texture(texture_slot_5, v_uvs) * v_color; break;
        case 6: o_color = texture(texture_slot_6, v_uvs) * v_color; break;
        case 7: o_color = texture(texture_slot_7, v_uvs) * v_color; break;
        case 8: o_color = texture(texture_slot_8, v_uvs) * v_color; break;
        case 9: o_color = texture(texture_slot_9, v_uvs) * v_color; break;
    }
    
    // Material material = materials[material_idx];

    // if (bool(renderOptions & RENDERING_OPTION_FONT)) {
    //     vec4 textureColor = texelFetch(fontAtlas, vec2(textureCoordsIn), 0);

    //     if (textureColor[0] == 0.0) {
    //         discard;
    //     }

    //     fragColor = textureColor[0] * material.color;
    // } else {
    //     vec4 textureColor = texelFetch(textureAtlas, vec2(textureCoordsIn), 0);

    //     if (textureColor[3] == 0.0) {
    //         discard;
    //     }

    //     fragColor = textureColor * material.color;
    // }
}
