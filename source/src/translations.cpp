#define DEFINE_TRANSLATIONS
#define USE_PT_PT

#ifdef USE_PT_PT
// System messages
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

// UI Text
const char *ui_caps = "Caps";
const char *ui_wpm = "PPM";
const char *ui_layer = "CAMADA";
const char *ui_now_playing = "Reproduzindo";
const char *ui_song_title = "Bury you"; // Song titles typically stay in original language
const char *ui_settings = "Definicoes";
const char *ui_modules = "Modulos";
const char *ui_underglow = "Iluminacao";
const char *ui_clock = "Relogio";
const char *ui_iot_link = "Link IoT";
const char *ui_secured_by = "Secured by Dux";
const char *ui_powered_by = "Powered by PsychOS";
const char *ui_build = "versao";
#endif

#ifdef USE_EN_US
// Module Menu
const char *ui_status = "Status";
const char *ui_help = "Help";
const char *ui_searching = "Searching...";
const char *ui_contact_us = "Contact us";
const char *ui_status_disconnected = "Status: Disconnected";
const char *ui_connected = "Connected: ";
const char *ui_key_presses = "Key Presses";
const char *ui_signal = "Signal";
const char *ui_delay = "Delay: ~";
const char *ui_delay_great = "ms (Great!)";
const char *ui_delay_good = "ms (Good)";
const char *ui_delay_ok = "ms (OK)";
const char *ui_delay_poor = "ms (Poor)";
const char *ui_status_connected = "Status: Connected";
#endif

#ifdef USE_PT_PT
// Module Menu
const char *ui_no_module = "Sem modulo";
const char *ui_no_module_desc = "Conecte um Modulo";
const char *ui_connected_to = "Conectado a:";
const char *ui_module_status = "Estado: Conectado";
const char *ui_status = "Estado";
const char *ui_help = "Ajuda";
const char *ui_searching = "Procurando...";
const char *ui_contact_us = "Contate-nos";
const char *ui_status_disconnected = "Estado: Desconectado";
const char *ui_connected = "Conectado: ";
const char *ui_key_presses = "Teclas Pressas";
const char *ui_signal = "Sinal";
const char *ui_delay = "Atraso: ~";
const char *ui_delay_great = "ms (Otimo!)";
const char *ui_delay_good = "ms (Bom)";
const char *ui_delay_ok = "ms (OK)";
const char *ui_delay_poor = "ms (Ruim)";
const char *ui_status_connected = "Estado: Conectado";

// RGB Menu
const char *ui_red = "Vermelho";
const char *ui_green = "Verde";
const char *ui_blue = "Azul";
const char *ui_brightness = "Brilho";
const char *ui_rotate_adjust = "Girar: Ajustar Valor";
const char *ui_press_next = "Pressionar: Proximo";
const char *ui_long_press_quit = "Press. Longo: Voltar";

// Clock Menu
const char *ui_hours = "Horas";
const char *ui_minutes = "Minutos";
const char *ui_seconds = "Segundos";
const char *ui_pixel_flush = "Limpeza Ecra";

#else
// System messages
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

// UI Text
const char *ui_caps = "Caps";
const char *ui_wpm = "WPM";
const char *ui_layer = "LAYER";
const char *ui_now_playing = "Now Playing";
const char *ui_song_title = "Bury you";
const char *ui_settings = "Settings";
const char *ui_modules = "Modules";
const char *ui_underglow = "Underglow";
const char *ui_clock = "Clock";
const char *ui_iot_link = "IoT link";
const char *ui_secured_by = "Secured by Dux";
const char *ui_powered_by = "Powered by PsychOS";
const char *ui_build = "build";

// Module Menu
const char *ui_no_module = "No module";
const char *ui_no_module_desc = "Connect a Module";
const char *ui_connected_to = "Connected to:";
const char *ui_module_status = "Status: Connected";

// RGB Menu
const char *ui_red = "Red";
const char *ui_green = "Green";
const char *ui_blue = "Blue";
const char *ui_brightness = "Brightness";
const char *ui_rotate_adjust = "Rotate: Adjust Value";
const char *ui_press_next = "Press: Next Parameter";
const char *ui_long_press_quit = "Long Press: Quit to Menu";

// Clock Menu
const char *ui_hours = "Hours";
const char *ui_minutes = "Minutes";
const char *ui_seconds = "Seconds";
const char *ui_pixel_flush = "Pixel Flush";
#endif