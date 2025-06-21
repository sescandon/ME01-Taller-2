/* Implementación de los módulos según capítulo 1.7.1 */

#include "SistemaDeColasModulo.h"
#include "lcgrand.cpp"

/* ========== IMPLEMENTACIÓN MÓDULO 1: INICIALIZACIÓN ========== */
void ModuloInicializacion::inicializar(TiempoEspacio &T, EstadoSistema &X,
                                       CaracteristicasSistema &theta, ListaEventos &L,
                                       const ParametrosSistema &xi)
{
    // T ← ⟨ValoresInicialesTiempoEspacio⟩
    T.tiempo_simulacion = 0.0;
    T.tiempo_ultimo_evento = 0.0;

    // X ← ⟨ValoresInicialesEstudioSistema⟩
    X.estado_servidor = LIBRE;
    X.num_entra_cola = 0;

    // θ ← ⟨ValoresInicialesCaracterísticasSistema⟩
    theta.num_clientes_espera = 0;
    theta.total_de_esperas = 0.0;
    theta.area_num_entra_cola = 0.0;
    theta.area_estado_servidor = 0.0;

    // L ← ⟨ValoresInicialesListaEventos⟩
    L.tiempo_sig_evento[EVENTO_LLEGADA] = T.tiempo_simulacion +
                                          ModuloPercentil::percentilExponencial(xi.media_entre_llegadas);
    L.tiempo_sig_evento[EVENTO_SALIDA] = 1.0e+30; // Infinito (no hay cliente en servicio)
}

/* ========== IMPLEMENTACIÓN MÓDULO 2: MANEJO TIEMPO-ESPACIO ========== */
int ModuloManejoTiempo::manejoTiempoEspacio(TiempoEspacio &T, ListaEventos &L,
                                            const ParametrosSistema &xi)
{
    float min_tiempo_sig_evento = 1.0e+29;
    int k_estrella = 0; // k* en el algoritmo

    // k* ← {k|L[k] = min{L[γ]}}
    for (int i = 1; i <= xi.num_eventos; ++i)
    {
        if (L.tiempo_sig_evento[i] < min_tiempo_sig_evento)
        {
            min_tiempo_sig_evento = L.tiempo_sig_evento[i];
            k_estrella = i;
        }
    }

    // Verificar lista vacía
    if (k_estrella == 0)
    {
        fprintf(stderr, "\nLa lista de eventos está vacía en tiempo %f", T.tiempo_simulacion);
        exit(1);
    }

    // T ← L[k*]
    T.tiempo_simulacion = min_tiempo_sig_evento;

    // RETORNAR(k*)
    return k_estrella;
}

/* ========== IMPLEMENTACIÓN MÓDULO 3: EVENTOS ========== */
void ModuloEventos::eventoLlegada(EstadoSistema &X, CaracteristicasSistema &theta,
                                  ListaEventos &L, const TiempoEspacio &T,
                                  const ParametrosSistema &xi)
{
    float espera;

    // L ← ⟨ActualizarListaEventos⟩
    L.tiempo_sig_evento[EVENTO_LLEGADA] = T.tiempo_simulacion +
                                          ModuloPercentil::percentilExponencial(xi.media_entre_llegadas);

    // X ← ⟨ActualizarEstudioSistema⟩
    if (X.estado_servidor == OCUPADO)
    {
        ++X.num_entra_cola;

        if (X.num_entra_cola > LIMITE_COLA)
        {
            fprintf(stderr, "\nDesbordamiento de la cola en tiempo %f", T.tiempo_simulacion);
            exit(2);
        }

        X.tiempo_llegada[X.num_entra_cola] = T.tiempo_simulacion;
    }
    else
    {
        espera = 0.0;
        theta.total_de_esperas += espera;

        ++theta.num_clientes_espera;
        X.estado_servidor = OCUPADO;

        L.tiempo_sig_evento[EVENTO_SALIDA] = T.tiempo_simulacion +
                                             ModuloPercentil::percentilExponencial(xi.media_atencion);
    }
}

void ModuloEventos::eventoSalida(EstadoSistema &X, CaracteristicasSistema &theta,
                                 ListaEventos &L, const TiempoEspacio &T,
                                 const ParametrosSistema &xi)
{
    float espera;

    // X ← ⟨ActualizarEstudioSistema⟩
    if (X.num_entra_cola == 0)
    {
        X.estado_servidor = LIBRE;
        L.tiempo_sig_evento[EVENTO_SALIDA] = 1.0e+30;
    }
    else
    {
        --X.num_entra_cola;

        // θ ← ⟨ActualizarCalculoCaracterísticas⟩
        espera = T.tiempo_simulacion - X.tiempo_llegada[1];
        theta.total_de_esperas += espera;

        ++theta.num_clientes_espera;

        // L ← ⟨ActualizarListaEventos⟩
        L.tiempo_sig_evento[EVENTO_SALIDA] = T.tiempo_simulacion +
                                             ModuloPercentil::percentilExponencial(xi.media_atencion);

        // Mover clientes en la cola
        for (int i = 1; i <= X.num_entra_cola; ++i)
            X.tiempo_llegada[i] = X.tiempo_llegada[i + 1];
    }
}

