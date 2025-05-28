// UI.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

enum EstadoMenu { MENU, CONFIGURACION, COMENZAR };

class Menu {
private:
    sf::RenderWindow ventana;
    EstadoMenu estado;

    sf::Music musicaFondo;
    sf::Music musicaClic;

    sf::Font fuente;

    sf::Texture fondoMenuTex, fondoConfigTex, fondoComenzarTex;
    sf::Sprite fondo;

    sf::Text btnComenzar, btnConfiguracion, btnSalir, btnVolver;
    sf::RectangleShape fondoComenzar, fondoConfiguracion, fondoSalir, fondoVolver;

    sf::Text textoMusica;
    sf::RectangleShape barra;
    sf::CircleShape control;
    bool arrastrando = false;

    bool menuTerminado = false;

    std::pair<sf::RectangleShape, sf::Text> crearBoton(const std::string& texto, sf::Vector2f pos);

public:
    Menu();
    void ejecutar();
    bool menuFinalizado() const; // Consulta si el menú terminó con "Comenzar"
    void detenerMusica();
};
