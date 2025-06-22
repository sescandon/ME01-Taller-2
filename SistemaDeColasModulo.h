#ifndef SISTEMA_DE_COLAS_MODULO_H
#define SISTEMA_DE_COLAS_MODULO_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ========== CONSTANTES ========== */
#define LIMITE_COLA 100
#define LIBRE 0
#define OCUPADO 1
#define EVENTO_LLEGADA 1
#define EVENTO_SALIDA 2

/* ========== ESTRUCTURAS DE DATOS ========== */

// Estructura para los parámetros del sistema
struct ParametrosSistema {
    float media_entre_llegadas;
    float media_atencion;
    int num_esperas_requerido;
    int num_eventos;
};

// Estructura para tiempo y espacio
struct TiempoEspacio {
    float tiempo_simulacion;
    float tiempo_ultimo_evento;
};

// Estructura para el estado del sistema
struct EstadoSistema {
    int estado_servidor;
    int num_entra_cola;
    float tiempo_llegada[LIMITE_COLA + 1];
};

// Estructura para las características del sistema
struct CaracteristicasSistema {
    int num_clientes_espera;
    float total_de_esperas;
    float area_num_entra_cola;
    float area_estado_servidor;
};

// Estructura para la lista de eventos
struct ListaEventos {
    float tiempo_sig_evento[3]; // Índices 1 y 2 para llegada y salida
};

// Estructura para datos de cada cliente (para logging)
struct DatosCliente {
    int numero;
    float tiempo_entre_llegadas;
    float tiempo_atencion;
    float tiempo_llegada;
};

// Estructura para el registro de eventos
struct RegistroEventos {
    FILE *archivo_log;
    int numero_cliente;
    float ultimo_tiempo_llegada;
    DatosCliente cliente_en_servicio;
    DatosCliente clientes_en_cola[LIMITE_COLA + 1];
};

/* ========== MÓDULOS ========== */

class ModuloInicializacion {
public:
    static void inicializar(TiempoEspacio &T, EstadoSistema &X,
                           CaracteristicasSistema &theta, ListaEventos &L,
                           const ParametrosSistema &xi);
};

class ModuloManejoTiempo {
public:
    static int manejoTiempoEspacio(TiempoEspacio &T, ListaEventos &L,
                                  const ParametrosSistema &xi);
};

class ModuloEventos {
public:
    static void eventoLlegada(EstadoSistema &X, CaracteristicasSistema &theta,
                             ListaEventos &L, const TiempoEspacio &T,
                             const ParametrosSistema &xi, RegistroEventos &registro);
    
    static void eventoSalida(EstadoSistema &X, CaracteristicasSistema &theta,
                            ListaEventos &L, const TiempoEspacio &T,
                            const ParametrosSistema &xi, RegistroEventos &registro);
    
    static void actualizarEstadisticasPromedio(const EstadoSistema &X,
                                              CaracteristicasSistema &theta,
                                              TiempoEspacio &T);
};

class ModuloPercentil {
public:
    static float percentilExponencial(float media);
};

class ModuloReportes {
public:
    static void generarReporte(const CaracteristicasSistema &theta,
                              const TiempoEspacio &T,
                              const ParametrosSistema &xi,
                              FILE *archivo_salida);
};

class ModuloRegistro {
public:
    static void inicializarRegistro(RegistroEventos &registro, const char *nombre_archivo);
    static void escribirEventoCliente(const DatosCliente &cliente, FILE *archivo);
    static void finalizarRegistro(RegistroEventos &registro);
};

class SimuladorPrincipal {
private:
    ParametrosSistema xi;
    TiempoEspacio T;
    EstadoSistema X;
    CaracteristicasSistema theta;
    ListaEventos L;

public:
    void ejecutarSimulacion(const char *archivo_parametros,
                           const char *archivo_resultados,
                           const char *archivo_log = "eventos_clientes.csv");
};

#endif // SISTEMA_DE_COLAS_MODULO_H