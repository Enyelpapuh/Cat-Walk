// UI.cpp
#include "UI.h"

using namespace sf;

bool Menu::esPantallaCompleta() const {
    return pantallaCompleta;
}

std::pair<RectangleShape, Text> Menu::crearBoton(const std::string& texto, Vector2f pos, unsigned int fontSize) {
    Text btn(texto, fuente, fontSize);
    btn.setPosition(pos);
    btn.setFillColor(Color::Black);
    FloatRect bounds = btn.getGlobalBounds();

    RectangleShape fondoBtn(Vector2f(bounds.width + 20, bounds.height + 10));
    fondoBtn.setFillColor(Color(232, 203, 165));
    fondoBtn.setPosition(bounds.left - 10, bounds.top - 5);

    return std::make_pair(fondoBtn, btn);
}


Menu::Menu() : ventana(VideoMode(1920, 1080), "Aplicacion", Style::Close), estado(MENU) {
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

    actualizarUIporResolucion();

    std::string resFolder = "Assets/Images/1920x1080/";
    std::string pathMenu = resFolder + "fondo_menu.jpg";
    std::string pathConfig = resFolder + "fondo_configuracion.jpg";
    std::string pathPreJuego = resFolder + "fondo_prejuego.jpg";

    fondoMenuTex.loadFromFile(pathMenu);
    fondoConfigTex.loadFromFile(pathConfig);
    fondoPreJuegoTex.loadFromFile(pathPreJuego);
    fondoPreJuego.setTexture(fondoPreJuegoTex);

    vistaMenu = sf::View(sf::FloatRect(0, 0, SCR_WIDTH, SCR_HEIGHT));
    ventana.setView(vistaMenu);

    actualizarUIporResolucion();

}

