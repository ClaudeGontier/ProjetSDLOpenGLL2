#include <stdlib.h>
#include <stdio.h>
#include "stringio.h"
#include "../types.h"
#include "../math/math_util.h"
#include "model.h"
#include "../memory.h"


/**
 * Allocates memory for an Object's vertex, face, normal, and texture data.
 *
 * @param obj {Object*} A pointer to the Object structure to be initialized.
 * @param vim {u32} The initial number of vertices to allocate memory for.
 * @param fim {u32} The initial number of faces to allocate memory for.
 * @param vnim {u32} The initial number of normals to allocate memory for.
 * @param vtim {u32} The initial number of texture coordinates to allocate memory for.
 *
 * This function allocates the necessary memory for various data fields in 
 * the specified Object, including vertices, faces, normals, and texture 
 * coordinates. It initializes the materials and materialsLength to NULL 
 * and sets the materialsCount to zero.
 *
 * Important Notes:
 * - The function assumes that `obj` is a valid pointer to an Object 
 *   structure.
 * - If any memory allocation fails, a pointer check function (POINTER_CHECK) 
 *   should handle the error, potentially freeing previously allocated memory 
 *   and terminating the program or logging the error.
 *
 * Example Usage:
 * malloc_obj(&object, initialVertexCount, initialFaceCount, 
 *             initialNormalCount, initialTextureVertexCount);
 *
 * Return:
 * Returns 0 on successful memory allocation.
 */


int malloc_obj(Object *obj, u32 vim, u32 fim, u32 vnim, u32 vtim) {
    obj->materials = NULL;
    obj->materialsLength = NULL;
    obj->materialsCount = 0;
    obj->vertex = malloc(sizeof(Vertex) * vim);
    obj->faces = malloc(sizeof(Face) * fim);
    obj->facesVertex = malloc(sizeof(Vertex)*3 * fim);
    obj->normals = malloc(sizeof(Normal) * vnim);
    obj->textureVertex = malloc(sizeof(TextureVertex) * vtim);
    POINTER_CHECK(obj->vertex);
    POINTER_CHECK(obj->faces);
    POINTER_CHECK(obj->facesVertex);
    POINTER_CHECK(obj->normals);
    POINTER_CHECK(obj->textureVertex);
    return 0;
}


/**
 * Creates and initializes a Vertex Array Object (VAO) for a given 3D 
 * object, including the necessary vertex buffer data.
 *
 * @param obj {Object*} A pointer to the Object structure for which the 
 *                      VAO will be created.
 *
 * This function generates a VAO and a Vertex Buffer Object (VBO) for the 
 * specified Object. It binds the VBO and uploads vertex data, including 
 * positions, normals, texture coordinates, tangents, and bitangents, to 
 * the GPU. The function also sets up the vertex attribute pointers to 
 * describe the layout of the vertex data in the VBO.
 *
 * Important Notes:
 * - The function assumes that the obj->facesVertex array has been populated 
 *   with valid vertex data prior to calling this function.
 * - The VAO generated by this function is stored in the Object structure, 
 *   allowing for easy rendering of the object in subsequent draw calls.
 * - Make sure to bind the VAO when rendering this object to ensure the 
 *   correct vertex data is used.
 *
 * Example Usage:
 * create_obj_vao(&object);
 */

void create_obj_vao(Object *obj) {
    VBO VBO;
    glGenBuffers(1, &VBO);

    VAO VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * obj->length * 3, obj->facesVertex, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    obj->VAO = VAO;
}


/**
 * Cleans up and reallocates the memory for the vertex data and faces of a 3D 
 * object, and generates the Vertex Array Object (VAO) for rendering.
 *
 * @param obj {Object*} A pointer to the Object structure to be cleaned up and 
 *                      reallocated.
 * @param vi {u32} The current number of vertices loaded for the object.
 * @param fi {u32} The current number of faces loaded for the object.
 * @param vni {u32} The current number of normals loaded for the object.
 * @param vti {u32} The current number of texture coordinates loaded for the object.
 *
 * This function frees the previously allocated memory for normals, texture 
 * vertices, and vertex data associated with the given Object structure. It 
 * then reallocates the facesVertex array to fit the actual number of faces 
 * and generates the corresponding Vertex Array Object (VAO) for rendering.
 *
 * Important Notes:
 * - This function assumes that the Object structure has been allocated 
 *   and initialized properly before calling.
 * - After calling this function, the obj->facesVertex will point to a 
 *   reallocated array suitable for the number of faces currently stored.
 * - Any previously allocated memory for normals, texture vertices, and 
 *   vertex data will be released, preventing memory leaks.
 * - It is the caller's responsibility to ensure that obj is valid and that 
 *   vi, fi, vni, and vti are correctly provided to reflect the current 
 *   state of the object.
 *
 * Example Usage:
 * close_realloc_obj(&object, vi, fi, vni, vti);
 */

