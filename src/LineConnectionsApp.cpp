#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include <vector>
#include "cinder/params/Params.h"


using namespace ci;
using namespace ci::app;
using namespace std;

#define CRITICDIST 60


class HypotrochoidDrawApp : public AppNative {
  public:
	void setup();
	void mouseMove( MouseEvent event );
    void mouseDrag( MouseEvent event );
    void keyDown( KeyEvent event );
	void update();
	void draw();
    void prepareSettings (Settings *settings);
    
    //Posición mouse
    ci::Vec2f              mousePos;
    
    //Vector para guardar posiciónes
    std::vector<ci::Vec2f> history;
    //Vector para guardar posiciónes invertidas en ambos ejes x/y (espejo)
    std::vector<ci::Vec2f> historyInvert;
    
    //Variables función
    float x;
    float y;
    
    //Pantalla
    float width;
    float height;
    
    //Parametros Hipotrocoide
    float r;
    float R;
    float d;
    
    //Frecuencia de dibujado
    float freq;
    
    //encendido y modo followMouse
    bool encendido;
    bool followMouse;
    
    //Colores
    float hue1;
    float hue2;
    float hue3;
    
    //centro de Hypotrocoide (mouse o centro de canvas)
    float modifyX;
    float modifyY;
    
    
    //Variable para GUI
    ci::params::InterfaceGlRef mParam;

};


void HypotrochoidDrawApp::prepareSettings(Settings *settings) {
    

}

void HypotrochoidDrawApp::setup()
{
    //Propiedades Generales
    setWindowSize(1500, 768);
    height = getWindowHeight();
    width = getWindowWidth();
    setFrameRate(60);
    
    //Encender Dibujado -> Letra "S"
    encendido = 0;
    
    //Centro del hipotrocoide -> Letra "M"
    followMouse  = 1;
    
    //Parámetros hipotrocoide
    r= 10;
    R= 30;
    d = 10;
    
    //Frecuencia de dibujado
    freq = 0.4;
    
    //Colores
    hue1= 0.25;
    hue2= 0.40;
    hue3= 0.30;
    
    //GUI
    mParam = ci::params::InterfaceGl::create(getWindow(), "HypotrochoidDrawApp", ci::Vec2i(200,200));
    mParam ->addParam("r", &r); //& means you are passing a variable by reference(address).
    mParam ->addParam("R", &R);
    mParam ->addParam("d", &d);
    mParam ->addParam("Freq", &freq);
    mParam ->addParam("Hue1", &hue1, "min=0.0 max=1.0 step=0.01");
    mParam ->addParam("Hue2", &hue2, "min=0.0 max=1.0 step=0.01");
    mParam ->addParam("Hue3", &hue3, "min=0.0 max=1.0 step=0.01");
    mParam ->addParam("Encendido", &encendido);
    mParam ->addParam("Follow Mouse", &followMouse);
    
    
}

void HypotrochoidDrawApp::mouseMove( MouseEvent event )
{
    mousePos = event.getPos();
    
}

void HypotrochoidDrawApp::mouseDrag(MouseEvent event)
{
    history.insert(history.begin(), event.getPos());
    ci::Vec2f Invertido = ci::Vec2f(width/2 + ((width/2)-event.getX()),height/2 + ((height/2)-event.getY()));
    historyInvert.insert(historyInvert.begin(), Invertido);
    //console()<<event.getPos()<<std::endl;
}

void HypotrochoidDrawApp::keyDown( KeyEvent event )
{
    //Limpiar Canvas
    if(event.getChar() == 'c'){
        history.clear();
        historyInvert.clear();
        console()<<"CLEAR CANVAS"<<endl;
        gl::color(1, 1, 1, 1);
        gl::drawSolidRect(getWindowBounds());
    }
    
    //Encender Dibujado
    if(event.getChar() == 's') {
        encendido=!encendido;
    }
    
    //Modo followMouse
    if(event.getChar() == 'm') {
        followMouse=!followMouse;
    }
}

void HypotrochoidDrawApp::update()
{
    
    
    
    //Selección de modo hypotrocoide. (centro o mouse)
    
    if(!followMouse) {
        modifyX = width/2;
        modifyY = height/2;
    }
    else {
        modifyX = mousePos.x;
        modifyY = mousePos.y;
        
    }
    
    
    double time = getElapsedSeconds();
    double theta = (2*pi*freq)*time; //frecuencia angular* time

if(encendido) {
    
    //Hyphotrochoid center of mouse;
    x = (R - r)*cos(theta) + d*cos(((R-r)/r)*theta) + modifyX;
    y = (R - r)*sin(theta) - d*sin(((R-r)/r)*theta) + modifyY;
    
    
    history.insert(history.begin(), ci::Vec2f(x,y));
    
    //Posiciones originales invertidas en x/y
    ci::Vec2f Invertido = ci::Vec2f(width/2 + ((width/2)-x),height/2 + ((height/2)-y));
    historyInvert.insert(historyInvert.begin(), Invertido);
}
}

void HypotrochoidDrawApp::draw()
{
    
    
    
	// clear out the window with black

    gl::enableAlphaBlending();
    gl::color(0, 0, 0, 0.5);
    gl::drawSolidRect(getWindowBounds());
    
    mParam -> draw();


    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history.size(); j++){
            double dist = historyInvert[i].distance(historyInvert[j]);
            if(dist < CRITICDIST){
                gl::color(Color( CM_HSV, hue1, dist/CRITICDIST, dist/CRITICDIST ));
                gl::drawLine(history[i], history[j]);
            }
        }
    }
    
    
    
    for(int i = 0; i < historyInvert.size(); i++){
        for(int j = 0; j < historyInvert.size(); j++){
            double dist = historyInvert[i].distance(historyInvert[j]);
            if(dist < CRITICDIST){
                gl::color(Color( CM_HSV, hue2 , dist/CRITICDIST, dist/CRITICDIST  ));
                gl::drawLine(historyInvert[i], historyInvert[j]);
            }
        }
    }
    
    
    for(int i = 0; i < historyInvert.size(); i++){
        for(int j = 0; j < history.size(); j++){
            double dist = historyInvert[i].distance(history[j]);
            if(dist < CRITICDIST){
                gl::color(Color( CM_HSV, hue3, dist/CRITICDIST, dist/CRITICDIST ));
                gl::drawLine(historyInvert[i], history[j]);
            }
        }
    }
    
    
}

CINDER_APP_NATIVE( HypotrochoidDrawApp, RendererGl )