void Menu::ejecutar() {
    menuTerminado = false; // Reinicia el estado cada vez que se ejecuta el menú

    // Opciones de resolución
    std::vector<std::pair<unsigned int, unsigned int>> resoluciones = {
        {640, 480},
        {800, 600},
        {1280, 720},
        {1920, 1080}
    };

    // Botón de resolución (se crea cada frame para mantener la posición actual)
    sf::Text btnResolucion;
    sf::RectangleShape fondoResolucion;



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
                        estado = PRE_JUEGO;
                        // NO cierres la ventana aquí
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
                    else if (btnResolucion.getGlobalBounds().contains(mousePos)) {
                        // Submenú de selección de resolución
                        sf::RenderWindow resVentana(sf::VideoMode(400, 350), "Seleccionar Resolución", sf::Style::Close);
                        std::vector<sf::Text> opciones;
                        for (size_t i = 0; i < resoluciones.size(); ++i) {
                            std::string texto = std::to_string(resoluciones[i].first) + "x" + std::to_string(resoluciones[i].second);
                            sf::Text opcion(texto, fuente, 32);
                            opcion.setPosition(80, 60 + i * 60);
                            opcion.setFillColor(sf::Color::White);
                            opciones.push_back(opcion);
                        }
                        while (resVentana.isOpen()) {
                            sf::Event event;
                            while (resVentana.pollEvent(event)) {
                                if (event.type == sf::Event::Closed)
                                    resVentana.close();
                                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                                    sf::Vector2f mousePos(sf::Mouse::getPosition(resVentana));
                                    for (size_t i = 0; i < opciones.size(); ++i) {
                                        if (opciones[i].getGlobalBounds().contains(mousePos)) {
                                            SCR_WIDTH = resoluciones[i].first;
                                            SCR_HEIGHT = resoluciones[i].second;
                                            resVentana.close();
                                            ventana.setSize(sf::Vector2u(SCR_WIDTH, SCR_HEIGHT));
                                            vistaMenu.setSize(SCR_WIDTH, SCR_HEIGHT);
                                            vistaMenu.setCenter(SCR_WIDTH / 2.f, SCR_HEIGHT / 2.f);
                                            ventana.setView(vistaMenu);

                                            // Carga las imágenes de fondo para la resolución seleccionada
                                            std::string resFolder = "Assets/Images/" + std::to_string(SCR_WIDTH) + "x" + std::to_string(SCR_HEIGHT) + "/";
                                            std::string pathMenu = resFolder + "fondo_menu.jpg";
                                            std::string pathConfig = resFolder + "fondo_configuracion.jpg";
                                            std::string pathPreJuego = resFolder + "fondo_prejuego.jpg";

                                            if (!fondoMenuTex.loadFromFile(pathMenu)) {
                                                std::cerr << "No se pudo cargar: " << pathMenu << std::endl;
                                            }
                                            if (!fondoConfigTex.loadFromFile(pathConfig)) {
                                                std::cerr << "No se pudo cargar: " << pathConfig << std::endl;
                                            }
                                            if (!fondoPreJuegoTex.loadFromFile(pathPreJuego)) {
                                                std::cerr << "No se pudo cargar: " << pathPreJuego << std::endl;
                                            }
                                            fondoPreJuego.setTexture(fondoPreJuegoTex);

                                            actualizarUIporResolucion();

                                        }
                                    }
                                }
                            }
                            resVentana.clear(sf::Color(30, 30, 30));
                            for (const auto& opcion : opciones)
                                resVentana.draw(opcion);
                            resVentana.display();
                        }
                    }
                    else if (btnPantallaCompleta.getGlobalBounds().contains(mousePos)) {
                        pantallaCompleta = !pantallaCompleta;
                        ventana.close(); // Cerramos la ventana actual para recrearla

                        // Recrea la ventana con el nuevo modo
                        if (pantallaCompleta) {
                            ventana.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "Aplicacion", sf::Style::Fullscreen);
                        }
                        else {
                            ventana.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "Aplicacion", sf::Style::Close);
                        }
                        vistaMenu.setSize(SCR_WIDTH, SCR_HEIGHT);
                        vistaMenu.setCenter(SCR_WIDTH / 2.f, SCR_HEIGHT / 2.f);
                        ventana.setView(vistaMenu);
                        actualizarUIporResolucion();
                    }


                }
                else if (estado == COMENZAR) {
                    if (btnVolver.getGlobalBounds().contains(mousePos)) {
                        estado = MENU;
                    }
                }

                else if (estado == PRE_JUEGO) {
                    // Botón "Continuar" en la parte inferior derecha
                    sf::Text btnContinuar("Continuar", fuente, 36);
                    float padding = 40.f;
                    btnContinuar.setPosition(
                        ventana.getSize().x - btnContinuar.getGlobalBounds().width - padding,
                        ventana.getSize().y - btnContinuar.getGlobalBounds().height - padding
                    );
                    if (btnContinuar.getGlobalBounds().contains(mousePos)) {
                        musicaClic.play();
                        estado = COMENZAR;
                        menuTerminado = true; // Ahora sí marca que debe iniciar el juego
                        ventana.close();      // Cierra la ventana del menú
                    }
                }

            }

            if (evento.type == sf::Event::Resized) {
                vistaMenu.setSize(evento.size.width, evento.size.height);
                vistaMenu.setCenter(evento.size.width / 2.f, evento.size.height / 2.f);
                ventana.setView(vistaMenu);
                actualizarUIporResolucion();
            }


            if (evento.type == Event::MouseButtonReleased && evento.mouseButton.button == Mouse::Left)
                arrastrando = false;

            if (evento.type == Event::MouseMoved && estado == CONFIGURACION && arrastrando) {
                // Obtén la posición y tamaño actual de la barra
                float barraX = barra.getPosition().x;
                float barraY = barra.getPosition().y;
                float barraW = barra.getSize().x;
                float barraH = barra.getSize().y;
                float controlRadio = control.getRadius();

                // Limita el mouse al rango de la barra
                float x = static_cast<float>(Mouse::getPosition(ventana).x);
                x = std::max(barraX, std::min(barraX + barraW, x));

                // Actualiza la posición de la bolita roja (centrada verticalmente en la barra)
                float controlY = barraY - controlRadio + barraH / 2.f;
                control.setPosition(x - controlRadio, controlY);

                // Calcula el volumen en base a la posición relativa en la barra
                float volumen = ((x - barraX) / barraW) * 100.f;
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

        case CONFIGURACION: {
            fondo.setTexture(fondoConfigTex);
            ventana.draw(fondo);
            ventana.draw(textoMusica);
            ventana.draw(barra);
            ventana.draw(control);
            // Crear y dibujar el botón de resolución
            btnResolucion.setFont(fuente);
            btnResolucion.setString("Resolución");
            btnResolucion.setCharacterSize(static_cast<unsigned int>(36 * (SCR_HEIGHT / 600.0f)));
            btnResolucion.setFillColor(sf::Color::Black);

            ventana.draw(fondoPantallaCompleta);
            ventana.draw(btnPantallaCompleta);

            // Posiciona el botón justo debajo de la barra de volumen
            float espacio = 30 * (SCR_HEIGHT / 600.0f); // Espacio vertical entre barra y botón
            float posX = barra.getPosition().x + (barra.getSize().x - btnResolucion.getLocalBounds().width) / 2.f;
            float posY = barra.getPosition().y + barra.getSize().y + espacio;
            btnResolucion.setPosition(posX, posY);

            sf::FloatRect bounds = btnResolucion.getGlobalBounds();
            fondoResolucion.setSize(sf::Vector2f(bounds.width + 20, bounds.height + 10));
            fondoResolucion.setFillColor(sf::Color(232, 203, 165));
            fondoResolucion.setPosition(bounds.left - 10, bounds.top - 5);


            ventana.draw(fondoResolucion);
            ventana.draw(btnResolucion);
            ventana.draw(fondoVolver); ventana.draw(btnVolver);
            break;
        }

        case PRE_JUEGO: {
            ventana.draw(fondoPreJuego);

            // Botón "Continuar" en la parte inferior derecha
            sf::Text btnContinuar("Continuar", fuente, 36);
            btnContinuar.setFillColor(sf::Color::Black);
            float padding = 40.f;
            btnContinuar.setPosition(
                ventana.getSize().x - btnContinuar.getGlobalBounds().width - padding,
                ventana.getSize().y - btnContinuar.getGlobalBounds().height - padding
            );
            sf::RectangleShape fondoBtn(sf::Vector2f(btnContinuar.getGlobalBounds().width + 20, btnContinuar.getGlobalBounds().height + 10));
            fondoBtn.setFillColor(sf::Color(232, 203, 165));
            fondoBtn.setPosition(btnContinuar.getGlobalBounds().left - 10, btnContinuar.getGlobalBounds().top - 5);

            ventana.draw(fondoBtn);
            ventana.draw(btnContinuar);
            break;
        }


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

void Menu::actualizarUIporResolucion() {
    float escalaX = SCR_WIDTH / 800.0f;
    float escalaY = SCR_HEIGHT / 600.0f;
    unsigned int fontSize = static_cast<unsigned int>(40 * escalaY);

    // Botones principales
    std::tie(fondoComenzar, btnComenzar) = crearBoton("Comenzar", { 300 * escalaX, 200 * escalaY }, fontSize);
    std::tie(fondoConfiguracion, btnConfiguracion) = crearBoton("Configuracion", { 300 * escalaX, 280 * escalaY }, fontSize);
    std::tie(fondoSalir, btnSalir) = crearBoton("Salir", { 300 * escalaX, 360 * escalaY }, fontSize);
    std::tie(fondoVolver, btnVolver) = crearBoton("Volver", { 650 * escalaX, 460 * escalaY }, fontSize);

    // Texto de música
    textoMusica.setFont(fuente);
    textoMusica.setString("Música");
    textoMusica.setCharacterSize(static_cast<unsigned int>(36 * escalaY));
    textoMusica.setFillColor(Color(0, 0, 0));
    textoMusica.setPosition(130 * escalaX, 285 * escalaY);

    // Barra de volumen (scroll)
    float barraX = 250 * escalaX;
    float barraY = 300 * escalaY;
    float barraW = 300 * escalaX;
    float barraH = 5 * escalaY;
    barra.setSize(Vector2f(barraW, barraH));
    barra.setPosition(barraX, barraY);
    barra.setFillColor(Color::White);

    // ... (después de la barra y el botón de resolución)
    float espacio = 30 * escalaY; // Espacio vertical entre controles

    // Calcula posición del botón de resolución (ya lo tienes)
    float resolucionBtnY = barra.getPosition().y + barra.getSize().y + espacio;

    // Botón de pantalla completa debajo del de resolución
    float pantallaCompletaBtnY = resolucionBtnY + 60 * escalaY; // 60 es un alto aproximado del botón

    // Puedes guardar la posición para usarla en el render y en el evento
    btnPantallaCompleta.setFont(fuente);
    btnPantallaCompleta.setString("Pantalla Completa");
    btnPantallaCompleta.setCharacterSize(static_cast<unsigned int>(36 * escalaY));
    btnPantallaCompleta.setFillColor(Color::Black);
    btnPantallaCompleta.setPosition(barra.getPosition().x, pantallaCompletaBtnY);

    sf::FloatRect boundsPC = btnPantallaCompleta.getGlobalBounds();
    fondoPantallaCompleta.setSize(sf::Vector2f(boundsPC.width + 20, boundsPC.height + 10));
    fondoPantallaCompleta.setFillColor(sf::Color(232, 203, 165));
    fondoPantallaCompleta.setPosition(boundsPC.left - 10, boundsPC.top - 5);


    // Control del volumen (círculo)
    float controlRadio = 10 * escalaY;
    control.setRadius(controlRadio);
    control.setFillColor(Color::Red);
    // El centro del control debe estar alineado con la barra
    float volumen = musicaFondo.getVolume();
    float controlX = barraX + (volumen / 100.f) * barraW - controlRadio;
    float controlY = barraY - controlRadio + barraH / 2.f;
    control.setPosition(controlX, controlY);
}

// --- NUEVO: Menú de pausa, reutiliza el menú principal pero solo muestra "Continuar" y "Salir" ---
bool Menu::ejecutarPausa() {
    // Crear ventana de pausa (bloqueante)
    sf::RenderWindow pausaVentana(sf::VideoMode(800, 600), "Pausa", sf::Style::Close);
    sf::Font font;
    font.loadFromFile("Assets/Fonts/ARIAL.ttf");

    // Por ejemplo, en el constructor de Menu:
    fondoPausaTex.loadFromFile("Assets/Images/800x600/fondo_pausa.jpg");
    fondoPausa.setTexture(fondoPausaTex);


    sf::Text titulo("PAUSA", font, 60);
    titulo.setFillColor(sf::Color(232, 203, 165));
    titulo.setPosition(300, 100);

    sf::Text btnContinuar("Continuar", font, 40);
    btnContinuar.setFillColor(sf::Color::Black);
    btnContinuar.setPosition(300, 250);
    sf::FloatRect continuarBounds = btnContinuar.getGlobalBounds();
    sf::RectangleShape fondoContinuar(sf::Vector2f(continuarBounds.width + 20, continuarBounds.height + 10));
    fondoContinuar.setFillColor(sf::Color(232, 203, 165));
    fondoContinuar.setPosition(continuarBounds.left - 10, continuarBounds.top - 5);

    sf::Text btnSalir("Salir", font, 40);
    btnSalir.setFillColor(sf::Color::Black);
    btnSalir.setPosition(300, 350);
    sf::FloatRect salirBounds = btnSalir.getGlobalBounds();
    sf::RectangleShape fondoSalir(sf::Vector2f(salirBounds.width + 20, salirBounds.height + 10));
    fondoSalir.setFillColor(sf::Color(232, 203, 165));
    fondoSalir.setPosition(salirBounds.left - 10, salirBounds.top - 5);

    while (pausaVentana.isOpen()) {
        sf::Event evento;
        while (pausaVentana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                pausaVentana.close();

            sf::Vector2f mousePos(sf::Mouse::getPosition(pausaVentana));
            if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
                if (btnContinuar.getGlobalBounds().contains(mousePos)) {
                    pausaVentana.close();
                    return true; // Continuar
                }
                if (btnSalir.getGlobalBounds().contains(mousePos)) {
                    pausaVentana.close();
                    return false; // Salir
                }
            }
        }
        pausaVentana.clear();

        // Dibuja el fondo antes de los botones
        pausaVentana.draw(fondoPausa);

        pausaVentana.draw(titulo);
        pausaVentana.draw(fondoContinuar); pausaVentana.draw(btnContinuar);
        pausaVentana.draw(fondoSalir); pausaVentana.draw(btnSalir);
        pausaVentana.display();
    }

    return false;
}