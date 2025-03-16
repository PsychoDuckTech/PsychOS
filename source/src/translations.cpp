#define DEFINE_TRANSLATIONS

#ifdef USE_PT_PT
const char *task_clock_started = "Tarefa do relógio iniciada.";
const char *task_knobHandler_started = "Tarefa do controlador de botão iniciada.";
const char *BLE_initialized_successfully = "BLE inicializado com sucesso.";
const char *BLE_initialization_failed = "Falha na inicialização do BLE.";
const char *received = "Recebido.";
const char *waitingForConnection = "Aguardando conexão.";
const char *moduleDisconnected = "Módulo desconectado.";
const char *moduleConnected = "Módulo conectado.";
const char *task_BLEHandler_started = "Tarefa do manipulador BLE iniciada.";
const char *matrix_initialized_successfully = "Matriz inicializada com sucesso.";
const char *matrix_initialization_failed = "Falha na inicialização da matriz.";
const char *task_keyScanning_started = "Tarefa de varredura de teclas iniciada.";
#else
const char *task_clock_started = "Clock task started.";
const char *task_knobHandler_started = "Knob handler task started.";
const char *BLE_initialized_successfully = "BLE initialized successfully.";
const char *BLE_initialization_failed = "BLE initialization failed.";
const char *received = "Received.";
const char *waitingForConnection = "Waiting for connection.";
const char *moduleDisconnected = "Module disconnected.";
const char *moduleConnected = "Module connected.";
const char *task_BLEHandler_started = "BLE handler task started.";
const char *matrix_initialized_successfully = "Matrix initialized successfully.";
const char *matrix_initialization_failed = "Matrix initialization failed.";
const char *task_keyScanning_started = "Key scanning task started.";
#endif