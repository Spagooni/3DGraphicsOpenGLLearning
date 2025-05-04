#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <fstream>
#include <string>

//Global variables
bool quit = false;
int screenWidth = 640;
int screenHeight = 360;
SDL_Window* graphicsAppWindow = nullptr;
SDL_GLContext graphicsAppContext = nullptr;
//Vertex Array Object
GLuint vertexArrayObject = 0;
//Vertex Buffer Object
GLuint vertexBufferObject = 0;

//Shader Program Object
GLuint shaderProgramObject = 0; 

std::string LoadShaderString(const std::string& fileName){
    std::string result = "";
    std::string line = "";
    std::ifstream readFile(fileName.c_str());

    if(readFile.is_open()){
        while(std::getline(readFile, line)){
            result += line + "\n";
        }
        readFile.close();
    }
    return result;
}


GLuint CompileShader(GLuint type, const std::string& source){
    GLuint shaderObject;
    if(type == GL_VERTEX_SHADER){
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }else if(type == GL_FRAGMENT_SHADER){
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);

    int result; 

    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

    if(result == GL_FALSE){
        int length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        char* errorMessages = new char[length];
        glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

        if(type == GL_VERTEX_SHADER){
            std::cout << "ERROR: Vertex Shader Compilation" << errorMessages << "\n";
        }else if(type == GL_FRAGMENT_SHADER){
            std::cout << "ERROR: Fragment Shader Compilation" << errorMessages << "\n";
        }
        delete[] errorMessages;
        glDeleteShader(shaderObject);
        return 0;
    }

    return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){
    GLuint programObject = glCreateProgram();

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    glValidateProgram(programObject);
    return programObject;

}

void CreateGraphicsPipeline(){

    std::string vertexShaderSource = LoadShaderString("./Shaders/vertexShader.glsl");
    //printf("Vertex Shader Source: %s\n", vertexShaderSource.c_str());
    std::string fragmentShaderSource = LoadShaderString("./Shaders/fragmentShader.glsl");
    //printf("fragment Shader Source: %s\n", fragmentShaderSource.c_str());

    shaderProgramObject = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}


void GetOpenGlVersion(){
    std::cout << "Vender: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Render: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "ShadingLang: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}
void VertexSpecification(){
    std::vector<GLfloat> vertexData{
        -0.8f, -0.8f, 0.0f, //vertex positions
        1.0f, 0.0f, 0.0f, //colours 
        0.8f, -0.8f, 0.0f,//vertex positions
        0.0f, 1.0f, 0.0f,//colour
        0.0f, 0.8f, 0.0f,//vertex positions
        0.0f, 0.0f, 1.0f,//colour
        // -0.8f, 0.8f, 0.0f,
        // 0.8f, -0.8f, 0.0f,
        // 0.8f, 0.8f, 0.0f,
        // -0.8f, 0.8f, 0.0f,
        // -0.8f, -0.8f, 0.0f,
        // 0.8f, -0.8f, 0.0f,
    };
    // std::vector<GLfloat> vertexColors{
    //     1.0f, 0.0f, 0.0f,
    //     0.0f, 1.0f, 0.0f,
    //     0.0f, 0.0f, 1.0f,
    //     // 1.0f, 0.0f, 0.0f,
    //     // 0.0f, 0.0f, 1.0f,
    //     // 0.0f, 1.0f, 0.0f,
    // };

    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    //Verts
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GL_FLOAT), 
                                  vertexData.data(), GL_STATIC_DRAW);

    //Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);

    //Colours
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));

    glBindVertexArray(0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}



void InitializeProgram(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        exit(1);
    }
    graphicsAppWindow = SDL_CreateWindow("Open GL Window", 
                                         screenWidth, screenHeight, 
                                         SDL_WINDOW_OPENGL);
    if(!graphicsAppWindow){
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    graphicsAppContext = SDL_GL_CreateContext(graphicsAppWindow);

    if(!graphicsAppContext){
        std::cout << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(graphicsAppWindow);
        SDL_Quit();
        exit(1);
    }

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        std::cout << "Glad Uninitiazlied" << std::endl;
    }
    
    GetOpenGlVersion();
}
  
void Input(){
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0){
        if(event.type == SDL_EVENT_QUIT){
            std::cout << "Quiting" << std::endl;
            quit = true;
        }


    }
}

void PreDraw(){
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    glViewport(0,0, screenWidth, screenHeight);
    glClearColor(0.f,0.f,0.f,1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramObject);
}

void Draw(){
    glBindVertexArray(vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glUseProgram(0);

}



void MainLoop(){
    while (!quit){
        Input();

        PreDraw();

        Draw();

        SDL_GL_SwapWindow(graphicsAppWindow);
    }
    
}

void CleanupProgram(){
    SDL_DestroyWindow(graphicsAppWindow);
    SDL_Quit();

} 

int main(){
    InitializeProgram();
    
    VertexSpecification();

    CreateGraphicsPipeline();

    MainLoop();

    CleanupProgram();
}