void close_realloc_obj(Object *obj, u32 vi, u32 fi, u32 vni, u32 vti) {
    obj->length = fi;

    free(obj->normals);
    free(obj->textureVertex);
    free(obj->vertex);
    free(obj->faces);
    obj->facesVertex = realloc(obj->facesVertex, sizeof(Vertex)*3 * fi);
    POINTER_CHECK(obj->facesVertex);
    create_obj_vao(obj);
}


/**
 * Loads a 3D model from an OBJ file, including its associated materials, 
 * vertices, normals, and texture coordinates.
 *
 * @param path {char*} The directory path where the OBJ file is located.
 * @param filename {char*} The name of the OBJ file to be loaded.
 * @param model {Model*} A pointer to a Model structure that will be populated 
 *                       with the loaded data.
 *
 * @return {int} Returns 0 on success, or -1 if an error occurs during loading.
 *
 * This function reads the specified OBJ file and extracts geometric data 
 * such as vertices, texture coordinates, normals, and faces. It also handles 
 * the associated materials through a corresponding MTL file, updating the 
 * Model structure accordingly. The model is dynamically allocated and resized 
 * as necessary to accommodate the loaded data.
 *
 * Important Notes:
 * - The function relies on external utility functions such as `load_mtl`, 
 *   `find_material`, `malloc_obj`, and `close_realloc_obj` for loading 
 *   materials and managing dynamic memory.
 * - The input OBJ file must follow the standard OBJ format; otherwise, 
 *   parsing may fail, leading to potential memory leaks if not handled correctly.
 * - Ensure that sufficient memory is available for the model structure and 
 *   associated objects, as this function dynamically allocates memory for 
 *   various arrays within the Model structure.
 *
 * Example Usage:
 * Model model;
 * if (load_obj_model("models/", "example.obj", &model) == 0) {
 *     // Successfully loaded the model
 * } else {
 *     // Handle error
 * }
 */


