#include "camera.h"


/**
 * Initializes a Camera structure by setting its position, direction,
 * rotation, target rotation, and target position to zero.
 * 
 * @param c {Camera*} - Pointer to the Camera structure to be initialized.
 * 
 * This function is typically called when creating a new camera instance 
 * to ensure all fields are set to a known starting state. This is 
 * particularly important in graphics applications where camera parameters 
 * can influence rendering and scene traversal.
 */

void init_camera(Camera *c) {


    Vec3fZero(c->pos);
    Vec3fZero(c->dir);
    Vec3fZero(c->rot);
    Vec3fSet(c->targetRot, c->rot);
    Vec3fSet(c->targetPos, c->pos);

}


/**
 * Updates the view and projection matrices based on the camera's current position and direction,
 * and sends these matrices to the appropriate shaders for rendering.
 * 
 * @param c {Camera*} - Pointer to the Camera structure containing the camera's position and direction.
 * @param shaders {shaders[]} - Array of shaders to be used for rendering the scene.
 */

void camera_projection(Camera *c, Shader shaders[]) {
    // Camera
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    vec3 cameraPos   = {c->pos[0],c->pos[1],c->pos[2]};
    vec3 cameraFront = {c->dir[0], c->dir[1], c->dir[2]};
    vec3 cameraUp    = {0.0f, 1.0f,  0.0f};
    vec3 cameraB;
    glm_vec3_sub(cameraPos, cameraFront, cameraB);
    glm_lookat(cameraPos, cameraB, cameraUp, view);

    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_perspective(PI/4, 2, 0.1f, 300.0f, projection);

    use_shader(shaders[SHADER_CLASSIC_LIGHTING]);
    glUniformMatrix4fv(glGetUniformLocation(shaders[SHADER_CLASSIC_LIGHTING], "projection"), 1, GL_FALSE, &projection);
    glUniformMatrix4fv(glGetUniformLocation(shaders[SHADER_CLASSIC_LIGHTING], "view"), 1, GL_FALSE, &view);
    glUniform3fv(glGetUniformLocation(shaders[SHADER_CLASSIC_LIGHTING], "viewPos"), 1, &c->pos);
    use_shader(shaders[SHADER_SKYBOX]);
    glUniformMatrix4fv(glGetUniformLocation(shaders[SHADER_SKYBOX], "projection"), 1, GL_FALSE, &projection);
    glUniformMatrix4fv(glGetUniformLocation(shaders[SHADER_SKYBOX], "view"), 1, GL_FALSE, &view);
    glUniform3fv(glGetUniformLocation(shaders[SHADER_SKYBOX], "camPos"), 1, &c->pos);
    use_shader(shaders[SHADER_AA]);
    glUniformMatrix4fv(glGetUniformLocation(shaders[SHADER_AA], "projection"), 1, GL_FALSE, &projection);
    glUniformMatrix4fv(glGetUniformLocation(shaders[SHADER_AA], "view"), 1, GL_FALSE, &view);
    glUniform3fv(glGetUniformLocation(shaders[SHADER_AA], "camPos"), 1, &c->pos);
}