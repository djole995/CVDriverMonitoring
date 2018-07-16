#define MAKE_GLSRC(src) "#version 300 es\n"#src

static GLbyte vShaderDiffuse[] = MAKE_GLSRC(
    in vec4 aPosition;
    void main()
    {
        gl_Position = aPosition;
    }

);

static GLbyte fShaderDiffuse[] = MAKE_GLSRC(
    precision mediump float;
    uniform vec3 uDiffuse;
    out vec4 fragColor;
    void main()
    {
        fragColor = vec4(uDiffuse, 1.0f);
    }
);


static GLbyte vShaderTexture[] = MAKE_GLSRC(
    in vec4 aPosition;
    in vec2 aTexCoordinate;
    out vec2 vTexCoordinate;
    void main()
    {
        vTexCoordinate = vec2(aTexCoordinate.x, aTexCoordinate.y);
        gl_Position = aPosition;
    }
);


static GLbyte fShaderTextureBGR[] = MAKE_GLSRC(
    precision mediump float;
    precision lowp sampler2D;
    uniform sampler2D uTexture;
    in vec2 vTexCoordinate;
    out vec4 fragColor;
    void main()
    {
        vec4 color = texture(uTexture, vTexCoordinate).bgra;
        fragColor = color;
    }
);

static GLbyte fShaderTextureUYVY[] = MAKE_GLSRC(
    precision highp float;
    in vec2 vTexCoordinate;
    out vec4 color;
    uniform sampler2D uTexture;
    void main()
    {
        float odd = mod(floor(gl_FragCoord.x), 2.0f);
        float y = 0.0f;
        float u = 0.0f;
        float v = 0.0f;
        vec2 pos = vec2(vTexCoordinate.x, vTexCoordinate.y);
        vec4 samp = texture(uTexture, pos).rgba;
        u = samp.r - 0.5f;
        v = samp.b - 0.5f;
        if (odd == 0.0f)
        {
            y = samp.g;
        }
        else
        {
            y = samp.a;
        }

        float r = clamp((y + (1.402f * v)), 0.0f, 1.0f);
        float g = clamp((y - (0.344f * u) - (0.714f * v)), 0.0f, 1.0f);
        float b = clamp((y + (1.772f * u)), 0.0f, 1.0f);
        color = vec4(r, g, b, 1.0f);
    }
);
