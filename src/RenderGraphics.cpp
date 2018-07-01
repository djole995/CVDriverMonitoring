#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "DisplayFactory.h"
#include "RenderGraphics.h"
#include "Shaders.h"

static std::unique_ptr<EGLWindow> egl_window;
static int window_width;
static int window_height;

static int frame_width;
static int frame_height;

static GLushort indicies_rectangle[] = { 0, 1, 2, 0, 2, 3};
static float image_texture_coords[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};
static GLuint img_texture;

static V_XYZ_UV frame_rect[] =
{
    {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
    { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    { 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
    { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
    { 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
    { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f}
};

// Shader program used for drawing rectangles in RGB color format
static GLuint programObjectDiffuse;
// Shader program used for drawing frames in YUV422 format (UYVY)
static GLuint programObjectTexture;

// Convert camera frame coordinates to GL world coordinates in -1 to 1 range
static void screenToWorld(V_XYZ& vertex)
{
    vertex.x = 2.0f * (float)vertex.x / ((float)(frame_width)) - 1.0f;
    vertex.y = 1.0f - 2.0f * (float)vertex.y / ((float)(frame_height));
}

static bool initImageTexture(unsigned int textureUnit)
{
    // Set the unique texture unit in which to store the data.
    glActiveTexture(GL_TEXTURE0 + textureUnit);

    // Generate an ID for the texture.
    glGenTextures(1, &img_texture);

    // Bind the texture as a 2D texture.
    glBindTexture(GL_TEXTURE_2D, img_texture);

    // Load the image data into the texture unit.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width / 2, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);


    // Set the texture color to either wrap around or clamp to the edge.

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // Set the texture filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Generate mipmaps for the texture.
    glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}

/* Create EGL window using Display utils API. */
static void initWindow()
{
    std::unique_ptr<GenericDisplay> wayland_display = DisplayFactory::createDisplay(DisplayFactory::DisplayBackend::Wayland);

    // Creating Wayland window
    wayland_display->setWidth(wayland_display->getMaxWidth() / 2);
    wayland_display->setHeight(wayland_display->getMaxHeight() / 2);
    wayland_display->setPos(0, 0);

    // Set window width and height
    window_height = wayland_display->getMaxHeight() / 2;
    window_width = wayland_display->getMaxWidth() / 2;

    wayland_display->createEGLWindow();

    // Config for EGL. It will try to find closest from that platfrom
    std::vector< EGLint > config =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    // Connecting native window to EGL
    egl_window = std::make_unique<EGLWindow>(wayland_display, 3, config);
}

static GLuint LoadShader(GLenum type, const char* shaderSrc)
{
    GLuint shader;
    GLint  compiled;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
        return 0;

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * (size_t) infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            fprintf(stderr, "Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static int initShaders()
{

    GLuint vertexShaderDiffuse;
    GLuint fragmentShaderDiffuse;
    GLuint vertexShaderTexture;
    GLuint fragmentShaderTexture;
    GLint  linked;

    // Load the vertex/fragment shaders
    vertexShaderDiffuse   = LoadShader( GL_VERTEX_SHADER, ( const char* ) vShaderDiffuse );
    fragmentShaderDiffuse = LoadShader( GL_FRAGMENT_SHADER, ( const char* ) fShaderDiffuse );

    vertexShaderTexture   = LoadShader( GL_VERTEX_SHADER, ( const char* ) vShaderTexture );
    fragmentShaderTexture = LoadShader( GL_FRAGMENT_SHADER, ( const char* ) fShaderTexture );

    // Create the program object
    programObjectDiffuse = glCreateProgram( );
    programObjectTexture = glCreateProgram( );

    if ( programObjectDiffuse == 0 || programObjectTexture == 0)
    {
        return 0;
    }

    glAttachShader( programObjectDiffuse, vertexShaderDiffuse );
    glAttachShader( programObjectDiffuse, fragmentShaderDiffuse );

    glAttachShader( programObjectTexture, vertexShaderTexture );
    glAttachShader( programObjectTexture, fragmentShaderTexture );

    // Bind vPosition to attribute 0
    glBindAttribLocation( programObjectDiffuse, 0, "aPosition" );

    // Bind vPosition to attribute 0
    glBindAttribLocation( programObjectTexture, 0, "aPosition" );
    glBindAttribLocation( programObjectTexture, 1, "aTexCoordinate" );

    // Link the program
    glLinkProgram( programObjectDiffuse );
    glLinkProgram( programObjectTexture );

    // Check the link status
    glGetProgramiv( programObjectDiffuse, GL_LINK_STATUS, &linked );

    if ( !linked )
    {
        GLint infoLen = 0;

        glGetProgramiv( programObjectDiffuse, GL_INFO_LOG_LENGTH, &infoLen );

        if ( infoLen > 1 )
        {
            char* infoLog = (char*)malloc( sizeof( char ) * ( size_t ) infoLen );

            glGetProgramInfoLog( programObjectDiffuse, infoLen, NULL, infoLog );
            fprintf( stderr, "Error linking program:\n%s\n", infoLog );

            free( infoLog );
        }

        glDeleteProgram( programObjectDiffuse );
        return GL_FALSE;
    }


    glGetProgramiv( programObjectTexture, GL_LINK_STATUS, &linked );

    if ( !linked )
    {
        GLint infoLen = 0;

        glGetProgramiv( programObjectTexture, GL_INFO_LOG_LENGTH, &infoLen );

        if ( infoLen > 1 )
        {
            char* infoLog = (char*)malloc( sizeof( char ) * ( size_t ) infoLen );

            glGetProgramInfoLog( programObjectTexture, infoLen, NULL, infoLog );
            fprintf( stderr, "Error linking program:\n%s\n", infoLog );

            free( infoLog );
        }

        glDeleteProgram( programObjectTexture );
        return GL_FALSE;
    }

    return GL_TRUE;
}

void initRenderTarget(int _frame_width, int _frame_height)
{
    // Set intern frame size which is used in rendering and texture creating
    frame_width = _frame_width;
    frame_height = _frame_height;

    // Crete window
    initWindow();

    // Load shaders used in program
    initShaders();

    // Create frame texture
    initImageTexture(0);

    // Set the viewport
    glViewport(0, 0, window_width, window_height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void render(Rectangle* user_rectangles, unsigned int count, unsigned char *original_img)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Bind the texture as a 2D texture.
    glBindTexture(GL_TEXTURE_2D, img_texture);
    // Load the image data into the texture unit.
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame_width / 2, frame_height, GL_RGBA, GL_UNSIGNED_BYTE, original_img);

    glUseProgram(programObjectTexture);

    GLint addr;
    addr = glGetUniformLocation(programObjectTexture, "uTexture");
    if(addr == -1)
    {
        std::cout << "Failed to get uniform variable address! Exiting rendering." << std::endl;
        return;
    }

    glUniform1i(addr, 0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V_XYZ_UV), frame_rect);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, image_texture_coords);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_TRIANGLES, 3, 3);

    glDisableVertexAttribArray(1);

    glUseProgram(programObjectDiffuse);

    for(unsigned int i = 0; i < count; i++)
    {
        screenToWorld(user_rectangles[i].vertices[0]);
        screenToWorld(user_rectangles[i].vertices[1]);
        screenToWorld(user_rectangles[i].vertices[2]);
        screenToWorld(user_rectangles[i].vertices[3]);

        // Load the vertex data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, user_rectangles[i].vertices);
        glEnableVertexAttribArray(0);

        addr = glGetUniformLocation(programObjectDiffuse, "uDiffuse");
        if(addr == -1)
        {
            std::cout << "Failed to get uniform variable address! Exiting rendering." << std::endl;
            return;
        }
        glUniform3f(addr, user_rectangles[i].r, user_rectangles[i].g, user_rectangles[i].b);

        if(!user_rectangles[i].color_fill)
        {
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        else
        {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indicies_rectangle);
        }
    }

    eglSwapBuffers(egl_window->getDisplay(), egl_window->getSurface());
}
