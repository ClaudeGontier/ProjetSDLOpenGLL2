#include "../types.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "../math/math_util.h"
#include "../io/model.h"
#include "framebuffer.h"
#include "../node.h"
#include "../window.h"
#include "color.h"
#include "camera.h"
#include "depth_map.h"
#include "viewport.h"
#include "../io/gltexture_loader.h"

/**
 * Configures the shaders for rendering the scene with the provided lighting and camera settings.
 * 
 * @param window {Window*} Pointer to the Window structure that contains display settings and context.
 * @param root {Node*} Pointer to the root node of the scene graph which contains all the drawable objects.
 * @param c {Camera*} Pointer to the Camera structure that provides the current view and projection settings.
 * @param shaders {Shader[]} Array of Shader structures that hold the shader programs to be configured.
 * 
 * This function sets up the global lighting configurations for the scene by calling the 
 * `configure_global_lighting` function. It ensures that the shaders are correctly prepared 
 * to render the scene based on the current state of the window, the root of the scene graph, 
 * and the camera's properties.
 */

void configure_shader(Window *window, Node *root, Camera *c, Shader shaders[]) {
    configure_global_lighting(window,root,c,shaders);
}

/**
 * Renders the scene recursively starting from the specified node, applying transformations and shaders.
 * 
 * @param window {Window*} Pointer to the Window structure that holds the current rendering context.
 * @param node {Node*} Pointer to the Node structure that represents the current node to render.
 * @param c {Camera*} Pointer to the Camera structure that defines the view and projection settings.
 * @param shader {Shader} The shader program to be used for rendering the current node.
 * @param shaders {Shader[]} Array of Shader structures that contain additional shaders for rendering.
 * @param modelMatrix {mat4} The transformation matrix applied to the current node to compute its final position and orientation.
 * 
 * This function checks if the current node is visible based on its flags. If the node is visible, 
 * it calculates the model matrix for the node by multiplying the current model matrix with the node's 
 * transformation matrix. It then calls `render_node` to render the current node with the specified shaders 
 * and model matrix. The function continues recursively to render all child nodes of the current node 
 * by calling itself with each child node, thus constructing the complete scene hierarchy.
 */

void render_scene(Window *window, Node *node, Camera *c, Shader shader, Shader shaders[], mat4 modelMatrix) {

    if (node->flags & NODE_VISIBLE) {
        mat4 nodeModelMatrix = GLM_MAT4_IDENTITY_INIT;
        glm_mat4_mul(nodeModelMatrix, modelMatrix, nodeModelMatrix);
        render_node(node, shaders, nodeModelMatrix);
        for (int i = 0; i < node->length; i++) {
            render_scene(window, node->children[i], c, shader, shaders, nodeModelMatrix);
        }
    }

}

/**
 * Renders the shadow map for the scene by drawing it from the perspective of the light source.
 * 
 * @param window {Window*} Pointer to the Window structure that holds the current rendering context.
 * @param root {Node*} Pointer to the root Node structure that represents the entire scene graph.
 * @param c {Camera*} Pointer to the Camera structure that defines the view settings for rendering the shadow map.
 * @param shaders {Shader[]} Array of Shader structures containing shaders used for rendering the scene.
 * @param depthMap {DepthMap*} Pointer to the DepthMap structure that stores the framebuffer and depth texture for shadows.
 * 
 * This function sets the viewport to the dimensions of the shadow map and binds the framebuffer 
 * associated with the depth map. It clears the depth buffer to prepare for rendering the shadow map. 
 * The culling face is disabled to ensure all geometry is rendered, regardless of orientation.
 * 
 * The scene is then rendered using the shadow shader to populate the depth map with depth information 
 * from the perspective of the light source. Finally, it restores the default framebuffer and 
 * re-enables face culling.
 */

