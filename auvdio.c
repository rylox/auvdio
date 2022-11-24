#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "gl.h"
#include <GLFW/glfw3.h>

#define ASSERT(Condition, Message) assert(Condition &&Message)

typedef struct
{
    GLFWwindow *Window;
    uint32_t Width;
    uint32_t Height;
    uint32_t Program;
} av_context;

uint8_t *AV_ReadFile(const char *Path, int32_t *OutSize)
{
    FILE *File = fopen(Path, "rb");
    fseek(File, 0, SEEK_END);
    *OutSize = ftell(File);
    rewind(File);
    uint8_t *Data = (uint8_t *)malloc(*OutSize);
    fread(Data, *OutSize, 1, File);
    fclose(File);
    return Data;
}

uint32_t AV_ReadAndCompileShader(const char *Path, uint32_t Type, const char *Tag)
{
    uint32_t Status;
    uint32_t Shader = glCreateShader(Type);

    int32_t FileSize = 0;
    uint8_t *FileBuffer = AV_ReadFile(Path, &FileSize);

    glShaderSource(Shader, 1, (const GLchar *const *)&FileBuffer, &FileSize);
    glCompileShader(Shader);
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Status);

    if (Status != GL_TRUE)
    {
        int32_t LogLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &LogLength);
        char *Log = (char *)malloc(LogLength);
        glGetShaderInfoLog(Shader, LogLength, &LogLength, Log);
        printf("[%s]\n===\n%.*s\n", Tag, LogLength, Log);
        free(Log);
        glDeleteShader(Shader);
        Shader = 0;
    }

    free(FileBuffer);
    return Shader;
}

uint32_t AV_LinkProgram(uint32_t VertexShader, uint32_t FragmentShader, const char *Tag)
{
    uint32_t Status;
    uint32_t Program = glCreateProgram();

    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);

    glGetProgramiv(Program, GL_LINK_STATUS, &Status);
    if (Status != GL_TRUE)
    {
        int32_t LogLength = 0;
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &LogLength);
        char *Log = (char *)malloc(LogLength);
        glGetProgramInfoLog(Program, LogLength, &LogLength, Log);
        printf("[%s]\n===\n%.*s\n", Tag, LogLength, Log);
        free(Log);
        glDeleteProgram(Program);
        Program = 0;
    }
    return Program;
}

void AV_Initialize(av_context *Context)
{
    ASSERT(glfwInit(), "Failed to initialize GLFW!");
    Context->Window = glfwCreateWindow(Context->Width, Context->Height, "Auvdio", NULL, NULL);
    ASSERT(Context->Window, "Failed to create window!");
    glfwMakeContextCurrent(Context->Window);
    uint32_t VertexShader = AV_ReadAndCompileShader("shaders/vertex.glsl", GL_VERTEX_SHADER, "VERTEX");
    uint32_t FragmentShader = AV_ReadAndCompileShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER, "FRAGMENT");
    Context->Program = AV_LinkProgram(VertexShader, FragmentShader, "PROGRAM");
}

int32_t AV_Alive(av_context *Context)
{
    return !glfwWindowShouldClose(Context->Window);
}

void AV_HandleInput(av_context *Context)
{
    glfwPollEvents();
}

void AV_Draw(av_context *Context)
{
    glUseProgram(Context->Program);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(Context->Window);
}

void AV_Deinitialize(av_context *Context)
{
    glfwDestroyWindow(Context->Window);
    glfwTerminate();
}

int32_t main(int32_t Count, char **Arguments)
{
    av_context Context = {};
    Context.Width = 800;
    Context.Height = 600;
    AV_Initialize(&Context);

    glClearColor(1, 0, 0, 1);
    while (AV_Alive(&Context))
    {
        AV_Draw(&Context);
        AV_HandleInput(&Context);
    }

    AV_Deinitialize(&Context);
    return (0);
}
