/* simulacion_modulos.h - Definición de módulos según arquitectura del documento */

#ifndef SIMULACION_MODULOS_H
#define SIMULACION_MODULOS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Constantes del sistema */
#define LIMITE_COLA 100
#define OCUPADO 1
#define LIBRE 0

/* Tipos de eventos */
#define EVENTO_LLEGADA 1
#define EVENTO_SALIDA 2

/* Estructura para las variables de estado del sistema (X) */
struct EstadoSistema
{
    int estado_servidor;
    int num_entra_cola;
    float tiempo_llegada[LIMITE_COLA + 1];
};

/* Estructura para las características del sistema (θ) */
struct CaracteristicasSistema
{
    int num_clientes_espera;
    float total_de_esperas;
    float area_num_entra_cola;
    float area_estado_servidor;
};

/* Estructura para el tiempo-espacio (T) */
struct TiempoEspacio
{
    float tiempo_simulacion;
    float tiempo_ultimo_evento;
};

/* Estructura para la lista de eventos (L) */
struct ListaEventos
{
    float tiempo_sig_evento[3]; // índice 0 no se usa
    int sig_tipo_evento;
};

/* Estructura para los parámetros del sistema (ξ) */
struct ParametrosSistema
{
    float media_entre_llegadas;
    float media_atencion;
    int num_esperas_requerido;
    int num_eventos;
};

/* ========== MÓDULO 1: INICIALIZACIÓN (Algoritmo 1-1) ========== */
class ModuloInicializacion
{
public:
    static void inicializar(TiempoEspacio &T, EstadoSistema &X,
                            CaracteristicasSistema &theta, ListaEventos &L,
                            const ParametrosSistema &xi);
};

/* ========== MÓDULO 2: MANEJO TIEMPO-ESPACIO (Algoritmo 1-2) ========== */
class ModuloManejoTiempo
{
public:
    static int manejoTiempoEspacio(TiempoEspacio &T, ListaEventos &L,
                                   const ParametrosSistema &xi);
};

/* ========== MÓDULO 3: EVENTOS (Algoritmo 1-3) ========== */
class ModuloEventos
{
public:
    // Evento de llegada
    static void eventoLlegada(EstadoSistema &X, CaracteristicasSistema &theta,
                              ListaEventos &L, const TiempoEspacio &T,
                              const ParametrosSistema &xi);

    // Evento de salida
    static void eventoSalida(EstadoSistema &X, CaracteristicasSistema &theta,
                             ListaEventos &L, const TiempoEspacio &T,
                             const ParametrosSistema &xi);

    // Actualizar estadísticas promedio (parte del manejo de eventos)
    static void actualizarEstadisticasPromedio(const EstadoSistema &X,
                                               CaracteristicasSistema &theta,
                                               TiempoEspacio &T);
};

/* ========== MÓDULO 4: PERCENTIL/GENERACIÓN ALEATORIA (Algoritmo 1-4) ========== */
class ModuloPercentil
{
public:
    // Función percentil para distribución exponencial
    static float percentilExponencial(float media);

    // Se pueden agregar otras distribuciones
    static float percentilNormal(float media, float desviacion);
    static float percentilUniforme(float a, float b);
};

/* ========== MÓDULO 5: GENERADOR DE REPORTES (Algoritmo 1-5) ========== */
class ModuloReportes
{
public:
    static void generarReporte(const CaracteristicasSistema &theta,
                               const TiempoEspacio &T,
                               const ParametrosSistema &xi,
                               FILE *archivo_salida);
};

/* ========== MÓDULO 6: PROCEDIMIENTO PRINCIPAL (Algoritmo 1-6) ========== */
class SimuladorPrincipal
{
private:
    TiempoEspacio T;
    EstadoSistema X;
    CaracteristicasSistema theta;
    ListaEventos L;
    ParametrosSistema xi;

public:
    void ejecutarSimulacion(const char *archivo_parametros,
                            const char *archivo_resultados);
};

#endif // SIMULACION_MODULOS_H