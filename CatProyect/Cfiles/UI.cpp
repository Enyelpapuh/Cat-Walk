// UI.cpp
#include "UI.h"

using namespace sf;


std::pair<RectangleShape, Text> Menu::crearBoton(const std::string& texto, Vector2f pos) {
    Text btn(texto, fuente, 40);
    btn.setPosition(pos);
    btn.setFillColor(Color::White);
    FloatRect bounds = btn.getGlobalBounds();

    RectangleShape fondoBtn(Vector2f(bounds.width + 20, bounds.height + 10));
    fondoBtn.setFillColor(Color(255, 182, 193));
    fondoBtn.setPosition(bounds.left - 10, bounds.top - 5);

    return std::make_pair(fondoBtn, btn);
}

Menu::Menu() : ventana(VideoMode(800, 600), "Aplicacion", Style::Close), estado(MENU) {
    if (!musicaFondo.openFromFile("Assets/Music/Musica_Fondo.wav")) {
        std::cerr << "No se pudo cargar Musica_Fondo.wav\n";
    }
    else {
        musicaFondo.setLoop(true);
        musicaFondo.play();
    }

    if (!musicaClic.openFromFile("Assets/Music/stereo.ogg")) {
        std::cerr << "No se pudo cargar stereo.ogg\n";
    }

    if (!fuente.loadFromFile("Assets/Fonts/ARIAL.ttf")) {
        std::cerr << "No se pudo cargar ARIAL.ttf\n";
        exit(1);
    }

    fondoMenuTex.loadFromFile("Assets/Images/fondo_principal.jpg");
    fondoConfigTex.loadFromFile("Assets/Images/fondo_configuracion.png");
    fondoComenzarTex.loadFromFile("Assets/Images/fondo_comenzar.png");

    std::tie(fondoComenzar, btnComenzar) = crearBoton("Comenzar", { 300, 200 });
    std::tie(fondoConfiguracion, btnConfiguracion) = crearBoton("Configuracion", { 300, 280 });
    std::tie(fondoSalir, btnSalir) = crearBoton("Salir", { 300, 360 });
    std::tie(fondoVolver, btnVolver) = crearBoton("Volver", { 650, 460 });

    textoMusica.setFont(fuente);
    textoMusica.setString("Música");
    textoMusica.setCharacterSize(36);
    textoMusica.setFillColor(Color(255, 182, 193));
    textoMusica.setPosition(130, 285);

    barra.setSize(Vector2f(300, 5));
    barra.setPosition(250, 300);
    barra.setFillColor(Color::White);

    control.setRadius(10);
    control.setFillColor(Color::Red);
    control.setPosition(250 + (musicaFondo.getVolume() / 100.f) * 300 - 10, 295);
}

void Menu::ejecutar() {
    menuTerminado = false; // Reinicia el estado cada vez que se ejecuta el menú
    while (ventana.isOpen()) {
        Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == Event::Closed)
                ventana.close();

            Vector2f mousePos(Mouse::getPosition(ventana));

            if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left) {
                if (estado == MENU) {
                    if (btnComenzar.getGlobalBounds().contains(mousePos)) {
                        musicaClic.play();
                        estado = COMENZAR;
                        menuTerminado = true; // Marca que el usuario presionó "Comenzar"
                        ventana.close();      // Cierra la ventana del menú
                    }
                    else if (btnConfiguracion.getGlobalBounds().contains(mousePos)) {
                        musicaClic.play();
                        estado = CONFIGURACION;
                    }
                    else if (btnSalir.getGlobalBounds().contains(mousePos)) {
                        musicaClic.play();
                        ventana.close();
                    }
                }
                else if (estado == CONFIGURACION) {
                    if (control.getGlobalBounds().contains(mousePos)) {
                        arrastrando = true;
                    }
                    else if (btnVolver.getGlobalBounds().contains(mousePos)) {
                        estado = MENU;
                    }
                }
                else if (estado == COMENZAR) {
                    if (btnVolver.getGlobalBounds().contains(mousePos)) {
                        estado = MENU;
                    }
                }
            }

            if (evento.type == Event::MouseButtonReleased && evento.mouseButton.button == Mouse::Left)
                arrastrando = false;

            if (evento.type == Event::MouseMoved && estado == CONFIGURACION && arrastrando) {
                float x = static_cast<float>(Mouse::getPosition(ventana).x);
                x = std::max(250.f, std::min(550.f, x));
                control.setPosition(x - 10, 295);
                float volumen = ((x - 250) / 300.f) * 100.f;
                musicaFondo.setVolume(volumen);
            }
        }

        // Renderizado
        ventana.clear();

        switch (estado) {
        case MENU:
            fondo.setTexture(fondoMenuTex);
            ventana.draw(fondo);
            ventana.draw(fondoComenzar); ventana.draw(btnComenzar);
            ventana.draw(fondoConfiguracion); ventana.draw(btnConfiguracion);
            ventana.draw(fondoSalir); ventana.draw(btnSalir);
            break;

        case CONFIGURACION:
            fondo.setTexture(fondoConfigTex);
            ventana.draw(fondo);
            ventana.draw(textoMusica);
            ventana.draw(barra);
            ventana.draw(control);
            ventana.draw(fondoVolver); ventana.draw(btnVolver);
            break;

        case COMENZAR:
            fondo.setTexture(fondoComenzarTex);
            ventana.draw(fondo);
            ventana.draw(fondoVolver); ventana.draw(btnVolver);
            break;
        }

        ventana.display();
    }
}

bool Menu::menuFinalizado() const {
    return menuTerminado;
}

void Menu::detenerMusica() {
    musicaFondo.stop();
}