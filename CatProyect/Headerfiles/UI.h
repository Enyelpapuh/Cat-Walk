// UI.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;


enum EstadoMenu { MENU, CONFIGURACION, PRE_JUEGO, COMENZAR };



class Menu {
private:
    sf::RenderWindow ventana;
    EstadoMenu estado;

    sf::Music musicaFondo;
    sf::Music musicaClic;

    sf::Font fuente;

    sf::View vistaMenu;

    sf::Text btnPantallaCompleta;
    sf::RectangleShape fondoPantallaCompleta;

    sf::Sprite fondo;

    sf::Text btnComenzar, btnConfiguracion, btnSalir, btnVolver;
    sf::RectangleShape fondoComenzar, fondoConfiguracion, fondoSalir, fondoVolver;

    sf::Texture fondoMenuTex, fondoConfigTex, fondoComenzarTex, fondoPreJuegoTex;
    sf::Sprite fondoPreJuego;

    sf::Texture fondoPausaTex;
    sf::Sprite fondoPausa;

    sf::Text textoMusica;
    sf::RectangleShape barra;
    sf::CircleShape control;
    bool arrastrando = false;

    bool pantallaCompleta = false;

    bool menuTerminado = false;

    void actualizarUIporResolucion();

    std::pair<sf::RectangleShape, sf::Text> crearBoton(const std::string& texto, sf::Vector2f pos, unsigned int fontSize);

public:
    Menu();
    void ejecutar();
    bool menuFinalizado() const; // Consulta si el menú terminó con "Comenzar"
    void detenerMusica();

    bool esPantallaCompleta() const;

    // --- NUEVO: Ejecutar menú de pausa, retorna true si se presionó "Continuar" ---
    bool ejecutarPausa();


};