int load_obj_model(char *path, Model **modelPtr) {

    for (int i = 0; i < memoryCaches.modelsCount; i++) {
        if (!strcmp(memoryCaches.modelCache[i].modelName, path)) {
            #ifdef DEBUG
                printf("Model loaded from cache!\n");
            #endif
            (*modelPtr) = memoryCaches.modelCache[i].model;
            return 0;
        }
    }

    Model *model = *modelPtr = malloc(sizeof(Model));

    int selectedMaterialId;
    FILE * file = fopen(path, "r");
    POINTER_CHECK(file);

    u32 oi = 0, offv = 1, offvt = 1, offvn = 1, fi = 0, vi = 0, vti = 0, vni = 0, fim = 30, vim = 50, vtim = 50, vnim = 50, oim = 1, mc = 0;

    model->objects = malloc(sizeof(Object) * oim);
    POINTER_CHECK(model->objects);
    Object *object = NULL;
    

    char symbol;
    do {
        if (object) {
            if (vi >= vim) {
                
                vim<<=1;
                object->vertex = realloc(object->vertex, sizeof(Vertex) * vim);
                POINTER_CHECK(object->vertex);
            }
            if (vni >= vnim) {
                
                vnim<<=1;
                object->normals = realloc(object->normals, sizeof(Normal) * vnim);
                POINTER_CHECK(object->normals);
            }
            if (vti >= vtim) {
                
                vtim<<=1;
                object->textureVertex = realloc(object->textureVertex, sizeof(TextureVertex) * vtim);
                POINTER_CHECK(object->textureVertex);
            }
            if (fi >= fim) {
                
                fim<<=1;
                object->faces = realloc(object->faces, sizeof(Face) * fim);
                object->facesVertex = realloc(object->facesVertex, sizeof(Vertex)*3 * fim);
                POINTER_CHECK(object->faces);
                POINTER_CHECK(object->facesVertex);
            }
        }
        symbol = getc(file);
        switch (symbol) {
        case 'v':
            symbol = getc(file);
            switch (symbol) {
                case ' ':
                    fscanf(file, "%f %f %f\n", &object->vertex[vi][0],&object->vertex[vi][1],&object->vertex[vi][2]);
                    vi++;
                    break;
                case 't':
                    fscanf(file, "%f %f\n", &object->textureVertex[vti][0],&object->textureVertex[vti][1]);
                    vti++;
                    break;
                case 'n':
                    fscanf(file, "%f %f %f\n", &object->normals[vni][0],&object->normals[vni][1],&object->normals[vni][2]);
                    vni++;
                    break;
                default: fscanf(file, "%*[^\n]\n");
            }
            break;
        case 'f':
            if (!object->materialsCount || model->materials[selectedMaterialId].name != object->materials[object->materialsCount-1]->name) {
                
                object->materialsCount++;
                object->materials = realloc(object->materials, sizeof(Material *) * (object->materialsCount));
                object->materialsLength = realloc(object->materialsLength, sizeof(u32) * (object->materialsCount));
                POINTER_CHECK(object->materials);
                POINTER_CHECK(object->materialsLength);
                object->materials[object->materialsCount-1] = &model->materials[selectedMaterialId];
                object->materialsLength[object->materialsCount-1] = 0;
                
            }
            object->materialsLength[object->materialsCount-1]++;
            int vertexId[MAX_VERTEX_PER_FACE], normalsId[MAX_VERTEX_PER_FACE], textureVertexId[MAX_VERTEX_PER_FACE];
            int i = 0;
            symbol = 0;
            do {
                fscanf(file, "%d/%d/%d", &vertexId[i], &textureVertexId[i], &normalsId[i]);
                i++;
            } while ((symbol = getc(file)) == ' ');
            object->faces[fi].length = i;
            for (int j = 0; j < i; j++) {
                object->faces[fi].vertex[j] = vertexId[j]-offv;

                
                object->facesVertex[fi][j][0] = object->vertex[vertexId[j]-offv][0];
                object->facesVertex[fi][j][1] = object->vertex[vertexId[j]-offv][1];
                object->facesVertex[fi][j][2] = object->vertex[vertexId[j]-offv][2];
                object->facesVertex[fi][j][3] = object->normals[normalsId[j]-offvn][0];
                object->facesVertex[fi][j][4] = object->normals[normalsId[j]-offvn][1];
                object->facesVertex[fi][j][5] = object->normals[normalsId[j]-offvn][2];
                object->facesVertex[fi][j][6] = object->textureVertex[textureVertexId[j]-offvt][0];
                object->facesVertex[fi][j][7] = -object->textureVertex[textureVertexId[j]-offvt][1];
                object->facesVertex[fi][j][8] = selectedMaterialId;

                //object->facesVertex[fi][j] = vertexId[j]-offv;
                object->faces[fi].normals[j] = normalsId[j]-offvn;
                object->faces[fi].textureVertex[j] = textureVertexId[j]-offvt;
            }


            
            
            vec2 deltaUV1;
            vec2 deltaUV2;
            glm_vec2_sub(&object->facesVertex[fi][1][6], &object->facesVertex[fi][0][6], deltaUV1);
            glm_vec2_sub(&object->facesVertex[fi][2][6], &object->facesVertex[fi][0][6], deltaUV2);
            vec3 edge1;
            vec3 edge2;
            glm_vec3_sub(object->vertex[vertexId[1]-offv], object->vertex[vertexId[0]-offv],edge1);
            glm_vec3_sub(object->vertex[vertexId[2]-offv], object->vertex[vertexId[0]-offv],edge2);

            vec3 tangent;
            vec3 bitangent;
            float f = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

            tangent[0] = f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
            tangent[1] = f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
            tangent[2] = f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[0]);

            bitangent[0] = f * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
            bitangent[1] = f * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
            bitangent[2] = f * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);

            for (int j = 0; j < i; j++) {
                object->facesVertex[fi][j][9] = tangent[0];
                object->facesVertex[fi][j][10] = tangent[1];
                object->facesVertex[fi][j][11] = tangent[2];

                object->facesVertex[fi][j][12] = bitangent[0];
                object->facesVertex[fi][j][13] = bitangent[1];
                object->facesVertex[fi][j][14] = bitangent[2];
            }
            fi++;
            break;
        case 'm':
            
            char materialFilename[50];
            fscanf(file, "%*[^ ] %s\n", (char *) &materialFilename);
            char * material_path = get_folder_path(path);
            if ((mc = load_mtl(material_path, materialFilename, &model->materials)) == -1) return -1;
            free(material_path);
            model->materialsCount = mc;
            break;
        case 's':
            
            fscanf(file, "%hhd\n", &object->smoothShading);
            break;
        case 'u':
            
            char materialName[50];
            fscanf(file, "%*[^ ] %[^\n]", (char *) materialName);
            selectedMaterialId = find_material(model->materials, mc, materialName);
            break;
        case 'o':
            
            fscanf(file, "%*[^\n]");
            if (object) {
                close_realloc_obj(object, vi, fi, vni, vti);
                oi++;
                if (oi >= oim) {
                    oim<<=1;
                    model->objects = realloc(model->objects, sizeof(Object) * oim);
                    POINTER_CHECK(model->objects);
                }
            }
            object = &(model->objects[oi]);
            offv += vi, offvt += vti, offvn += vni, fi = 0, vi = 0, vti = 0, vni = 0, fim = 30, vim = 50, vtim = 50, vnim = 50;
            malloc_obj(object, vim, fim, vnim, vtim);
            break;
        case '\n':
            break;
        case '#':
        default:
            fscanf(file, "%*[^\n]\n");
            break;
        }
    } while (symbol != -1);
    
    close_realloc_obj(object, vi, fi, vni, vti);
    oi++;
    model->objects = realloc(model->objects, sizeof(Object) * oi);
    POINTER_CHECK(model->objects);
    model->length = oi;

    memoryCaches.modelCache = realloc(memoryCaches.modelCache, sizeof (ModelCache) * (++memoryCaches.modelsCount));
    memoryCaches.modelCache[memoryCaches.modelsCount-1].model = model;
    strcpy(memoryCaches.modelCache[memoryCaches.modelsCount-1].modelName, path);
    
    return 0;

}