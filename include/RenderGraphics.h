/***************************************************************************
 *
 *
 * Copyright (c) 2017 RT-RK
 * Bulevar Narodnog Fronta 23a, Novi Sad, Serbia
 *
 * All Rights Reserved
 *
 * P R O P R I E T A R Y    &    C O N F I D E N T I A L
 *
 * -----------------------------------------------------
 * http://www.rt-rk.com
 * -----------------------------------------------------
 *
 *
 *
 * Created on 22.11.2017.
 *
 * @author Milan Djokic
 *
 *
 ************************************************************************/
/**
* @file draw_output.h
*
* @brief OpenGL API for rendering rectangles over image texture on screen with 1280x720 resolution.
*
* @note (c) 2017 RT-RK All rights reserved.
*
* @author Milan Djokic
*
*/

#ifndef _DRAW_OUTPUT_H_
#define _DRAW_OUTPUT_H_


/**
 * @brief structure which describes vertices in 3D space.
*/
typedef struct Vertex
{
    float x, y, z;
} Vertex, V_XYZ;

/**
 * @brief structure which describes vertices in 3D space and their texture coordinates.
*/
typedef struct VertexTex
{
    float x, y, z;
    float u, v;
} VertexTex, V_XYZ_UV;

/**
 * @brief structure which describes vertices in 3D space and their diffuse color RGB components.
*/
typedef struct V_XYZ_DIFFUSE
{
    float x, y, z;
    float r, g, b;
} V_XYZ_DIFFUSE;

/**
 * @brief structure which describes rectangle which contains its vertices color and whether it is filled with color or not.
*/
typedef struct tRectangle
{
    V_XYZ vertices[4];
    float r, g, b;
    bool color_fill;
} Rectangle;

/**
 * @brief Create window, connect EGL with Wayland and init rendering parameters.
 *
 * @details When window is created, shaders and textures are loaded.
 * Window size is calculated inside init function and it will be screen_width / 2 * screen_height / 2 size.
 *
 * @param _frame_width camera frame width
 * @param _frame_height camera frame height
*/
void initRenderTarget(int _frame_width, int _frame_height);

/**
 * @brief Render graphics with input image and rectangles parameters.
 *
 * @details This function is periodically called to refresh screen pixels.
 *
 * @param rectangles Array of rectangle structs which will be drawn on screen over image frame.
 * @param count Size of rectanles array.
 * @param original_frame UYVY image which will be drawn on screen (conversion from YUV to RGB is done in pixel shader).
*/
void render(Rectangle* rectangles, unsigned int count, unsigned char *original_frame);

#endif // _DRAW_OUTPUT_H_
