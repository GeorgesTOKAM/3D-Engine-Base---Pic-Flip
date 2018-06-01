#include "core/Application.h"
#include <iostream>
#include <vector>
#include <stdlib.h>  



using namespace std;

struct Particule
{
    vec4 position;
    vec4 velocity;
    Particule(vec3 pos, vec3 vel)
    {
        position = vec4(pos[0], pos[1], pos[2], 1.0f);
        velocity = vec4(vel[0], vel[1], vel[2], 1.0f);
    }
};

class Simulator: public Application
{
private:
    Program* program = NULL;
    Program* compute = NULL;
    Renderer* renderer = NULL;
    Renderer* boxRend = NULL;
    Computer* computer = NULL;
    Buffer* particules = NULL;
    Buffer* partLines = NULL;
    Buffer* velocities = NULL;
    
public:
    Simulator(int argc, char* argv[]);
    void update(int time);
    void render();
    void setup();
    void teardown();
};

Simulator::Simulator(int argc, char* argv[]) : Application(argc, argv) {}

void Simulator::update(int elapsedTime)
{
    /*argu1: Nombre_particules/4*/
    computer->compute(1000, 1, 1);
}

void Simulator::render()
{
    renderer->render(PRIMITIVE_POINTS, 4000);
    boxRend->render(PRIMITIVE_LINES,24);
}

void Simulator::setup()
{   
    setClearColor(0.95f, 0.95f, 0.95f, 1.0f); 

    //int i = 0;
    vector<Particule> pp;

    for(int i;i<4000;i++){
        pp.push_back
        ( 
            Particule(
                vec3((-rand() %13)+13, (rand()%25),(rand()%25)), 
                vec3((rand()%100),(rand()%100),(rand()%100))
            )
        );
    }

    particules = new Buffer(pp.data(), 4000*sizeof(Particule));
    
    float boxcontent[]={
        -13,0,0,
        13,0,0,

        -13,0,26,
        13,0,26,

        -13,25,0,
        13,25,0,

        -13,25,26,
        13,25,26,

        

        -13,0,0,
        -13,25,0,

        13,0,0,
        13,25,0,

        -13,0,26,
        -13,25,26,

        13,0,26,
        13,25,26,



        -13,0,0,
        -13,0,26,

        13,0,0,
        13,0,26,

        -13,25,0,
        -13,25,26,

        13,25,0,
        13,25,26,

    };

    partLines = new Buffer(boxcontent,sizeof(boxcontent));


    program = new Program();
    program->addShader(Shader::fromFile("shaders/perspective.vert"));
    program->addShader(Shader::fromFile("shaders/blue.frag"));

    program->link();

    renderer = program->createRenderer();
    renderer->setVertexData("vertex", particules, TYPE_FLOAT, 0, 3, sizeof(Particule));



    boxRend = program->createRenderer();
    /*Last param: Size in byte between vertices */
    boxRend->setVertexData("vertex", partLines, TYPE_FLOAT, 0, 3, 3*sizeof(float));


    /* Window's parameters*/
    /*zoom - window screen - nearplan - farplan*/
    mat4 projection = perspective(90.0f, 640.0/180.0, 0.1, 50);

    /*PARAMS
    *1: CAM position
    *2: Where the CAM look at
    *3: Reference Axis to high (y>0)*/
    //mat4 view = lookat(vec3(20, 10, 40), vec3(5, 10, 0), vec3(0, 1, 0));

    mat4 view = lookat(vec3(20, 10, 40), vec3(5, 10, 0), vec3(0, 1, 0));

    renderer->setMatrix("projectionMatrix", projection);
    renderer->setMatrix("modelViewMatrix", view);



    boxRend->setMatrix("projectionMatrix", projection);
    boxRend->setMatrix("modelViewMatrix", view);



    compute = new Program();
    compute->addShader(Shader::fromFile("shaders/gravity.comp"));
    compute->link();

    computer = compute->createComputer();
    computer->setData(1, particules);
    //computer->setData(2, velocities);
}

void Simulator::teardown()
{
   
}

int main(int argc, char** argv)
{
    try 
    {

        Simulator app = Simulator(argc, argv); 
	    return app.run();
    }
	catch(Exception e)
    {
        cout << e.getMessage() << endl;
        return 1;
    }
}