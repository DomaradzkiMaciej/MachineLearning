#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "openglheader.h"   /* naj2*PIerw ten */
#include <GL/freeglut.h>  /* potem ten */

#include "utilities.h"
#include "lights.h"
#include "mygltext.h"
#include "app1d.h"

GLuint shader_id[17];
GLuint program_id[5];
GLuint trbi, trbuf, trbbp;
GLint trbsize, trbofs[6];

GLuint icos_vao, icos_vbo[4];

float model_rot_axis[3] = {0.0, 1.0, 0.0},
        model_rot_angle0 = 0.0, model_rot_angle;
const float viewer_pos0[4] = {0.0, 0.0, 10.0, 1.0};
TransBl trans;

myTextObject *vptext;
myFont *font;

void LoadMyShaders(void) {
    static const char *filename[12] =
            {"app1d0.glsl.vert", "app1d1.glsl.vert", "app1d2.glsl.vert",
             "app1d2.glsl.tesc", "app1d3.glsl.tesc",
             "app1d2.glsl.tese", "app1d3.glsl.tese", "app1d4.glsl.tese",
             "app1d1.glsl.geom",
             "app1d0.glsl.frag", "app1d1.glsl.frag", "app1d2.glsl.frag"};
    static const GLuint shtype[12] =
            {GL_VERTEX_SHADER, GL_VERTEX_SHADER, GL_VERTEX_SHADER,
             GL_TESS_CONTROL_SHADER, GL_TESS_CONTROL_SHADER,
             GL_TESS_EVALUATION_SHADER, GL_TESS_EVALUATION_SHADER,
             GL_TESS_EVALUATION_SHADER,
             GL_GEOMETRY_SHADER,
             GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
    static const GLchar *UTBNames[] =
            {"TransBlock", "TransBlock.mm", "TransBlock.mmti", "TransBlock.vm",
             "TransBlock.pm", "TransBlock.mvpm", "TransBlock.eyepos"};
    static const GLchar *ULSNames[] =
            {"LSBlock", "LSBlock.nls", "LSBlock.mask",
             "LSBlock.ls[0].ambient", "LSBlock.ls[0].direct", "LSBlock.ls[0].position",
             "LSBlock.ls[0].attenuation", "LSBlock.ls[1].ambient"};
    GLuint sh[4];
    int i;

    for (i = 0; i < 12; i++)
        shader_id[i] = CompileShaderFiles(shtype[i], 1, &filename[i]);
    sh[0] = shader_id[0];
    sh[1] = shader_id[9];
    program_id[0] = LinkShaderProgram(2, sh);
    sh[0] = shader_id[1];
    sh[1] = shader_id[8];
    sh[2] = shader_id[10];
    program_id[1] = LinkShaderProgram(3, sh);
    sh[0] = shader_id[2];
    sh[1] = shader_id[3];
    sh[2] = shader_id[5];
    sh[3] = shader_id[11];
    program_id[2] = LinkShaderProgram(4, sh);
    sh[0] = shader_id[2];
    sh[1] = shader_id[4];
    sh[2] = shader_id[6];
    sh[3] = shader_id[11];
    program_id[3] = LinkShaderProgram(4, sh);
    sh[0] = shader_id[2];
    sh[1] = shader_id[4];
    sh[2] = shader_id[7];
    sh[3] = shader_id[10];
    program_id[4] = LinkShaderProgram(4, sh);
    GetAccessToUniformBlock(program_id[1], 6, &UTBNames[0],
                            &trbi, &trbsize, trbofs, &trbbp);
    GetAccessToUniformBlock(program_id[1], 7, &ULSNames[0],
                            &lsbi, &lsbsize, lsbofs, &lsbbp);
    AttachUniformBlockToBP(program_id[0], UTBNames[0], trbbp);
    AttachUniformBlockToBP(program_id[2], UTBNames[0], trbbp);
    AttachUniformBlockToBP(program_id[3], UTBNames[0], trbbp);
    AttachUniformBlockToBP(program_id[4], UTBNames[0], trbbp);
    AttachUniformBlockToBP(program_id[4], ULSNames[0], lsbbp);
    trbuf = NewUniformBlockObject(trbsize, trbbp);
    lsbuf = NewUniformBlockObject(lsbsize, lsbbp);
    ExitIfGLError("LoadMyShaders");
} /*LoadMyShaders*/

void SetupMVPMatrix(void) {
    GLfloat m[16], mvp[16];

    M4x4Multf(m, trans.vm, trans.mm);
    M4x4Multf(mvp, trans.pm, m);
    glBindBuffer(GL_UNIFORM_BUFFER, trbuf);
    glBufferSubData(GL_UNIFORM_BUFFER, trbofs[4], 16 * sizeof(GLfloat), mvp);
    ExitIfGLError("SetupNVPMatrix");
} /*SetupMVPMatrix*/

void SetupModelMatrix(float axis[3], float angle) {
    M4x4RotateVf(trans.mm, axis[0], axis[1], axis[2], angle);
    glBindBuffer(GL_UNIFORM_BUFFER, trbuf);
    glBufferSubData(GL_UNIFORM_BUFFER, trbofs[0], 16 * sizeof(GLfloat), trans.mm);
    glBufferSubData(GL_UNIFORM_BUFFER, trbofs[1], 16 * sizeof(GLfloat), trans.mm);
    ExitIfGLError("SetupModelMatrix");
    SetupMVPMatrix();
} /*SetupModelMatrix*/

void InitViewMatrix(void) {
    memcpy(trans.eyepos, viewer_pos0, 4 * sizeof(GLfloat));
    M4x4Translatef(trans.vm, -viewer_pos0[0], -viewer_pos0[1], -viewer_pos0[2]);
    glBindBuffer(GL_UNIFORM_BUFFER, trbuf);
    glBufferSubData(GL_UNIFORM_BUFFER, trbofs[2], 16 * sizeof(GLfloat), trans.vm);
    /* !!!! */
    glBufferSubData(GL_UNIFORM_BUFFER, trbofs[5], 4 * sizeof(GLfloat), trans.eyepos);
    ExitIfGLError("InitViewMatrix");
    SetupMVPMatrix();
} /*InitViewMatrix*/

void RotateViewer(int delta_xi, int delta_eta) {
    float vi[3], lgt, angi, vk[3], angk;
    GLfloat tm[16];

    if (delta_xi == 0 && delta_eta == 0)
        return;  /* natychmiast uciekamy - nie chcemy dzielic przez 0 */
    vi[0] = (float) delta_eta * (right - left) / (float) win_height;
    vi[1] = (float) delta_xi * (top - bottom) / (float) win_width;
    vi[2] = 0.0;
    lgt = sqrt(V3DotProductf(vi, vi));
    vi[0] /= lgt;
    vi[1] /= lgt;
    angi = -0.052359878;  /* -3 stopnie */
    V3CompRotationsf(vk, &angk, viewer_rvec, viewer_rangle, vi, angi);
    memcpy(viewer_rvec, vk, 3 * sizeof(float));
    viewer_rangle = angk;
    M4x4Translatef(trans.vm, -viewer_pos0[0], -viewer_pos0[1], -viewer_pos0[2]);
    M4x4MRotateVf(trans.vm, viewer_rvec[0], viewer_rvec[1], viewer_rvec[2],
                  -viewer_rangle);
    glBindBuffer(GL_UNIFORM_BUFFER, trbuf);
    glBufferSubData(GL_UNIFORM_BUFFER, trbofs[2], 16 * sizeof(GLfloat), trans.vm);
    M4x4Transposef(tm, trans.vm);
    tm[3] = tm[7] = tm[11] = 0.0;
    M4x4MultMVf(trans.eyepos, tm, viewer_pos0);
    glBufferSubData(GL_UNIFORM_BUFFER, trbofs[5], 4 * sizeof(GLfloat), trans.eyepos);
    ExitIfGLError("RotateViewer");
    SetupMVPMatrix();
} /*RotateViewer*/

void ConstructIcosahedronVAO(void) {
    static const GLfloat vertpos[12][3] =
            {{0.0,                        0.0,                        -0.3},
             {1.0 * cos(2 * PI * 1 / 10), 1.0 * sin(2 * PI * 1 / 10), 0.0},
             {1.0 * cos(2 * PI * 3 / 10), 1.0 * sin(2 * PI * 3 / 10), 0.0},
             {1.0 * cos(2 * PI * 5 / 10), 1.0 * sin(2 * PI * 5 / 10), 0.0},
             {1.0 * cos(2 * PI * 7 / 10), 1.0 * sin(2 * PI * 7 / 10), 0.0},
             {1.0 * cos(2 * PI * 9 / 10), 1.0 * sin(2 * PI * 9 / 10)},
             {0.3 * cos(2 * PI * 1 / 5),  0.3 * sin(2 * PI * 1 / 5),  0.0},
             {0.3 * cos(2 * PI * 2 / 5),  0.3 * sin(2 * PI * 2 / 5),  0.0},
             {0.3 * cos(2 * PI * 3 / 5),  0.3 * sin(2 * PI * 3 / 5),  0.0},
             {0.3 * cos(2 * PI * 4 / 5),  0.3 * sin(2 * PI * 4 / 5),  0.0},
             {0.3 * cos(2 * PI * 5 / 5),  0.3 * sin(2 * PI * 5 / 5),  0.0},
             {0.0,                        0.0,                        0.3}};
    static const GLubyte vertcol[12][3] =
            {{255, 140, 0},
             {220, 200, 0},
             {200, 150, 0},
             {220, 210, 0},
             {255, 160, 0},
             {220, 220, 0},
             {200, 170, 0},
             {220, 230, 0},
             {255, 180, 0},
             {220, 240, 0},
             {200, 190, 0},
             {220, 250, 0}};
    static const GLubyte vertind[111] =
            {1, 6, 2, 7, 3, 8, 4, 9, 5, 10, 1, /* łamana */
             0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, /* odcinki  */
             11, 1, 11, 2, 11, 3, 11, 4, 11, 5, 11, 6, 11, 7, 11, 8, 11, 9, 11, 10, /* odcinki */
             0,1,6,0,6,2,0,2,7,0,7,3,0,3,8,
             0,8,4,0,4,9,0,9,5,0,5,10,0,10,1,
             11,1,6,11,6,2,11,2,7,11,7,3,11,3,8,
             11,8,4,11,4,9,11,9,5,11,5,10,11,10,1}; /* trójkąty */

    glGenVertexArrays(1, &icos_vao);
    glBindVertexArray(icos_vao);
    glGenBuffers(3, icos_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, icos_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 12 * 3 * sizeof(GLfloat), vertpos, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, icos_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 12 * 3 * sizeof(GLubyte), vertcol, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE,
                          3 * sizeof(GLubyte), (GLvoid *) 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icos_vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 111 * sizeof(GLubyte), vertind, GL_STATIC_DRAW);
    ExitIfGLError("ConstructIcosahedronVAO");
} /*ConstructIcosahedronVAO*/

void DrawIcosahedron(int opt, char enlight) {
    glBindVertexArray(icos_vao);
    switch (opt) {
        case 0:    /* wierzcholki */
            glUseProgram(program_id[0]);
            glPointSize(5.0);
            glDrawArrays(GL_POINTS, 0, 12);
            break;
        case 1:    /* krawedzie */
            glUseProgram(program_id[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icos_vbo[2]);
            glDrawElements(GL_LINE_STRIP, 11,
                           GL_UNSIGNED_BYTE, (GLvoid *) 0);
            glDrawElements(GL_LINES, 40,
                           GL_UNSIGNED_BYTE, (GLvoid * )(11 * sizeof(GLubyte)));
            break;
        default:   /* sciany */
            glUseProgram(program_id[enlight ? 1 : 0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icos_vbo[2]);
            glDrawElements(GL_TRIANGLES, 60,
                           GL_UNSIGNED_BYTE, (GLvoid * )(51 * sizeof(GLubyte)));
            break;
    }
} /*DrawIcosahedron*/

void DrawTessIcos(int opt, char enlight) {
    glBindVertexArray(icos_vao);
    switch (opt) {
        case 0:
            break;
        case 1:
            glUseProgram(program_id[2]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icos_vbo[2]);
            glPatchParameteri(GL_PATCH_VERTICES, 2);
            glDrawElements(GL_PATCHES, 10, GL_UNSIGNED_BYTE, (GLvoid *) 0);
            glDrawElements(GL_PATCHES, 10, GL_UNSIGNED_BYTE, (GLvoid *) 1);
            glDrawElements(GL_PATCHES, 40, GL_UNSIGNED_BYTE, (GLvoid * )(11 * sizeof(GLubyte)));
            break;
        default:
            glUseProgram(program_id[enlight ? 4 : 3]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icos_vbo[2]);
            glPatchParameteri(GL_PATCH_VERTICES, 3);
            glDrawElements(GL_PATCHES, 60, GL_UNSIGNED_BYTE, (GLvoid * )(51 * sizeof(GLubyte)));
    }
    ExitIfGLError("DrawTessIcos");
} /*DrawTessIcos*/

void InitLights(void) {
    GLfloat amb0[4] = {1.0, 0.8, 0.0, 0.0};
    GLfloat dif0[4] = {1.0, 1.0, 0.7, 0.0};
    GLfloat pos0[4] = {0.0, 1.0, 1.0, 0.0};
    GLfloat atn0[3] = {0.5, 0.5, 1.0};

    SetLightAmbient(0, amb0);
    SetLightDiffuse(0, dif0);
    SetLightPosition(0, pos0);
    SetLightAttenuation(0, atn0);
    SetLightOnOff(0, 1);
} /*InitLights*/

void NotifyViewerPos(void) {
    GLchar s[60];

    sprintf(s, "x = %5.2f, y = %5.2f, z = %5.2f",
            trans.eyepos[0], trans.eyepos[1], trans.eyepos[2]);
    SetTextObjectContents(vptext, s, 0, 17, font);
} /*NotifyViewerPos*/

void InitMyObject(void) {
    TimerInit();
    font = NewFont18x10();
/*  font = NewFont12x6 ();*/
    vptext = NewTextObject(60);
    memset(&trans, 0, sizeof(TransBl));
    memset(&light, 0, sizeof(LightBl));
    SetupModelMatrix(model_rot_axis, model_rot_angle);
    InitViewMatrix();
    NotifyViewerPos();
    ConstructIcosahedronVAO();
    InitLights();
} /*InitMyObject*/

void Cleanup(void) {
    int i;

    glUseProgram(0);
    for (i = 0; i < 5; i++)
        glDeleteProgram(program_id[i]);
    for (i = 0; i < 12; i++)
        glDeleteShader(shader_id[i]);
    glDeleteBuffers(1, &trbuf);
    glDeleteBuffers(1, &lsbuf);
    glDeleteVertexArrays(1, &icos_vao);
    glDeleteBuffers(3, icos_vbo);
    DeleteTextObject(vptext);
    DeleteFontObject(font);
    ExitIfGLError("Cleanup");
    glutDestroyWindow(WindowHandle);
} /*Cleanup*/