void draw_shadow_map(Window *window, Node *root, Camera *c, Shader shaders[], DepthMap *depthMap) {
    // Draw shadow map (render scene with depth map shader)
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMap->frameBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
    render_scene(window, root, c, shaders[SHADER_SHADOW],shaders,modelMatrix);

    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * Renders the entire scene using the specified shaders and configurations.
 * 
 * @param window {Window*} Pointer to the Window structure that holds the current rendering context.
 * @param root {Node*} Pointer to the root Node structure representing the scene graph.
 * @param c {Camera*} Pointer to the Camera structure that defines the view settings for rendering the scene.
 * @param shaders {Shader[]} Array of Shader structures containing shaders used for rendering the scene.
 * @param depthMap {DepthMap*} Pointer to the DepthMap structure that holds the depth texture for shadow mapping.
 * 
 * This function sets the viewport to the dimensions of the screen and clears the color and depth buffers 
 * to prepare for rendering. It then configures the shaders based on the current scene settings and binds 
 * the depth texture used for shadows. The function sets the appropriate texture units for the shader 
 * program and renders the scene using the classic lighting shader. 
 * The scene is rendered recursively from the root node, ensuring all visible objects are drawn.
 */

void draw_scene(Window *window, Node *root, Camera *c, Shader shaders[], DepthMap *depthMap) {

    // Draw scene (render scene with scene shader)
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    configure_shader(window, root, c, shaders);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMap->texture);
    set_shader_int(shaders[SHADER_CLASSIC_LIGHTING], "diffuseMap", 0);
    set_shader_int(shaders[SHADER_CLASSIC_LIGHTING], "normalMap", 1);
    set_shader_int(shaders[SHADER_CLASSIC_LIGHTING], "displacementMap", 2);
    set_shader_int(shaders[SHADER_CLASSIC_LIGHTING], "shadowMap", 3);

    mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
    render_scene(window, root, c, shaders[SHADER_CLASSIC_LIGHTING],shaders,modelMatrix);
}

/**
 * Renders the scene onto the screen with optional post-processing effects.
 * 
 * @param window {Window*} Pointer to the Window structure that holds the current rendering context.
 * @param viewportNode {Node*} Pointer to the Node structure representing the viewport for the scene.
 * @param c {Camera*} Pointer to the Camera structure that defines the view settings for rendering.
 * @param shaders {Shader[]} Array of Shader structures containing shaders used for rendering the scene.
 * @param depthMap {DepthMap*} Pointer to the DepthMap structure that holds the depth texture for shadow mapping.
 * 
 * This function initializes the viewport and sets up the camera projection for rendering. It configures 
 * the directional lighting and draws the shadow map before rendering the main scene. If there are 
 * active filters associated with the viewport, it applies these filters and renders the scene accordingly. 
 * The rendered scene is then drawn onto a quad in screen space using anti-aliasing techniques, if applicable. 
 * If no filters are active, the scene is drawn directly.
 */

void draw_screen(Window *window, Node *viewportNode, Camera *c, Shader shaders[], DepthMap *depthMap) {

    
    Viewport *viewport = (Viewport *) viewportNode->object;
    Node *scene = viewport->scene;
    TextureMap screenTexture;
    glGenTextures(1, &screenTexture);
    init_viewport();

    camera_projection(c,shaders);
    configure_directional_lighting(window,scene,c,shaders,depthMap);
    draw_shadow_map(window,scene,c,shaders,depthMap);
    

    if (viewportNode->length) {
        for (int i = 0; i < viewportNode->length; i++) {
            const Filter *filter = (Filter *) viewportNode->children[i]->object;
            if (filter->flags & FILTER_ACTIVE) {
                filter->beginScript(viewportNode->children[i], &screenTexture);
                if (filter->flags & FILTER_NEED_RENDER) draw_scene(window,scene,c,shaders,depthMap);
                filter->endScript(viewportNode->children[i], &screenTexture);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // Draw screen (render scene in the screen plane)
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        use_shader(shaders[SHADER_AA]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture); // use the now resolved color attachment as the quad's texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, window->ui_texture); // use the now resolved color attachment as the quad's texture
        set_shader_int(shaders[SHADER_AA], "screenTexture", 0);
        set_shader_int(shaders[SHADER_AA], "UITexture", 1);
        glUniform1fv(glGetUniformLocation(shaders[SHADER_AA], "iTime"), 1, &window->time);
        glUniform2fv(glGetUniformLocation(shaders[SHADER_AA], "iResolution"), 1, (vec2){1, 1});

        mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
        render_mesh(viewport->screenPlane, modelMatrix);
        
    } else draw_scene(window,scene,c,shaders,depthMap);
}