void ModuloEventos::actualizarEstadisticasPromedio(const EstadoSistema &X,
                                                   CaracteristicasSistema &theta,
                                                   TiempoEspacio &T)
{
    float tiempo_desde_ultimo_evento = T.tiempo_simulacion - T.tiempo_ultimo_evento;
    T.tiempo_ultimo_evento = T.tiempo_simulacion;

    theta.area_num_entra_cola += X.num_entra_cola * tiempo_desde_ultimo_evento;
    theta.area_estado_servidor += X.estado_servidor * tiempo_desde_ultimo_evento;
}

/* ========== IMPLEMENTACIÓN MÓDULO 4: PERCENTIL ========== */
float ModuloPercentil::percentilExponencial(float media)
{
    // u ← Aleatorio(•)
    float u = lcgrand(1);

    // x ← F_x^(-1)(u)
    float x = -media * log(u);

    // RETORNAR(x)
    return x;
}

/* ========== IMPLEMENTACIÓN MÓDULO 5: REPORTES ========== */
void ModuloReportes::generarReporte(const CaracteristicasSistema &theta,
                                    const TiempoEspacio &T,
                                    const ParametrosSistema &xi,
                                    FILE *archivo_salida)
{
    // θ ← ⟨CalculoFinalDeCaracterísticas⟩
    float espera_promedio = theta.total_de_esperas / theta.num_clientes_espera;
    float num_promedio_cola = theta.area_num_entra_cola / T.tiempo_simulacion;
    float utilizacion_servidor = theta.area_estado_servidor / T.tiempo_simulacion;

    // ESCRIBIR(θ)
    fprintf(archivo_salida, "\n\n==== REPORTE FINAL DE SIMULACIÓN ====\n");
    fprintf(archivo_salida, "Espera promedio en la cola: %11.3f minutos\n", espera_promedio);
    fprintf(archivo_salida, "Número promedio en cola: %10.3f\n", num_promedio_cola);
    fprintf(archivo_salida, "Utilización del servidor: %15.3f\n", utilizacion_servidor);
    fprintf(archivo_salida, "Tiempo total de simulación: %12.3f minutos\n", T.tiempo_simulacion);
    fprintf(archivo_salida, "Total de clientes atendidos: %d\n", theta.num_clientes_espera);
}

/* ========== IMPLEMENTACIÓN PROCEDIMIENTO PRINCIPAL ========== */
void SimuladorPrincipal::ejecutarSimulacion(const char *archivo_parametros,
                                            const char *archivo_resultados)
{
    FILE *parametros, *resultados;

    // Abrir archivos
    parametros = fopen(archivo_parametros, "r");
    resultados = fopen(archivo_resultados, "w");

    if (!parametros || !resultados)
    {
        fprintf(stderr, "Error al abrir archivos\n");
        exit(1);
    }

    // Leer parámetros
    fscanf(parametros, "%f %f %d", &xi.media_entre_llegadas,
           &xi.media_atencion, &xi.num_esperas_requerido);
    xi.num_eventos = 2;

    // Escribir encabezado
    fprintf(resultados, "Sistema de Colas Simple - Implementación Modular\n\n");
    fprintf(resultados, "Tiempo promedio de llegada: %11.3f minutos\n", xi.media_entre_llegadas);
    fprintf(resultados, "Tiempo promedio de atención: %16.3f minutos\n", xi.media_atencion);
    fprintf(resultados, "Número de clientes objetivo: %14d\n\n", xi.num_esperas_requerido);

    // LLAMAR INICIALIZACIÓN
    ModuloInicializacion::inicializar(T, X, theta, L, xi);

    // MIENTRAS (condición no se cumpla) HACER
    while (theta.num_clientes_espera < xi.num_esperas_requerido)
    {
        // LLAMAR ManejoTiempoEspacio
        int tipo_evento = ModuloManejoTiempo::manejoTiempoEspacio(T, L, xi);

        // Actualizar estadísticas promedio
        ModuloEventos::actualizarEstadisticasPromedio(X, theta, T);

        // SEGÚN (tipo_evento) HACER
        switch (tipo_evento)
        {
        case EVENTO_LLEGADA:
            ModuloEventos::eventoLlegada(X, theta, L, T, xi);
            break;
        case EVENTO_SALIDA:
            ModuloEventos::eventoSalida(X, theta, L, T, xi);
            break;
        }
    }

    // LLAMAR GeneradorReporte
    ModuloReportes::generarReporte(theta, T, xi, resultados);

    // Cerrar archivos
    fclose(parametros);
    fclose(resultados);
}

/* Función main para usar la clase */
int main()
{
    SimuladorPrincipal simulador;
    simulador.ejecutarSimulacion("param.txt", "result.txt");
    return 0;
}